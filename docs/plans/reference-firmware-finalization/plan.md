+++
type = "plan"
id = "reference-firmware-finalization"
status = "active"
created = "2026-08-24"

[[steps]]
id = "resume-checkpoint"
title = "Resume from clean avc commit 76a0f2f with the physically proven race arming, EXE stop, neutral PWM, USB telemetry, and LCD race-entry behavior"
status = "done"

[[steps]]
id = "test-substate-contract"
title = "Define separate TEST_CAMERA and TEST_ACTUATORS substates, their button selection, pot meanings, LCD labels, telemetry, and safe entry/exit behavior"
status = "active"
depends_on = ["resume-checkpoint"]

[[steps]]
id = "test-camera"
title = "Implement the camera/algorithm test with motors disabled and simple pot-controlled scan-line and threshold experiments"
status = "pending"
depends_on = ["test-substate-contract"]

[[steps]]
id = "test-actuators"
title = "Move the existing motor and steering exercise into TEST_ACTUATORS with EXE arming, the three-pot midpoint interlock, command telemetry, and QDC wheel-speed feedback"
status = "pending"
depends_on = ["test-substate-contract"]

[[steps]]
id = "test-transition-safety"
title = "Make every TEST substate change immediately disable motors, clear the motor lease, center steering, and require deliberate re-arming"
status = "pending"
depends_on = ["test-camera", "test-actuators"]

[[steps]]
id = "embedded-state-presentation"
title = "Give every TEST and RACE state an unambiguous typed state, framework telemetry label, and LCD presentation without stale camera data"
status = "pending"
depends_on = ["test-transition-safety"]

[[steps]]
id = "race-state-closeout"
title = "Re-audit RACE_WAITING, RACE_RUNNING, EXE start/stop, zero-duty audible arming, camera loss, callback overrun, and the 100 ms command lease"
status = "pending"
depends_on = ["embedded-state-presentation"]

[[steps]]
id = "firmware-bench-regression"
title = "Manually validate cold boot, both TEST substates, every transition, actuator signs, wheel RPM, race start/stop, lease expiration, faults, LCD, and USB behavior on Rev A hardware"
status = "pending"
depends_on = ["race-state-closeout"]

[[steps]]
id = "interface-freeze"
title = "Freeze public API symbols, internal state enums, telemetry names and types, system-action commands, wire limits, and participant-editable boundaries from bench evidence"
status = "pending"
depends_on = ["firmware-bench-regression"]

[[steps]]
id = "relay-dashboard-template"
title = "Port the approved projector-oriented telemetry template to the Android relay HTML after the embedded state and telemetry interfaces are frozen"
status = "pending"
depends_on = ["interface-freeze"]

[[steps]]
id = "consumer-regression"
title = "Rebuild and test firmware, native Windows tools, direct WebSerial viewer, Android app, relay modes, commands, and reconnect behavior against the frozen interfaces"
status = "pending"
depends_on = ["relay-dashboard-template"]

[[steps]]
id = "documentation-freeze"
title = "Update design, setup, participant, maintainer, telemetry, and manual validation documentation only after code and interfaces are stable"
status = "pending"
depends_on = ["consumer-regression"]

[[steps]]
id = "design-doc-intent-audit"
title = "Audit design, setup, participant, maintainer, telemetry, and validation documentation against the frozen implementation and organizer intent"
status = "pending"
depends_on = ["documentation-freeze"]

[[steps]]
id = "test-runtime-impact-audit"
title = "Audit build and test duration plus firmware binary, RAM, frame-loop, service, LCD, encoder, and USB runtime impact"
status = "pending"
depends_on = ["consumer-regression"]

[[steps]]
id = "external-review"
title = "Obtain independent review of firmware safety, public interfaces, consumer compatibility, documentation, validation evidence, and repository handoff readiness"
status = "pending"
depends_on = ["design-doc-intent-audit", "test-runtime-impact-audit"]

[[steps]]
id = "nxp-cup-repository-handoff"
title = "After final audits and explicit authorization, reconcile this history with the separate nxp_cup work, then perform the repository cutover without silently changing the avc remote"
status = "pending"
depends_on = ["external-review"]

[[exit_criteria]]
id = "test-separation"
title = "Camera/algorithm testing cannot command actuators, and actuator testing cannot silently inherit camera-test pot meanings"
status = "pending"

