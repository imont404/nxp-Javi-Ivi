+++
type = "plan"
id = "alternate-240x320-display-bringup"
status = "pending"
created = "2026-07-10"

[[steps]]
id = "collateral-intake"
title = "Capture ER-TFT020-7 source material and initial AVC fit"
status = "done"

[[steps]]
id = "spi-pin-strategy"
title = "Map ER-TFT020-7 4-wire SPI mode onto the existing AVC LCD pins"
status = "done"
depends_on = ["collateral-intake"]

[[steps]]
id = "spi-bringup"
title = "Bring up the ER-TFT020-7 through the current ST7789 SPI display path"
status = "done"
depends_on = ["spi-pin-strategy"]

[[steps]]
id = "spi-validation"
title = "Validate orientation, color order, update rate, and camera/display fit in SPI mode"
status = "done"
depends_on = ["spi-bringup"]

[[steps]]
id = "parallel-ezh-strategy"
title = "Define the parallel MCU bus and EZH/bunny-build transfer strategy"
status = "pending"
depends_on = ["spi-validation"]

[[steps]]
id = "te-sync-evaluation"
title = "Evaluate the TE output as a display/camera timing signal"
status = "pending"
depends_on = ["spi-validation"]

[[steps]]
id = "revb-decision"
title = "Decide whether the alternate display path belongs on the Rev B shield"
status = "pending"
depends_on = ["parallel-ezh-strategy", "te-sync-evaluation"]

[[steps]]
id = "design-doc-intent-audit"
title = "Verify design docs match current user intent and implemented behavior before closeout"
status = "pending"

[[steps]]
id = "test-runtime-impact-audit"
title = "Review validation runtime impact and record any slower-lane decisions before closeout"
status = "pending"

[[steps]]
id = "external-review"
title = "Record independent review of the display bringup plan"
status = "pending"
depends_on = ["design-doc-intent-audit", "test-runtime-impact-audit"]

[[exit_criteria]]
id = "sources-cataloged"
title = "BuyDisplay product, datasheet, controller, connector, and demo-code sources are cataloged"
status = "met"

[[exit_criteria]]
id = "collateral-local"
title = "Required PDFs and demo code are available locally or their download blocker is resolved"
status = "met"

[[exit_criteria]]
id = "spi-pin-plan"
title = "SPI wiring, IM strap state, power, reset, backlight, and unused pins are documented before wiring"
status = "met"

[[exit_criteria]]
id = "spi-display-proof"
title = "ER-TFT020-7 shows a known RGB565 test image using the current AVC SPI display path"
status = "met"

[[exit_criteria]]
id = "avc-frame-fit"
title = "The current 320x240 LCD path and 320x200 camera view are validated on the alternate display"
status = "met"

[[exit_criteria]]
id = "parallel-bus-decision"
title = "The actual parallel mode is verified and the EZH/bunny transfer architecture is selected or rejected"
status = "pending"

[[exit_criteria]]
id = "te-decision"
title = "TE timing is measured and accepted or rejected as a useful synchronization signal"
status = "pending"

[[exit_criteria]]
id = "revb-display-decision"
title = "Rev B shield display recommendation is recorded with pin, timing, and firmware tradeoffs"
status = "pending"

[[exit_criteria]]
id = "design-doc-intent-audit"
title = "Design docs match current user intent and implemented behavior"
status = "pending"

[[exit_criteria]]
id = "test-runtime-impact-audit"
title = "New tests are listed and runtime impact is reviewed"
status = "pending"

[[exit_criteria]]
id = "external-review"
title = "Independent review is recorded"
status = "pending"
+++

# Alternate 240x320 Display Bringup

## Current Competition-Week Decision

Park this plan until after the competition unless the installed display blocks the race
algorithm. The Rev A competition image remains on the working ER-TFT020-3 SPI display;
the accepted 37.5 MHz configuration is the race-week baseline. Do not spend camera-
assembly time on the alternate parallel bus, EZH writer, TE evaluation, or Rev B choice.
The completed ER-TFT020-7 SPI proof and parallel research remain durable follow-up input.

