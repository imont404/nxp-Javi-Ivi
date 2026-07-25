# Camera Capture Backend — Can FlexIO Use the Existing EZH Pins?

**Question:** Do the pins the camera is *already routed to* on Rev A also carry FlexIO —
so the capture backend becomes a software choice rather than a board respin?

**Answer: yes, with three jumper wires.** After them, `P1_4..P1_11` is *simultaneously*
contiguous `FLEXIO0_D12..D19` and contiguous `SMARTDMA_PIO0..PIO7` — EZH vs FlexIO
becomes **alt7 vs alt6** on identical wiring.

**Date:** 2026-07-25. Desk research against netlists and NXP signal data,
**then built and verified on hardware the same day.**

## Result

**Proven.** Three jumpers on `J9_EXT`, and FlexIO captures on the Rev A camera wiring at
**23.39 fps** against the EZH baseline of 23.43, with zero shifter, timeout, or DMA errors.
Both backends were then flashed alternately with no hands on the hardware, and both
produced a good image on the LCD.

| Measure | EZH (baseline) | FlexIO Port 1 group |
|---|---|---|
| Frame rate | 23.43 fps | **23.39 fps** |
| Frame time | ~42.7 ms | 42.42 ms |
| VSYNC period | 42.76 ms | 42.76 ms |
| Errors | — | timeout 0, submit 0, callback 0, `shifterr` 00 |
| Wires needed | 0 (as routed) | **3 jumpers** |

Backend selection is now **alt7 versus alt6 in firmware** on one physical harness.

**Beyond freeing the EZH, this makes core1 usable.** The EZH shares a code bus with core1,
which is why the second CM33 has gone unused. FlexIO capture runs on eDMA instead, so the
contention that made core1 impractical goes away. See §Purpose.

One caveat worth carrying forward: this was achieved with **dupont stubs** on three
signals. It still hit the EZH frame rate with no shifter errors, so a properly routed
Rev B should be at least as good — but **no PCLK ceiling should be inferred from this
setup in either direction.**

See `docs/plans/camera-flexio-pin-migration/` for the full bring-up record.

## Purpose — read this first

**The goal is to free the EZH, not to raise the frame rate.**

FlexIO camera capture **already works** in this firmware, at the existing frame rate, on
the Port 4 pin group (`CAMERA_CAPTURE_BACKEND_FLEXIO_EDMA`, `P4_12..P4_22`). That path is
proven. What it needs is eleven fly-wires, because Port 4 is not where the camera is
routed.

Two things come out of moving capture off the EZH, and the second matters more than it
first appears:

1. **The EZH is released** for other work that is not I/O-driven on these pins. Capture is
   currently what occupies it.
2. **Core1 becomes genuinely usable.** The EZH shares a code bus with core1, so running
   capture on the EZH is what has made the second CM33 impractical. FlexIO capture uses
   eDMA instead, which does not contend the same way.

That second point changes the architecture available to the vision pipeline. The MCXN947
is a dual-core part whose second core has been effectively unavailable; this is what makes
it real. It bears directly on:

- **Full-frame colour processing** — the chroma-LUT work in
  [`AVC_Vision_Pipeline_Design.md`](AVC_Vision_Pipeline_Design.md) makes per-pixel HSV
  affordable for the first time, and a free core1 is where that could live without
  touching the 41 ms control loop on core0.
- **PowerQuad overlap** — same document, §5. PowerQuad's native driver is fire-and-forget,
  but the overlap only pays when there is enough work to hide; a second core widens what
  can be structured around it.
- **The NPU**, if ever pursued. `neutronRunBlocking` is the only implemented execution
  path (see [`neutron_npu/evidence/blocking_execution_proof.md`](neutron_npu/evidence/blocking_execution_proof.md)),
  so the only way to keep it off the control loop is to run it from core1 — which requires
  core1 to be usable in the first place.

So this investigation is about **wiring, not throughput**:

- Can the *existing* Rev A camera wiring drive FlexIO with minimal change?
- Does that let us prove the EZH can be freed on hardware we already have?
- What should the next board revision route?

**Frame rate is not a success criterion.** Matching the current rate is sufficient. Any
improvement is incidental and must not be claimed from a jumpered setup anyway.

**Both FlexIO pin groups stay selectable.** The Port 4 path is working and must not be
regressed — this adds a second option behind a `#define`, it does not replace anything.

---

## 1. Silicon: every SmartDMA pin also carries FlexIO

From the authoritative NXP signal data for **MCXN947VDF** (184-pin VFBGA — the FRDM part),
**56 pins expose both**, consistently:

- **alt7 = `SMARTDMA_PIOn`**
- **alt6 = `FLEXIO0_Dn`**

Source: `MCXN947/json/ksdk2_0/MCXN947VDF/signal_configuration.json`, `pins[].@name`
mux lists.

**But capability is not the constraint — FlexIO *numbering* is.** FlexIO parallel shift
needs **contiguous** `D` pins. SmartDMA does not care which physical pin carries a given
`PIOn`, so the same wiring can be ideal for one and useless for the other.

## 2. The collision in the current wiring

Current assignment, from `src/avc/avc_core0/source/avc_io/bv_camera__interface.c:1438-1445`:

| Cam | Pin | SmartDMA | FlexIO | |
|---|---|---|---|---|
| D0 | P1_4 | PIO0 | `D12` | ok |
| D1 | P1_5 | PIO1 | `D13` | ok |
| D2 | P1_6 | PIO2 | `D14` | ok |
| D3 | P1_7 | PIO3 | `D15` | ok |
| **D4** | **P3_4** | PIO4 | **`D12`** | **collides with D0** |
| **D5** | **P3_5** | PIO5 | **`D13`** | **collides with D1** |
| D6 | P1_10 | PIO6 | `D18` | ok |
| D7 | P1_11 | PIO7 | `D19` | ok |

FlexIO set is `{12,13,14,15,12,13,18,19}` — a hard collision, and 16/17 missing.
**SmartDMA is contiguous today (PIO0..PIO7); FlexIO is not.** That asymmetry is the entire
problem.

### The stale comment named the answer

Lines 1442–1443 read `// EZH_PIO4, PIO1_8,P1_8/...` and `// EZH_PIO5, PIO1_9,P1_9/...`
while the code writes `PORT3->PCR[4]` and `PORT3->PCR[5]`. **The comments are wrong about
the code and right about the intent.** `SMARTDMA_PIO4/PIO5` exist on both pin pairs, so
EZH never noticed.

## 3. The fix

| Cam | Pin | SmartDMA | FlexIO |
|---|---|---|---|
| D0–D3 | P1_4…P1_7 | PIO0–3 | `D12`–`D15` |
| **D4** | **P1_8** | **PIO4** | **`D16`** |
| **D5** | **P1_9** | **PIO5** | **`D17`** |
| D6–D7 | P1_10, P1_11 | PIO6–7 | `D18`–`D19` |

Contiguous both ways. Verified programmatically against the signal dump.

## 4. Sync signals

Today the syncs use **no pin alt function at all** — they reach SmartDMA through INPUTMUX
(`bv_camera__interface.c:1430-1432`):

```c
INPUTMUX_AttachSignal(INPUTMUX0, 0, kINPUTMUX_GpioPort0Pin4ToSmartDma);  // VSYNC P0_4
INPUTMUX_AttachSignal(INPUTMUX0, 1, kINPUTMUX_GpioPort0Pin11ToSmartDma); // HSYNC P0_11
INPUTMUX_AttachSignal(INPUTMUX0, 2, kINPUTMUX_GpioPort0Pin5ToSmartDma);  // PCLK  P0_5
```

| Signal | Pin | FlexIO | Action |
|---|---|---|---|
| **PCLK** | P0_5 | **none** | **must reroute** |
| HSYNC | P0_11 | `FLEXIO0_D3` | **none needed** |
| VSYNC | P0_4 | none | none needed — GPIO IRQ, as the earlier FlexIO test used |

## 5. Board evidence

Netlists: `frdm-avc/tracks/A/output/design_review/design/` and
`avc/docs/FRDM-MCXN947/.../design_review/design/`.

