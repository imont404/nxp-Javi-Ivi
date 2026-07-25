+++
type = "plan"
id = "camera-flexio-pin-migration"
status = "active"
created = "2026-07-25"

[[steps]]
id = "confirm-board-access"
title = "Confirm the three target pins are free and unclaimed on the test board"
status = "done"

[[steps]]
id = "baseline-capture"
title = "Record the Port 4 FlexIO baseline the Port 1 group must match"
status = "pending"

[[steps]]
id = "pin-group-selector"
title = "Add a FlexIO pin-group selector that keeps the Port 4 group as default"
status = "done"

[[steps]]
id = "decouple-port-hardcoding"
title = "Decouple the FlexIO pin setup from PORT4 and from single-port contiguity"
status = "done"
depends_on = ["pin-group-selector"]

[[steps]]
id = "port4-regression"
title = "Prove the Port 4 FlexIO path still works after the refactor"
status = "active"
depends_on = ["decouple-port-hardcoding"]

[[steps]]
id = "jumper-fit"
title = "Fit the three J9_EXT jumpers and verify EZH still runs unchanged"
status = "done"
depends_on = ["confirm-board-access"]

[[steps]]
id = "port1-signal-diag"
title = "Confirm live PCLK, HREF, and data toggling on the Port 1 FlexIO pins"
status = "pending"
depends_on = ["jumper-fit", "decouple-port-hardcoding"]

[[steps]]
id = "port1-frame-capture"
title = "Capture a correct frame through FlexIO plus eDMA on the Rev A camera pins"
status = "pending"
depends_on = ["port1-signal-diag"]

[[steps]]
id = "ezh-freed-demo"
title = "Demonstrate the EZH is idle and claimable while FlexIO captures"
status = "pending"
depends_on = ["port1-frame-capture"]

[[steps]]
id = "backend-switch-parity"
title = "Prove EZH and FlexIO both run on the identical harness by alt7/alt6 selection"
status = "pending"
depends_on = ["port1-frame-capture"]

[[steps]]
id = "rev-b-recommendation"
title = "Record the Rev B routing recommendation"
status = "pending"
depends_on = ["ezh-freed-demo", "backend-switch-parity"]

[[steps]]
id = "design-doc-intent-audit"
title = "Audit design docs, ADRs, and requirements against implementation"
status = "pending"
depends_on = ["backend-switch-parity"]

[[steps]]
id = "test-runtime-impact-audit"
title = "Audit new test runtime impact"
status = "pending"
depends_on = ["backend-switch-parity"]

[[steps]]
id = "external-review"
title = "Obtain independent external review"
status = "pending"
depends_on = ["rev-b-recommendation", "design-doc-intent-audit", "test-runtime-impact-audit"]

[[exit_criteria]]
id = "port4-not-regressed"
title = "The existing Port 4 FlexIO path still builds and captures after the refactor"
status = "pending"

[[exit_criteria]]
id = "both-groups-selectable"
title = "Port 4 and Port 1 FlexIO pin groups are both selectable by configuration, Port 4 remaining the default"
status = "met"

[[exit_criteria]]
id = "jumpers-non-destructive"
title = "The three jumpers are fitted with no cuts and EZH capture still works with them in place"
status = "met"

[[exit_criteria]]
id = "port1-frame-correct"
title = "FlexIO captures a correct frame on P1_4..P1_11 with PCLK on P1_14, at no worse than the current frame rate"
status = "pending"

[[exit_criteria]]
id = "ezh-available"
title = "The EZH is demonstrably idle and claimable for other work while FlexIO captures"
status = "pending"

[[exit_criteria]]
id = "software-only-switch"
title = "Backend selection is alt7 versus alt6 on identical wiring, with no rewiring between builds"
status = "pending"

[[exit_criteria]]
id = "rev-b-decision"
title = "Rev B routing is recommended or declined, with the jumpered result recorded as the evidence"
status = "pending"

[[exit_criteria]]
id = "signoff"
title = "Focused signoff passes"
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

# Camera FlexIO Pin Migration

## Purpose

