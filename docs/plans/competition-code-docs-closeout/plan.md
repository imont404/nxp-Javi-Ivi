+++
type = "plan"
id = "competition-code-docs-closeout"
status = "pending"
created = "2026-08-21"

[[steps]]
id = "freeze-student-surface"
title = "Consume the nxp-cup-framework-migration firmware-contract checkpoint and frozen public surface"
status = "pending"

[[steps]]
id = "code-api-cleanup"
title = "Audit the frozen camera, motor, servo, encoder, mode, and telemetry facade without creating a second API"
status = "pending"
depends_on = ["freeze-student-surface"]

[[steps]]
id = "student-example-alignment"
title = "Align the minimal student skeleton and telemetry example with the frozen APIs"
status = "pending"
depends_on = ["code-api-cleanup"]

[[steps]]
id = "english-doc-freeze"
title = "Freeze accurate English setup, API, hardware, Rev A, and race-day troubleshooting documentation"
status = "pending"
depends_on = ["student-example-alignment"]

[[steps]]
id = "spanish-docs"
title = "Produce and review Spanish student documentation from the frozen English source"
status = "pending"
depends_on = ["english-doc-freeze"]

[[steps]]
id = "clean-machine-handoff"
title = "Consume the toolchain plan's clean-machine result and verify the downloadable student handoff as one coherent workflow"
status = "pending"
depends_on = ["english-doc-freeze"]

[[steps]]
id = "competition-regression"
title = "Consume the framework plan's Rev A regression evidence and verify the documentation describes it"
status = "pending"
depends_on = ["code-api-cleanup", "clean-machine-handoff"]

[[steps]]
id = "design-doc-intent-audit"
title = "Audit code, hardware, setup, and student documentation against the final implementation and organizer intent"
status = "pending"
depends_on = ["spanish-docs", "competition-regression"]

[[steps]]
id = "test-runtime-impact-audit"
title = "Audit build/test runtime and competition-loop impact of the closeout changes"
status = "pending"
depends_on = ["competition-regression"]

[[steps]]
id = "external-review"
title = "Obtain independent review of the student handoff and frozen competition image"
status = "pending"
depends_on = ["design-doc-intent-audit", "test-runtime-impact-audit"]

[[exit_criteria]]
id = "framework-not-solution"
title = "Students receive clear framework APIs and examples without a line-following, steering, or speed-control solution"
status = "pending"

[[exit_criteria]]
id = "rev-a-truth"
title = "All student material describes the actual Rev A hardware and selected competition build"
status = "pending"

[[exit_criteria]]
id = "bilingual-handoff"
title = "Frozen English and reviewed Spanish instructions cover setup, build, flash, APIs, and troubleshooting"
status = "pending"

[[exit_criteria]]
id = "clean-machine"
title = "A clean Windows machine completes the documented student setup, competition build, and Ozone flash"
status = "pending"

[[exit_criteria]]
id = "competition-regression"
title = "The frozen Rev A image passes the final disconnected and connected bench regression"
status = "pending"

[[exit_criteria]]
id = "design-doc-intent-audit"
title = "Code and documentation match implementation and organizer intent"
status = "pending"

[[exit_criteria]]
id = "test-runtime-impact-audit"
title = "Tests and runtime impact are reviewed"
status = "pending"

[[exit_criteria]]
id = "external-review"
title = "Independent review is complete"
status = "pending"
+++

# Competition Code, API, and Documentation Closeout

## Integration ownership

`docs/plans/nxp-cup-framework-migration` owns the framework implementation, public API,
editable mode files, NXP Cup naming, canonical commands, and Rev A regression. This plan
starts from its named firmware/tooling checkpoints and owns English/Spanish instructional
content and final curriculum coherence. It must not create another API, rename frozen
paths, or repeat the firmware migration.

## Scheduling

The camera-assembly screening workflow and Android relay foundation are complete enough
to stop driving firmware architecture. This is now the next high-priority plan: freeze the small
student algorithm/API surface, reconcile the competition image, and prove the clean
handoff before the first Wednesday class. Translate only after the English student
surface is frozen so corrections are made once.

## Scope and Ownership

This plan owns the final coherence pass across code APIs and student-facing documents. It
does not duplicate implementation owned elsewhere:

- `cmake-build-and-toolchain` owns clean-machine provisioning, presets, and Ozone proof;
- `usb-debug-telemetry` owns the generic telemetry API/example and protocol truth;
- `camera-startup-diagnostics` owns deeper SCCB/signal fault classification if needed;
- `android-telemetry-bridge` remains maintainer/race-display infrastructure, not a student
  prerequisite;
- motor PID, line following, steering policy, and race strategy remain student work.

## Cleanup Standard

Prefer a small obvious API over new abstraction. Keep one low-level initialization pass,
an explicit system-mode switch, a visible student-algorithm call, bounded service work,
and concise comments at the decision points. Remove or quarantine stale experiments only
when their durable hardware findings are already captured in design/research docs.

The documentation must name the real competition configuration: Rev A board, EZH camera
capture by default, ER-TFT020-3 SPI display at the accepted clock, USB stream enabled but
session-gated, encoders disabled in the default image, CMake presets for building, and
Segger Ozone for student flashing. Maintainer J-Link scripts stay out of the student path.

## Spanish Documentation

Translate the frozen meaning, not intermediate wording. Preserve commands, paths, API
names, pin names, and diagnostic codes exactly. Have a Spanish-capable reviewer check the
student workflow and safety language rather than accepting machine translation alone.
