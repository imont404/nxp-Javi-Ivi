+++
type = "plan"
id = "lcd-spi-throughput"
status = "active"
created = "2026-07-25"

[[steps]]
id = "measure-baseline"
title = "Instrument the frame path and measure where the time actually goes, before changing anything"
status = "done"

[[steps]]
id = "confirm-spi-clock"
title = "Establish the real SCK frequency and the actual ceiling, on a scope and from the divider registers"
status = "pending"
depends_on = ["measure-baseline"]

[[steps]]
id = "split-init-from-reconfigure"
title = "Replace fsl_lpspi_edma with a purpose-built driver: one-time init, no RX path"
status = "done"
depends_on = ["measure-baseline"]

[[steps]]
id = "polled-small-transfers"
title = "Send control bytes by polled FIFO writes, waiting on transfer-complete before moving RS"
status = "pending"
depends_on = ["split-init-from-reconfigure"]

[[steps]]
id = "hoist-window-setup"
title = "Set the ST7789 address window once instead of once per frame"
status = "pending"
depends_on = ["polled-small-transfers"]

[[steps]]
id = "raise-spi-clock"
title = "Raise SCK as far as the peripheral, the panel, and the signal integrity allow"
status = "pending"
depends_on = ["confirm-spi-clock"]

[[steps]]
id = "single-block-dump"
title = "Collapse the 15-block dump into one pre-built TCD armed by writing SADDR"
status = "pending"
depends_on = ["split-init-from-reconfigure"]

[[steps]]
id = "verify-and-document"
title = "Re-measure, confirm the display is visually correct, and update the design docs"
status = "pending"
depends_on = ["hoist-window-setup", "raise-spi-clock", "single-block-dump"]

[[steps]]
id = "design-doc-intent-audit"
title = "Audit design docs, ADRs, and requirements against implementation"
status = "pending"
depends_on = ["verify-and-document"]

[[steps]]
id = "test-runtime-impact-audit"
title = "Audit new test runtime impact"
status = "pending"
depends_on = ["verify-and-document"]

[[steps]]
id = "external-review"
title = "Obtain independent external review"
status = "pending"
depends_on = ["design-doc-intent-audit", "test-runtime-impact-audit"]

[[exit_criteria]]
id = "overhead-removed"
title = "Per-frame control overhead is reduced to the wire time of the bytes that must be sent"
status = "pending"

[[exit_criteria]]
id = "clock-understood"
title = "The SCK ceiling is known and justified from the reference manual and the panel datasheet, not guessed"
status = "pending"

[[exit_criteria]]
id = "frame-time-improved"
title = "Measured frame dump time is materially lower and the number is recorded"
status = "pending"

[[exit_criteria]]
id = "display-correct"
title = "The display is visually correct at the new settings, including after a cold reset"
status = "pending"

[[exit_criteria]]
id = "budget-headroom"
title = "The change is expressed as recovered milliseconds inside the 41 ms frame budget"
status = "pending"

[[exit_criteria]]
id = "design-doc-intent-audit"
title = "Design docs, ADRs, and requirements match implementation"
status = "pending"

[[exit_criteria]]
id = "test-runtime-impact-audit"
title = "New tests are listed and runtime impact is reviewed"
status = "pending"

[[exit_criteria]]
id = "external-review"
title = "Independent external review is complete"
status = "pending"
+++

# Making the ST7789 SPI path stop wasting the frame budget

## What prompted this

The observation was ~500 us of control overhead before each frame, caused by
small DMA transfers going through the stock NXP driver, plus SCK being capped
at 37.5 MHz.

Both are real. The 37.5 MHz figure is corroborated: it is exactly what the clock
tree predicts, derived below independently of the measurement. There is nothing
unexplained about the current speed - only the question of how far up it can go.

Reading the code first turned up something larger that was not in the original
description, and it changes what to fix first.

## What the code actually does per frame

`eGFX_Dump()` in `source/avc_io/eGFX_Driver_ER-TFT020-3.c:35` runs this sequence
for every frame:

1. `lpspi1_init(8)`
2. `LCD_SetPos(0, 319, 0, 239)`
3. `lpspi1_init(32)`
4. `ST7789__display_img(buffer)` - 15 blocks of 10,240 bytes

Step 1 and step 3 are the surprise. `lpspi1_init()` at `source/avc_io/lpspi1.c:28`
is not a cheap mode switch. Every call does:

- `LPSPI_MasterInit()` - a full peripheral reset and reconfigure
- `EDMA_GetDefaultConfig()` then **`EDMA_Init(DMA0, ...)`** - re-initialises the
  entire DMA controller
