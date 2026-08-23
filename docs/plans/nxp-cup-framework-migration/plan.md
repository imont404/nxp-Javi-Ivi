+++
type = "plan"
id = "nxp-cup-framework-migration"
status = "active"
created = "2026-08-23"

[[steps]]
id = "lock-contract"
title = "Freeze public naming, compatibility, frame ownership, editable boundaries, mode behavior, safety, encoder default, and flash decisions"
status = "done"

[[steps]]
id = "plan-ownership-reconcile"
title = "Assign framework, build, telemetry, host, documentation, and regression ownership across the existing active plans"
status = "done"
depends_on = ["lock-contract"]

[[steps]]
id = "state-machine-contract"
title = "Freeze STARTUP, TEST, RACE_WAITING, RACE_RUNNING, ENTERING_ISP, and SAFE_FAULT behavior with framework-owned safety"
status = "done"
depends_on = ["lock-contract", "plan-ownership-reconcile"]

[[steps]]
id = "frame-contract"
title = "Hide camera-buffer ownership behind one callback-lifetime RGB565 pointer with latest-frame-only drop behavior"
status = "done"
depends_on = ["lock-contract", "state-machine-contract"]

[[steps]]
id = "public-api-contract"
title = "Define one small umbrella public API with simple frame, vision, input, actuator, wheel, telemetry, timing, unit, and error contracts"
status = "done"
depends_on = ["lock-contract", "frame-contract", "state-machine-contract"]

[[steps]]
id = "background-services"
title = "Move bounded camera, button, encoder, USB, ISP, and mode work behind the cooperative system service"
status = "done"
depends_on = ["state-machine-contract", "frame-contract"]

[[steps]]
id = "asynchronous-output-lease"
title = "Implement a timer-driven motor-command lease that safe-stops even when participant code stalls; bench proof is owned by competition-regression"
status = "done"
depends_on = ["state-machine-contract"]

[[steps]]
id = "mode-files"
title = "Make test_mode.c and race_mode.c the only normally edited files and move the TEST experiment out of main.c"
status = "done"
depends_on = ["public-api-contract", "background-services", "asynchronous-output-lease"]

[[steps]]
id = "actuator-feedback-facade"
title = "Expose clamped motor duty, steering, measured wheel speed, and tuning inputs without supplying closed-loop control"
status = "done"
depends_on = ["public-api-contract", "background-services", "asynchronous-output-lease"]

[[steps]]
id = "vision-facade"
title = "Expose RGB565/YHSV, scanline, and TEST-overlay primitives; review edge/segment helpers separately for solution leakage"
status = "done"
depends_on = ["public-api-contract"]

[[steps]]
id = "telemetry-facade"
title = "Expose bounded log and named-value helpers that remain optional and session-gated"
status = "done"
depends_on = ["public-api-contract"]

[[steps]]
id = "minimal-main"
title = "Reduce main.c to initialization, bounded service, latest-frame acquisition, and a readable mode switch"
status = "done"
depends_on = ["mode-files", "state-machine-contract", "background-services", "frame-contract"]

[[steps]]
id = "rename-inventory"
title = "Classify product names, code symbols, paths, artifacts, USB text, documentation links, third-party content, and H.264 AVC terminology"
status = "done"
depends_on = ["lock-contract"]

[[steps]]
id = "rename-tool"
title = "Create a manifest-driven Python refactor tool with dry-run, apply, collision, exclusion, and stale-name check modes"
status = "done"
depends_on = ["rename-inventory"]

[[steps]]
id = "pre-refactor-baseline"
title = "Commit the reviewed tool/manifest, record its exact clean hash, and prove all firmware/host consumers before the AVC rename"
status = "done"
depends_on = ["rename-tool", "minimal-main", "actuator-feedback-facade", "vision-facade", "telemetry-facade"]

[[steps]]
id = "repository-avc-refactor"
title = "From the exact clean baseline, use the checked manifest for every owned AVC identifier, path, target, package, artifact, and label rename"
status = "done"
depends_on = ["rename-tool", "pre-refactor-baseline"]

[[steps]]
id = "firmware-public-layout"
title = "Validate and freeze the manifest-produced public header, facade symbols, editable paths, firmware artifacts, USB text, and source links"
status = "done"
depends_on = ["repository-avc-refactor"]

[[steps]]
id = "mcuxpresso-sync"
title = "After final firmware moves, prove a fresh MCUXpresso headless import/build, CMake drift/build, linker inputs, artifacts, and Ozone paths"
status = "done"
depends_on = ["firmware-public-layout"]