**Free the EZH.** The EZH is wanted for other work that is not I/O-driven on these pins,
and camera capture is what currently occupies it.

**This is not a frame-rate exercise.** FlexIO capture already works in this firmware at
the existing frame rate on the Port 4 pin group (`CAMERA_CAPTURE_BACKEND_FLEXIO_EDMA`,
`P4_12..P4_22`). What that path costs is **eleven fly-wires**, because Port 4 is not where
the camera is routed.

The question this plan answers is whether the **existing Rev A camera wiring** can drive
FlexIO with minimal change — proving on hardware we already have that the EZH can be
released, and informing what the next board revision should route.

**Matching the current frame rate is success.** Any improvement is incidental and cannot
be claimed from a jumpered setup anyway.

Analysis, netlist evidence, and pin tables:
**`docs/research/AVC_Camera_FlexIO_Pin_Migration.md`**

## The Change

Three short dupont jumpers on `J9_EXT` (spare 2x16 header on the same nets as J9, so the
stack is undisturbed). **Confirmed populated and ready on the test board.**

| # | From | To | Why |
|---|---|---|---|
| 1 | pin 12 (`P3_4`, cam D4) | pin 32 (`P1_8`) | `D12` -> `D16`, resolves collision with D0 |
| 2 | pin 11 (`P3_5`, cam D5) | pin 30 (`P1_9`) | `D13` -> `D17`, resolves collision with D1 |
| 3 | pin 15 (`P0_5`, PCLK) | pin 2 (`P1_14`) | P0_5 has no FlexIO function at all |

Contingent: remove **R173** (0603, FRDM board) if cam D4 misbehaves — MCU-Link VCOM TX
drives P1_8 through 330 ohm. The VCOM is unused; debug is SEGGER RTT.

**No cuts.** The camera signal stays on its original net; the old pin is muxed off in
firmware. Both pins see the signal, only the new one is read.

HSYNC needs no jumper — `P0_11` already carries `FLEXIO0_D3`. VSYNC stays a GPIO IRQ on
`P0_4`.

## Firmware Shape

Keep **both** FlexIO pin groups selectable. The Port 4 path works and must not be
regressed; this adds a second option, it does not replace anything.

```c
#define CAMERA_FLEXIO_PIN_GROUP_PORT4  1   /* proven, needs 11 fly-wires      */
#define CAMERA_FLEXIO_PIN_GROUP_PORT1  2   /* Rev A camera wiring + 3 jumpers */

#ifndef CONFIG__CAMERA_FLEXIO_PIN_GROUP
#define CONFIG__CAMERA_FLEXIO_PIN_GROUP (CAMERA_FLEXIO_PIN_GROUP_PORT4)
#endif
```

| | Port 4 group (proven) | Port 1 group (new) |
|---|---|---|
| Data D0-D7 | `P4_12..P4_19` -> `D20..D27` | `P1_4..P1_11` -> `D12..D19` |
| PCLK | `P4_20` -> `D28` | `P1_14` -> `D22` |
| HREF | `P4_21` -> `D29` | `P0_11` -> `D3` |
| VSYNC | `P4_22`, GPIO4 IRQ | `P0_4`, GPIO0 IRQ |

## Step Notes

### confirm-board-access

`J9_EXT` is confirmed populated. Still confirm nothing on the shield needs `P1_14`
(`EZH_LCD_D10`) while the SPI panel is selected.

Avoid `P1_12` (drives Q2 FDV301N plus R3 100k on the shield) and `P1_13` (R163 0 ohm to
`ENET_RXDV/MODE2`, an Ethernet PHY bootstrap strap). `P1_15` -> `FLEXIO0_D23` is the clean
alternate to `P1_14` if needed.

### baseline-capture

**Rescoped 2026-07-25.** The jumpers were fitted before a written baseline was taken, and
the EZH build was verified clean with them in place by observation. That settles
`jumpers-non-destructive`, so the pre-jumper EZH baseline is no longer needed and the
dependency from `jumper-fit` has been dropped.

What still matters: record frame rate and a reference image for the **Port 4 FlexIO**
build. That is the number the Port 1 group has to match, and it is still obtainable
because the jumpers do not affect the Port 4 path.

