+++
type = "plan"
id = "repository-structure-cleanup"
status = "active"
created = "2026-08-25"

[[steps]]
id = "preserve-learning-work"
title = "Review, validate, and commit an explicit whitelist of the current lecture-pages lesson sources, generated standalone pages, pinned runtime assets, tests, plan updates, logs, and this cleanup plan without committing unrelated docs, caches, or node_modules"
status = "done"

[[steps]]
id = "reconcile-target-baseline"
title = "Bring the two post-split avc commits into local nxp_cup history and integrate the preserved learning commit on a dedicated cleanup branch without changing or pushing remotes"
status = "done"
depends_on = ["preserve-learning-work"]

[[steps]]
id = "component-layout"
title = "Create src/README.md; establish src/embedded, src/host, src/android, and src/common component boundaries; keep src/common fixed and retain nxp_cup_core0 one level below embedded so MCUXpresso linked-resource depth remains stable"
status = "done"
depends_on = ["reconcile-target-baseline"]

[[steps]]
id = "embedded-build-relocation"
title = "Move firmware CMake configuration, presets, build entry point, and maintainer flash, RTT, MCUXpresso, and diagnostic tools under src/embedded; explicitly repair repo-root, generator, toolchain, test, and Ozone paths while preserving the competition default"
status = "done"
depends_on = ["component-layout"]

[[steps]]
id = "consumer-build-cleanup"
title = "Rename nxp_cup_host to src/host, make explicit LLVM-MinGW the canonical Windows host compiler, consolidate host build/package/serve entry points, and colocate Android setup/build plus bench/deploy tools under src/android"
status = "done"
depends_on = ["component-layout"]

[[steps]]
id = "root-and-output-cleanup"
title = "Leave only repository-wide entry points and configuration at root, retain one Windows setup entry point, and converge repository-owned toolchains, build products, test output, and distributable artifacts under out while leaving ignored tool-managed component caches conventional"
status = "done"
depends_on = ["embedded-build-relocation", "consumer-build-cleanup"]

[[steps]]
id = "path-and-agent-contracts"
title = "Update active code, tests, build metadata, AGENTS.md files, src component READMEs, and minimum live plan/design path references while deferring broad documentation reorganization"
status = "done"
depends_on = ["root-and-output-cleanup"]

[[steps]]
id = "build-regression"
title = "Prove every retained firmware preset and artifact, CMake metadata drift, fresh reset MCUXpresso headless build, explicit LLVM host build/package/tests, learning-page generation/browser tests, Android unit/APK build, and full L0/L1/L2/L99/dev-standard gates"
status = "done"
depends_on = ["path-and-agent-contracts"]

[[steps]]
id = "structure-checkpoint"
title = "Commit a clean local repository-structure checkpoint with no lost learning work, no stale active build paths, and recorded validation evidence; do not push or create downstream worktrees yet"
status = "active"
depends_on = ["build-regression"]

[[steps]]
id = "design-doc-intent-audit"
title = "Audit the new component boundaries, student/maintainer entry points, and deferred documentation scope against organizer intent"
status = "pending"
depends_on = ["structure-checkpoint"]

[[steps]]
id = "test-runtime-impact-audit"
title = "Record the validation commands, durations, generated-output locations, and any tests deliberately left hardware-dependent"
status = "pending"
depends_on = ["build-regression"]

[[steps]]
id = "external-review"
title = "Obtain independent review of the move map, build entry points, ignored/generated files, Git history preservation, and parallel-worktree readiness"
status = "pending"
depends_on = ["design-doc-intent-audit", "test-runtime-impact-audit"]

[[exit_criteria]]
id = "learning-work-preserved"
title = "The authored and generated learning tools are committed and reproducible, while node_modules, caches, and transient test output remain untracked"
status = "pending"

[[exit_criteria]]
id = "component-navigation"
title = "A new contributor can identify embedded, host, Android, and shared code from src/README.md and find one normal build entry point in each buildable component"
status = "pending"

[[exit_criteria]]
id = "root-clean"
title = "The repository root contains only project-wide configuration, README.md, AGENTS.md, and the intentional Windows setup entry point; component build and maintainer scripts live with their component"
status = "pending"

