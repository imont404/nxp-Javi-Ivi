+++
type = "plan"
id = "alternate-240x320-display-bringup"
status = "active"
created = "2026-07-10"

[[steps]]
id = "collateral-intake"
title = "Capture ER-TFT020-7 source material and initial AVC fit"
status = "done"

[[steps]]
id = "spi-pin-strategy"
title = "Map ER-TFT020-7 4-wire SPI mode onto the existing AVC LCD pins"
status = "pending"
depends_on = ["collateral-intake"]

[[steps]]
id = "spi-bringup"
title = "Bring up the ER-TFT020-7 through the current ST7789 SPI display path"
status = "pending"
depends_on = ["spi-pin-strategy"]

[[steps]]
id = "spi-validation"
title = "Validate orientation, color order, update rate, and camera/display fit in SPI mode"
status = "pending"
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
status = "pending"

[[exit_criteria]]
id = "spi-display-proof"
title = "ER-TFT020-7 shows a known RGB565 test image using the current AVC SPI display path"
status = "pending"

[[exit_criteria]]
id = "avc-frame-fit"
title = "The current 320x240 LCD path and 320x200 camera view are validated on the alternate display"
status = "pending"

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
+++

# Alternate 240x320 Display Bringup

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
- Whether readback is needed or whether `RD` can remain tied inactive.
- Which MCXN947 pins should carry D0..D7, WR/E, D/C, CS, reset, backlight, and
  optional TE on Rev B.
- Whether the EZH program should generate GPIO strobes directly, use a timer or
  DMA assist, or remain unnecessary if SPI is already adequate.

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

### spi-bringup

- Add a guarded display-test mode rather than changing the normal AVC boot path.
- Reuse the current `ST7789_Initial()` sequence first, then only adjust command
  parameters needed for the ER-TFT020-7 panel geometry.
- Use a deterministic RGB565 fill/pattern before routing live camera data.
- Keep all wiring observations in this plan log before each commit.

### spi-validation

- Validate display address windows in both full-screen 320x240 and AVC
  320x200-camera plus 320x40-overlay modes.
- Check color channel order with a red/green/blue/white/black pattern.
- Check frame pacing and any visible tearing in SPI mode.
- Decide whether SPI is good enough for the immediate race work or only a
  low-risk stepping stone to the parallel path.

### parallel-ezh-strategy

- Do not start bunny/EZH implementation until the module-level bus timing is
  verified.
- Project-specific EZH/bunny build documentation will be supplied when this
  phase becomes active; keep the current work focused on SPI bringup.
- Prefer the simplest write-only bus first: D0..D7, write strobe, D/C, CS,
  reset, backlight, and optional TE. Keep readback as a second-stage feature
  unless it is needed for panel identification.
- Compare available MCXN947 pin groups against camera pins already used for the
  FlexIO experiment and against Rev B shield routing constraints.

### te-sync-evaluation

- Start by timestamping TE with GPIO and the existing OSTIMER approach used for
  camera timing.
- If the GPIO ISR is sufficient, keep it simple.
- Only consider hardware event/DMA timestamping if ISR jitter prevents a useful
  camera/display phase measurement.

## Immediate Next Steps

1. Inspect `ER-TFT020-7_Interfacing.pdf` and the `4SPI` example for IM strap
   requirements, command sequence, and signal naming.
2. Confirm the SPI wiring map and backlight/power details before moving wires.
3. Add a small guarded SPI display-test mode using the current ST7789 driver.
4. Flash and verify a synthetic RGB565 pattern before enabling live AVC frames.
5. Leave EZH/bunny work parked until the SPI proof is complete and the proper
   project-specific EZH/bunny references are supplied.

## Open Questions

- Is the purchased module no-touch or capacitive-touch, and does it include the
  touch FPC/controller we need to account for mechanically?
- What is the exact IM2..IM0 strap configuration for 4-wire SPI on this module?
- Is the module's parallel path actually 6800-style, 8080-style, or a naming
  mismatch between product title and datasheet?
- Which existing shield pin drives LCD backlight or panel enable today?
- Where should TE land for a clean Rev B route if it proves useful?