[[steps]]
id = "cross-consumer-rename-validation"
title = "Prove the stale-name scan, native host, browser, Android, and golden protocol fixtures after the repository-wide rename"
status = "done"
depends_on = ["firmware-public-layout"]

[[steps]]
id = "mcuxpresso-gui-smoke"
title = "Hand off the headless-proven project for an owner GUI import/build, source-navigation, and Ozone-path smoke check"
status = "pending"
depends_on = ["mcuxpresso-sync"]

[[steps]]
id = "firmware-contract-checkpoint"
title = "Publish the headless-proven public header, editable paths, frame/mode/safety contracts, Rev A image, and compile-only exercises; physical proof remains in competition-regression"
status = "done"
depends_on = ["mode-files", "mcuxpresso-sync", "cross-consumer-rename-validation"]

[[steps]]
id = "host-branding-layout"
title = "Validate manifest-produced Windows, WebSerial, and Android labels/packages while preserving USB identities and AVCU v1 wire bytes"
status = "done"
depends_on = ["firmware-contract-checkpoint", "rename-inventory"]

[[steps]]
id = "post-branding-consumer-validation"
title = "Rebuild native/browser/Android consumers and retest install, USB permission, packages, artifacts, fixtures, and final stale-name checks"
status = "active"
depends_on = ["host-branding-layout"]

[[steps]]
id = "canonical-build-flash"
title = "Make setup.ps1, build.ps1, and flash.ps1 the conventional entry points; require an explicit flash backend during evaluation"
status = "done"
depends_on = ["firmware-contract-checkpoint"]

[[steps]]
id = "retire-wrapper-clutter"
title = "Remove obsolete root build variants and relocate necessary diagnostics and drift tooling under maintainer scripts"
status = "done"
depends_on = ["canonical-build-flash"]

[[steps]]
id = "docs-branding-links"
title = "Migrate current docs, setup, diagrams, examples, commands, and links to NXP Cup with a legacy allowlist"
status = "pending"
depends_on = ["firmware-contract-checkpoint", "post-branding-consumer-validation", "canonical-build-flash", "select-flash-default"]

[[steps]]
id = "competition-regression"
title = "Bench Rev A TEST/RACE with LCD, camera, encoders, callback overruns, actuator safety, and USB absent/connected/reconnected"
status = "pending"
depends_on = ["firmware-contract-checkpoint"]

[[steps]]
id = "clean-machine-backend-evaluation"
title = "On a clean Windows machine, compare explicit Ozone, ROM-HID, and maintainer J-Link flash backends and recovery behavior"
status = "pending"
depends_on = ["canonical-build-flash", "competition-regression"]

[[steps]]
id = "select-flash-default"
title = "Select and document the no-argument default flash backend from recovery and clean-environment evidence"
status = "pending"
depends_on = ["clean-machine-backend-evaluation"]

[[steps]]
id = "clean-machine-proof"
title = "Prove setup, canonical build/flash, MCUXpresso/Ozone fallback, viewer, and recovery on a clean Windows machine"
status = "pending"
depends_on = ["select-flash-default", "retire-wrapper-clutter", "competition-regression", "post-branding-consumer-validation"]

[[steps]]
id = "tooling-checkpoint"
title = "Publish stable build/flash commands, artifacts, setup behavior, compatibility fixtures, and clean-machine evidence for downstream agents"
status = "pending"
depends_on = ["clean-machine-proof", "docs-branding-links"]

[[steps]]
id = "repository-create"
title = "With explicit authorization, create or select wavenumber-eng/nxp_cup and push preserved history without changing existing remotes"
status = "pending"
depends_on = ["tooling-checkpoint"]

[[steps]]
id = "repository-remote-cutover"
title = "With separate explicit authorization, update remotes and verify clone, build, links, and Pages from the new repository"
status = "pending"
depends_on = ["repository-create"]

[[steps]]
id = "old-repository-redirect"
title = "With separate explicit authorization after verification, archive or redirect the former repository"
status = "pending"
depends_on = ["repository-remote-cutover"]

[[steps]]
id = "design-doc-intent-audit"
title = "Audit naming, public API, editable boundary, state machine, hardware truth, setup, host tools, and downstream plan alignment"
status = "pending"
depends_on = ["docs-branding-links", "competition-regression"]

[[steps]]
id = "test-runtime-impact-audit"
title = "Audit test duration, binary/RAM cost, service/frame timing, LCD cost, encoder cadence, and USB runtime impact"
status = "pending"
depends_on = ["competition-regression"]

