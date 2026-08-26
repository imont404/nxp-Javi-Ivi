+++
type = "plan"
id = "reference-firmware-finalization"
status = "active"
created = "2026-08-24"

[[steps]]
id = "resume-checkpoint"
title = "Resume from the nxp_cup cleanup branch, whose history contains clean avc checkpoint 76a0f2f and the physically proven race arming, EXE stop, neutral PWM, USB telemetry, and LCD race-entry behavior"
status = "done"

[[steps]]
id = "test-substate-contract"
title = "Freeze the three-page TEST lab contract: CAMERA / IO, VISION, and MOTORS, with framework-owned navigation, presentation, output permissions, and safe entry/exit behavior"
status = "done"
depends_on = ["resume-checkpoint"]

[[steps]]
id = "competition-optimization"
title = "Normalize CMake and MCUXpresso competition builds to global -O2 with debug symbols and no unmeasured per-file overrides; audit -fno-builtin, ISR-shared state, and effective last-option-wins commands"
status = "done"
depends_on = ["race-state-closeout"]

[[steps]]
id = "optimized-baseline-regression"
title = "After the functional framework is standing, rebuild and physically smoke-test the optimized image for boot, camera, LCD, USB, every TEST/RACE state, neutral outputs, EXE stop, and unchanged wire behavior"
status = "active"
depends_on = ["competition-optimization"]

[[steps]]
id = "button-input-normalization"
title = "Clean the 1 ms debouncer and implement sole-producer press/release sequence counters plus held snapshots, defined simultaneous-button behavior, and inherited-release suppression without exposing button_t"
status = "done"
depends_on = ["test-substate-contract"]

[[steps]]
id = "rgb565-graphics-surface"
title = "Implement canary-tested RGB565 pixel, horizontal/vertical/general line, rectangle fill, and bounded fixed-font text with signed clipped coordinates, fixed surfaces, no allocation, and reviewed worst-case timing"
status = "done"
depends_on = ["button-input-normalization"]

[[steps]]
id = "test-page-safety-core"
title = "Before page implementations, add typed TEST page state, an atomic safe-transition operation, page-aware output gating, CAMERA/IO reset on TEST entry, bounded deliberate arming, and MOTORS-only capped motor permission"
status = "done"
depends_on = ["button-input-normalization"]

[[steps]]
id = "test-page-navigation"
title = "Implement framework-owned left/right release-event navigation across CAMERA / IO, VISION, and MOTORS with a persistent page name, arrows, and contextual EXE hint in the 320x40 status strip"
status = "done"
depends_on = ["test-page-safety-core", "rgb565-graphics-surface"]

[[steps]]
id = "test-camera-io"
title = "Implement the safe default CAMERA / IO page with live image, normalized floating-point pot values, and battery voltage while actuator output is impossible"
status = "done"
depends_on = ["test-page-navigation"]

[[steps]]
id = "test-vision-lab"
title = "Keep the three student-readable TEST handlers together in test_mode.c while VISION remains the editable camera-processing sandbox and framework-owned gates retain all motion authority"
status = "done"
depends_on = ["test-page-navigation"]

[[steps]]
id = "test-actuators"
title = "Implement the MOTORS page with alpha=left motor, beta=steering, gamma=right motor, concise EXE arming, midpoint interlock, command telemetry, and QDC wheel feedback"
status = "done"
depends_on = ["test-page-navigation"]

[[steps]]
id = "test-transition-safety"
title = "Run pure trace and fake-actuator regression over every TEST entry/page/arming/lease transition, including forbidden participant motor calls, bounce, hold, simultaneous buttons, wraparound, and entry while held"
status = "done"
depends_on = ["test-camera-io", "test-vision-lab", "test-actuators"]

[[steps]]
id = "student-api-teaching-surface"
title = "Give race_mode.c a concise camera-to-analysis-to-steering-and-speed scaffold, document the common API in nxp_cup.h, and remove obsolete private processing or graphics paths without supplying a race solution"
status = "done"
depends_on = ["test-transition-safety"]

[[steps]]
id = "embedded-state-presentation"
title = "Give every TEST and RACE state an unambiguous typed state, framework telemetry label, and LCD presentation without stale camera data"
status = "done"
depends_on = ["student-api-teaching-surface"]

[[steps]]
id = "race-state-closeout"
title = "Re-audit RACE_WAITING, RACE_RUNNING, EXE start/stop, zero-duty audible arming, camera loss, callback overrun, and the 100 ms command lease"
status = "done"
depends_on = ["embedded-state-presentation"]