**Connector identity:** FRDM-AVC **J9** (2×16, 2.54 mm) mates to FRDM-MCXN947 **J11**
(the FRDM board's `J9` carries no nets). The shield also has **`J9_EXT`** — an identical
2×16 header on the *same nets*. **`J9_EXT` is the jumper point**: every camera signal is
reachable without disturbing the stack.

### P1_8 / P1_9 loading

| Net | Shield | FRDM board |
|---|---|---|
| `P1_8` (J9.32) | J9, J9_EXT only | J11.32, **R173 → `LPC_MCU_LINK_TX`**, U1.A1 |
| `P1_9` (J9.30) | J9, J9_EXT only | J11.30, **R172 → `LPC_MCU_LINK_RX`**, U1.B1 |

**R172/R173 are 330 Ω** series isolation into the MCU-Link VCOM UART.

- **P1_9: harmless.** MCU-Link only listens.
- **P1_8: contention.** MCU-Link TX drives it through 330 Ω. Nothing is damaged — that
  resistor exists for exactly this, limiting the fight to ~10 mA — and TX idles high.
  **Remove R173 (0603, FRDM board) if D4 misbehaves.** The VCOM is unused: debug is
  SEGGER RTT (`E_LOG` → `SEGGER_RTT_LOG`) and `BOARD_InitDebugConsole()` is never called.

### PCLK reroute candidates

| Pin | J9 | FlexIO | Verdict |
|---|---|---|---|
| **P1_14** | **2** | **`D22`** | **use this** — J9/J9_EXT + J11/U1 only |
| P1_15 | 1 | `D23` | equally clean, alternate |
| P1_12 | 28 | `D20` | **avoid** — drives Q2 (FDV301N MOSFET) + R3 (100 k) on the shield |
| P1_13 | 27 | `D21` | **avoid** — R163 0 Ω to `ENET_RXDV/MODE2`, an Ethernet PHY strap |

P1_14/P1_15 are nominally `EZH_LCD_D10/D11` — parallel-LCD signals, unused with the
shipped SPI panel (`CONFIG__DISPLAY_PANEL = DISPLAY_PANEL_ER_TFT020_3`).

## 6. Minimum change

**Hardware — three short dupont jumpers on `J9_EXT`:**

| # | From | To |
|---|---|---|
| 1 | pin 12 (`P3_4`, cam D4) | pin 32 (`P1_8`) |
| 2 | pin 11 (`P3_5`, cam D5) | pin 30 (`P1_9`) |
| 3 | pin 15 (`P0_5`, PCLK) | pin 2 (`P1_14`) |

Contingent: remove **R173**.

**No cuts.** The camera signal stays on its original net; the old pin is simply muxed off
in firmware, so both pins see the signal and only the new one is read. Each jumper adds a
stub — acceptable at the current PCLK for a bench fixture, and a reason not to raise PCLK
until Rev B routes it properly.

**Firmware:**
- `P1_4..P1_11` → **alt6** (`FLEXIO0_D12..D19`)
- `P1_14` → alt6 (`FLEXIO0_D22`, PCLK)
- `P0_11` → alt6 (`FLEXIO0_D3`, HSYNC)
- `P0_4` VSYNC → GPIO IRQ, unchanged
- Mux `P3_4`, `P3_5`, `P0_5` to disabled
- Drop the three `INPUTMUX_AttachSignal` calls on the FlexIO path

## 7. Code analysis — what has to change

Source: `src/avc/avc_core0/source/avc_io/bv_camera__interface.c`.

The working FlexIO path is **already partly parameterised**, which helps:

```c
#define CAMERA_FLEXIO_DATA_GPIO_START_PIN (12U)  /* PORT pin index of camera D0     */
#define CAMERA_FLEXIO_DATA_PIN_START      (20U)  /* FLEXIO0_D index of camera D0    */
#define CAMERA_FLEXIO_PCLK_PIN            (28U)  /* FLEXIO0_D index for PCLK        */
#define CAMERA_FLEXIO_HREF_PIN            (29U)  /* FLEXIO0_D index for HREF        */
#define CAMERA_DIAG_PCLK_PIN              (20U)  /* PORT4 pin index                 */
#define CAMERA_DIAG_HSYNC_PIN             (21U)  /* PORT4 pin index                 */
#define CAMERA_DIAG_VSYNC_PIN             (22U)  /* PORT4 pin index                 */
```

### What blocks a second pin group

| Coupling | Where | Why it blocks Port 1 |
|---|---|---|
| **Port hardcoded** | ~48 `PORT4`/`GPIO4` references, `kCLOCK_Port4`, `kCLOCK_Gpio4` | Port 1 group needs `PORT1`/`GPIO1` for data |
| **Single contiguous loop** | `camera__configure_flexio_edma_pins()`: `for (pin = DATA_GPIO_START_PIN; pin <= CAMERA_DIAG_HSYNC_PIN; pin++)` on `PORT4` | Assumes data + PCLK + HREF are consecutive pins on **one** port. Port 1 group has data on `P1_4..P1_11`, PCLK on `P1_14` (gap at 12/13), HREF on **`P0_11` — a different port** |
| **VSYNC IRQ hardcoded** | `GPIO40_IRQn`, `GPIO40_IRQHandler` | Port 1 group's VSYNC is `P0_4` → `GPIO00_IRQHandler` |

### What is already in our favour

- **`GPIO00_IRQHandler` already exists** in this file and already services `P0_4` (VSYNC)
  and `P0_11` (HSYNC) as the *reference* diagnostic counters
  (`CAMERA_REF_VSYNC_PIN 4`, `CAMERA_REF_HSYNC_PIN 11`). The Port 1 group's sync pins are
  therefore **already configured, already interrupting, already counted**. The work is
  promoting that handler from counting to driving capture — not building it.
- Shifter/timer/eDMA config (`camera__configure_flexio_camera()`) is expressed purely in
  terms of the `FLEXIO0_D` indices above, so it needs **no structural change** — only
  different constants.
- `camera__flexio_timer_trigger_sel_pininput()` derives the timer trigger from the PCLK
  FlexIO index, so PCLK moving from `D28` to `D22` is a constant change.

### Required shape

Add a pin-group selector alongside the existing backend selector, defaulting to the
proven Port 4 group:

```c
#define CAMERA_FLEXIO_PIN_GROUP_PORT4  1   /* proven, needs 11 fly-wires   */
#define CAMERA_FLEXIO_PIN_GROUP_PORT1  2   /* Rev A camera wiring + 3 jumpers */

#ifndef CONFIG__CAMERA_FLEXIO_PIN_GROUP
#define CONFIG__CAMERA_FLEXIO_PIN_GROUP (CAMERA_FLEXIO_PIN_GROUP_PORT4)
#endif
```

Then per-group constants for: data port/GPIO/clock/start-pin/FlexIO-start, PCLK
port+pin+FlexIO index, HREF port+pin+FlexIO index, VSYNC port+GPIO+pin+IRQn. And split
the one contiguous loop into a data loop plus explicit PCLK and HREF configuration, since
they are no longer guaranteed to be adjacent or even on the same port.

**This is a prove-it-is-possible exercise.** A tidy build-system-wide cleanup comes later;
the near-term requirement is only that the Port 4 group remains selectable and unbroken.

### Group constants

| | Port 4 group (proven) | Port 1 group (proposed) |
|---|---|---|
| Data D0–D7 | `P4_12..P4_19` → `D20..D27` | `P1_4..P1_11` → `D12..D19` |
| PCLK | `P4_20` → `D28` | `P1_14` → `D22` |
| HREF | `P4_21` → `D29` | `P0_11` → `D3` |
| VSYNC | `P4_22`, GPIO4 IRQ | `P0_4`, GPIO0 IRQ |
| Wires needed | 11 fly-wires | **3 jumpers on `J9_EXT`** |

## 8. Relationship to the earlier FlexIO work

`FlexIO_Camera_Test_Plan.md` and `MCXN947/flexio_camera_io_pin_map.md` target
`P4_12..P4_23`. **That path works and is not being replaced.** It simply is not where the
camera is routed, so it costs eleven fly-wires. The `P1_4..P1_11` group **is** the
existing camera wiring — three jumpers, and EZH keeps working on the same harness.

## 9. Rev B — recommended

**Recommendation: route it.** The jumpered build met its bar — correct capture at the
existing frame rate with zero errors — so the remaining reason to respin is to remove the
stubs and free the EZH permanently.

Two routing changes:

| Signal | Rev A | Rev B |
|---|---|---|
| Camera **D4** | `P3_4` | **`P1_8`** |
| Camera **D5** | `P3_5` | **`P1_9`** |
| **PCLK** | `P0_5` | **`P1_14`** |

Everything else stays. D0–D3 and D6–D7 are already correct, HSYNC on `P0_11` already
carries `FLEXIO0_D3`, and VSYNC on `P0_4` stays a GPIO interrupt.

What that buys:

- **Zero jumpers.** `P1_4..P1_11` becomes contiguous `FLEXIO0_D12..D19` *and* contiguous
  `SMARTDMA_PIO0..PIO7` by routing, so both backends are native.
- **PCLK routed properly** instead of hanging off a dupont wire — the one signal where
  stub length plausibly limits headroom.
- **The EZH is free** for other work, which is the entire point of the exercise.

Two things to settle before committing the layout:

1. **`P1_8` and MCU-Link.** `R173` (330 Ω) ties `P1_8` to the MCU-Link VCOM TX. It caused
   no trouble in testing — camera D4 drove the shared net cleanly — but on a respin the
   sensible move is to decide deliberately whether the VCOM stays connected to that pin at
   all, rather than inherit the contention.
2. **`P1_15` is the clean alternate** to `P1_14` for PCLK (`FLEXIO0_D23`) if layout
   prefers it. Avoid `P1_12` (drives Q2 on the shield) and `P1_13` (Ethernet PHY strap).

## 10. Open items

**Resolved by the bring-up:**

- ~~Confirm `J9_EXT` is accessible~~ — populated and used.
- ~~Confirm nothing else on the shield needs `P1_14`~~ — no conflict observed with the SPI
  panel selected.
- ~~Whether MCU-Link TX on `P1_8` disturbs D4~~ — **it does not.** R173 was never removed
  and capture is clean. Note the jumper makes `P3_4` and `P1_8` one net, so camera D4 was
  driving into the 330 Ω the whole time, including during the EZH runs.

**Still open:**
- Whether stub length limits usable PCLK **above** the current rate. It did not stop the
  group matching 23.4 fps, which was the bar. Untested beyond that, and untestable on
  stubs.
- **Port 4 FlexIO is build-verified but not hardware-verified** after the pin-group
  refactor. Rewiring eleven fly-wires to re-prove a path being moved away from was judged
  not worth the bench time. Relevant only if someone selects that group again.
- `ezh-freed-demo` was not performed. Capture running on FlexIO is necessary but not
  sufficient evidence that the EZH is claimable; that belongs with whatever work actually
  wants the EZH.

## 10a. What the freed EZH can reach

Moving capture to FlexIO frees `PIO0..PIO7` on `P1_4..P1_11` plus `PIO13` on `P1_17` —
a **contiguous 8-bit SmartDMA group**.

If you were hoping the now-unused `J12` header offers more, it does not:
[`AVC_J12_SmartDMA_Availability.md`](AVC_J12_SmartDMA_Availability.md) finds only six
distinct channels there, with `PIO26`/`PIO27` unreachable, so the widest contiguous run is
4 bits. **The pins the EZH already has are the best wide bus available on this board.**

## 11. Bring-up notes

Operational detail from the 2026-07-25 bring-up, kept because it is the part that would
otherwise be rediscovered painfully.

### Count the jumper slots twice

The first attempt failed with a distinctive signature: **VSYNC correct at 23.4 fps, eDMA
arming every frame, but all eight data bits reading zero and every shifter erroring.**
Cause was mundane — **the PCLK jumper was one slot over on `J9_EXT`.** Refitting it fixed
capture immediately. Nothing in firmware was wrong.

That signature is worth recognising: sync timing intact while data reads zero points at
PCLK, not at the data lines.

### The pin-mux readback earns its keep

`camera__configure_flexio_edma_pins()` reads the PCR MUX fields back at init and prints:

```
FlexIO camera pin mux readback: data(D7..D0)=0x66666666 pclk=6 href=6 vsync=0 OK
```

Expect alt6 on all eight data pins plus PCLK and HREF, alt0 on VSYNC. Anything else prints
`UNEXPECTED`.

This is what separated "firmware muxed the wrong pins" from "the wiring is wrong" during
the failure above — it said `OK` throughout, which correctly directed attention to the
bench rather than the code. **Check this line first** on any FlexIO capture problem.

### MCU-Link contention on `P1_8` is a non-issue in practice

Once jumper 1 is fitted, `P3_4` and `P1_8` are one net, so camera D4 drives into the
MCU-Link VCOM TX through `R173`'s 330 Ω continuously — including during EZH runs.
`R173` was never removed and capture is clean in both backends. The 330 Ω is doing its job
as series isolation.

### Two J-Link probes

Serials are not hardcoded anywhere; `scripts\tools\jlink_common.ps1` resolves an explicit
`-UsbSerial`, then `$env:AVC_JLINK_SERIAL`, then auto-detects when exactly one probe is
attached, and refuses to guess when several are. Relevant here because this bench had two
probes and one belonged to unrelated work.

### Build commands

```powershell
.uild_cmake.ps1                              # Rev A competition default, EZH
.uild_flexio_camera.ps1 -PinGroup Port1      # FlexIO on the Rev A camera wiring
.uild_flexio_camera.ps1 -PinGroup Port4      # original group, needs 11 fly-wires
```

## 12. Related

- [`FlexIO_Camera_Test_Plan.md`](FlexIO_Camera_Test_Plan.md) — prior FlexIO plan (Port 4 group)
- [`MCXN947/flexio_camera_io_pin_map.md`](MCXN947/flexio_camera_io_pin_map.md) — prior pin map
- [`MCXN947/flexio_pin_candidates/`](MCXN947/flexio_pin_candidates/) — earlier derived candidates
- [`AVC_Competition_Overview.md`](AVC_Competition_Overview.md) §4 — capture path context
- [`AVC_Vision_Pipeline_Design.md`](AVC_Vision_Pipeline_Design.md) — what the frame budget is for
