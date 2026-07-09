# FlexIO Camera I/O Pin Map

This file is the working I/O record for the FRDM-MCXN947 AVC FlexIO camera
bring-up. Update it whenever a camera wire, mux function, electrical treatment,
or diagnostic role changes.

## Active Camera Configuration

| Item | Value | Source/status |
| --- | --- | --- |
| Camera | OV5640 | Active firmware config |
| Capture format | RGB565 | Active firmware config |
| Active camera view | 320x200 | `CONFIG__CAMERA_RESOLUTION` |
| Frame rate target | 30 fps | OV5640 init config |
| Display layout | 320x40 overlay plus 320x200 camera view | Existing LCD path |
| Current backend | `FLEXIO_DIAG` | Diagnostic build on board |

The display is 320x240, but the active camera image is 320x200. The current
RTT diagnostic reports `p4_lines=200`, which matches this active image height.

## Current Wiring And Diagnostic Roles

| Camera signal | Current MCU pin | MCU function now | Electrical/wiring status | Bring-up status |
| --- | --- | --- | --- | --- |
| SCCB/I2C SDA | `P3_2` | LP_FLEXCOMM7 data | Existing wiring unchanged | Keep |
| SCCB/I2C SCL | `P3_3` | LP_FLEXCOMM7 clock | Existing wiring unchanged | Keep |
| XCLK/CLKIN | `P2_2` | CLKOUT | Existing wiring unchanged | Keep |
| RESET | `P1_19` | GPIO output | Existing wiring unchanged | Keep |
| PWDN | `P1_18` | GPIO output | Existing wiring unchanged | Keep |
| PCLK | `P4_20` | GPIO flag now; `FLEXIO0_D28` candidate | Shortened jumper with 330 ohm pulldown/termination | Validated edge activity |
| HSYNC/HREF | `P4_21` | GPIO IRQ now; `FLEXIO0_D29` candidate | Shortened jumper with 330 ohm pulldown/termination | Validated about 6040 edges/sec |
| VSYNC | `P4_22` | GPIO IRQ now; `FLEXIO0_D30` candidate | Shortened jumper with 330 ohm pulldown/termination | Validated about 30-31 edges/sec |
| D0 | `P1_4` | Existing SmartDMA/EZH data wiring | Existing wiring unchanged | Planned move to `P4_12` |
| D1 | `P1_5` | Existing SmartDMA/EZH data wiring | Existing wiring unchanged | Planned move to `P4_13` |
| D2 | `P1_6` | Existing SmartDMA/EZH data wiring | Existing wiring unchanged | Planned move to `P4_14` |
| D3 | `P1_7` | Existing SmartDMA/EZH data wiring | Existing wiring unchanged | Planned move to `P4_15` |
| D4 | `P3_4` | Existing SmartDMA/EZH data wiring | Existing wiring unchanged | Planned move to `P4_16` |
| D5 | `P3_5` | Existing SmartDMA/EZH data wiring | Existing wiring unchanged | Planned move to `P4_17` |
| D6 | `P1_10` | Existing SmartDMA/EZH data wiring | Existing wiring unchanged | Planned move to `P4_18` |
| D7 | `P1_11` | Existing SmartDMA/EZH data wiring | Existing wiring unchanged | Planned move to `P4_19` |
| Spare/debug | `P4_23` | `FLEXIO0_D31` candidate | Unused | Keep available |

The original sync pins `P0_5` PCLK, `P0_11` HSYNC/HREF, and `P0_4` VSYNC are
currently reference-only diagnostic inputs. They remained static in the latest
RTT captures after the active sync signals were routed to Port 4.

## Planned FlexIO Data Wiring