[[steps]]
id = "firmware-bench-regression"
title = "Manually validate cold boot, all three TEST pages, navigation indicators, every transition, actuator signs, wheel RPM, race start/stop, lease expiration, faults, LCD, USB, and optimized timing on Rev A hardware"
status = "pending"
depends_on = ["optimized-baseline-regression"]

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
title = "After final audits and explicit authorization, integrate the finalized branch within nxp_cup local history without changing or pushing its remote"
status = "pending"
depends_on = ["external-review"]

[[exit_criteria]]
id = "test-separation"
title = "CAMERA / IO and VISION cannot command actuators, and MOTORS cannot silently inherit another page's pot meanings or arming state"
status = "pending"

[[exit_criteria]]
id = "transition-safety"
title = "Every mode and TEST-substate transition disables motion, clears leases, centers steering, and makes the next arm deliberate"
status = "pending"

[[exit_criteria]]
id = "state-observability"
title = "The LCD persistently shows TEST page navigation and safety state, while Windows, WebSerial, and Android receive the same stable typed page/state and framework-owned actuator telemetry"
status = "pending"

[[exit_criteria]]
id = "optimized-teaching-loop"
title = "The competition image uses reviewed optimized flags; algorithm, overlay, LCD, and USB costs are measured separately; bounded RGB565 drawing and timing indicators remain understandable to students"
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

[[steps]]
id = "vision-edge-reference"
title = "Replace the VISION threshold sandbox with an efficient one-scanline luma-gradient example that marks and counts strong rising/falling edges while leaving lane selection and color extension to students"
status = "done"
depends_on = ["test-vision-lab"]

[[steps]]
id = "vision-black-white-validation"
title = "Validate scanline luma edges against stable low-value black and high-luma low-saturation white regions, with two-pixel LCD references for validated transitions only"
status = "done"
depends_on = ["vision-edge-reference"]

[[steps]]
id = "vision-scanline-low-pass"
title = "Apply an in-place symmetric three-tap low-pass to scanline Y/S/V before edge and black/white classification to reduce orphan transitions from pixel noise"
status = "done"
depends_on = ["vision-black-white-validation"]
+++

# Reference Firmware Finalization

## Resume point

Work now resumes from `C:\ELI\fit2026\nxp_cup-worktrees\structure` on branch
`cleanup/repository-structure`, after the repository-structure cleanup plan is
closed. That branch contains clean firmware checkpoint `76a0f2f`, later local
firmware planning, the preserved lesson work, and the component-layout
checkpoint. Do not resume implementation in the legacy `avc` checkout. Keep the
primary `nxp_cup` lecture checkout and both repository remotes unchanged until
the owner explicitly authorizes branch integration or new component worktrees.

The latest firmware was built, flashed through ROM-HID with full readback, and
observed on Rev A hardware. EXE starts and stops race execution, race entry arms
the motor PWM at neutral, the neutral-write defect found during physical testing
was corrected, and cold race entry clears stale LCD camera data. This is bench
evidence for those narrow behaviors, not proof of the remaining regression list.

## Intended TEST lab

Installing the TEST jumper enters a framework-owned three-page lab. Left and right
button release events move one page at a time and wrap across:

1. `TEST_CAMERA_IO` is the safe default. It shows the live image, normalized pot
   values in the same `0.00` to `1.00` form used by the public input API, and
   battery voltage to one decimal place. CAMERA / IO uses two rows inside the
   framework-owned status strip: mode/page first, then spaced A/B/G/BAT groups;
   it does not draw diagnostics over the 200-row camera image. Motors are prohibited regardless of
   participant callback behavior.
