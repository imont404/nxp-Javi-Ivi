+++
type = "plan"
id = "motor-encoder-qdc-bringup"
status = "active"
created = "2026-07-21"

[[steps]]
id = "qdc-routing-research"
title = "Research Rev A motor encoder routing to MCXN947 QDC hardware"
status = "done"

[[steps]]
id = "pin-owner-build-profile"
title = "Add explicit Rev A build-time ownership for encoder pins versus alternate LCD/EZH uses"
status = "done"
depends_on = ["qdc-routing-research"]

[[steps]]
id = "qdc-driver-strategy"
title = "Choose and integrate the QDC access layer"
status = "done"
depends_on = ["pin-owner-build-profile"]

[[steps]]
id = "qdc-static-self-test"
title = "Validate QDC module setup without motor wiring using self-test or synthetic inputs"
status = "done"
depends_on = ["qdc-driver-strategy"]

[[steps]]
id = "bench-fixture-and-safety"
title = "Define the chassis-on-blocks low-speed motor test mode"
status = "done"
depends_on = ["qdc-driver-strategy"]

[[steps]]
id = "encoder-signal-diag"
title = "Add bench diagnostic mode for raw encoder signal visibility"
status = "done"
depends_on = ["bench-fixture-and-safety"]

[[steps]]
id = "dual-qdc-count-diag"
title = "Count both motor encoders with QDC0 and QDC1"
status = "done"
depends_on = ["encoder-signal-diag"]

[[steps]]
id = "open-loop-speed-telemetry"
title = "Report per-motor QDC rate from low-speed open-loop PWM sweeps"
status = "done"
depends_on = ["dual-qdc-count-diag"]

[[steps]]
id = "wheel-geometry-calibration"
title = "Record encoder geometry and convert QDC rate to wheel RPM"
status = "active"
depends_on = ["open-loop-speed-telemetry"]

[[steps]]
id = "pid-speed-control"
title = "Prototype fixed-rate PID speed control using encoder feedback"
status = "pending"
depends_on = ["wheel-geometry-calibration"]

[[steps]]
id = "design-doc-intent-audit"
title = "Audit design docs, ADRs, and requirements against implementation"
status = "pending"
depends_on = ["pid-speed-control"]

[[steps]]
id = "test-runtime-impact-audit"
title = "Audit new test runtime impact"
status = "pending"
depends_on = ["pid-speed-control"]

[[steps]]
id = "external-review"
title = "Obtain independent external review"
status = "pending"
depends_on = ["pid-speed-control", "design-doc-intent-audit", "test-runtime-impact-audit"]

[[exit_criteria]]
id = "qdc-routing-known"
title = "Schematic and MCXN947 routing for both encoder pairs is documented"
status = "met"

[[exit_criteria]]
id = "pin-ownership-safe"
title = "Firmware build/profile selection prevents encoder pins from conflicting with alternate LCD/EZH functions"
status = "pending"

[[exit_criteria]]
id = "qdc-access-layer"
title = "QDC0 and QDC1 can be initialized through the selected driver or thin HAL"
status = "pending"

[[exit_criteria]]
id = "bench-signal-visibility"
title = "Bench diagnostic confirms live encoder A/B transitions on J11 and J17"
status = "pending"

[[exit_criteria]]
id = "safe-blocks-test"
title = "Low-speed chassis-on-blocks diagnostic has explicit motor limits and stop conditions"
status = "pending"

[[exit_criteria]]
id = "dual-encoder-counts"
title = "QDC0 and QDC1 report directionally correct position deltas for both motors"
status = "pending"

[[exit_criteria]]
id = "speed-control-feasible"
title = "Open-loop speed telemetry is stable enough to support PID tuning"
status = "pending"

[[exit_criteria]]
id = "rpm-conversion-known"
title = "Encoder counts per wheel revolution and wheel geometry are recorded well enough to report RPM"
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

# Bring Up Motor Encoder QDC Feedback

## Current Research Summary

Rev A routes two six-pin motor connectors:

- J17/M0: pin 3 is `P1_0`, pin 4 is `P1_1`.
- J11/M1: pin 3 is `P2_0/ENC_B`, pin 4 is `P1_22/ENC_A`.

The MCXN947 QDC blocks do not take arbitrary GPIO pins directly. The usable
path is to pinmux each encoder signal to its `TRIG_IN` alternate function, then
route the trigger inputs to QDC phase inputs through INPUTMUX.

Proposed first-pass mapping:

- J17 pin 3 `P1_0/TRIG_IN0` -> `QDC0_PHASEA`
- J17 pin 4 `P1_1/TRIG_IN1` -> `QDC0_PHASEB`
- J11 pin 4 `P1_22/TRIG_IN3` -> `QDC1_PHASEA`
- J11 pin 3 `P2_0/TRIG_IN5` -> `QDC1_PHASEB`

Direction can be corrected later by swapping phase assignments or setting the
QDC reverse-direction bit. The existing Rev A J11 encoder pair matches NXP's
FRDM-MCXN947 QDC example wiring for `P1_22` and `P2_0`, which is a good
external sanity check.

See `docs/research/AVC_Motor_Encoder_QDC_Research.md` for the detailed routing,
conflicts, and validation notes.

## Bench Bring-Up Shape

The first practical test should be isolated from camera work:

- Put the chassis on blocks and keep the camera disconnected if needed.
- Build a diagnostic profile that owns the encoder pins and caps motor PWM to a
  deliberately low value.
- Print raw A/B pin state, QDC position delta, direction, overflow flags, and
  counts per second over RTT and/or the existing USB debug transport.
- Run each motor independently first, then both motors together.
- Convert QDC rate to wheel RPM only after counts-per-revolution, gear ratio,
  and wheel diameter/circumference are measured or confirmed for the platform.

The PID step should wait until the open-loop telemetry is stable and the RPM
conversion constants are documented.

## Resume State: 2026-07-21

The first QDC diagnostic implementation is in the working tree and has been
build-validated, but it has not been flashed or bench-validated yet.

Current normal firmware defaults are intentionally still the Rev A competition
baseline:

- `CONFIG__CAMERA_CAPTURE_BACKEND = CAMERA_CAPTURE_BACKEND_SMARTDMA_EZH`
- `CONFIG__DISPLAY_PANEL = DISPLAY_PANEL_ER_TFT020_3`
- `CONFIG__USB_DEBUG_STREAM_ENABLE = 0`
- `CONFIG__MOTOR_ENCODER_BACKEND = MOTOR_ENCODER_BACKEND_DISABLED`

Current diagnostic implementation:

- `src/avc/avc_core0/source/avc_io/avc__motor_encoder_qdc.c`
- `src/avc/avc_core0/source/avc_io/avc__motor_encoder_qdc.h`
- `build_motor_encoder_diag.ps1`
- `flash_motor_encoder_diag.ps1`
- `rtt_motor_encoder_diag.ps1`

The diagnostic image:

- Skips camera and LCD startup.
- Configures `P1_0`, `P1_1`, `P1_22`, and `P2_0` as `TRIG_IN` inputs.
- Routes `TRIG_IN0/1` to `QDC0_PHASEA/B`.
- Routes `TRIG_IN3/5` to `QDC1_PHASEA/B`.
- Reports raw `IMR`, position, delta, counts/sec, direction, and QDC flags over
  the existing `DEBUG` path.
- Starts with motors off.
- When built with `-EnableMotors`, the center button toggles both motors at
  `CONFIG__MOTOR_ENCODER_DIAG_PWM_PERCENT`, capped at 20 percent by config.

Builds completed before reboot:

- `.\build_cmake.ps1`
- `.\build_motor_encoder_diag.ps1`
- `.\build_motor_encoder_diag.ps1 -EnableMotors`

The current motor-enabled diagnostic AXF is:

- `build/cmake/avc_core0-MotorEncoderDiag/avc_core0.axf`

Do not mark `encoder-signal-diag`, `dual-qdc-count-diag`, or the runtime exit
criteria done until the board is flashed on the chassis-on-blocks setup and the
RTT output confirms live encoder transitions/counts for both motors.
