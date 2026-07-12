# ER-TFT020-7 Parallel EZH I/O Map

This is the working I/O map for the ER-TFT020-7 8080-style parallel LCD proof
on the FRDM-MCXN947 AVC hardware.

## Proposed Direction

Reuse the original camera/EZH shield data signals as the LCD 8-bit data bus.
Those signals are already named as shared LCD/camera nets in the FRDM design
files and map cleanly to `SMARTDMA_PIO0..7`, which is the right shape for an
EZH byte-wide bus.

This map is for the parallel LCD experiment. Do not confuse it with the current
FlexIO camera validation wiring, which moved the live camera data bus to
`P4_12..P4_19`.

## Evidence Checked

This map is based on the FRDM design-file netlist, current `pin_mux.c`, and the
NXP Monkey pin captures under
`docs/research/MCXN947/flexio_pin_candidates`:

- `ezh_camera_header.json` confirms the original camera-header signals.
- `flexio0_pins.json` confirms the `SMARTDMA_PIO` functions.
- The imported schematic JSON still uses the older `EZH_PIO` name for the same
  SmartDMA pin functions.

The proposed data pins, old `HSYNC`, old `VSYNC`, and `RESET` are all existing
camera-related shield signals. `P1_19` is not a new reset-only choice; it is
already named `EZH_LCD_D15_CAMERA_RST` in the design files and has
`SMARTDMA_PIO15` available if needed later.

## Data Bus

| LCD signal | MCU pin | SmartDMA role | Existing design-file net |
| --- | --- | --- | --- |
| D0 | `P1_4` | `SMARTDMA_PIO0` | `EZH_LCD_D0_CAMERA_D0` |
| D1 | `P1_5` | `SMARTDMA_PIO1` | `EZH_LCD_D1_CAMERA_D1` |
| D2 | `P1_6` | `SMARTDMA_PIO2` | `EZH_LCD_D2_CAMERA_D2` |
| D3 | `P1_7` | `SMARTDMA_PIO3` | `EZH_LCD_D3_CAMERA_D3` |
| D4 | `P3_4` | `SMARTDMA_PIO4` | `EZH_LCD_D4_CAMERA_D4` |
| D5 | `P3_5` | `SMARTDMA_PIO5` | `EZH_LCD_D5_CAMERA_D5` |
| D6 | `P1_10` | `SMARTDMA_PIO6` | `EZH_LCD_D6_CAMERA_D6` |
| D7 | `P1_11` | `SMARTDMA_PIO7` | `EZH_LCD_D7_CAMERA_D7` |

The first EZH write proof should set `GPD[7:0]` as outputs and drive pixel or
command bytes through the low byte of `GPO`. The old camera EZH code cleared
these direction bits for input capture; LCD output needs the opposite
direction.

## Control Signals

User-proposed pin-exact control reuse:

| LCD signal | Intended reused net | Candidate MCU pin | Status |
| --- | --- | --- | --- |
| `WR` | old camera sync net | `P0_4` | Proposed |
| `RS` / D-C | old camera sync net | `P0_11` | Proposed |
| `RST` | old camera reset | `P1_19` | Confirmed shared camera/LCD net |

Important conflict: the design-file netlist and current firmware name `P0_11`
as `EZH_CAMERA_HSYNC` and `P0_4` as `EZH_CAMERA_VSYNC`. The latest proposed
text used the pin-exact assignment `WR=P0_4` and `RS=P0_11`, while also
describing those as `WR=HSYNC` and `RS=VSYNC`. Those labels are swapped relative
to the checked net names. Before Rev B routing, decide whether the real intent
is:

- `WR` on old HSYNC, therefore `P0_11`, and `RS` on old VSYNC, therefore
  `P0_4`; or
- `WR` on `P0_4` and `RS` on `P0_11`, regardless of the old camera net names.

`P0_4` and `P0_11` are SmartDMA trigger sources, and SmartDMA can access GPIO
control/data registers through AHB. However, unlike D0-D7, they are not the
clean low-byte `SMARTDMA_PIO0..7` data lane. Treat their output timing as a
measurement item before depending on a fast `WR` strobe.

## Existing Design-File LCD Controls

The FRDM design-file netlist also contains older LCD-specific control names:

| Design-file net | MCU pin | Possible LCD role |
| --- | --- | --- |
| `EZH_LCD_WR` | `P2_0` / `SMARTDMA_PIO20` | 8080 write strobe |
| `EZH_LCD_DC` | `P1_22` / `SMARTDMA_PIO18` | data/command select |
| `EZH_LCD_CS` | `P4_4` / `SMARTDMA_PIO28` | chip select |

These may be useful alternatives if the reused camera sync pins are awkward for
EZH output timing or Rev B routing.

## Recommended Baby Step

Implement the first 8080 write proof as CM33 C code that bit-bangs the same
candidate pins with conservative delays and a fixed RGB565 test pattern. That
will validate the panel mode straps, `RD`/`CS` treatment, `RS` polarity, `WR`
polarity, byte order, and wiring before any EZH timing/tooling issues are
introduced.

After the C proof works and the `WR` waveform is scoped, port the same
transaction order to a bounded EZH/bunny program: command/data bytes in,
explicitly paced `WR` pulses out, `EZH2ARM` completion interrupt, then halt.

## Still Open

- `CS`: assign a pin or decide to hold active for scoped test bursts.
- `RD`: hold inactive for write-only operation, likely high for 8080 `RDX`.
- `TE`: optional timing/debug input after basic parallel writes work.
- Backlight and panel power/enable: keep outside the first EZH proof unless a
  conflict is found.
- IM straps: keep the verified module strap state for 8080 parallel mode before
  connecting the bus.