2. VISION (`TEST_VISION` internally) combines line, edge, color, pixel drawing, fixed-font text, and
   student algorithm experimentation in one motor-prohibited page. Its reference
   algorithm converts one Alpha-selected row to luma, applies a cheap one-dimensional
   Sobel-style gradient, uses Beta for the edge threshold and Gamma for a one-to-four
   pixel gradient radius, collapses each contiguous response to its magnitude-weighted center,
   marks falling and rising edges in different colors, and publishes the total edge
   count. It deliberately retains edges from other lanes or objects and does not pair,
   select, center, steer from, or otherwise turn them into a race solution. The same
   Y/HSV feature array supports a second classification stage: low HSV value identifies
   black, while high luma plus low saturation identifies white. A four-pixel region vote
   validates black/white transitions around each gradient candidate. Unclassified luma edges
   remain unmarked; long red/green marks show only validated transitions. Alpha selects the
   row, Beta adjusts brightness strictness, and Gamma adjusts
   the saturation allowed for white. Before detection, an in-place symmetric `1, 2, 1`
   filter smooths Y, S, and V without shifting edge locations or allocating another row;
   circular hue is copied rather than averaged. All LCD reference marks are at least two pixels wide.
   Its second status row reports `ALGO x.xms / 41ms FRAME`; it does not add
   explanatory text over the image.
   Native classification, region-vote, boundary, and drawing-contract tests pass. The
   competition image was ROM-HID flashed with full readback on 2026-08-25 (SHA-256
   `ADD94D24EDB3E3E8FAF0BE5833C759181B305ED25A8AC3173143B1DCC4978B36`), rebooted
   on COM22, and completed a clean camera/telemetry probe with motors disabled. Visual
   threshold tuning on the physical VISION page remains part of the bench regression.
3. MOTORS (`NXPC_TEST_PAGE_MOTORS` internally) demonstrates alpha as left
   motor, beta as steering, and gamma as right motor. EXE requests arming, all three pots must be centered before
   outputs become live, and framework telemetry plus QDC feedback make commands
   and measured wheel motion visible.

The 320x40 framework-owned status strip uses the 10x14 font and a two-line layout.
In TEST, the top row places `TEST MODE` on the left and the current page with `<` /
`>` on the right; the second row shows only the page's most useful values or concise
MOTORS action such as `PRESS EXE TO TEST` or `CENTER POTS`; while armed it shows
only the signed left/steering/right commands. A page change always disables motors,
clears the command lease, centers steering, cancels pending arming, and requires a
fresh deliberate arm. The framework owns navigation and output permission; the
participant callback cannot weaken them.

Use real typed TEST substates for safety, presentation, and telemetry. The
participant-owned `test_mode.c` callback contains a short read-only dispatcher
with CAMERA / IO, VISION, and MOTORS handlers so the common input, telemetry,
graphics, motor, and steering APIs remain visible together. The framework owns
page selection, all status rendering, deliberate arming, the centered-pot
interlock, TEST duty caps, the motor lease, and safe transitions. Participant
code can read but cannot select the current page or weaken those gates.

## Buttons and participant inputs

Retain the existing button state-machine debouncer, sampled every 1 ms and
configured for approximately 50 ms stability. Normalize its naming, formatting,
types, and comments; remove obsolete duplicate entry points; and keep `button_t`
private. The debouncer is the sole producer of monotonic press and release sequence
counters plus held state. Framework service captures one coherent snapshot;
consumers compare epochs and never clear a shared event. Public queries provide a
non-destructive per-callback view. If left and right releases arrive in one snapshot,
ignore both. TEST entry and each page change establish new baselines and suppress
releases inherited from buttons that were already held. Framework navigation and
EXE handling have priority over participant observation.

## Graphics and timing

The competition/student surface needs only bounded RGB565 pixels, horizontal and
vertical lines, a general line, a filled rectangle, and simple fixed-font text.
Coordinates are signed; null input is a no-op; all operations clip to strict surface
bounds; general-line work is bounded after clipping; text accepts at most 48
printable ASCII bytes; and no operation allocates memory. Do not expose eGFX image
planes, fonts, sprites, or drivers. Internally preserve the useful two-surface
separation: a framework-owned 320x40 status buffer and a 320x200 camera surface that
aliases the current captured frame rather than allocating another camera-sized
buffer.

Implement and benchmark the narrow primitives directly, with canary-buffer host
tests around every edge and worst-case timing checks. Keep the working eGFX/LCD path
until the replacement is proven equivalent on hardware, then remove only
competition-path code that is demonstrably unused. Participant overlays modify the
live camera buffer and therefore appear in both the later LCD dump and USB frame.
Navigation and safety text remain framework-owned. The public participant text API
remains the compact 5x7 font; the framework presentation also has a bounded 10x14
renderer so status and hardware-check values are readable on the car.

The reviewed `competition` preset now uses one global `-O2` setting with `-g3` /
DWARF-4 debug symbols and no per-file optimization overrides. Both build entry
points validate every effective C compile command so last-option-wins drift cannot
silently restore `-O0` or `-O3`. `-fno-builtin` remains explicit pending a measured
reason to change the established SDK/project behavior. ISR-written ADC, camera
frame-pointer, button, motor-lease, and DMA-completion state is volatile and/or
captured in bounded IRQ-disabled snapshots.