## Purpose

Evaluate the BuyDisplay ER-TFT020-7 2 inch 240x320 IPS ST7789 display as an
alternate display for the AVC FRDM-MCXN947 shield work before the NXP Cup Rev B
hardware decision.

Start with the least invasive proof: run the new display in 4-wire SPI mode
using the same MCU-side LCD pins and the existing ST7789/LPSPI display path.
Only after the panel is proven should we invest in a new parallel-interface
EZH/bunny build. Also evaluate whether the panel TE output can help schedule
display updates or correlate display timing with camera VSYNC.

## Source Material

- Product page:
  `https://www.buydisplay.com/color-2-inch-240x320-ips-tft-lcd-display-mcu-8080-interface`
- Display datasheet:
  `https://www.buydisplay.com/download/manual/ER-TFT020-7_Datasheet.pdf`
- ST7789VW controller datasheet:
  `https://www.buydisplay.com/download/ic/ST7789.pdf`
- Capacitive touch controller datasheet:
  `https://www.buydisplay.com/download/ic/DS-CST816S_DS_V1.3.pdf`
- 22-pin FPC connector drawing:
  `https://www.buydisplay.com/download/connector/ER-CON22HT-1.pdf`
- 8051 demo-code link:
  `https://www.buydisplay.com/8051/ER-TFT020-7_8051_Tutorial.zip`
- 8051 dev-board page:
  `https://www.buydisplay.com/8051-microcontroller-development-board-for-tft-display-er-tft020-7`
- Dev-board user guide:
  `https://www.buydisplay.com/download/manual/ER-DBT020-7_UserGuide.pdf`
- Current AVC ST7789 SPI driver:
  `src/avc/avc_core0/source/avc_io/st7789.c`,
  `src/avc/avc_core0/source/avc_io/st7789.h`
- Current AVC LPSPI1 path:
  `src/avc/avc_core0/source/avc_io/lpspi1.c`
- Current LCD pin mux:
  `src/avc/avc_core0/board/pin_mux.c`
- Local research folder:
  `docs/research/ER-TFT020-7`

## Current Source Intake Status

The source catalog exists in `docs/research/ER-TFT020-7/source_manifest.md`.
Automated shell downloads from `buydisplay.com` were blocked by the site's
Cloudflare challenge on 2026-07-10, but the core collateral was later downloaded
through a browser and copied into `docs/research/ER-TFT020-7/downloads`.

Local core collateral now includes:

- `ER-TFT020-7_Datasheet.pdf`
- `ST7789.pdf`
- `DS-CST816S_DS_V1.3.pdf`
- `ER-CON22HT-1.pdf`
- `ER-TFT020-7_8051_Tutorial.zip`

The tutorial ZIP is extracted under
`docs/research/ER-TFT020-7/extracted/ER-TFT020-7_8051_Tutorial`. The older
`ST7789vw_existing_local_copy.pdf` remains as a reference copy from the repo's
pre-existing `docs/lcd/ST7789vw.pdf`.

The dev-board user guide and schematics are still useful supplementary material
for the later parallel/EZH work, but they are no longer blocking the first SPI
bringup.

## Working Strategy

### Phase 1: SPI proof on the existing display path

Keep the current AVC LCD architecture intact. The current code already drives
an ST7789-class display over LPSPI1 with a GPIO D/C line, a GPIO reset line,
EDMA-backed transfers, RGB565 data, and a landscape 320x240 logical display.
Use that as the first test path.

The early SPI bringup should answer only these questions:

- Does the ER-TFT020-7 power, reset, and respond in 4-wire SPI mode?
- Are the IM2..IM0 straps and any hidden/fixed IM3 state correct for 4-wire SPI?
- Does the current ST7789 initialization produce visible RGB565 color bars or
  a known synthetic frame?