- two `memset`s over the handle structures
- three `EDMA_CreateHandle()` calls
- two `EDMA_SetChannelMux()` calls
- `LPSPI_MasterTransferCreateHandleEDMA()`
- `LPSPI_MasterTransferPrepareEDMALite()`

**That happens twice per frame.** Re-initialising a shared DMA controller as part
of drawing a frame is the kind of thing that works right up until something else
starts using DMA - and the camera does. This is worth fixing for correctness,
not only for speed.

The reason for the two calls is a frame-size change: control bytes are sent as
8-bit frames, pixel data as 32-bit frames for FIFO efficiency. That is a
sensible goal implemented in the most expensive way available. Frame size lives
in the LPSPI `TCR` register and can be changed by writing it, with no re-init of
anything.

### The 11 single-byte transfers

`LCD_SetPos()` at `source/avc_io/st7789.c:513` issues eleven calls, each landing
in `lpspi1_transfer_byte()` -> `lpspi1_transfer_block()` -> a full
`LPSPI_MasterTransferEDMALite()` descriptor setup, DMA start, completion
interrupt and callback - to move **one byte**. At 37.5 MHz a byte is 213 ns on
the wire. Everything else is software.

Each transfer also opens with `while (!isTransferCompleted);`, so nothing
overlaps. The path pays DMA setup cost for blocking behaviour.

### And the window does not change

`LCD_SetPos(0, 319, 0, 239)` sets the full screen. It is called with the same
arguments every frame, and it is already called once in `eGFX_InitDriver()`.
After a full-window `RAMWR`, the ST7789 address counter wraps back to the window
origin, so the column and row addresses do not need resending. At most `0x2C`
needs re-issuing to start a new write.

Eleven single-byte transfers per frame can become one.

## The part that matters more than all of the above

The panel is 320x240 at 16 bpp, so a frame is **153,600 bytes**. That is
1,228,800 bits, and at SPI clock it costs:

| SCK | Frame dump time | Share of the 41 ms budget |
|---|---|---|
| 27.5 MHz | 44.7 ms | over budget on its own |
| 37.5 MHz | 32.8 ms | 80% |
| 50 MHz | 24.6 ms | 60% |
| 75 MHz | 16.4 ms | 40% |

**The clock is worth twenty to thirty milliseconds per frame. The control
overhead is worth half of one.** Both should be fixed - the control overhead is
cheap to fix and the DMA re-init is a latent correctness bug - but the effort
should be weighted accordingly, and success should be judged on the dump time.

## What the clock is actually set to

Traced through the clock tree rather than assumed:

- `clock_config.c:348` - PLL0 configured to **150 MHz**
- `clock_config.c:390` - `kCLOCK_DivPllClk` set to **2**, so PLLCLKDIV is
  **75 MHz**
- `eGFX_Driver_ER-TFT020-3.c:17-18` - FLEXCOMM1 divider 1, attached to
  `kPLL_DIV_to_FLEXCOMM1`, so the LPSPI source clock is **75 MHz**
- `lpspi1.c:12` - `CONFIG__TRANSFER_BAUDRATE` requests **50 MHz**

LPSPI cannot divide by less than two, so the fastest SCK from a 75 MHz source is
**37.5 MHz**. The requested 50 MHz is not achievable and is being silently
clamped - `LPSPI_MasterSetBaudRate` picks the closest divider it can and does not
complain.

So the configured request is fiction, and the real number is 37.5 MHz - which is
the observed figure. The clock tree and the measurement agree, so `confirm-spi-clock`
is now about establishing the *ceiling*, not the current value.

The lever for going faster is PLLCLKDIV. At divide-by-1 the source becomes
150 MHz and SCK could reach 75 MHz. **PLLCLKDIV is not private to the LCD** -
before changing it, find every peripheral hanging off it, because raising it
changes their clocks too.

Two ceilings then apply, and both need looking up rather than assuming:

1. **The LPSPI module maximum** in the MCXN947 reference manual and datasheet.
   A peripheral fed 150 MHz is not automatically rated to clock SCK at 75 MHz.
2. **The panel.** The ST7789 datasheet write cycle is far slower than what these
   displays are commonly driven at, and the flex cable on a car is not a
   controlled transmission line. Expect the practical limit to be set by signal
   integrity, and expect it to look like sparkle or torn pixels rather than a
   clean failure.

## The driver to write