[[steps]]
id = "external-review"
title = "Obtain independent review of compatibility, editable boundaries, safety, tool consolidation, migration, tests, and docs"
status = "pending"
depends_on = ["design-doc-intent-audit", "test-runtime-impact-audit", "tooling-checkpoint"]

[[exit_criteria]]
id = "two-files"
title = "Every supplied exercise builds with changes confined to test_mode.c and race_mode.c, each called once per accepted frame in its mode"
status = "pending"

[[exit_criteria]]
id = "small-api"
title = "One header exposes camera, generic vision, inputs, telemetry, motor duty, steering, and wheel feedback without internals"
status = "pending"

[[exit_criteria]]
id = "simple-frame-api"
title = "Mode callbacks receive only a callback-lifetime writable RGB565 pointer and constants; buffer ownership types remain private"
status = "pending"

[[exit_criteria]]
id = "no-audience-names"
title = "The word student appears in no API symbol, header, source path, target, artifact, package, or command"
status = "pending"

[[exit_criteria]]
id = "not-a-solution"
title = "No lane-center decision, steering controller, speed PID, active differential, or completed race strategy is supplied"
status = "pending"

[[exit_criteria]]
id = "mode-contract"
title = "TEST jumper, RACE MODE waiting screen, EXE gate, running dispatch, ISP, and fault transitions are deterministic and safe"
status = "pending"

[[exit_criteria]]
id = "background-services"
title = "Cooperative services meet measured latency bounds and asynchronous motor timeout/watchdog safety survives a stalled callback"
status = "pending"

[[exit_criteria]]
id = "nxp-cup-identity"
title = "Owned identifiers, files, paths, builds, tools, apps, and current docs use NXP Cup/nxpc except reviewed compatibility/history cases"
status = "pending"

[[exit_criteria]]
id = "repeatable-refactor"
title = "The committed rename manifest/tool reproduces the migration from its baseline, detects collisions/stale names, and is clean on a second run"
status = "pending"

[[exit_criteria]]
id = "wire-compatible"
title = "PC, browser, and Android hosts remain interoperable without an accidental protocol-version break"
status = "pending"

[[exit_criteria]]
id = "canonical-build-flash"
title = "One documented build.ps1 and one documented flash.ps1 cover normal presets and explicit backends; direct CMake remains supported"
status = "pending"

[[exit_criteria]]
id = "mcuxpresso-compatible"
title = "A fresh headless MCUXpresso import/build, owner GUI smoke check, CMake drift/build, and Ozone project all work after the rename"
status = "pending"

[[exit_criteria]]
id = "build-output"
title = "The competition preset emits NXP Cup AXF/BIN artifacts and retained diagnostics share the same source of truth"
status = "pending"

[[exit_criteria]]
id = "clean-machine"
title = "A clean Windows machine completes setup, competition build, selected flash path, and camera/telemetry observation"
status = "pending"

[[exit_criteria]]
id = "repo-cutover"
title = "The project is available from wavenumber-eng/nxp_cup with correct links and an old-repository redirect"
status = "pending"

[[exit_criteria]]
id = "firmware-contract-checkpoint"
title = "Documentation agents have a reviewed commit with stable public API, editable paths, frame/mode/safety contracts, and diagrams"
status = "pending"

[[exit_criteria]]
id = "tooling-checkpoint"
title = "Setup/tool agents have a later reviewed commit with stable commands, artifacts, compatibility fixtures, and clean-machine evidence"
status = "pending"

[[exit_criteria]]
id = "competition-regression"
title = "Rev A passes final TEST/RACE, camera, LCD, encoder, actuator-safety, and USB disconnected/connected validation"
status = "pending"

[[exit_criteria]]
id = "design-doc-intent-audit"
title = "Design and setup documentation match implementation and organizer intent"
status = "pending"

[[exit_criteria]]
id = "test-runtime-impact-audit"
title = "Tests and competition runtime impact are measured and accepted"
status = "pending"

[[exit_criteria]]
id = "external-review"
title = "Independent review is complete"
status = "pending"
+++

# NXP Cup Framework, Public API, and Repository Migration

## Priority

Freeze the framework before parallel documentation and tool branches. Students first see
this platform Wednesday and have effectively two productive days, so code flow must be
obvious on a projector and low-level services must stay outside the files they edit.

Sequence the work in four deliberately separate phases:

1. **Firmware contract:** lock modes, simple frame lifetime, safety, the small public API,
   exactly two normally edited files, and their final public paths. Build and commit a
   known-good baseline, then run the reviewed repository-wide AVC rename tool. Prove the
   settled names/layout through CMake, MCUXpresso, Ozone, and host builds before publishing
   `firmware-contract-checkpoint`. Nothing named in that checkpoint moves afterward.