| Camera data signal | Planned MCU pin | FlexIO signal | FRDM FlexIO header pin |
| --- | --- | --- | --- |
| D0 | `P4_12` | `FLEXIO0_D20` | 17 |
| D1 | `P4_13` | `FLEXIO0_D21` | 18 |
| D2 | `P4_14` | `FLEXIO0_D22` | 19 |
| D3 | `P4_15` | `FLEXIO0_D23` | 20 |
| D4 | `P4_16` | `FLEXIO0_D24` | 21 |
| D5 | `P4_17` | `FLEXIO0_D25` | 22 |
| D6 | `P4_18` | `FLEXIO0_D26` | 23 |
| D7 | `P4_19` | `FLEXIO0_D27` | 24 |
| PCLK | `P4_20` | `FLEXIO0_D28` | 25 |
| HSYNC/HREF | `P4_21` | `FLEXIO0_D29` | 26 |
| VSYNC | `P4_22` | `FLEXIO0_D30` | 27 |
| Spare/debug | `P4_23` | `FLEXIO0_D31` | 28 |

For the first FlexIO byte-capture implementation, assume the shifter data start
index is `20` for the eight camera data bits on `FLEXIO0_D20..D27`. Keep
PCLK, HSYNC/HREF, and VSYNC available to FlexIO only if hardware timing or
gating requires it; otherwise they can remain GPIO diagnostic/control inputs.

## Current Incremental Data-Bus Test State

The noisy full data-bus pass was backed out for a smaller test. The sync lines
were refitted as `PCLK -> P4_20`, `HSYNC/HREF -> P4_21`, and `VSYNC -> P4_22`.
All data lines were disconnected, then only camera `D0` and `D1` were attached
to their planned FlexIO pins:

| Camera data signal | Current test MCU pin | Status |
| --- | --- | --- |
| D0 | `P4_12` / `FLEXIO0_D20` | Attached for current test |
| D1 | `P4_13` / `FLEXIO0_D21` | Attached for current test |
| D2-D7 | `P4_14..P4_19` / `FLEXIO0_D22..D27` | Disconnected for current test |

Data-line termination remains deferred. The existing low-value
termination/pulldown treatment remains on the sync lines.

## Change History

| Date/time | Change | Result |
| --- | --- | --- |
| 2026-07-09 09:34 EDT | Prepared first Port 4 sync wiring handoff. | Planned PCLK to `P4_20`, HSYNC/HREF to `P4_21`, VSYNC to `P4_22`. |
| 2026-07-09 10:57 EDT | Wired PCLK, HSYNC/HREF, and VSYNC to Port 4. | First diagnostic showed stable HSYNC and PCLK activity, but noisy VSYNC IRQ counts. |
| 2026-07-09 11:44 EDT | Shortened Port 4 sync jumpers and added 330 ohm pulldown/termination to VSYNC. | P4 sync wiring now matches termination treatment across the three new signals. |
| 2026-07-09 11:48 EDT | Flashed guarded VSYNC IRQ diagnostic. | HSYNC about 6040 edges/sec, VSYNC about 30-31 edges/sec, `p4_lines=200`, PCLK flag present. |
| 2026-07-09 11:52 EDT | Confirmed active OV5640 geometry is 320x200. | `p4_lines=200` is consistent with firmware and LCD layout. |
| 2026-07-09 11:57 EDT | Planned first data-bus wiring pass without data-line terminators. | Use visual/capture quality to decide whether data-line termination is needed later. |
| 2026-07-09 12:07 EDT | Ran first RTT sync check after data-bus wiring. | HSYNC and PCLK remain active, but VSYNC now reports about 59-79 edges/sec with unstable `p4_lines`, so inspect `P4_22`/VSYNC wiring before data-bit sampling. |
| 2026-07-09 12:09 EDT | Re-ran RTT sync check after wiring re-check request. | VSYNC is worse: first interval exceeded the 120 edges/sec guard, `p4_vs_off=1`, and `p4_lines` remains unstable. |
| 2026-07-09 13:57 EDT | Backed out full data-bus wiring, refitted sync, and attached only D0/D1. | Ready to rerun sync diagnostic before adding more data wires. |
| 2026-07-09 14:50 EDT | Reran RTT with SEGGER J-Link V9.54 after V9.40 path disappeared. | Probe connected, but VSYNC was extremely noisy at about 3100-3300 extra edges/sec after the first interval, with `p4_vs_off=1`. |
| 2026-07-09 14:52 EDT | Reflashed current CMake image with J-Link V9.54 and reran RTT. | Flash matched existing image; VSYNC still shows about 3000-3335 extra edges/sec, `p4_vs_off=1`, while HSYNC/PCLK remain active. |
