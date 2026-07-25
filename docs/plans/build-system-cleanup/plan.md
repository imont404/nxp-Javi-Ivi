+++
type = "plan"
id = "build-system-cleanup"
status = "pending"
created = "2026-07-25"

[[steps]]
id = "inventory-variants"
title = "Inventory every build variant, wrapper script, and CONFIG__ combination in use"
status = "pending"

[[steps]]
id = "variant-matrix"
title = "Define the supported variant matrix and which combinations must keep building"
status = "pending"
depends_on = ["inventory-variants"]

[[steps]]
id = "consolidate-wrappers"
title = "Consolidate the per-experiment PowerShell wrappers into one parameterised entry point"
status = "pending"
depends_on = ["variant-matrix"]

[[steps]]
id = "guard-coverage"
title = "Ensure every illegal CONFIG__ combination fails at compile time rather than at runtime"
status = "pending"
depends_on = ["variant-matrix"]

[[steps]]
id = "variant-build-check"
title = "Add a scripted build of the whole supported matrix"
status = "pending"
depends_on = ["consolidate-wrappers", "guard-coverage"]

[[steps]]
id = "flexio-diag-pin-groups"
title = "Decide whether the FlexIO signal and pipeline diagnostics follow the pin-group selector"
status = "pending"
depends_on = ["variant-matrix"]

[[steps]]
id = "port4-verify-or-retire"
title = "Verify the Port 4 FlexIO group on hardware or retire it"
status = "pending"
depends_on = ["variant-matrix"]

[[steps]]
id = "student-facing-review"
title = "Review the resulting build surface from a three-day student's point of view"
status = "pending"
depends_on = ["consolidate-wrappers"]

[[steps]]
id = "design-doc-intent-audit"
title = "Audit design docs, ADRs, and requirements against implementation"
status = "pending"
depends_on = ["variant-build-check", "student-facing-review"]

[[steps]]
id = "test-runtime-impact-audit"
title = "Audit new test runtime impact"
status = "pending"
depends_on = ["variant-build-check"]

[[steps]]
id = "external-review"
title = "Obtain independent external review"
status = "pending"
depends_on = ["design-doc-intent-audit", "test-runtime-impact-audit"]

[[exit_criteria]]
id = "matrix-defined"
title = "The supported build variant matrix is written down and agreed"
status = "pending"

[[exit_criteria]]
id = "one-entry-point"
title = "Variants are selected by parameter rather than by a separate script per experiment"
status = "pending"

[[exit_criteria]]
id = "illegal-combos-fail-early"
title = "Illegal configuration combinations fail at compile time with a clear message"
status = "pending"

[[exit_criteria]]
id = "matrix-builds"
title = "The whole supported matrix builds from one command"
status = "pending"

[[exit_criteria]]
id = "student-surface-simple"
title = "A student who has never seen the repository can build and flash the competition image without reading this plan"
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

# Build System and Configuration Cleanup

## Purpose

**Deferred debt, deliberately incurred.** A run of bring-up work through July 2026 proved
several things possible — the QDC encoder path, FlexIO capture on the Rev A camera pins,
the pin-group selector — and each one added a build variant and usually a wrapper script.
That was the right trade at the time: the goal was proving what needs work, not tidying
how it is built.

The result is a build surface that grew one experiment at a time and now wants a
deliberate pass.

**Not urgent, and it must not compete with race preparation.** The race is late August
2026 and the Rev A competition image builds correctly today.

## Current State

Wrapper scripts at the repository root, each wrapping `build_cmake.ps1` with a different
set of `-Define` values:

- `build_cmake.ps1` — the general entry point, takes `-Define`
- `build.ps1` — MCUXpresso headless fallback
- `build_motor_encoder_diag.ps1` — QDC bench diagnostic, plus `-EnableMotors`
- `build_flexio_camera.ps1` — FlexIO capture, `-PinGroup Port4|Port1`
- `flash.ps1`, `flash_motor_encoder_diag.ps1`
- `rtt.ps1`, `rtt_motor_encoder_diag.ps1`

Configuration axes in `avc__master_config.h` — **22 knobs**, accurate as of 2026-07-25:

| Knob | Values / default |
|---|---|
| `CONFIG__CAMERA_CAPTURE_BACKEND` | SMARTDMA_EZH *(default)*, FLEXIO_DIAG, FLEXIO_EDMA, FLEXIO_PIPELINE_DIAG |
| `CONFIG__CAMERA_FLEXIO_PIN_GROUP` | PORT4 *(default)*, PORT1 |
| `CONFIG__DISPLAY_PANEL` | ER_TFT020_3 *(default)*, ER_TFT020_7 |
| `CONFIG__DISPLAY_TE_ENABLE` / `_TEST_MODE` / `_PARALLEL_BITBANG_TEST_MODE` | 0 *(default)* / 1 |
| `CONFIG__DISPLAY_PARALLEL_BITBANG_WR_DELAY_CYCLES` / `_FRAME_DELAY_MS` | 16U / 250U |
| `CONFIG__SERVO_PWM_OUTPUT` | P2_3_B2 *(default)*, P3_20_A3 |
| `CONFIG__USB_DEBUG_STREAM_ENABLE` | 0 *(default)*, 1 |
| `CONFIG__MOTOR_ENCODER_BACKEND` | DISABLED *(default)*, QDC |
| `CONFIG__MOTOR_ENCODER_DIAG_ENABLE` / `_MOTOR_ENABLE` | 0 *(default)* / 1 |
| `CONFIG__MOTOR_ENCODER_DIAG_PWM_PERCENT` / `_M0` / `_M1` | 12, capped at 20 by `#error` |
| `CONFIG__MOTOR_ENCODER_DIAG_AUTOSTART_MS` / `_REPORT_MS` | 0U *(button only)* / 250U |
| `CONFIG__MOTOR_ENCODER_COUNTS_PER_WHEEL_REV` | 1320U — **measured**, not a guess |
| `CONFIG__MOTOR_ENCODER_INVERT_M0` / `_M1` | 1 / 1 — forward reads positive |
| `CONFIG__MOTOR_ENCODER_WHEEL_DIAMETER_MM` | 75U |

The product of those axes is large and mostly meaningless. Only a handful of combinations
are real, and nothing currently records which.

## Known Work

### Pin-group coverage for the FlexIO diagnostics

`CONFIG__CAMERA_FLEXIO_PIN_GROUP` applies only to `FLEXIO_EDMA`. The older `FLEXIO_DIAG`
and `FLEXIO_PIPELINE_DIAG` backends remain Port 4 only, enforced by an `#error`. That was
a deliberate scope limit during bring-up. Decide whether they follow the selector or get
retired.

### Port 4 FlexIO is hardware-unverified

The pin-group refactor touched working capture code, and the Port 4 hardware regression
was skipped because rewiring eleven fly-wires to re-prove a path being moved away from was
not worth the bench time. Port 4 builds, and its pin set was checked statically against
the pre-refactor image, but it has not captured since. Either verify it or retire the
group — leaving a build variant that nobody has run is worse than having one fewer.

Reasoning is recorded in `docs/research/AVC_Camera_FlexIO_Pin_Migration.md` (the plan it
came from has been retired; its logs are in git history).

### Servo PWM output selection

`CONFIG__SERVO_PWM_OUTPUT` exists to let the Rev B candidate pin be scoped without a
respin, and it has served that purpose — `P3_20/PWM1_A3` is verified. Once Rev B routing is
decided, one of the two options becomes dead weight. See
`docs/research/AVC_RevB_Servo_PWM_Options.md`.

### Wrapper proliferation

Each experiment added a script. They are thin and correct, but the pattern does not scale
and it obscures which combinations are actually supported.

### Student-facing surface

The audience is students with about three days who have never seen this repository. Every
variant that is visible but not needed is a way to get lost. Whatever the internal build
matrix becomes, the path to *the competition image* should be one obvious command.

## Constraints

- **Race first.** Late August 2026. Nothing here may destabilise the Rev A competition
  image: `CAMERA_CAPTURE_BACKEND_SMARTDMA_EZH`, `DISPLAY_PANEL_ER_TFT020_3`, USB stream
  off, encoder backend disabled.
- Any consolidation must keep the existing wrappers working, or update every reference to
  them in `AGENTS.md`, `README.md`, and the plan documents in the same change.

## Source Material

- `src/avc/avc_core0/source/avc_config/avc__master_config.h` — the configuration axes
- `build_cmake.ps1`, `build.ps1`, and the per-experiment wrappers at the repository root
- `scripts/tools/jlink_common.ps1` — probe selection shared by the flash and RTT wrappers
- `AGENTS.md` — documents the current build and flash flow
- `docs/research/AVC_Camera_FlexIO_Pin_Migration.md` — where the pin-group selector came from
- `docs/research/AVC_Motor_Encoder_QDC_Research.md` — where the encoder diagnostic variants came from

Both plans have been retired; their work logs are in git history.