2. **Remaining public migration:** update host labels, docs, and the conventional
   `build.ps1` / `flash.ps1` workflow without changing the frozen identifiers or paths.
3. **Tooling contract:** prove the documented flow on a clean Windows machine and publish
   `tooling-checkpoint`. Documentation and setup agents branch only from the checkpoint
   containing the contract they consume.
4. **Repository cutover:** create, verify, change remotes, and redirect as separately
   authorized external operations.

This plan owns the cross-subsystem framework contract, public API, two editable files,
public NXP Cup naming, stable paths, and integration checkpoints. It does not silently
take implementation ownership from the active subsystem plans:

| Existing plan | Retained ownership | This plan consumes |
| --- | --- | --- |
| `competition-code-docs-closeout` | English/Spanish student content and final curriculum closeout | frozen firmware/tooling checkpoints; its duplicate API/framework steps are retired or rewritten |
| `cmake-build-and-toolchain` | CMake generation, provisioning, clean-machine proof mechanics, MCUXpresso drift tooling | canonical commands and proof evidence |
| `camera-startup-diagnostics` | implemented startup diagnostics and state-machine history | settled startup/fault behavior |
| `usb-debug-telemetry` | transport, protocol, telemetry implementation, and golden fixtures | small public telemetry facade |
| `one-cable-host-tool` | native viewer, ROM-HID workflow, reconnects, and host validation | stable names, artifacts, and flash backend contract |

`plan-ownership-reconcile` updates those plans before implementation so work is not
claimed twice. Downstream branches receive explicit file ownership and may not rename the
public facade, wire contract, or canonical commands independently.

## Naming and Compatibility Contract

| Surface | Target |
| --- | --- |
| Product prose | `NXP Cup` |
| Repository | `wavenumber-eng/nxp_cup` |
| File/tool slug | `nxp_cup` or `nxp-cup` by platform convention |
| New framework C namespace | `nxpc_*` / `NXPC_*` where a prefix prevents collisions |
| Public API | short domain names: `camera_*`, `vision_*`, `motors_*`, `steering_*`, `wheel_*`, `telemetry_*` |
| Audience term `student` | prose only; never part of an API symbol, header, source path, target, artifact, package, or command |
| Firmware outputs | `nxp_cup_core0.axf`, `nxp_cup_core0.bin` |
| Host outputs | `nxpc_viewer.exe`, `nxpc_tool.exe`, `nxp-cup-host-<version>.zip` |
| Former event name | `AVC` only in explicit historical context |
| Video codec | retain `AVC` where it means H.264 Advanced Video Coding |

Do not break proven transport bytes for branding. Keep runtime VID/PID `1FC9:0094`, ROM
HID `1FC9:014F`, and version-1 `AVCU` magic/message values compatible this year. Rename
source identifiers coherently only after every in-repository producer/consumer can move
together. A future protocol version can choose new wire naming deliberately.

Visible USB product text, viewer titles, Android label/package, pages, paths, and artifacts
move to NXP Cup. Android package/label changes require reinstall and USB-permission tests.
Before edits, classify every `AVC` occurrence as branding, public API, internal code,
stable wire contract, historical prose, third-party/generated content, or H.264
terminology. Finish with a checked allowlist rather than a blind replacement. After the
known-good pre-refactor commit, use a manifest-driven Python tool for exact identifier and
path mappings across firmware, shared code, CMake, MCUXpresso, Ozone, native host, browser,
and Android. The tool must support dry-run, collision detection, apply, and stale-name
check modes; be idempotent; use reviewed path moves; and fail on an unmapped owned
`avc`/`AVC` occurrence. Preserve old wire constants or aliases in a clearly marked
compatibility header and validate golden packet fixtures across firmware, native host,
browser, and Android.

### Scripted rename safety

The committed manifest is the complete authority for owned renames, including C/C++/JS/
Java identifiers, files, directories, CMake and MCUXpresso targets, Ozone paths, Android
package, host outputs, USB product text, and visible application labels. Later layout and
branding steps validate or edit prose; they do not invent additional identifier/path
renames outside the manifest.

The Python tool must:

- require a clean worktree at the exact recorded baseline commit and refuse any other
  starting state;
- read only explicitly allowlisted tracked text files and move only manifest-listed tracked
  paths;
- reject unexpected file types, symlinks/reparse points, missing sources, destination
  collisions, and Windows case-only collisions before making a change;