- Are orientation, row/column offsets, color order, and byte order compatible
  with the current AVC LCD path?

Tentative MCU-side reuse map from the current firmware:

| Current AVC signal | MCXN947 pin from current pin mux | ER-TFT020-7 signal to verify |
| --- | --- | --- |
| LPSPI1 serial clock | `PIO0_24/FC1_P0` | FPC pin 17, `RS` used as serial clock in SPI |
| LPSPI1 serial data out | `PIO0_25/FC1_P1` | FPC pin 21, `SDA` |
| LPSPI1 PCS0/chip select path | `PIO0_27/FC1_P3` | FPC pin 18, `CS` |
| GPIO D/C (`LCD_RS`) | `PIO0_26` | FPC pin 16, `WR` used as D/C in 4-wire SPI |
| GPIO reset (`LCD_RST`) | `PIO0_28` | FPC pin 20, `RST` |
| Existing always-high output to confirm | `PIO1_12` | Possible LCD enable/backlight control, verify before use |
| New optional timing input | TBD | FPC pin 19, `TE` |

The ER-TFT020-7 pin names are easy to misread: in 4-wire serial mode the panel
datasheet says FPC pin 17 (`RS`) becomes the serial clock and FPC pin 16 (`WR`)
becomes the data/command select. Do not wire this from the signal names alone;
wire it from the verified SPI-mode functions.

For SPI mode, FPC pin 15 (`RD`) should be held inactive if it is not used, and
FPC pins 7..14 (`D7..D0`) are not part of the serial data path. The backlight is
specified as a separate `A/K` pair and should be current-limited according to
the module data.

The downloaded `ER-TFT020-7_Interfacing.pdf` reference schematic gives a more
specific 4-wire SPI setup for first bringup: `IM2=VDD`, `IM1=VDD`, `IM0=GND`,
FPC pins `D7..D0` tied low, FPC pin 15 `/RD` tied low in the serial reference,
FPC pin 16 `/WR(DC)` used as D/C, FPC pin 17 `DC(SCL)` used as SCK, FPC pin 18
as active-low CS, and FPC pin 21 as serial data.

### Phase 2: SPI validation with AVC data

After basic color bars work, use the existing eGFX display path and camera view
to validate the actual AVC layout:

- Full-screen 320x240 synthetic RGB565 pattern.
- Current AVC 320x200 camera view plus the 320x40 overlay/header region.
- Byte order and RGB/BGR behavior against the current `0x3A`, `0x36`, and
  `0xB0` initialization choices.
- Update rate and CPU/DMA cost compared with the current display.
- Visual artifact check before changing the camera/FlexIO work again.

### Phase 3: Parallel-interface research before EZH work

The product title says "MCU 8080 Parallel Interface", but the ER-TFT020-7
datasheet and 8051 dev-board guide both describe the exposed interface as
`3/4-wire SPI, 6800 8-bit Parallel`. The ST7789 controller datasheet contains
8080-series and serial interface tables, while the module-level collateral
appears to use 6800 wording. Resolve this before assigning Rev B pins or
writing a new EZH/bunny build.

The downloaded 8051 tutorial adds an important data point: its `ER-TFT020-7_8BIT`
example labels the display interface as `8080-8BIT` and the code drives an
8080-style `_WR` strobe with `_RD` available. Treat the module datasheet's
`6800 8-bit Parallel` wording as suspect until the interfacing PDF or schematic
is checked directly.

The interfacing schematic confirms the parallel reference is `8080 Series I`.

This phase is intentionally deferred. Do not spend implementation time on the
EZH/bunny display writer until the SPI path is proven and the project-specific
EZH/bunny build references are available.

The parallel phase should use the demo ZIP, dev-board user guide, and schematic
to determine:

- Which IM2..IM0 strap state the actual module expects.
- Whether FPC pins 15..18 behave as 6800 `RD/WR/RS/CS`, 8080 `RDX/WRX/DCX/CSX`,
  or a BuyDisplay naming variant.
- Do not carry the SPI-mode meaning of FPC pin 17 into the parallel pin map.
  FPC pin 17 is `RS`: it is used as serial clock in SPI mode, but in parallel
  mode it is the display data/command select. It should be treated as a
  required control signal for the 8080/EZH writer.
- Use the BuyDisplay `ER-TFT020-7_8BIT` sample as the first transaction model:
  put the byte on D0..D7, set `RS` low for command or high for data, assert
  `CS`, pulse `_WR` low then high, and deassert `CS` when the burst is done.
- Whether readback is needed or whether `RD` can remain tied inactive.
- Which MCXN947 pins should carry D0..D7, WR/E, D/C, CS, reset, backlight, and
  optional TE on Rev B.
- Whether the EZH program should generate GPIO strobes directly, use a timer or
  DMA assist, or remain unnecessary if SPI is already adequate.
- Before the EZH implementation, make a CM33 C bit-bang proof on the same
  proposed 8080 bus pins. This validates the panel straps, `RD`/`CS`
  treatment, `RS` polarity, `WR` polarity, byte order, and wiring while keeping
  EZH timing/tooling out of the first electrical test.

### Phase 4: TE timing evaluation

Do not make TE part of first light-up. Once SPI updates are stable, enable TE
in the ST7789 command set and route FPC pin 19 to a safe MCXN947 input. Start
with GPIO/interrupt timestamping, compare TE period and phase against camera
VSYNC and display transfers, then decide whether it is useful.

The likely first questions are:

- Does TE toggle at the expected frame cadence for the selected orientation and
  refresh setting?
- Can a frame transfer be scheduled immediately after TE without visible
  tearing?
- Is there any useful phase relationship between camera VSYNC and display TE?
- Would TE meaningfully help the Rev B hardware, or is it only a debug signal?

## Step Notes

### collateral-intake

- Created `docs/research/ER-TFT020-7`.
- Cataloged BuyDisplay product, datasheet, controller, touch, connector, demo
  ZIP, dev-board, user-guide, and schematic sources.
- Recorded the automated-download blocker in the plan log and source manifest.
- Copied the existing local ST7789 datasheet into the research downloads folder
  as a temporary local reference.
- Imported the browser-downloaded core collateral from
  `C:\Users\EliHughes\Downloads`, including the module datasheet, ST7789
  datasheet, touch-controller datasheet, connector drawing, and 8051 tutorial
  ZIP.
- Extracted the 8051 tutorial. It contains `3SPI`, `4SPI`, and `8BIT` Keil C51
  projects plus `ER-TFT020-7_Interfacing.pdf`.
- Rendered the one-page interfacing schematic as
  `docs/research/ER-TFT020-7/extracted/ER-TFT020-7_8051_Tutorial/ER-TFT020-7_Interfacing_page1.png`
  for quick visual inspection.
- The schematic confirms the first SPI target strap state:
  `IM2=VDD`, `IM1=VDD`, `IM0=GND`.

### spi-pin-strategy

- Confirm the current FRDM/shield LCD header pinout against the ER-TFT020-7 FPC
  and any adapter board before wiring.
- Use the vendor interfacing schematic's 4-wire SPI strap state:
  `IM2=VDD`, `IM1=VDD`, `IM0=GND`.
- Confirm whether tying `/RD` low as shown in the vendor serial reference is
  required, harmless, or simply a schematic convention before final shield
  routing.
- Confirm backlight current limiting and whether `PIO1_12` is already the
  intended LCD power/backlight enable.
- Pick a temporary MCXN947 input for TE that does not disturb the current camera
  FlexIO wiring.
- Completed the firmware-side pin strategy: the ER-TFT020-7 path deliberately
  reuses the existing LCD SPI, D/C, and reset pins. No pin mux or board I/O
  changes were made for this driver step.