[[exit_criteria]]
id = "transition-safety"
title = "Every mode and TEST-substate transition disables motion, clears leases, centers steering, and makes the next arm deliberate"
status = "pending"

[[exit_criteria]]
id = "state-observability"
title = "LCD, Windows, WebSerial, and Android consumers show the same stable typed mode/state and framework-owned actuator telemetry"
status = "pending"

[[exit_criteria]]
id = "physical-regression"
title = "Rev A hardware passes the documented TEST, RACE, lease, fault, camera-buffer, LCD, encoder, and USB regression"
status = "pending"

[[exit_criteria]]
id = "frozen-interface"
title = "Public APIs and cross-consumer protocol fields are intentionally frozen before documentation and relay UI closeout"
status = "pending"

[[exit_criteria]]
id = "repository-handoff"
title = "The finalized implementation, tests, documentation, and history are reconciled into the authorized NXP Cup repository without losing work from either checkout"
status = "pending"

[[exit_criteria]]
id = "design-doc-intent-audit"
title = "Documentation matches the frozen implementation and organizer intent"
status = "pending"

[[exit_criteria]]
id = "test-runtime-impact-audit"
title = "Build, test, binary, memory, and competition-loop runtime impact are reviewed"
status = "pending"

[[exit_criteria]]
id = "external-review"
title = "Independent review is complete"
status = "pending"
+++

# Reference Firmware Finalization

## Resume point

Work remains in `C:\ELI\fit2026\avc`. The durable starting point for the next
session is clean commit `76a0f2f` (`feat: refine telemetry workflow and race
controls`). The sibling `nxp_cup` checkout may contain lesson-plan work owned by
another agent; do not overwrite it or change this repository's remote without
explicit authorization.

The latest firmware was built, flashed through ROM-HID with full readback, and
observed on Rev A hardware. EXE starts and stops race execution, race entry arms
the motor PWM at neutral, the neutral-write defect found during physical testing
was corrected, and cold race entry clears stale LCD camera data. This is bench
evidence for those narrow behaviors, not proof of the remaining regression list.

## Intended TEST flow

TEST mode should teach and diagnose two concepts separately:

- `TEST_CAMERA` is the safe default. Motors remain disabled. The camera image and
  a small amount of processing are visible, with pots controlling simple concepts
  such as scan-line position and a brightness/edge threshold. It must remain an
  experiment, not a completed lane detector.
- `TEST_ACTUATORS` demonstrates left motor, steering, and right motor commands.
  It retains framework-owned EXE arming and the three-pot midpoint interlock.
  Framework telemetry reports commanded outputs, and standard QDC feedback reports
  measured left/right wheel speed.

Left/right buttons are the leading candidate for selecting the two substates, but
the exact interaction and the final meaning of every camera-test pot remain open
until tried on the car. EXE should remain the arm/disarm control inside the actuator
test. Any substate switch must safe-stop and require a new deliberate arm.

## Sequencing rule

Finish the embedded state machine first. Do not freeze the relay dashboard around
temporary state names or pot meanings. Once manual firmware testing establishes
the state model, freeze the public API, internal enums that cross components,
telemetry names/types, and command behavior together. Only then update the Android
relay page to the approved projector-oriented template and complete the user-facing
documentation.

## Manual validation gate

Compile-only evidence is insufficient. Before interface freeze, manually exercise:

1. Cold boot into TEST and RACE with correct LCD content and no stale pixels.
2. Both TEST substates and every button transition.
3. Midpoint interlock, arm/disarm, actuator signs, neutral behavior, and QDC signs.
4. RACE_WAITING and RACE_RUNNING from physical EXE and framed host actions.
5. Immediate EXE/host stop and 100 ms lease expiration during a deliberate stall.
6. Camera-loss and callback-overrun safe faults.
7. Camera-buffer stability with slow callbacks plus USB/LCD load.
8. USB absent, attached, subscribed, disconnected, and reconnected.

Record what was physically observed. Do not promote a test or exit criterion from
compile, unit-test, or telemetry-only evidence.

## Scope boundaries

QDC wheel feedback remains standard in the competition image. Do not add build
options for ordinary use. Do not supply lane following, PID, active differential,
or a completed race solution. Keep normal participant edits confined to
`source/app/test_mode.c` and `source/app/race_mode.c`; framework-owned safety and
protected telemetry must not depend on participant callbacks.