- exclude `.git`, build/output/workspace caches, binaries, generated sources, vendored SDK/
  third-party trees, historical plans/logs, H.264 AVC terminology, and wire-compatibility
  bytes/constants unless an item is individually mapped;
- preserve encoding and line endings, produce a complete dry-run report, and fail on an
  unmapped owned `avc`/`AVC` occurrence;
- be idempotent: `check` after `apply` reports no remaining work.

The exact baseline commit contains the reviewed tool and manifest. Before apply, prove at
that clean commit: retained CMake presets, a fresh MCUXpresso headless import/build, drift
check, native host, browser, Android, and golden protocol fixtures. Rollback means returning
to the untouched baseline commit; the tool never tries to reverse-guess mappings.

## Editable Boundary

Students normally edit exactly:

- `app/test_mode.c`: camera, generic color/edge, telemetry, pot/button, steering, and
  motor experiments under framework safety gates;
- `app/race_mode.c`: per-frame execution/race algorithm, initially with no solution.

Both include one framework-owned public header, tentatively `nxp_cup.h`. Recommended
initial callback shape, to be frozen by `lock-contract`:

```c
void test_mode_on_frame(uint16_t *frame);
void race_mode_on_frame(uint16_t *frame);
```

`frame` is a flat RGB565 image with compile-time width, height, and stride constants. It is
valid only during the callback and must not be saved. It is deliberately writable so TEST
or RACE code can draw reference marks that appear in the later LCD/USB presentation. The
framework owns both physical camera buffers and all acquire/release details; none of those
types or states appear in `nxp_cup.h`. The newest complete frame wins, frames are never
queued, and an over-budget callback causes a drop rather than growing latency.

`nxp_cup.h` is the one public include, but implementation-owned domain headers may sit
beneath it. Every exposed type defines units, range, polarity, cadence, stale/error
behavior, and build-configuration availability. Use named wheel identifiers rather than
raw numeric indexes. The initial facade exposes camera row/pixel access,
RGB565-to-YHSV LUT conversion, scanline primitives, TEST overlays, alpha/beta/gamma inputs,
clamped independent motor duty, explicit stop, normalized steering, independent wheel
RPM/m/s, frame timing, bounded logs, and typed named telemetry. Generic edge/segment
helpers are admitted only after an explicit solution-leakage review; they can remain
teaching examples outside the frozen API.

The framework supplies measured speed and PWM duty, not a speed controller. It supplies
generic edge/segment ingredients, not lane center, crossing policy, steering error/PID,
active differential, or race strategy. The useful current scanline/color/edge and motor
experiments may move into `test_mode.c`; safety arming, midpoint interlock, clamps, and
transition/fault stops remain framework-owned.

An L0 contract test builds every supplied exercise after modifications confined to
`test_mode.c` and `race_mode.c`, compiles examples for every public API, and rejects direct
inclusion of internal headers. Generated LUTs, build configuration, and framework sources
never require normal participant edits.

## Runtime Shape

The projector-readable shape is one initialization, one bounded service, one latest-frame
handoff, and an obvious switch:

```c
int main(void)
{
    nxpc_framework__init();
    for (;;)
    {
        nxpc_framework__service();
        uint16_t *frame = nxpc_framework__take_latest_frame();
        if (frame == NULL) { continue; }

        nxpc_framework__begin_callback();
        switch (nxpc_system__mode())
        {
            case NXPC_SYSTEM_MODE_TEST:         test_mode_on_frame(frame); break;
            case NXPC_SYSTEM_MODE_RACE_RUNNING: race_mode_on_frame(frame); break;
            default: break;
        }
        nxpc_framework__end_callback();
        nxpc_framework__finish_frame(frame);
    }
}
```

This is the frozen runtime shape. Waiting-mode host preview is hidden inside
service/finalize, but the visible concepts remain.

`nxpc_framework__service()` owns bounded button debounce, TEST jumper observation, safe mode
transitions, encoder sampling, USB session/telemetry service, and confirmed ISP entry.
This is a cooperative loop: it does not execute while a mode callback is running.
Callbacks must return within the 41 ms frame period. Instrument callback duration,
consecutive overruns, dropped frames, and maximum service latency; never queue frames.
Discard stale work on an overrun and safe-stop after the owner-selected consecutive
overrun limit.