The first `-O2` gate on 2026-08-25 regressed the physical LCD because the legacy
ST7789 reset/sleep timing used an empty software loop that optimization legally
deleted. It now uses `SDK_DelayAtLeastUs`. LCD byte/word helpers and legacy display
helpers wait for DMA before local or caller-owned source storage can change, while
the current raw block path retains pipelining and waits after its final submission.
The optimized image builds and passes native regression, but physical LCD/camera/USB/actuator
validation remains mandatory in `optimized-baseline-regression` before the
interface is frozen. Report participant algorithm time in milliseconds and as a
percentage of the 41 ms frame budget; benchmark overlay, LCD transfer, and USB
publication separately rather than calling their sum an ambiguous CPU percentage.

The 2026-08-25 optimized ROM-HID flash completed full readback verification,
rebooted into TEST / CAMERA / IO, published USB telemetry and camera frames, and
the organizer confirmed the LCD is operating. The other TEST pages, actuator
signs/feedback, RACE transitions, and fault behavior remain to be checked manually.

## TEST safety contract

Page state, output gating, and a single atomic safe-transition operation must exist
before any page or navigation implementation. The safe transition first commands
zero/disable, clears the motor lease, centers steering, clears pending and armed
state, suppresses inherited button releases, and only then changes the page. TEST
entry always selects CAMERA / IO. `motors_set_duty()` and `steering_set()` reject
participant output throughout CAMERA / IO and VISION even if participant code
calls them.

The TEST safety control plane is frame-independent. `nxpc_framework__service()`
processes navigation, EXE arming/disarming, safe transitions, and bounded
dirty/status refresh even when the camera produces no frame. Arming issues a
framework-owned neutral first command. Later MOTORS commands come from the
student-readable frame handler, so a missing or slower-than-lease camera causes
the 100 ms lease to expire and requires a fresh EXE arm. Camera loss remains
diagnosable and never freezes page navigation or EXE stop behavior.

EXE arming is processed only on MOTORS. One EXE release opens a five-second
arming window; all three pots must remain within the midpoint band continuously for
250 ms before outputs arm. Leaving the midpoint during the dwell restarts it;
timeout, another EXE release, a page change, TEST exit, lease expiry, or fault
cancels the request and safe-stops. MOTORS duty is capped to +/-25 percent
inside the framework; race-mode range remains unchanged. Confirm or lower that cap
during the first actuator bench test. On MOTORS, motor-lease expiry explicitly
clears pending and armed state, centers steering, and requires a new EXE release;
RACE_RUNNING retains its separate dead-man behavior in which the next valid race
command may resume output.

## Teaching surface

Keep `nxp_cup.h` as the only participant-facing header. The TEST implementation
should demonstrate the commonly used API, and declarations should contain enough
usage guidance that students can follow calls into the header. `race_mode.c` should
show the intended flow in comments--choose/read camera rows, extract features, run
student decision logic, command steering and speed, and publish optional telemetry--
without providing the decision algorithm. Audit the old private line-processing and
general graphics code; retain generic color conversion, but remove obsolete or
unused experiments after their replacements are proven.

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
2. All three TEST pages, persistent navigation hints, one-page-per-press behavior,
   and every page transition.
3. Midpoint interlock, arm/disarm, actuator signs, neutral behavior, and QDC signs.
4. RACE_WAITING and RACE_RUNNING from physical EXE and framed host actions.
5. Immediate EXE/host stop and 100 ms lease expiration during a deliberate stall.
6. Camera-loss and callback-overrun safe faults.
7. Camera-buffer stability with slow callbacks plus USB/LCD load.
8. Algorithm/overlay/LCD/USB timing under the optimized competition image.
9. USB absent, attached, subscribed, disconnected, and reconnected.

Record what was physically observed. Do not promote a test or exit criterion from
compile, unit-test, or telemetry-only evidence.

## Scope boundaries

QDC wheel feedback remains standard in the competition image. Do not add build
options for ordinary use. Do not supply lane following, PID, active differential,
or a completed race solution. Keep normal participant edits confined to
`source/app/test_mode.c` (VISION only) and `source/app/race_mode.c`;
framework-owned safety, CAMERA / IO, MOTORS, navigation, and protected telemetry
must not depend on participant callbacks.
