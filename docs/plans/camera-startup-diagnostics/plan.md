+++
type = "plan"
id = "camera-startup-diagnostics"
status = "pending"
created = "2026-08-21"

[[steps]]
id = "reproduce-and-mode-check"
title = "Reproduce the splash symptom and record TEST-switch state, RTT milestones, and electrical basics"
status = "done"

[[steps]]
id = "lock-startup-contract"
title = "Define one-time platform initialization, camera startup states, operational modes, and fault invariants"
status = "done"
depends_on = ["reproduce-and-mode-check"]

[[steps]]
id = "surface-sensor-status"
title = "Return and display SCCB probe, OV5640 identity, and CAMERA_DEVICE_Init results"
status = "pending"
depends_on = ["system-startup-dispatch"]

[[steps]]
id = "bound-sccb-transactions"
title = "Guarantee SCCB transactions and retries terminate with specific failure status"
status = "pending"
depends_on = ["surface-sensor-status"]

[[steps]]
id = "classify-camera-signals"
title = "Report XCLK and returned PCLK, HREF, and VSYNC activity without disturbing the active capture backend"
status = "pending"
depends_on = ["surface-sensor-status"]

[[steps]]
id = "first-frame-watchdog"
title = "Time-bound first-frame acquisition and distinguish no signals, no capture completion, and invalid cadence"
status = "pending"
depends_on = ["classify-camera-signals"]

[[steps]]
id = "system-startup-dispatch"
title = "Implement explicit test, race-waiting, student-running, and safe-fault dispatch with a camera-ready student-start gate"
status = "done"
depends_on = ["lock-startup-contract"]

[[steps]]
id = "fast-screening-boot"
title = "Remove the startup splash and delay so a healthy assembly reaches live camera display immediately"
status = "done"
depends_on = ["system-startup-dispatch"]

[[steps]]
id = "fault-retry-policy"
title = "Add safe bounded camera retry and power-cycle behavior with persistent fault evidence"
status = "pending"
depends_on = ["system-startup-dispatch", "bound-sccb-transactions", "first-frame-watchdog"]

[[steps]]
id = "assembly-test-view"
title = "Provide a fast LCD and RTT pass/fail view for camera, LVDS module, and flex-cable screening"
status = "pending"
depends_on = ["first-frame-watchdog"]

[[steps]]
id = "characterize-sccb-rate"
title = "Measure SCCB integrity and decide whether a configurable slower rate is justified"
status = "pending"
depends_on = ["assembly-test-view"]

[[steps]]
id = "failure-matrix"
title = "Validate healthy and deliberately disconnected SCCB, XCLK, sync, data, and TEST-switch cases"
status = "pending"
depends_on = ["fault-retry-policy", "characterize-sccb-rate"]

[[steps]]
id = "student-boundary-docs"
title = "Document startup status APIs and student-mode entry without providing a race algorithm"
status = "pending"
depends_on = ["failure-matrix"]

[[steps]]
id = "design-doc-intent-audit"
title = "Audit startup, camera, and competition documentation against implementation"
status = "pending"
depends_on = ["student-boundary-docs"]

[[steps]]
id = "test-runtime-impact-audit"
title = "Audit startup tests, boot-time cost, and competition-loop runtime impact"
status = "pending"
depends_on = ["failure-matrix"]

[[steps]]
id = "external-review"
title = "Obtain independent external review"
status = "pending"
depends_on = ["design-doc-intent-audit", "test-runtime-impact-audit"]

[[exit_criteria]]
id = "splash-is-actionable"
title = "The LCD cannot remain on an ambiguous splash; it identifies mode, startup stage, or camera fault"
status = "met"

[[exit_criteria]]
id = "mode-is-visible"
title = "TEST, RACE/WAITING, STUDENT/RUNNING, and SAFE/FAULT modes are visibly distinct from camera health"
status = "met"

[[exit_criteria]]
id = "sensor-result-visible"
title = "OV5640 chip ID, SCCB status, and full sensor-configuration result are visible on LCD and RTT"
status = "pending"