Camera interrupt context publishes only that a complete frame is available. The framework
keeps the two-buffer details private: hand the newest complete RGB565 buffer to one
callback, do not reuse that buffer until the callback and immediate LCD/USB handoff are
finished, never queue frames, and drop an older pending frame when a newer one arrives.
LCD completes synchronously; USB copies into USB-owned storage before the camera buffer is
released. Neither consumer retains a camera buffer, and main never blocks waiting for an
asynchronous consumer. If the next EZH or FlexIO capture target is still the callback's
buffer, do not re-arm that capture and increment one drop counter. Bench-prove that both
retained capture backends enforce this rule without changing the buffer during a stalled
callback. The public contract remains only “valid during this callback; do not save the
pointer.”

A hardware timer/watchdog motor-command lease supplies the safety guarantee that the
cooperative service cannot: outputs expire unless refreshed, even if participant code
stalls. `asynchronous-output-lease` must inventory timer/watchdog resources and conflicts,
select the clock/time base, define refresh and expiry semantics, keep the ISR bounded,
define motor-driver shutdown interaction, and bench-prove expiry while main context is
deliberately stalled. Measure the actual worst-case interrupt and service latency.

## Mode Contract

| Mode | Entry and display | Callback and outputs |
| --- | --- | --- |
| `STARTUP` | Reset; concise camera initialization state | No callback; motors disabled; steering centered |
| `TEST` | TEST jumper installed; `TEST MODE`; live camera/diagnostics | `test_mode_on_frame()`; framework-owned arm/interlock/clamps apply |
| `RACE_WAITING` | No jumper; exactly `RACE MODE` and `PRESS EXE TO START` | No mode callback; optional raw host preview; outputs safe |
| `RACE_RUNNING` | EXE accepted only after a valid frame; `RACE RUNNING` low-cost indication | `race_mode_on_frame()` per accepted frame; no mandatory full-frame LCD dump |
| `ENTERING_ISP` | Confirmed framed request | No callback; outputs safe and bounded ROM handoff |
| `SAFE_FAULT` | Specific visible fault and `MOTORS DISABLED` | No callback; outputs safe and latched |

USB never selects TEST or starts a race. Installing TEST safe-stops before entering TEST;
removing it returns to RACE_WAITING. Preserve current EXE start-only behavior until the
owner decides whether a later press should stop. Race waiting keeps camera capture and
optional host preview. TEST overlays follow the frozen frame/presentation contract; race
mode avoids LCD frame-copy cost by default.

Freeze the TEST motor-arm sequence before implementation. Recommended race-week behavior
is to preserve the proven center-button arm/disarm action plus potentiometer-midpoint
interlock. The framework owns the armed state; mode code can only request output.
Commands must be refreshed each accepted frame and expire to zero through the asynchronous
motor-command lease. Safe-stop on boot, every mode transition, either TEST-jumper edge,
camera loss, callback overrun limit, ISP entry, and fault. Repeated EXE presses follow the
chosen start/stop policy deterministically and never bypass the camera-valid gate.

## Encoders and Control

Rev A QDC is hardware-proven at 1320 counts/wheel revolution and is compatible with the
normal SPI LCD and USB path. It conflicts only with the parked parallel-LCD experiment.
Recommendation: enable QDC feedback in the final competition preset so speed PID and active
differential are real participant options, remove the redundant normal `encoders` preset, and
retain motors-off/on diagnostics as maintainer presets. This changes today's default and
requires owner confirmation.

Sampling stays fixed-rate in framework service. Public reads report latest value, polarity,
units, cadence, age, and stale state. Motor/steering commands reject NaN/infinity, clamp
finite inputs, and remain inert unless the mode/safety layer permits output. Bench tests
verify wheel polarity and deliberately unequal wheel commands so cross-wiring is visible.

## Canonical Build and Flash Commands

CMake presets remain the authoritative scripted build interface. Keep the conventional
root command names instead of introducing a branded dispatcher:

```powershell
.\setup.ps1
.\build.ps1                         # competition by default
.\build.ps1 -Preset flexio-port1
.\flash.ps1                         # valid after select-flash-default
.\flash.ps1 -Preset flexio-port1 -Backend Ozone
```

Direct `cmake --preset ...` and `cmake --build --preset ...` commands remain documented and
supported. `build.ps1` is a thin preset wrapper; it must not carry a second source list or
configuration model. Before assigning that name to the normal CMake flow, relocate the
current MCUXpresso-only `build.ps1` behavior to an explicit maintainer command such as
`scripts/maintainer/build_mcuxpresso.ps1`.