The stock `fsl_lpspi_edma` driver is general-purpose: full duplex, arbitrary
transfer descriptors rebuilt per call, callbacks, state machines. We use one
direction, to one device, with one buffer layout. Writing a small purpose-built
driver is less code than configuring the general one, and the register facts
below make it concrete.

### Is bypassing the HAL worth it? Measured, not argued

**Flash is not the reason.** All LPSPI and eDMA HAL code reaching the linked
image totals **10,596 bytes** - 4.3% of `text`, and **1.0% of the 1 MB flash**,
which is 23% used. Removing it is real but nobody would notice. The two largest
contributors are `LPSPI_MasterTransferEDMALite` at 1,368 bytes and
`EDMA_ConfigChannelSoftwareTCDExt` at 1,014 bytes.

**Cycles are the reason.** Submitting one transfer costs **32.3 us, which is
4,845 cycles at 150 MHz** - measured, consistent across both the single-byte and
the 8,192-byte paths, so it is fixed cost independent of size. Writing a TCD is
roughly eight register writes. The HAL is spending something like a hundred
times what the hardware operation needs, on argument checking, handle
bookkeeping, a software TCD build, and a completion path through
`EDMA_HandleIRQ` into two nested callbacks.

The sharpest illustration: `LCD_SetPos` moves 11 bytes, which at 37.5 MHz is
**2.3 us of wire time**, and measures **355 us**. That is 154x overhead to move
eleven bytes.

**What it is worth: about 1.36 ms per frame** - 42 transfer submissions at
32.3 us. That is a third of what raising the clock is worth, and it is the
entire remaining gap between the current 34.14 ms and the 32.8 ms wire-time
floor.

**So: yes, but for the right reason.** Not to save flash, and not primarily for
the 1.36 ms. The better arguments are that a purpose-built path removes the
shared-DMA0 hazard by construction rather than by discipline, and that a
readable driver of a couple of hundred lines is something a student can follow
when the display misbehaves, which 10 KB of general-purpose HAL is not.

Sequence it after the clock work, since the clock is worth more and is
independent.

### Setup code specifically: leave it alone

Splitting the same 10,596 bytes by which path it is on:

| | Bytes | Share | Runs |
|---|---|---|---|
| setup / configuration | 3,412 | 32% | **once, at boot** |
| transfer submission | 7,184 | 68% | 42x per frame |

**The setup path is the part not worth rewriting**, which is the opposite of the
intuition. Now that `lpspi1_init()` runs its expensive work once, that 3,412
bytes costs 770 us one time at boot and nothing thereafter. Hand-writing it
would save 0.3% of flash and no measurable time.

It is also the part where the HAL genuinely earns its place. `LPSPI_MasterSetDelayTimes`
is the largest single function at 644 bytes, and it is doing scaler search
arithmetic; `LPSPI_MasterSetBaudRate` at 272 bytes is doing the prescaler and
`SCKDIV` search. That is fiddly, easy to get subtly wrong, and it runs once
where being slow costs nothing.

All the cost that remains is in the **transfer** path - the 7,184 bytes that run
42 times a frame at 4,845 cycles a call. That is where a hand-written driver
pays, and it is a much smaller thing to write than a full init sequence.

**One useful side effect of the baud rate search living in the HAL:** it picked
the divider that clamped the requested 50 MHz to 37.5 MHz. `CCR[SCKDIV]` and
`TCR[PRESCALE]` can be read back to recover the exact SCK the hardware settled
on, which gives `confirm-spi-clock` an answer without a scope.

### Drop the receive path entirely

`TCR[RXMSK]` masks received data so nothing is ever loaded into the RX FIFO.
Set it once and the entire receive side disappears: no RX DMA channel, no RX
FIFO draining, no overflow handling, no `MASTER_DMA_RX_CHANNEL`. That is one of
the two DMA channels currently allocated, and roughly half the driver's
complexity, removed by one bit.

This is safe here because nothing is ever read back from the panel on this path.

### Control bytes: polled FIFO writes

The TX FIFO is **8 words deep** (`FCR[TXWATER]` is 3 bits; confirm against
`PARAM[TXFIFO]` at runtime). Every byte `LCD_SetPos` needs fits in the FIFO at
once, so the whole sequence is: set `TCR[FRAMESZ]` to 8-bit, write the bytes to
`TDR`, wait for completion. No descriptors, no interrupts, no callback.

**The one trap is the RS/DC line.** It selects command versus data, and it must
not change until the byte it applies to has actually left the shifter. Waiting
for the FIFO to drain is not enough - the last byte is still shifting out. Wait
on the transfer-complete flag or the module-busy flag in `SR`, not on FIFO
count. This is the classic way a polled display driver produces intermittent
garbage that looks like a signal-integrity problem.

