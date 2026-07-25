+++
type = "plan"
id = "camera-flexio-pin-migration"
status = "pending"
created = "2026-07-25"

[[steps]]
id = "confirm-board-access"
title = "Confirm J9_EXT is accessible and the three target pins are free on the test board"
status = "pending"

[[steps]]
id = "baseline-ezh-capture"
title = "Record the EZH baseline frame rate and image before touching anything"
status = "pending"

[[steps]]
id = "jumper-fit"
title = "Fit the three J9_EXT jumpers and verify EZH still runs unchanged"
status = "pending"
depends_on = ["confirm-board-access", "baseline-ezh-capture"]

[[steps]]
id = "flexio-pinmux-profile"
title = "Add a build profile that muxes the camera bus to FlexIO alt6"
status = "pending"
depends_on = ["confirm-board-access"]

[[steps]]
id = "flexio-signal-diag"
title = "Confirm live PCLK, HSYNC, and data toggling on the FlexIO pins"
status = "pending"
depends_on = ["jumper-fit", "flexio-pinmux-profile"]

[[steps]]
id = "flexio-frame-capture"
title = "Capture a correct frame through FlexIO plus eDMA on the migrated pins"
status = "pending"
depends_on = ["flexio-signal-diag"]

[[steps]]
id = "pclk-ceiling"
title = "Find the usable PCLK ceiling with jumper stubs in place"
status = "pending"
depends_on = ["flexio-frame-capture"]

[[steps]]
id = "backend-switch-parity"
title = "Prove EZH and FlexIO both run on the identical harness by alt7/alt6 selection"
status = "pending"
depends_on = ["flexio-frame-capture"]

[[steps]]
id = "rev-b-recommendation"
title = "Record the Rev B routing recommendation with measured justification"
status = "pending"
depends_on = ["pclk-ceiling", "backend-switch-parity"]

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
id = "jumpers-non-destructive"
title = "The three jumpers are fitted with no cuts and EZH capture still works with them in place"
status = "pending"

[[exit_criteria]]
id = "flexio-frame-correct"
title = "FlexIO captures a correct frame on P1_4..P1_11 with PCLK on P1_14"
status = "pending"

[[exit_criteria]]
id = "software-only-switch"
title = "Backend selection is alt7 versus alt6 on identical wiring, with no rewiring between builds"
status = "pending"

[[exit_criteria]]
id = "pclk-headroom-known"
title = "The usable PCLK ceiling and resulting frame rate are measured and compared against the EZH baseline"
status = "pending"

[[exit_criteria]]
id = "rev-b-decision"
title = "Rev B routing is either recommended with measured justification or explicitly declined"
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

Make the camera capture backend a **software choice** instead of a board respin.

Rev A is wired for EZH/SmartDMA. EZH caps PCLK, and it competes with core1 for the shared
code bus, which is part of why core1 is unused today. The EZH is also wanted for other
work. Moving capture to FlexIO frees it — but the previous FlexIO attempt targeted a
completely different pin group and needed eleven fly-wires.

**Research finding:** the existing camera pins *do* carry FlexIO, and three jumpers make
`P1_4..P1_11` simultaneously contiguous `FLEXIO0_D12..D19` **and** contiguous
`SMARTDMA_PIO0..PIO7`. Backend selection then reduces to **alt7 vs alt6**.

Full analysis, netlist evidence, and pin tables:
**`docs/research/AVC_Camera_FlexIO_Pin_Migration.md`**

## The Change

Three short dupont jumpers on `J9_EXT` (spare 2x16 header on the same nets as J9, so the
stack is undisturbed):

| # | From | To | Why |
|---|---|---|---|
| 1 | pin 12 (`P3_4`, cam D4) | pin 32 (`P1_8`) | `D12` -> `D16`, resolves collision with D0 |
| 2 | pin 11 (`P3_5`, cam D5) | pin 30 (`P1_9`) | `D13` -> `D17`, resolves collision with D1 |
| 3 | pin 15 (`P0_5`, PCLK) | pin 2 (`P1_14`) | P0_5 has no FlexIO function at all |