- The current old display remains the default firmware panel:
  `CONFIG__DISPLAY_PANEL = DISPLAY_PANEL_ER_TFT020_3`.
- The alternate panel is selected with:
  `CONFIG__DISPLAY_PANEL = DISPLAY_PANEL_ER_TFT020_7`.
- TE defaults disabled in firmware with `CONFIG__DISPLAY_TE_ENABLE = 0`.

### spi-bringup

- Add a guarded display-test mode rather than changing the normal AVC boot path.
- Reuse the current `ST7789_Initial()` sequence first, then only adjust command
  parameters needed for the ER-TFT020-7 panel geometry.
- Use a deterministic RGB565 fill/pattern before routing live camera data.
- Keep all wiring observations in this plan log before each commit.
- First firmware slice is in place. `st7789.c` now dispatches the ST7789 init
  sequence based on `CONFIG__DISPLAY_PANEL`, with separate command tables for
  the old ER-TFT020-3 path and the alternate ER-TFT020-7 path.
- Both panel selections share the same LPSPI1 transfer path, GPIO D/C pin, GPIO
  reset pin, orientation helper, RGB565 pixel format, and existing AVC/eGFX
  frame transfer code.
- CMake verification passed for the default old panel and for a separate
  alternate-panel build using `-DCONFIG__DISPLAY_PANEL=2`.
- A guarded display-test mode is now available with
  `CONFIG__DISPLAY_TEST_MODE = 1`. It defaults off and, when enabled, runs
  before camera, FlexIO, ADC, motor, and servo init so the SPI display path can
  be tested alone.
- The display-test mode uses the selected `CONFIG__DISPLAY_PANEL`, draws a
  chunked full-screen RGB565 color-bar pattern with border/diagonal references,
  shifts the bars once per second, and emits RTT `display_test frame=` messages.
- CMake verification passed for the committed default configuration and for a
  separate ER-TFT020-7 display-test build using
  `-DCONFIG__DISPLAY_PANEL=2 -DCONFIG__DISPLAY_TEST_MODE=1`.
- The ER-TFT020-7 display-test image was flashed on 2026-07-11 using the
  separate CMake test build artifact. The next bench step is to probe the
  existing LCD SPI pins, power down, and jumper the ER-TFT020-7 breakout.
- The display is active on the ER-TFT020-7 after correcting the IM pin wiring
  interpretation. On 2026-07-12 the display-test image was simplified to three
  fixed RGB bands and reflashed for color/order validation.
- Bench validation confirmed the three-band test image appears red, green, and
  blue from left to right in physical landscape orientation. This satisfies the
  first SPI display proof.

### spi-validation

- Validate display address windows in both full-screen 320x240 and AVC
  320x200-camera plus 320x40-overlay modes.
- Check color channel order with a red/green/blue/white/black pattern.
- Check frame pacing and any visible tearing in SPI mode.
- Decide whether SPI is good enough for the immediate race work or only a
  low-risk stepping stone to the parallel path.
- On 2026-07-12 the normal AVC frame-path image was built and flashed with
  `CONFIG__DISPLAY_PANEL=2` and no display-test define. Bench observation of
  the normal AVC display path was successful.
- The SPI path is validated enough to unlock the parallel/EZH phase. Detailed
  SPI pacing/tearing observations remain useful comparison data but no longer
  block the parallel strategy work.

### parallel-ezh-strategy

- Parallel/EZH research is active as of 2026-07-12.
- Prefer the simplest write-only bus first: D0..D7, write strobe, D/C, CS,
  reset, backlight, and optional TE. Keep readback as a second-stage feature
  unless it is needed for panel identification.
- Compare available MCXN947 pin groups against camera pins already used for the
  FlexIO experiment and against Rev B shield routing constraints.
