# ER-TFT020-7 Research Notes

## Target

BuyDisplay/EastRising ER-TFT020-7, a 2 inch IPS TFT display panel with 240x320
active pixels and an ST7789 controller. This is being evaluated as an alternate
display for the AVC FRDM-MCXN947 project.

## Local Collateral Status

Automated downloads from `buydisplay.com` were attempted on 2026-07-10 and were
blocked by Cloudflare/HTTP 403 responses. The exact URLs are recorded in
`source_manifest.md`.

The core product-page files were later downloaded through a browser to
`C:\Users\EliHughes\Downloads`, copied into `downloads`, and the tutorial ZIP
was extracted into `extracted`.

Current local files:

- `downloads/ER-TFT020-7_Datasheet.pdf`
- `downloads/ST7789.pdf`
- `downloads/DS-CST816S_DS_V1.3.pdf`
- `downloads/ER-CON22HT-1.pdf`
- `downloads/ER-TFT020-7_8051_Tutorial.zip`
- `downloads/ST7789vw_existing_local_copy.pdf`, copied from the existing repo
  file `docs/lcd/ST7789vw.pdf` before the fresh download was available.
- `extracted/ER-TFT020-7_8051_Tutorial`, extracted from the 8051 tutorial ZIP.
- `extracted/ER-TFT020-7_8051_Tutorial/ER-TFT020-7_Interfacing_page1.png`,
  a rendered view of the one-page interfacing schematic for quick inspection.

## Datasheet Snapshot

From the BuyDisplay product page and ER-TFT020-7 datasheet:

- Part: `ER-TFT020-7`.
- Resolution: 240x320 pixels.
- Display type: 2 inch IPS TFT.
- Controller: ST7789.
- Interface listed by the display datasheet: `3/4-wire SPI, 6800 8-bit
  Parallel`.
- Product title lists `MCU 8080 Parallel Interface`; this conflicts with the
  module datasheet wording and must be resolved before EZH/parallel work.
- FPC: 22 pins, 0.50 mm pitch.
- Logic/panel VCC: 2.8 to 3.3 V at the module-level pins.
- Backlight: separate `A/K` pins, about 3.0 V typical and 45 to 60 mA in the
  module datasheet.

Important 22-pin LCD FPC functions:

| FPC pin | Name | Notes |
| --- | --- | --- |
| 1 | A | Backlight anode |
| 2 | K | Backlight cathode |
| 3..5 | IM2..IM0 | Interface-mode select |
| 6 | VCC | 2.8 to 3.3 V I/O/panel supply |
| 7..14 | D7..D0 | Parallel data bus |
| 15 | RD | Read enable in MCU parallel mode |
| 16 | WR | Write strobe in parallel mode; D/C select in 4-wire serial |
| 17 | RS | D/C select in parallel mode; serial clock in SPI mode |
| 18 | CS | Chip select |
| 19 | TE | Tearing-effect output |
| 20 | RST | Reset input |
| 21 | SDA | Serial data input/output |
| 22 | GND | Ground |

## ST7789 Interface Notes

The ST7789 serial-interface table lists 4-wire serial interface I at
`IM[3:0] = 0110` and 3-wire serial interface I at `IM[3:0] = 0101`. The module
FPC exposes only IM2..IM0, so the effective IM3 state must be verified from the
module, adapter board, or demo-code collateral before hardware is wired.

In 4-wire serial mode, ST7789 uses:

- `CSX` as chip select.
- `D/CX` as data/command flag.
- `SCL` as serial clock.
- `SDA` as serial data.
- MSB-first byte transfers.

The ER-TFT020-7 FPC naming maps this awkwardly: FPC pin 17 (`RS`) is the serial
clock in SPI mode, and FPC pin 16 (`WR`) is the D/C select in 4-wire SPI mode.

The ST7789 datasheet also documents TE commands:

- `34h`: tearing-effect output off.
- `35h`: tearing-effect output on.
- `44h`: set tear scanline.

The TE line should be treated as an optional timing/debug input until basic SPI
display operation is stable.

## Current AVC Firmware Fit

The existing AVC display path already uses an ST7789-style SPI driver:

- `src/avc/avc_core0/source/avc_io/st7789.c`
- `src/avc/avc_core0/source/avc_io/st7789.h`
- `src/avc/avc_core0/source/avc_io/lpspi1.c`
- `src/avc/avc_core0/source/avc_io/eGFX_Driver_ER-TFT020-3.c`