[[exit_criteria]]
id = "embedded-compatibility"
title = "Every retained firmware preset builds after relocation and fresh MCUXpresso, CMake metadata drift, linked-resource, artifact, and Ozone path checks pass"
status = "pending"

[[exit_criteria]]
id = "consumer-builds"
title = "The canonical LLVM-MinGW host build, learning-page checks, and Android unit/APK build pass from their documented component entry points"
status = "pending"

[[exit_criteria]]
id = "history-and-worktree-ready"
title = "The post-split firmware work and lecture work are both present in clean local history, the old checkout remains recoverable, and the checkpoint can safely seed separate embedded, host, Android, and docs worktrees"
status = "pending"

[[exit_criteria]]
id = "design-doc-intent-audit"
title = "Component boundaries and entry points match organizer intent without prematurely reorganizing documentation or changing the firmware API"
status = "pending"

[[exit_criteria]]
id = "test-runtime-impact-audit"
title = "Validation coverage, duration, output locations, and hardware-dependent omissions are recorded"
status = "pending"

[[exit_criteria]]
id = "external-review"
title = "Independent review is complete"
status = "pending"
+++

# Repository Structure and Build Cleanup

## Purpose

Establish `nxp_cup` as the clean canonical repository before firmware API and TEST-mode
work resumes. Preserve the completed learning-tool work first, reconcile the latest local
firmware history, then organize source and build entry points around four understandable
components: embedded MCXN947 firmware, Windows/browser host tools, Android relay, and
shared libraries.

## Target Shape

```text
src/
  README.md
  embedded/
    README.md
    AGENTS.md
    build.ps1
    CMakeLists.txt
    CMakePresets.json
    nxp_cup_core0/
    tools/
  host/
    README.md
    AGENTS.md
    build.ps1
    package.ps1
    serve.ps1
  android/
    README.md
    AGENTS.md
    setup.ps1
    build.ps1
    tools/
  common/
    README.md
```

Keep the MCUXpresso project at `src/embedded/nxp_cup_core0` for this deadline. Its
linked-resource paths currently rely on the project being two levels below the repository
`src` directory; flattening it would add unnecessary project-metadata risk.

## Build and Output Contract

- Root `setup.ps1` remains the one Windows base-tool bootstrap for Arm GCC, CMake, Ninja,
  `uv`, and LLVM-MinGW. Android license acceptance remains explicit under `src/android`.
- Each buildable `src` component owns its normal `build.ps1` and component-specific tools.
- LLVM-MinGW is the explicit canonical native host compiler rather than an `Auto`
  preference. Remove or quarantine superseded MSVC-only experiments only after the
  CMake/Clang route produces and packages both the viewer and CLI.
- Repository-owned generated state converges under ignored `out/toolchains`,
  `out/build`, `out/test-results`, and `out/artifacts` directories. Tool-managed caches
  such as npm `node_modules`, Gradle `app/build`, and pytest caches may remain in their
  conventional component locations when ignored.
- The ordinary embedded command continues to build the Rev A `competition` image. Special
  diagnostics remain maintainer-only and cannot become student-facing defaults.

## Execution Safety

The existing dirty `lecture-pages` checkout is the only copy of some learning work. Do
not switch, reset, clean, move, or rebase that checkout until its intentional files are
reviewed, validated, and committed through an exact whitelist; never use `git add .` in
the concurrently edited checkout. Record that snapshot commit and then leave the checkout
alone. Fetch the exact local `avc` history without configuring a remote, create a separate
cleanup worktree at `e0ef4be`, and cherry-pick the learning snapshot there. Preserve the
old `avc` checkout and do not alter or push either remote as part of this plan.

Before and after moving the embedded project, audit every `PARENT-2-PROJECT_LOC/common`
and generated `../../common` resolution. `src/common` must not move in this deadline.
Moved PowerShell tools must calculate the repository root from their new depth rather
than retaining old `../..` assumptions.

## Scope Boundary

This plan makes only the documentation edits required to keep active paths, build
instructions, and agent guidance truthful. Broad `docs` taxonomy cleanup, publication
structure, lesson expansion, repository pushing, downstream worktree creation, public
firmware/API renaming, and the planned TEST_CAMERA/TEST_ACTUATORS implementation remain
separate follow-up work.