[[exit_criteria]]
id = "signal-result-visible"
title = "Missing returned PCLK, HREF, VSYNC, or first frame is distinguishable from SCCB failure"
status = "pending"

[[exit_criteria]]
id = "startup-is-bounded"
title = "No SCCB transaction, retry sequence, sync wait, or first-frame wait can hang boot indefinitely"
status = "pending"

[[exit_criteria]]
id = "safe-gating"
title = "Motors remain disabled through startup and fault; camera failure cannot enter a moving mode"
status = "met"

[[exit_criteria]]
id = "assembly-screening"
title = "A known-good operator can classify each camera assembly and cable quickly without a debugger"
status = "pending"

[[exit_criteria]]
id = "competition-backend"
title = "The Rev A EZH competition path passes repeated cold boots and sustained frame acquisition"
status = "pending"

[[exit_criteria]]
id = "design-doc-intent-audit"
title = "Startup, camera, and competition documentation match implementation"
status = "pending"

[[exit_criteria]]
id = "test-runtime-impact-audit"
title = "Startup tests, boot-time cost, and runtime impact are reviewed"
status = "pending"

[[exit_criteria]]
id = "external-review"
title = "Independent external review is complete"
status = "pending"
+++

# Camera Startup Diagnostics and State Machine

## Integration ownership

This plan retains SCCB/camera fault diagnosis, signal classification, retry behavior, and
assembly-test evidence. Its completed startup dispatch is input to
`docs/plans/nxp-cup-framework-migration`, which owns the final public mode names, editable
callbacks, main-loop shape, and safety integration. Do not create a second dispatcher here.

## Priority

**Park the deeper diagnostics unless assembly screening exposes a repeatable failure.**
USB telemetry and the Android bridge are now proven on the healthy Rev A camera path, so
this plan no longer blocks either one. The current operator need is fast screening of 24
camera/LVDS/flex assemblies; live LCD video is accepted as the immediate pass criterion.

On 2026-08-21 the FIT splash draw and its two-second delay were removed from the
competition image, rebuilt, flashed, and committed as `cfbee55`. The display driver still
initializes normally, but a healthy camera now reaches the live frame path immediately.
This is intentionally simpler than claiming the pending SCCB/signal fault classifier is
complete.

On 2026-08-21 the state-machine foundation was deliberately moved ahead of the deeper
SCCB and signal diagnostics after restoring the TEST jumper returned live video. The
foundation makes mode and first-frame readiness visible without making USB part of mode
selection. The detailed camera-fault work remains sequenced behind that stable dispatcher
and should resume only if one of the 24 assemblies cannot be classified from live video.

## Trigger and Resolved Immediate Symptom

On 2026-08-21, after moving the car for photographs, the Rev A car powered the camera but
remained on the FIT splash image. The physical TEST jumper had been removed.

That symptom did **not** isolate SCCB:

- `nxpc__init()` calls `nxpc_camera__init()` before initializing the LCD and drawing the
  splash. Seeing the splash proves the monolithic camera-init call returned; it does not
  prove that it succeeded, because `CAMERA_DEVICE_Init()`'s return value is discarded.
- The splash is replaced only inside the TEST-mode branch and only after
  `nxpc__next_frame()` sets `next_frame_ready`. The removed TEST jumper therefore fully
  explains why the screen stayed on the splash even if frames were arriving. Camera-init
  failure remains possible but is **not established by this symptom**.
- If TEST mode is active, the remaining possibilities include sensor configuration
  failure, missing XCLK or returned PCLK/HREF/VSYNC, capture-backend failure, or no valid
  first-frame callback.

Restoring TEST mode returned live video. The later state-machine foundation made the
operational mode visible, and removal of the splash/delay eliminated the unnecessary
screening wait. No current evidence justifies changing SCCB timing or camera capture.

The first bench action is to restore/force TEST mode and confirm whether live preview
returns, while recording the TEST input and RTT boot milestones. Then separate sensor
control from signal return and capture completion.