### pin-group-selector / decouple-port-hardcoding

The existing FlexIO path is already parameterised by `CAMERA_FLEXIO_DATA_GPIO_START_PIN`,
`CAMERA_FLEXIO_DATA_PIN_START`, `CAMERA_FLEXIO_PCLK_PIN`, and `CAMERA_FLEXIO_HREF_PIN`, so
the shifter, timer, and eDMA setup need **no structural change** — only different
constants. Three couplings do block a second group:

- **~48 hardcoded `PORT4`/`GPIO4` references**, plus `kCLOCK_Port4` / `kCLOCK_Gpio4`.
- **`camera__configure_flexio_edma_pins()` walks one contiguous range on one port**
  (`for (pin = DATA_GPIO_START_PIN; pin <= CAMERA_DIAG_HSYNC_PIN; pin++)`). The Port 1
  group has a gap (data ends at `P1_11`, PCLK is `P1_14`) and puts HREF on a **different
  port** (`P0_11`). Split into a data loop plus explicit PCLK and HREF configuration.
- **VSYNC IRQ is hardcoded** to `GPIO40_IRQn` / `GPIO40_IRQHandler`. The Port 1 group needs
  `GPIO00_IRQHandler`.

In our favour: **`GPIO00_IRQHandler` already exists** in this file and already services
`P0_4` and `P0_11` as the reference diagnostic counters (`CAMERA_REF_VSYNC_PIN`,
`CAMERA_REF_HSYNC_PIN`). The Port 1 sync pins are already configured, already
interrupting, already counted — the work is promoting that handler from counting to
driving capture. **Watch for a conflict with the reference diagnostic**, which uses the
same pins.

This is a **prove-it-is-possible** exercise. A build-system-wide cleanup comes later; the
near-term bar is only that the Port 4 group stays selectable and unbroken.

### port4-regression

Non-negotiable. The refactor touches a working capture path. Rebuild the Port 4 group and
confirm it still captures before trusting anything about Port 1.

### jumper-fit

After fitting, **the EZH build must still work unchanged**. That is the proof the change
is non-destructive and the fallback intact.

### ezh-freed-demo

The actual point of the plan. Show the EZH is idle and claimable while FlexIO captures —
not merely that FlexIO works. A minimal EZH program doing something unrelated, running
concurrently with FlexIO capture, is the convincing demonstration.

### rev-b-recommendation

Rev B would route cam D4/D5 to `P1_8`/`P1_9` and PCLK to `P1_14`: zero jumpers, PCLK
routed properly instead of hanging off a dupont stub, EZH free. **The jumpered result
informs this decision rather than justifying it on frame rate.** If the Port 1 group
captures correctly and matches the current rate, Rev B is worth doing purely to free the
EZH.

Note the jumpers add stubs to three signals. Do not conclude anything about a properly
routed board's PCLK ceiling from them.

## Constraints

- **The race is late August 2026.** This must not put the competition image at risk.
  Everything stays behind configuration; Rev A EZH remains the default. See
  `docs/research/AVC_Competition_Overview.md`.
- Rev A hardware is in Guatemala. This work happens on the local test board.
- The Port 4 FlexIO path is working code. Regressing it is a failure, not a tradeoff.

## Source Material

- `docs/research/AVC_Camera_FlexIO_Pin_Migration.md` — the analysis behind this plan
- `docs/research/FlexIO_Camera_Test_Plan.md` — the Port 4 FlexIO work
- `docs/research/MCXN947/flexio_camera_io_pin_map.md` — Port 4 pin map
- `docs/research/MCXN947/json/ksdk2_0/MCXN947VDF/signal_configuration.json` — pin mux authority
- `frdm-avc/tracks/A/output/design_review/design/` — shield netlist
- `docs/FRDM-MCXN947/.../design_review/design/` — FRDM board netlist
- `src/avc/avc_core0/source/avc_io/bv_camera__interface.c` — pin setup at 1430-1447 (EZH),
  546-607 (FlexIO pins), 609+ (FlexIO/shifter config), 1074 and 1119 (IRQ handlers)