`flash.ps1` accepts `-Preset` and an explicit backend rather than spawning per-variant
wrappers. During backend evaluation, omitting `-Backend` is an error. `-Backend Ozone`
launches/opens the validated `.jdebug` project for the competition preset; it does not claim
unattended programming. `-Backend Rom` performs the bounded one-cable ROM programming
flow, and `-Backend JLink` retains direct maintainer programming. Compare recovery and
environment evidence, select the no-argument default, and then rerun that exact documented
default on the clean machine. The viewer remains its own application; it does not need to
be hidden behind build/flash. Move RTT, MCUXpresso regeneration/drift checks, specialized
diagnostics, and direct probe helpers under `scripts/maintainer`, but do not remove them.
Remove old root wrappers only after docs, tests, and `AGENTS.md` use the canonical commands.

The top-level CMake host-tools placeholder is stale because host tools now exist. Add a real
host preset or remove the misleading option; do not mix Arm and MSVC toolchains in one
cache.

## MCUXpresso and Ozone Compatibility Gate

MCUXpresso is not required for the normal build, but its project must remain usable. Keep
the current durable `.project` / `.cproject` linked-resource metadata and its generated
CMake relationship unless a separately reviewed change replaces that source of truth.
After every source or path move:

1. update MCUXpresso linked resources and project options;
2. delete/create only the disposable generated headless workspace, import from durable
   project metadata, and build the settled renamed core target without relying on cached
   Eclipse state;
3. run `scripts/maintainer/build_cmake.ps1 -CheckDrift`, regenerate only when the durable metadata changed,
   and build the equivalent CMake preset;
4. compare source roots, includes, defines, linker scripts, and expected AXF/BIN artifacts;
5. open/validate the Ozone project against the resulting artifact and source paths.

The headless MCUXpresso wrapper can move under `scripts/maintainer`, but it remains a
required repeatable validation tool. After it passes, provide the exact project path and
short checklist for the owner's GUI import/build, source-navigation, and Ozone smoke test.
`mcuxpresso-sync` blocks the firmware-contract checkpoint. The owner GUI smoke remains a
separate physical handoff after the fresh headless build has proved the project metadata;
no documentation branch should inherit paths that only one build system understands.

## Repository and Parallel-Agent Cutover

Complete the scripted owned-code rename and freeze the public editable paths and facade
before conceptual documentation branches fork. A possible eventual new-repo layout is:

```text
src/firmware/
src/host/
src/android/nxp_cup_bridge/
app/test_mode.c
app/race_mode.c
docs/
```

If MCUXpresso/Ozone linking makes root `app/` fragile, keep it inside firmware but settle
the public path before `firmware-contract-checkpoint`. Preserve history through
moves, regenerate committed source metadata, and repair HTML/CMake/PowerShell/Ozone/
Android/Pages links together.

At `firmware-contract-checkpoint`, record the commit and publish the public header, editable
paths, callback signatures, frame contract, mode/safety diagram, LCD wording, and Rev A
hardware/preset truth. Conceptual HTML and participant-code documentation agents can branch
there with explicit file ownership. At `tooling-checkpoint`, publish final build/flash
commands, artifacts, setup behavior, compatibility fixtures, clean-machine evidence, and
legacy allowlist for setup/packaging agents. Host agents may work below host-specific
directories but may not rename the wire contract or public facade independently.

Repository creation/history push, local remote changes, Pages verification, and old-repo
archive/redirect are separate external mutations. Each requires explicit owner
authorization. Do not redirect the old repository until a clean clone, build, links, and
Pages content have been verified from the new location.

## Validation

Host/compile checks:

- commit the reviewed rename tool/manifest, record the exact clean baseline hash, and at
  that hash prove CMake, fresh MCUXpresso headless, drift, native, browser, Android, and
  golden protocol tests;
- prove the tool refuses dirty/wrong-commit trees, unexpected types/reparse points, and
  collisions; dry-run from the exact baseline; apply once; and verify a second check
  reports no owned stale names or accidental third-party/generated edits;
- build every retained preset and verify artifact paths;
- create a fresh MCUXpresso headless workspace, import/build from durable metadata, run
  CMake drift/build, validate Ozone paths, and compare source roots, includes, defines,
  linker inputs, and artifacts;
- complete the separate owner GUI import/build, source-navigation, and Ozone smoke check;
- run rack L0/L1, host, browser, and Android tests;
- compile minimal TEST and empty RACE files using only the public header;
- compile examples for every public API and reject internal-header inclusion;
- prove every supplied exercise builds after edits confined to the two mode files;
- enforce the legacy-name allowlist while excluding reviewed generated/third-party trees
  and preserving valid H.264 AVC terms;
- replay golden USB packets through firmware/native/browser/Android parsers;
- after visible branding/docs work, rerun native/browser/Android builds, Android reinstall
  and USB permission checks, artifact/package-path tests, fixtures, and the final allowlist
  stale-name scan;