Current driver traits:

- LPSPI1 with EDMA-backed transfers.
- 8-bit command mode and 32-bit image-transfer mode.
- GPIO D/C on `PIO0_26`.
- GPIO reset on `PIO0_28`.
- Landscape 320x240 address window.
- RGB565 pixel mode (`0x3A = 0x05`).
- Little-endian control through the current ST7789 `0xB0` initialization.

Tentative SPI reuse map for planning:

| Existing firmware resource | Current pin | ER-TFT020-7 target |
| --- | --- | --- |
| LPSPI1 clock path | `PIO0_24/FC1_P0` | FPC 17 `RS` as SPI clock |
| LPSPI1 data-out path | `PIO0_25/FC1_P1` | FPC 21 `SDA` |
| LPSPI1 PCS0 path | `PIO0_27/FC1_P3` | FPC 18 `CS` |
| LCD D/C GPIO | `PIO0_26` | FPC 16 `WR` as SPI D/C |
| LCD reset GPIO | `PIO0_28` | FPC 20 `RST` |
| Output initialized high | `PIO1_12` | Verify before using for LCD power/backlight |
| Optional TE input | TBD | FPC 19 `TE` |

This map is not yet a wiring instruction. Confirm the FRDM/shield header route,
the existing display cable/adapter, and the ER-TFT020-7 IM strap state first.

## Downloaded Example Findings

The extracted 8051 tutorial includes separate projects:

- `ER-TFT020-7_3SPI`
- `ER-TFT020-7_4SPI`
- `ER-TFT020-7_8BIT`
- `ER-TFT020-7_Interfacing.pdf`

The `4SPI` example confirms the expected four display control signals in the
driver code:

- `CS`: chip select, active low.
- `RS`: data/command select, high for data and low for command.
- `SCK`: serial clock.
- `SDI`: serial data.
- `RES`: reset.

The `ER-TFT020-7_Interfacing.pdf` schematic confirms the reference 4-wire SPI
strap and signal map:

| ER-TFT020-7 signal | 4-wire SPI reference connection |
| --- | --- |
| `IM2` | `VDD3.3V` |
| `IM1` | `VDD3.3V` |
| `IM0` | `GND` |
| `D7..D0` | Tied low in the reference schematic |
| `/RD` | Tied low in the reference schematic |
| `/WR(DC)` | Data/command select |
| `DC(SCL)` | Serial clock |
| `CS` | Active-low chip select |
| `SDA` | Serial data input/output |
| `RST` | Reset |
| `TE` | Left available as tearing-effect output |

The `4SPI` reference init sequence uses `0x35` with parameter `0x00`, so the
vendor example enables TE by default. It also uses `0x36 = 0x00`,
`0x3A = 0x05`, and `0x21` before `0x29`.

The `8BIT` example is important for the later EZH path: it labels the interface
as `8080-8BIT`, uses an `_WR` write strobe, exposes `_RD`, and writes command
or data bytes on an 8-bit data bus. The interfacing schematic labels this as
`8080 Series I`. That conflicts with the module datasheet's `6800 8-bit
Parallel` wording, so the parallel bus should be treated as likely 8080-style
until final shield routing.

## Initial Test Strategy

1. Confirm the exact module and adapter hardware, including touch/no-touch and
   backlight wiring.
2. Use `IM2=VDD`, `IM1=VDD`, and `IM0=GND` for 4-wire SPI.
3. Wire only power, backlight, reset, CS, clock, data, D/C, and ground for the
   first SPI test. Leave D0..D7 and TE disconnected unless needed.
4. Add a guarded firmware test mode that drives a known RGB565 pattern through
   the current ST7789 path.
5. Validate orientation, offsets, color order, and byte order.
6. Route the current AVC frame path only after the synthetic pattern is clean.
7. After SPI is stable, wire TE to a spare input and timestamp it.
8. Use the demo ZIP and schematics to decide whether the parallel path is 6800,
   8080, or a naming mismatch before assigning EZH pins.

## Risks

- The product title and datasheet disagree about 8080 vs 6800 parallel wording.
- The display FPC pin names change meaning between parallel and SPI modes.
- The IM strap state cannot be inferred safely from the three exposed IM pins
  without understanding the module's hidden/fixed IM3 state.
- Direct downloads are currently blocked from the command line, so the demo ZIP
  and module PDF need a manual or alternate import step.
- SPI may prove the panel but not meet the desired display-update timing; the
  parallel path still needs a separate architecture decision.