## Existing Technical Facts

- Competition hardware is Rev A using the EZH/SmartDMA capture backend by default.
- The locally modified FlexIO Port 1 board is a diagnostic alternative, not the default
  assumption for screening the twenty assemblies.
- OV5640 SCCB uses LPI2C7 at the SDK default 100 kHz over single-ended SCL/SDA alongside
  the LVDS camera data path.
- OV5640 identification registers are `0x300A`/`0x300B`, expected `0x56`/`0x40`.
- `CAMERA_DEVICE_Init()` returns a `status_t`, but `camera__init_sensor()` discards it.
- SDK `I2C_RETRY_TIMES` is zero, documented by NXP as waiting indefinitely for some driver
  flags. Boundedness must be established, not assumed from the blocking API name.
- Existing FlexIO diagnostics already demonstrate useful PCLK/HREF/VSYNC counters, but
  their pin routing cannot simply be enabled over the active EZH mux. The competition
  backend needs non-invasive evidence or a separate diagnostic preset.
- The working LCD is the required local fault surface. RTT provides the detailed trace;
  USB must not be a prerequisite for diagnosing USB's own camera source.

## Startup Architecture

Use two related but distinct state concepts:

1. **System startup state** gates progress from safe boot to an operational mode.
2. **Operational mode** selects local test, student algorithm, or safe/fault behavior
   after required startup gates pass.

USB link/session state remains separate and belongs to `usb-debug-telemetry`.

Proposed system-startup states, with final names settled by `lock-startup-contract`:

```text
LOW_LEVEL_INIT (one pass, motors safe)
    -> CAMERA_POWER_XCLK
    -> CAMERA_PROBE
    -> CAMERA_CONFIGURE
    -> CAMERA_START_CAPTURE
    -> CAMERA_WAIT_FIRST_FRAME
    -> READY
    -> TEST or RACE_WAITING operational dispatch
    -> STUDENT_RUNNING only after an explicit safe start request

Any failed or timed-out transition -> CAMERA_FAULT -> SAFE
```

`nxpc_system__init()` should contain only one-time, bounded platform setup: clocks, pins,
debug output, inputs, an early diagnostic display surface, and safe actuator defaults.
Before reordering LCD and camera initialization, explicitly check shared DMA and clock
dependencies; preserve the current order until the alternative is proven safe.

`nxpc_system__service()` should advance bounded state transitions and service platform
work. It must not hide the student algorithm. A clear top-level switch eventually calls
the local assembly/test service, student algorithm service, or safe fault service.

The physical TEST input selects the requested local-test versus race workflow, but it must
not silently change a moving mode. `lock-startup-contract` must decide whether it is sampled
once at boot or accepted later only through a safe transition. In either case, a
deasserted TEST input leads to a visible `RACE / WAITING` screen; an explicit button/start
event may enter `STUDENT / RUNNING` only after camera readiness and other safety gates pass.

The camera owns its subordinate status:

```text
OFF / POWERING / PROBING / CONFIGURING / WAITING_FOR_SIGNALS /
WAITING_FOR_FRAME / READY / FAULT
```

Expose read-only status containing at least stage, last `status_t`, chip ID, SCCB attempt
and failure counts, signal activity, first-frame age, frame count, and a stable fault code.

## Race-Week Implementation Slices

### Slice A — classify today's failure

1. Confirm and visibly report the physical TEST switch state.
2. Preserve the return from `CAMERA_DEVICE_Init()` and print its numeric and named class.
3. Read and report the OV5640 chip ID before the full register-table load.
4. Add a bounded first-frame deadline and report whether any capture callback arrived.
5. Show a terse status/fault code on the LCD even when TEST mode is not active.

The always-visible state must distinguish at least `TEST`, `RACE / WAITING`,
`STUDENT / RUNNING`, and `SAFE / CAMERA FAULT`. A race-mode screen is not itself proof of
camera health; show the camera startup/result alongside it.