- detect stale wrapper names, old artifact paths, and broken links.

Rev A bench checks:

- TEST jumper boot/insert/remove transitions and safe stops;
- TEST-switch bounce, insertion during motion, frozen motor-arm/interlock sequence, and
  command expiration when callbacks issue no output;
- `RACE MODE` / `PRESS EXE TO START`, camera-frame start gate, and running dispatch;
- repeated EXE presses and camera loss after race start;
- live TEST overlay, raw RACE frame, and no mandatory race LCD-frame cost;
- frame replacement while TEST overlay, LCD, and USB are active, with no retained or
  overwritten loaned buffer;
- stalled callbacks and slow USB on EZH and retained FlexIO presets: the next unsafe capture
  is not armed, the callback pointer stays stable, and the drop counter increments;
- callback just below/above 41 ms, repeated overruns, drop policy, maximum service latency,
  and asynchronous motor safe-stop;
- motor/steering signs/clamps, NaN/infinity handling, command lifetime, and independent
  wheel-speed polarity/cadence/stale reporting;
- USB absent, unsubscribed, subscribed, disconnected, and reconnected without mode changes;
- USB backpressure under worst-case mode-callback load;
- one-cable ISP safe-stop and physical SW3/reset recovery;
- competition and bare-camera preview;
- binary, RAM, and stack deltas by preset plus service/frame timing within accepted budgets.

Clean-machine proof begins from the participant clone/zip and uses only documented commands.
Do not count workstation PATH state, caches, SDKs, or build directories as prerequisites.

## Locked Execution Decisions

1. **Encoder default:** QDC is enabled in `competition` so measured wheel speed is available.
2. **EXE while running:** preserve start-only behavior; TEST insertion, reset, or fault stops.
3. **TEST motor arming:** preserve the center-button arm/disarm plus
   potentiometer-midpoint interlock; the framework, not mode code, owns arming.
4. **Flash default:** keep canonical `build.ps1` and `flash.ps1`; require `-Backend`
   while collecting candidate recovery evidence, select Ozone versus J11 ROM-HID, then
   validate that no-argument default on the clean machine. Both and maintainer J-Link
   remain available.
5. **Mode-file location:** use the settled firmware `source/app/` path validated by CMake
   and MCUXpresso; do not force a repository-root path for aesthetics.
6. **Wire name:** preserve `AVCU` v1 bytes this year while all visible naming
   becomes NXP Cup.

## Explicitly Out of Scope

- A lane-center, crossing, obstacle, steering, speed PID, or differential solution.
- Rev B adoption or a camera-format/backend change before the race.
- Replacing the working RGB565, EZH Rev A default, or SPI LCD.
- A protocol v2 merely to remove the former acronym.
- Completing all HTML/Spanish content before the framework checkpoint.
- Deleting Ozone, ROM-HID, J-Link, RTT, or diagnostics just to reduce root files.
- Creating/pushing the new repository without explicit authorization.

## Source Material

- `docs/research/AVC_Competition_Overview.md`
- `docs/research/AVC_Motor_Encoder_QDC_Research.md`
- `docs/research/AVC_Vision_Pipeline_Design.md`
- `docs/research/AVC_USB_Debug_Transport_Protocol.md`
- `docs/research/AVC_USB_Debug_Display_Current_State.md`
- `docs/setup.html`
- `docs/plans/competition-code-docs-closeout/plan.md`
- `docs/plans/cmake-build-and-toolchain/plan.md`
- `docs/plans/camera-startup-diagnostics/plan.md`
- `docs/plans/usb-debug-telemetry/plan.md`
- `docs/plans/one-cable-host-tool/plan.md`
- `src/nxp_cup/nxp_cup_core0/source/main.c`
- `src/nxp_cup/nxp_cup_core0/source/nxpc_system.c`
- `src/nxp_cup/nxp_cup_core0/source/nxpc_framework.c`
- `src/nxp_cup/nxp_cup_core0/source/app/test_mode.c`
- `src/nxp_cup/nxp_cup_core0/source/app/race_mode.c`
- `src/nxp_cup/nxp_cup_core0/source/nxpc__line_processor.c`
- `src/nxp_cup/nxp_cup_core0/source/nxpc_io/`
- `src/common/nxpc_usb_debug/nxpc_usb_debug_protocol.h`
- `src/nxp_cup_host/`
- `src/android/nxp_cup_bridge/`
- `CMakeLists.txt`, `CMakePresets.json`, `setup.ps1`, and root wrappers