Contingent: remove **R173** (0603, FRDM board) if cam D4 misbehaves — MCU-Link VCOM TX
drives P1_8 through 330 ohm. The VCOM is unused; debug is SEGGER RTT.

**No cuts.** The camera signal stays on its original net; the old pin is muxed off in
firmware. Both pins see the signal, only the new one is read.

Firmware side:

- `P1_4..P1_11` -> alt6 (`FLEXIO0_D12..D19`)
- `P1_14` -> alt6 (`FLEXIO0_D22`, PCLK)
- `P0_11` -> alt6 (`FLEXIO0_D3`, HSYNC) — no jumper needed, already FlexIO-capable
- `P0_4` VSYNC -> GPIO IRQ, unchanged
- Mux `P3_4`, `P3_5`, `P0_5` to disabled
- Drop the three `INPUTMUX_AttachSignal` calls on the FlexIO path

## Sequencing Notes

### confirm-board-access

Desk research says `J9_EXT` is an unpopulated 2x16 on the same nets as J9. **Confirm on the
physical test board before ordering any work around it.** Also confirm nothing on the
shield needs `P1_14` (`EZH_LCD_D10`) while the SPI panel is selected.

Avoid `P1_12` (drives Q2 FDV301N plus R3 100k on the shield) and `P1_13` (R163 0 ohm to
`ENET_RXDV/MODE2`, an Ethernet PHY bootstrap strap). `P1_15` is the equally clean
alternate to `P1_14`.

### baseline-ezh-capture

Record frame rate, a reference image, and the current PCLK **before** fitting jumpers.
Without this there is nothing to compare the FlexIO result against, and the whole point is
the comparison.

### jumper-fit

After fitting, **the EZH build must still work unchanged**. That is the proof the change
is non-destructive and the fallback is intact. If EZH breaks with jumpers in, stop — the
stubs are already a problem and the PCLK ceiling question is answered badly.

### flexio-pinmux-profile

Follow the existing pattern: a `CONFIG__` selection with `#error` guards, matching how
`CONFIG__CAMERA_CAPTURE_BACKEND` and `CONFIG__MOTOR_ENCODER_BACKEND` already gate risky
subsystems. **The Rev A competition default must not change.**

### pclk-ceiling

The jumpers add stubs to three signals. Expect this to limit PCLK below what properly
routed traces would allow. **Do not conclude anything about FlexIO's real frame-rate
ceiling from the jumpered setup** — that is what Rev B routing is for. Measure and record
the difference honestly.

### backend-switch-parity

The headline claim is that the same physical harness runs both backends. Demonstrate it:
build EZH, capture, build FlexIO, capture, no hands on the hardware in between.

### rev-b-recommendation

Rev B would route cam D4/D5 to `P1_8`/`P1_9` and PCLK to `P1_14`, giving zero jumpers,
properly routed PCLK, and a freed EZH. **Only recommend it with a measured frame-rate
number** — the whole justification is beating the ~24 FPS EZH ceiling, and if FlexIO does
not clearly beat it, say so and decline.

## Constraints

- **The race is late August 2026.** This work must not put the competition image at risk.
  Everything stays behind a build flag; Rev A EZH remains the default until there is a
  measured reason to change and time to validate it. See
  `docs/research/AVC_Competition_Overview.md`.
- Rev A hardware is in Guatemala. This work happens on the local test board.

## Source Material

- `docs/research/AVC_Camera_FlexIO_Pin_Migration.md` — the analysis behind this plan
- `docs/research/FlexIO_Camera_Test_Plan.md` — prior FlexIO plan, Port 4 pin group
- `docs/research/MCXN947/flexio_camera_io_pin_map.md` — prior pin map
- `docs/research/MCXN947/json/ksdk2_0/MCXN947VDF/signal_configuration.json` — pin mux authority
- `frdm-avc/tracks/A/output/design_review/design/` — shield netlist
- `docs/FRDM-MCXN947/.../design_review/design/` — FRDM board netlist
- `src/avc/avc_core0/source/avc_io/bv_camera__interface.c:1430-1447` — current pin setup