- AVC's vendored `src/common/bunny_build` is not current with the upstream
  `wavenumber-eng/bunny_build` `main` branch. The current upstream commit is
  `9c5e8d6`, which matches both `C:\eli\bunny_build` and the gibbon module
  copy under `D:\prj\teenage_engineering`.
- Updating bunny_build is likely the first implementation step, but it is not a
  blind copy. Upstream split instruction encoders out of `bunny_build.h` into
  `bunny_build__instr.*` and `bunny_build__psuedo_instr.*`; the AVC MCUXpresso
  and CMake source lists currently include only `bunny_build.c` and
  `ezh_init.c`.
- Upstream also added a generic `include/bunny_build__config.h`. AVC currently
  relies on the project-specific config in `source/avc_config`, so the update
  must avoid accidentally shadowing AVC's logging and target configuration.
- The gibbon PSRAM example demonstrates the bounded-command pattern we want for
  LCD work: build EZH programs into RAM, boot one command program at a time,
  write `EZH2ARM` to raise the ARM interrupt, then hold/stop the EZH.
- On 2026-07-12 the AVC bunny_build copy was updated to the upstream
  source-refactor layout while leaving the project-local
  `source/avc_config/bunny_build__config.h` in control. The scripted CMake
  source discovery was also changed to use durable `.cproject` source
  roots/options plus `.project` linked resources, not generated `Debug`
  makefiles. The generated `Debug` makefiles are transient MCUXpresso output and
  must not be used as source-of-truth for scripted builds.
- A clean CMake/Ninja rebuild passed after the bunny_build update. The build
  compiled the split upstream files `bunny_build__instr.c` and
  `bunny_build__psuedo_instr.c`.
- FPC pin 17 `RS` must be part of the parallel-control strategy. It is only the
  serial clock in SPI mode; in parallel mode it is the display data/command
  select. For the bounded EZH command proof, either EZH must drive this line
  directly, or ARM must set it around separate command/data EZH bursts with a
  strict ownership rule.
- The first EZH 8080 writer must be explicitly paced. The ST7789 8080 timing
  table gives a 66 ns minimum write cycle, 15 ns minimum `WRX` high/low pulse
  widths, and 10 ns data setup/hold. At a 150 MHz EZH instruction rate, one
  instruction is about 6.67 ns, so a tight GPIO-write sequence can violate the
  controller timing. Use conservative instruction-counted padding or the
  `E_HEART_RYTHM_IMM` plus `E_WAIT_FOR_BEAT()` mechanism for the first scoped
  proof, then tighten only after the `WR` waveform is measured.
- The BuyDisplay `8BIT` sample's write routines are the direct behavioral
  reference for the bounded EZH proof: command writes set `RS=0`, data writes
  set `RS=1`, both write one byte to the data bus before pulsing `_WR`, and
  RGB565 pixel data is sent high byte first then low byte.
- The first I/O map is recorded in
  `docs/research/ER-TFT020-7/parallel_ezh_io_map.md`. D0-D7 should reuse the
  original shared camera/LCD shield nets on `SMARTDMA_PIO0..7`:
  `P1_4`, `P1_5`, `P1_6`, `P1_7`, `P3_4`, `P3_5`, `P1_10`, and `P1_11`.
  These are distinct from the current FlexIO camera validation wiring on
  `P4_12..P4_19`.
- Tentative control reuse is `WR=P0_4`, `RS=P0_11`, and `RST=P1_19`.
  `P1_19` is already the shared camera reset / LCD D15 net and has
  `SMARTDMA_PIO15` available. The old sync-net labels need one confirmation
  before Rev B routing: the netlist says `P0_11` is old HSYNC and `P0_4` is old
  VSYNC, while the latest proposed text used the opposite names for the
  pin-exact `WR`/`RS` assignment. Also decide whether to use existing
  design-file LCD controls `P2_0/EZH_LCD_WR`, `P1_22/EZH_LCD_DC`, and
  `P4_4/EZH_LCD_CS` as alternatives or keep them out of this scoped proof.
