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
| Current backend | `FLEXIO_EDMA` | Active build on board |

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
| PCLK | `P4_20` | `FLEXIO0_D28` timer clock | Shortened jumper with 330 ohm pulldown/termination | FlexIO/eDMA capture clocking |
| HSYNC/HREF | `P4_21` | `FLEXIO0_D29` HREF gate | Shortened jumper with 330 ohm pulldown/termination | FlexIO/eDMA capture gating |
| VSYNC | `P4_22` | GPIO IRQ frame-start trigger | Shortened jumper with 330 ohm pulldown/termination | Validated about 30 frames/sec |
| D0 | `P4_12` | `FLEXIO0_D20` | Camera data attached | Live image data present; ordering under review |
| D1 | `P4_13` | `FLEXIO0_D21` | Camera data attached | Live image data present; ordering under review |
| D2 | `P4_14` | `FLEXIO0_D22` | Camera data attached | Live image data present; ordering under review |
| D3 | `P4_15` | `FLEXIO0_D23` | Camera data attached | Live image data present; ordering under review |
| D4 | `P4_16` | `FLEXIO0_D24` | Camera data attached | Live image data present; ordering under review |
| D5 | `P4_17` | `FLEXIO0_D25` | Camera data attached | Live image data present; ordering under review |
| D6 | `P4_18` | `FLEXIO0_D26` | Camera data attached | Live image data present; ordering under review |
| D7 | `P4_19` | `FLEXIO0_D27` | Camera data attached | Live image data present; ordering under review |
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

## Rev B Shield Notes

The current Port 4 camera map is a validation setup for deciding whether the
FlexIO camera interface is viable before making the Rev B FRDM shield for the
NXP Cup car. Do not treat every current fly-wire choice as final shield routing.

The main open pin-routing question is VSYNC timing. `P4_22` is working well as a
GPIO IRQ frame-start trigger and can also be `FLEXIO0_D30`, but its CTIMER
function is `CT2_MAT2` match output, not a CTIMER capture input. If the Rev B
design needs hardware-latched frame timestamps, consider routing VSYNC to a pin
that can also expose a true CTIMER capture or trigger-capable input, such as
the candidate `FLEXIO0_D30` alternatives noted in the MCXN947 pin data. If CPU
ISR timestamping is sufficient, keep the current `P4_22` approach and sample a
free-running CTIMER counter at the start of the GPIO IRQ.

There is also an exact-pin future option for lower-jitter timestamps without
moving VSYNC: GPIO4 exposes pin event DMA request sources. A later firmware
revision could configure a `P4_22` rising-edge GPIO event to DMA-copy a
free-running CTIMER count into memory. Treat this as a second-stage optimization
only; the initial cleanup should use the simpler GPIO ISR timestamp because the
camera frame-DMA setup still needs CPU-side buffer/health management.

## Current Incremental Data-Bus Test State

The current incremental test has all camera data lines attached to the Port 4
FlexIO data-bus wiring. The image is visible and generally good. There may be
one or more adjacent data-bit swaps, so data-bit ordering is still under review.

| Camera data signal | Current test MCU pin | Status |
| --- | --- | --- |
| D0-D7 | `P4_12..P4_19` / `FLEXIO0_D20..D27` | Camera data attached; possible adjacent pair swap to verify |

RTT confirms live nonzero data after wiring: `cam_dma done` advances at
30 frames/sec, `sample_nz=64`, `submit_err=0`, and `cb_err=0`. A few timeout
counters accumulated during the incremental live-wiring period, but they did
not increase during the post-wiring RTT stability sample.

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
| 2026-07-09 14:53 EDT | Disconnected all data lines from the Port 4 FlexIO data-bus wiring. | Ready to rerun sync-only diagnostic with PCLK/HSYNC/VSYNC still fitted. |
| 2026-07-09 14:54 EDT | Reran sync-only RTT diagnostic with all data lines disconnected. | VSYNC improved but is still noisy at about 318-366 extra edges/sec, so the remaining issue is in the sync/VSYNC path. |
| 2026-07-09 14:55 EDT | Rerouted VSYNC farther away from other camera signals and reran sync-only RTT diagnostic. | Sync recovered: VSYNC 30-31 edges/sec, HSYNC about 6040 edges/sec, `p4_lines=200`, and `p4_vs_off=0`. |
| 2026-07-09 14:57 EDT | Restored D0/D1 while keeping VSYNC physically separated. | Ready to rerun sync diagnostic before restoring more data lines. |
| 2026-07-09 14:59 EDT | Reran RTT with D0/D1 restored. | VSYNC stayed below the guard but rose to about 35-43 edges/sec, with occasional `p4_lines` dips to 149 and 188. |
| 2026-07-10 15:31 EDT | Switched active firmware to `FLEXIO_EDMA` and tested with D0-D7 disconnected/pulled low. | Full-frame DMA completes at 30 frames/sec with black samples: `done` +30/sec, `first=0000,0000,0000,0000`, `sample_nz=0`, no submit/callback errors, no timeouts. |
| 2026-07-10 17:16 EDT | Attached all camera D0-D7 lines to `P4_12..P4_19`. | LCD shows a mostly good live image with visible color changes as each wire was added. RTT shows live nonzero data and stable 30 frame/sec DMA completions; possible adjacent data-bit swaps remain to verify. |
| 2026-07-10 18:33 EDT | Added Rev B shield routing note for VSYNC timing. | Current `P4_22` wiring remains valid for bring-up, but final shield routing may change if hardware CTIMER capture or GPIO event-DMA timestamping is needed. |