This slice should answer “SCCB or sync/capture?” without waiting for the full architectural
cleanup.

### Slice B — separate missing signals

Establish backend-appropriate observations for XCLK, PCLK, HREF, and VSYNC. Do not remux
pins away from EZH merely to count them. Prefer existing peripheral flags/counters where
they are trustworthy; otherwise use a dedicated diagnostic preset and scope checkpoints at
the camera, LVDS TX/RX, and MCU. Record signal-present/absent/invalid-cadence separately.

### Slice C — bounded startup dispatcher

Move the successful Slice A checks behind explicit camera/system state APIs. It is
acceptable initially for the large OV5640 register-table load to be one bounded
`CAMERA_CONFIGURE` action; do not build a complicated asynchronous register sequencer
unless measurement shows it is necessary. The critical property is explicit result and
finite time, not artificial nonblocking complexity during one-time boot.

### Slice D — resilience

Add a bounded retry policy only after failures are observable. Preserve the first failure
and count retries. If evidence shows SCL/SDA can be stuck, add documented LPI2C recovery
before one retry; otherwise avoid speculative bus-reset code. A manual retry from the
fault screen may power-cycle/reset the camera, but must keep motors disabled.

## Camera Assembly Test View

The local screen should be useful without USB or J-Link:

- startup stage and physical TEST-mode state;
- `OV5640 5640` or a specific SCCB/status error;
- XCLK/PCLK/HREF/VSYNC activity summary;
- first-frame PASS/timeout and measured frame cadence;
- stable short fault code large enough to photograph or transcribe;
- retry count and an explicit safe manual-retry instruction.

Once a valid frame arrives, show the live image as today. Do not require a rich graphics
framework: fixed RGB565 text/lines/blocks are enough.

## Failure Matrix

Validate with a known-good assembly first, then introduce one fault at a time:

| Case | Required classification |
|---|---|
| TEST switch inactive, healthy camera | Camera may reach READY; screen says local preview is not selected |
| Camera absent or SCCB disconnected | Probe/NACK or bounded bus error; no indefinite wait |
| Wrong sensor/read data | Explicit chip-ID mismatch with values read |
| XCLK absent | XCLK/startup fault before blaming returned sync |
| SCCB succeeds, returned sync absent | PCLK/HREF/VSYNC fault, not SCCB fault |
| Sync present, no capture callback | Capture/first-frame timeout |
| Data path disconnected with sync intact | Frame arrives but validation/pattern indicates data-path fault where feasible |
| Cable disturbed during operation | Safe loss-of-frame indication; no motor enable or automatic moving-mode transition |
| Known-good full assembly | Repeated cold-boot PASS and stable frame cadence |

## Student Boundary

Students receive a clear `camera ready / frame available / health` framework API and a
simple operational-mode dispatch point. They do not receive the line-following solution,
steering policy, speed controller, or a hidden algorithm inside `nxpc_system__service()`.

## Deferred Until Evidence

- Slowing SCCB below 100 kHz. Measure rise time and failures first.
- General runtime hot-plug support; a safe explicit retry may be sufficient for the event.
- Moving all camera register writes into a fine-grained asynchronous sequencer.
- USB telemetry integration beyond preserving the later service boundary.

## Source Material

- `src/nxp_cup/nxp_cup_core0/source/main.c`
- `src/nxp_cup/nxp_cup_core0/source/nxpc_io/nxpc__io.c`
- `src/nxp_cup/nxp_cup_core0/source/nxpc_io/bv_camera__interface.c`
- `src/nxp_cup/nxp_cup_core0/board/board.c`
- `src/nxp_cup/nxp_cup_core0/video/fsl_sccb.c`
- `src/nxp_cup/nxp_cup_core0/video/fsl_ov5640.c`
- `src/nxp_cup/nxp_cup_core0/drivers/fsl_lpi2c.h`
- `docs/research/AVC_LVDS_Adapter.md`
- `docs/research/AVC_Camera_FlexIO_Pin_Migration.md`