### Frame data: one pre-built TCD, armed by writing an address

Build the TCD once at init with everything that never changes - destination
`TDR`, destination offset zero, source offset, attributes, minor loop size,
`CSR`. Per frame, write `SADDR` and start the channel. That is the shape you
described, and it is achievable.

**But the obvious version does not fit in one TCD, and that is almost certainly
why the code has a 15-block loop.** The arithmetic:

- `CITER`/`BITER` are 15 bits - `DMA_TCD_CITER_ELINKNO_CITER_MASK` is `0x7FFF`,
  so **32,767 iterations maximum**
- a frame is 153,600 bytes
- with a 4-byte minor loop, that needs **38,400 iterations** - over the limit

Hence 15 blocks of 10,240 bytes, each comfortably inside the limit. The loop is
not arbitrary; it is working around a hardware ceiling.

Two ways past it, both worth trying:

1. **Widen the minor loop.** `NBYTES` is 30 bits, so it is not the constraint.
   At 8 bytes per minor loop the frame needs **19,200 iterations**, which fits
   in a single TCD. The FIFO is 8 words, so moving two words per DMA request is
   comfortable - set `FCR[TXWATER]` so a request is only raised with room for
   the whole minor loop.
2. **Scatter-gather two linked TCDs.** Pre-build a chain covering the frame and
   let the eDMA walk it with no CPU involvement at all.

Option 1 is simpler and should be tried first. If the frame buffers are at fixed
addresses - and with double buffering there are only two - then both TCDs can be
pre-built at init and a frame costs *one register write to select which*, which
is as close to free as this gets.

### What stays

`TCR[BYSW]` provides the byte swapping the 32-bit pixel path depends on. Keep
it, and verify colours after the change; wrong byte order shows as wrong colours
rather than a dead display.

## Approach

`measure-baseline` comes first and is not optional. There is already a GPIO
toggle pattern in `lpspi1.c` (`GPIO_PinWrite(GPIO4, 0, ...)` in the DMA
callback) that can be extended to bracket each phase, and a scope is available.
Measure: total `eGFX_Dump()`, each `lpspi1_init()`, `LCD_SetPos()`, and the
15-block loop. Without that, none of the later claims can be checked, and the
500 us figure stays an estimate.

Then the cheap structural fixes, in order of risk:

- `split-init-from-reconfigure` - one-time init at startup, `TCR[RXMSK]` set so
  the receive path and its DMA channel disappear, and a small frame-size helper
  that writes `TCR` only. Biggest correctness win.
- `polled-small-transfers` - push control bytes straight into the TX FIFO and
  wait on transfer-complete. Mind the RS timing noted above.
- `hoist-window-setup` - set the window at init; per frame send only `RAMWR`.
- `single-block-dump` - widen the minor loop to 8 bytes so the frame fits one
  TCD, pre-build it, and arm it by writing `SADDR`.

Then `raise-spi-clock`, which is the payoff and the risk, and is deliberately
sequenced last so it is not confounded with the structural changes.

## Things to be careful about

**The DMA re-init is shared state.** `EDMA_Init(DMA0, ...)` per frame while the
camera backend also uses DMA is a real hazard. Check what the camera path
assumes about DMA0 before and after this change.

**`kLPSPI_MasterByteSwap` interacts with frame size.** The 32-bit path relies on
byte swapping to get pixel byte order right. Changing how frame size is set must
preserve that, and the failure mode is wrong colours rather than no picture.

**Do not test this only on a static image.** A torn or sparkling display at a
raised clock may only show up on live camera data with changing content.

**Verify after a cold reset, not just after a reflash.** Initialisation order
bugs hide behind an already-configured peripheral.

## What success looks like

A recorded before-and-after of `eGFX_Dump()` wall time, with the recovered
milliseconds stated against the 41 ms budget. That is the number that matters to
students - it is the time they get back for their own algorithm.

## Files

- `src/avc/avc_core0/source/avc_io/lpspi1.c` - the init and transfer primitives
- `src/avc/avc_core0/source/avc_io/st7789.c` - `LCD_SetPos`, `Write_Cmd`,
  `ST7789__display_img`
- `src/avc/avc_core0/source/avc_io/eGFX_Driver_ER-TFT020-3.c` - the per-frame
  sequence
- `src/avc/avc_core0/board/clock_config.c` - PLL0 and PLLCLKDIV