- The next implementation slice should be a CM33 C bit-bang display proof, not
  the EZH port yet. Use the same candidate pins and the vendor 8-bit write
  order, draw a known RGB565 pattern, and scope the control/data lines. Once
  that works, port the known-good byte-write sequence into a bounded
  EZH/bunny program with explicit `WR` pacing and `EZH2ARM` completion.
- On 2026-07-12 the CM33 C bit-bang proof was added behind
  `CONFIG__DISPLAY_PARALLEL_BITBANG_TEST_MODE`. It uses D0-D7 on
  `P1_4`, `P1_5`, `P1_6`, `P1_7`, `P3_4`, `P3_5`, `P1_10`, and `P1_11`;
  `WR=P0_4`; `RS=P0_11`; `CS=P4_4`; and `RST=P1_19`. `RD` is not MCU-driven
  in this slice and should be held externally inactive, expected high for 8080
  `RDX`.
- The CM33 proof duplicates the BuyDisplay 8-bit write behavior: command
  bytes with `RS=0`, data bytes with `RS=1`, one active-low `WR` pulse per
  byte, and RGB565 pixel data high byte then low byte. It draws fixed red,
  green, and blue vertical bands and emits `parallel_lcd frame=` RTT messages.
- Verification passed for the default CMake build with the proof disabled and
  for the separate test image in
  `build/cmake/avc_core0-Debug-parallel-bitbang` using
  `-DCONFIG__DISPLAY_PANEL=2 -DCONFIG__DISPLAY_TEST_MODE=1
  -DCONFIG__DISPLAY_PARALLEL_BITBANG_TEST_MODE=1`.
- On 2026-07-12 the parallel-bitbang image was flashed with SEGGER J-Link V9.54
  and started. The BuyDisplay interfacing schematic shows the 8080 Series I IM
  straps as `IM2=GND`, `IM1=GND`, and `IM0=GND`.

### te-sync-evaluation

- Start by timestamping TE with GPIO and the existing OSTIMER approach used for
  camera timing.
- If the GPIO ISR is sufficient, keep it simple.
- Only consider hardware event/DMA timestamping if ISR jitter prevents a useful
  camera/display phase measurement.

## Deferred Next Steps

1. Power down and strap the display for 8080 Series I:
   `IM2=GND`, `IM1=GND`, and `IM0=GND`.
2. Wire the 8080 proof bus with `RD` held inactive high:
   D0-D7 on the shared camera/LCD data pins, `WR=P0_4`, `RS=P0_11`,
   `CS=P4_4`, and `RST=P1_19`.
3. Flash `build/cmake/avc_core0-Debug-parallel-bitbang/avc_core0.axf`, then
   scope `WR`, `RS`, `CS`, reset, and at least one data line before judging the
   display image.
4. Confirm the expected fixed red/green/blue vertical bands and log any
   bit-order, byte-order, or control-polarity corrections.
5. Port the known-good byte-write sequence to the smallest LCD EZH command
   proof with conservative `WR` pacing:
   write one command/data burst, signal completion through `EZH2ARM`, and
   stop/hold.
6. Raise the parallel write rate only after the scoped CM33 and EZH waveforms
   satisfy the ST7789 timing margins.

## Open Questions

- Is the purchased module no-touch or capacitive-touch, and does it include the
  touch FPC/controller we need to account for mechanically?
- What is the exact IM2..IM0 strap configuration for 4-wire SPI on this module?
- Is the module's parallel path actually 6800-style, 8080-style, or a naming
  mismatch between product title and datasheet?
- Which existing shield pin drives LCD backlight or panel enable today?
- Where should TE land for a clean Rev B route if it proves useful?
- Should AVC keep using its project-local `bunny_build__config.h`, or should
  that configuration move into the vendored bunny_build layout with explicit
  AVC overrides?
