+++
type = "plan"
id = "cmake-build-and-toolchain"
status = "pending"
created = "2026-07-25"

[[steps]]
id = "study-reference"
title = "Extract the reusable pattern from the bunny_vision build system"
status = "done"

[[steps]]
id = "toolchain-installer"
title = "Write setup.ps1 provisioning the Arm toolchain, CMake, and Ninja"
status = "done"
depends_on = ["study-reference"]

[[steps]]
id = "toolchain-file"
title = "Add a CMake toolchain file with ordered toolchain discovery"
status = "done"
depends_on = ["study-reference"]

[[steps]]
id = "governance-tooling"
title = "Adopt the rack signoff suite, lizard, and clang-format/clang-tidy"
status = "done"
depends_on = ["study-reference"]

[[steps]]
id = "break-ide-dependencies"
title = "Remove the two hard MCUXpresso dependencies: the NXP-only newlib and cr_section_macros.h"
status = "done"
depends_on = ["toolchain-file"]

[[steps]]
id = "own-the-source-list"
title = "Replace the .cproject-generated source list with a checked-in CMakeLists"
status = "done"
depends_on = ["toolchain-file"]

[[steps]]
id = "byte-parity"
title = "Prove the standalone toolchain reproduces the MCUXpresso build"
status = "done"
depends_on = ["own-the-source-list", "toolchain-installer", "break-ide-dependencies"]

[[steps]]
id = "cmake-presets"
title = "Replace the wrapper scripts with CMakePresets build configurations"
status = "done"
depends_on = ["byte-parity"]

[[steps]]
id = "vscode-integration"
title = "Make VS Code a working editor: presets visible, IntelliSense resolving. No tasks or debug integration by decision"
status = "done"
depends_on = ["cmake-presets"]

[[steps]]
id = "verify-script"
title = "Add a build-everything verification gate"
status = "done"
depends_on = ["cmake-presets"]

[[steps]]
id = "signoff-gate"
title = "Wire the build and quality strata into the signoff gate"
status = "done"
depends_on = ["governance-tooling", "cmake-presets"]

[[steps]]
id = "retire-mcuxpresso"
title = "Remove the MCUXpresso dependency from the documented workflow"
status = "done"
depends_on = ["byte-parity", "cmake-presets", "vscode-integration"]

[[steps]]
id = "build-tooling-docs"
title = "Document the build system, toolchain, and quality gates"
status = "done"
depends_on = ["cmake-presets", "governance-tooling"]

[[steps]]
id = "student-onboarding"
title = "Prove a clean machine builds the competition image and follows the documented Ozone flash path"
status = "pending"
depends_on = ["retire-mcuxpresso", "build-tooling-docs"]

[[steps]]
id = "design-doc-intent-audit"
title = "Audit design docs, ADRs, and requirements against implementation"
status = "pending"
depends_on = ["student-onboarding"]

[[steps]]
id = "test-runtime-impact-audit"
title = "Audit new test runtime impact"
status = "pending"
depends_on = ["verify-script"]

[[steps]]
id = "external-review"
title = "Obtain independent external review"
status = "pending"
depends_on = ["design-doc-intent-audit", "test-runtime-impact-audit"]

[[exit_criteria]]
id = "one-script-setup"
title = "A clean Windows machine provisions and builds with setup.ps1, then reaches a flashed board through the documented Ozone path"
status = "pending"

[[exit_criteria]]
id = "no-mcuxpresso"
title = "Building, flashing, and debugging require no MCUXpresso installation"
status = "met"

[[exit_criteria]]
id = "build-reproduces"
title = "The standalone toolchain reproduces the MCUXpresso build byte-for-byte, or every difference is explained and accepted"
status = "met"

[[exit_criteria]]
id = "presets-replace-wrappers"
title = "Build variants are CMakePresets rather than a wrapper script each, and the competition image is the obvious default"
status = "met"

[[exit_criteria]]
id = "vscode-works"
title = "VS Code opens the tree, shows the presets, and resolves includes for IntelliSense with no extra configuration"
status = "met"

[[exit_criteria]]
id = "verify-gate"
title = "One command builds every supported target from clean and asserts the artifacts exist"
status = "pending"

[[exit_criteria]]
id = "signoff-runs"
title = "The rack signoff suite runs from one command, with build, complexity, and format checks wired in"
status = "met"

[[exit_criteria]]
id = "tooling-provisioned"
title = "setup.ps1 provisions everything the build and signoff need - Arm toolchain, CMake, Ninja, uv, and the clang tooling"
status = "met"

[[exit_criteria]]
id = "build-docs-exist"
title = "The build system, toolchain provisioning, and quality gates are published as HTML, good enough for a student to follow without asking, with the rack suite asserting the claims"
status = "met"

[[exit_criteria]]
id = "competition-image-safe"
title = "The Rev A competition image is unchanged in behaviour and reproducible throughout"
status = "met"

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

# CMake Build System and Student Toolchain

## Integration ownership

This plan retains CMake generation, provisioning, preset behavior, clean-machine proof
mechanics, and MCUXpresso drift tooling. `docs/plans/nxp-cup-framework-migration` owns the
final `build.ps1` / `flash.ps1` command contract, NXP Cup artifact names, and checkpoint
sequencing. Build changes land here only when they implement that frozen contract; they do
not establish a competing workflow.

## Current Competition-Week Status

The CMake/preset migration, repository-local toolchain, student setup page, Ozone flash
path, and maintainer flash/RTT helpers are implemented. The competition preset built and
flashed successfully again on 2026-08-21 after the splash removal. The camera-screening
workflow and Android relay no longer block this work; the clean-machine proof remains
part of the student-handoff closeout.

Resume on Monday/Tuesday before the first Wednesday class. The next decisive step is a
clean-machine student run from `docs/setup.html`: provision, build `competition`, and
flash with Ozone without relying on MCUXpresso or workstation-global tools. Evaluate an
NSIS/Nullsoft bundle and a centralized download only as optional convenience; do not put
an unproven installer on the critical path. The portable Guatemala laptop setup and an
offline tool cache remain explicit acceptance work.

## Purpose

**Keep MCUXpresso optional, and make the primary student toolchain provisionable by one
script.**

The model already exists and works: `bunny_vision_sw`, another MCXN947 project with a
camera and `bunny_build`, has a raw CMake build that eliminates MCUXpresso and builds much
faster. AVC should end up in the same place for firmware build tooling. Host tooling is
owned by its feature plan, not this one.

Two outcomes:

1. **One build setup, explicit flash path.** A student on a clean Windows machine runs
   `setup.ps1` and can build without MCUXpresso or PATH surgery, then flashes through the
   documented Segger Ozone workflow.
2. **CMake presets instead of wrapper scripts.** Build configurations become data that
   VS Code understands natively, rather than eight PowerShell files that grew one
   experiment at a time.
**Supersedes `build-system-cleanup`**, whose remaining items are absorbed here. Presets
*are* the wrapper consolidation, so keeping both plans would put two owners on one surface.

## The reference implementation

`D:\prj\wavenumber\bunny_vision\bunny_vision_firmware-west\bunny_vision_sw\src`

Treat the reference tree as read-only.

| Piece | What it does |
|---|---|
| `setup.ps1` | Downloads Arm GNU Toolchain into `out/toolchains/`, installs CMake and Ninja via winget if absent. Idempotent, persists no environment changes. |
| `cmake/arm-none-eabi.cmake` | Toolchain file. Discovery order: explicit `-D`, then env var, then the locally provisioned toolchain, then MCUXpresso as fallback. |
| `CMakePresets.json` | Configure and build presets for firmware roles and host tools. What VS Code reads. |
| `CMakeLists.txt` | Orchestrates host and firmware targets behind `option()` switches. |
| `bunny_cam_sim`, `bunny_display_sim` | Host tools; SDL and Dear ImGui pulled in by `FetchContent` behind a `WITH_GUI` option. |
| `verify.ps1` | Configures and builds everything from clean and asserts the artifacts exist. |

## Why a strong safety net exists here

**AVC's MCUXpresso 25.6 bundles Arm GNU Toolchain 14.2.Rel1 — the same version
`setup.ps1` downloads.** Verified:

```
arm-none-eabi-gcc.exe (Arm GNU Toolchain 14.2.Rel1 (Build arm-14.52)) 14.2.1 20241119
```

So this migration has an unusually strong check available: **the standalone toolchain
should reproduce the current build byte-for-byte.** Any difference is then a real finding
about flags or source lists rather than noise. Use it — it is the difference between "the
new build seems fine" and "the new build is the same build."

## Where the work actually is

`build_cmake.ps1` generates `src\avc\avc_core0\cmake\mcuxpresso_debug.cmake` from
`.cproject` source roots and options plus `.project` linked resources. **That file is 310
lines listing 179 source files, derived from MCUXpresso project metadata.**

Eliminating MCUXpresso means **owning that list** — a checked-in `CMakeLists.txt` that does
not need the IDE's project files to exist. For comparison `bunny_cam/CMakeLists.txt` is
212 lines with roughly 45 explicit sources, so AVC pulls in considerably more SDK.

Everything else in this plan is plumbing around that step.

## Two hard dependencies on the IDE, both already confirmed present

These are not theoretical. Both exist in AVC today, and **both will stop a standalone
toolchain build dead.** The reference project hit exactly these and solved them, so the
fixes are known.

### 1. The linker pulls an NXP-IDE-only library

`src/avc/avc_core0/link/avc_core0_Debug_library.ld:16` has:

```
GROUP (
  ...
  "libcr_newlib_nohost.a"
```

**That library ships only with MCUXpresso.** The standalone Arm GNU toolchain does not
have it.

**Known fix, from the reference:** replace it with `libnosys.a`, which is present in both
the MCUXpresso bundle and standalone Arm GNU 14.2.Rel1. The reference verified this is
**output-neutral** — `text`/`data`/`bss` unchanged and the flashable `.bin` byte-identical
across both toolchains. Their firmware referenced nothing from it (no heap, no syscalls);
**confirm the same is true for AVC** rather than assuming, since AVC links more SDK.

### 2. `cr_section_macros.h` does not exist in this repository

`bv_camera__interface.c:14` includes it, and it currently resolves out of the MCUXpresso
install tree:

```
C:
C:\nxp\MCUXpressoIDE_25.6.136\ide\plugins\...\tools\arm-none-eabi\include\cr_section_macros.h
```

It is not incidental — AVC uses `__BSS(SRAM_H)` for the EZH program image and
`__BSS(FRAME_BUFFERS)` for the **camera frame buffers**. Placement of those is not
optional.

**Fix:** copy the header into the repository as the reference did
(`source/shared/cr_section_macros.h` there). Check what else in the tree resolves only out
of the IDE include path while doing it — these two were found by inspection, and there may
be more.

### Static linker scripts — mostly already done

AVC already copies its linker scripts into `src/avc/avc_core0/link/`, so the scripted build
does not depend on MCUXpresso regenerating `Debug/`. That is the same move the reference
made, and it is the reason only the two items above remain.

Worth adopting from the reference: a **header comment in each static script** recording
that it is a capture, where it came from, how to regenerate for comparison, and any
deliberate edit. Their `libnosys.a` change is documented in-file precisely so a later drift
diff shows one expected line instead of an unexplained difference.

## Step Notes

### study-reference

Read the reference before copying it. In particular, understand why `setup.ps1` streams the
zip through the .NET API rather than `Expand-Archive` (pathologically slow on the ~13k-file
Arm toolchain zip, and it looks like a hang), and why the toolchain lands in a git-ignored
`out/` rather than on PATH.

### toolchain-installer

Mirror `setup.ps1`: Arm GNU Toolchain **14.2.Rel1** to match the MCUXpresso bundle, into a
git-ignored machine-local directory, plus CMake and Ninja via winget when not already on
PATH. Idempotent, and it must not modify the user's environment.

**The audience is a student with no idea.** Failure messages must say what to do next, not
what went wrong internally. Assume winget may be absent or blocked on a locked-down laptop
and handle that explicitly rather than failing obscurely.

### break-ide-dependencies

Do this **early and independently** — it is small, it is a hard blocker for everything
after it, and it can be verified against the existing MCUXpresso build before any of the
larger changes land. Swapping `libnosys.a` and vendoring the header should both be
output-neutral; prove that with the current toolchain first, so `byte-parity` is not
debugging two things at once.

### governance-tooling

`wn-dev-std` **2026.7.18 is already the latest resolvable** and is what AVC pins — verified,
and it bundles `wn-rack 2026.7.16`. So "update to latest" is satisfied; the real gap is
that AVC has almost none of the tooling the reference project runs.

| | reference | AVC today |
|---|---|---|
| `tests/` rack suite | L0_foundation, L1_build, L2_quality, L99_signoff | **missing** |
| `signoff.toml` | present, `uv run rack run L99` | **missing** |
| `.clang-format` | present | **missing** |
| clang-tidy | per the `zephyr-firmware` profile | **missing** |
| `lizard` complexity | dev dependency, drives L2 | **missing** |
| dev-std scopes | `docs.plans`, `docs.adrs` | `docs.plans` only |

**Sequencing note that makes this worth doing early rather than last:** the reference's
**L1_build** stratum is a "does everything still build" gate. That is exactly the check
that de-risks `own-the-source-list`, which is the highest-risk step in this plan. Getting
L0 and L1 up front pays for itself; L2 complexity and L99 signoff can follow.

**DECIDED 2026-07-25:** `setup.ps1` provisions everything, including LLVM via winget for
`clang-format` and `clang-tidy`. The extra download is acceptable — one script that
installs everything beats a second-class setup that works until someone runs a gate.

So `setup.ps1` covers: the Arm GNU toolchain, CMake, Ninja, `uv` for the Python tooling,
and LLVM. Same idempotent, no-PATH-surgery rules as the rest.

### build-tooling-docs

**Governance requires this, and it is also the thing that makes the one-script install
real.** A script a student cannot find or does not understand is not an install path.

The reference's `docs/setup.html` is the model, with sections covering: prerequisites,
toolchain provisioning (no MCUXpresso required), workspace checkout, build, flash and
bench, host tool quick checks, conventions, and quality gates. That is the right table of
contents.

Cover at minimum:

- **Getting started from nothing** — run one script, then build, then flash. What a student
  reads first, written for someone who has never seen the repo.
- **The build variants** — what each preset is for, and which one is the competition image.
- **Toolchain provisioning** — what `setup.ps1` installs, where it puts it, and that it
  changes no environment. Also what to do when winget is blocked on a locked-down laptop.
- **Quality gates** — how to run the rack strata and the signoff suite, and what each is
  checking.
- **The linker capture convention** — that `link/` holds static captures, where they came
  from, and how to regenerate for a drift diff.

Published as **HTML** with a small shared stylesheet — see the decision below.

### DECIDED 2026-07-25: HTML, and it is less disruptive than it sounds

**Published docs are HTML.** The reference is not a wholesale conversion — it uses each
format where it fits, and AVC's existing structure already matches:

| Format | Used for | AVC today |
|---|---|---|
| **HTML** | `setup.html`, `architecture.html`, `governance/` (ADRs, published plans, index) | none yet — this is the new work |
| **Markdown** | plan documents, work logs, research notes, firmware notes | already the case under `docs/plans` and `docs/research` |

So **no mass migration.** The Markdown research corpus stays exactly as it is; HTML is for
the published entry points a student or reviewer lands on.

**Authoring is plain hand-written HTML** — no generator, no Sphinx, no MkDocs. A shared
stylesheet does the work, and the reference's is **eleven lines**: CSS variables for ink,
paper, accent and rule, then body, headings, code, and table rules. Pages carry no inline
styling. That is the whole convention, and it is worth keeping that small — a doc toolchain
is another thing to install and another thing to break four weeks before a race.

**One pattern worth stealing outright.** The reference's `setup.html` opens with:

> Machine-checkable invariants for this page are held by the rack suite (`tests/`).

The doc makes claims and the rack suite asserts them, so setup instructions cannot rot
silently into fiction. That is the difference between documentation and a promise. Do the
same: whatever `setup.html` says the one-script install produces, L0 or L1 should verify.

Also copy `docs/quality/lizard-baseline.txt`, so complexity debt is a tracked baseline
rather than a pass/fail cliff.

### own-the-source-list

The riskiest step. Suggested approach: generate the list once from the current
`.cproject`-derived output, check it in, then **diff every later regeneration against it**
so drift is visible rather than silent. Do not hand-curate 179 paths in one pass.

Keep `build.ps1`, the MCUXpresso headless fallback, working until `byte-parity` passes.

### byte-parity

Build the Rev A competition image with the MCUXpresso toolchain and with the provisioned
one, and compare. Identical `text`/`data`/`bss` is a good signal; an identical binary is
the real one. Where build paths get embedded, expect and explain the difference rather than
waving at it.

**Do this before the presets work**, so any later divergence has a known-good baseline.

### cmake-presets

Presets replace the wrapper scripts. The competition image must be the obvious default,
not one option among many.

Absorbed from `build-system-cleanup`: decide which variant combinations are actually
supported, and make illegal combinations fail at configure or compile time rather than at
runtime. The `#error` guards in `avc__master_config.h` already do much of this.

### vscode-integration

Presets are most of it; VS Code's CMake Tools reads `CMakePresets.json` directly. The
decision is now fixed: `.vscode/` contains recommendations and settings only, IntelliSense
resolves the SDK includes, and flashing stays in Ozone rather than adding a second VS Code
debug/build workflow.

Students edit here. **If IntelliSense cannot resolve `avc__line_processor.h`, they will
assume the code is broken**, and they will be right to.

### student-onboarding

The real test is not that it builds on this machine. **Take a clean Windows machine with no
NXP software, run `setup.ps1`, build the competition preset, then follow the documented
Ozone path to a flashed board.** Time it, and write down every point where a student would
have to ask a question.

## Competition-week closeout

The plan splits cleanly into what students need and what we want. **Only the first is
race-critical.**

### Critical path — students cannot work without this

| Step | Why it is required |
|---|---|
| `toolchain-installer` | The one script. This is the whole point. |
| `toolchain-file` | Nothing builds without toolchain discovery. |
| `break-ide-dependencies` | Two hard blockers: the NXP-only newlib and the vendored header. Small, and everything after depends on it. |
| `own-the-source-list` | The actual MCUXpresso break. Highest risk, earliest possible start. |
| `byte-parity` | The gate that says the new build is the same build. |
| `cmake-presets` | How students select a build; also what VS Code reads. |
| `vscode-integration` | Students edit here. IntelliSense must resolve. |
| `retire-mcuxpresso` | Removes the install students would otherwise need. |
| `build-tooling-docs` | Governance requires it, and a script nobody can find is not an install path. |
| `student-onboarding` | Clean machine through setup, build, and the documented Ozone flash. The only test that counts. |

**Recommended order:** finish the clean-machine student onboarding and closeout audits
before the first class. Host tooling belongs to `usb-debug-telemetry`; capture-backend
refactoring belongs to a later code/API cleanup plan.

Evaluation of an NSIS/Nullsoft bundle, a centralized tool-download page, and renewed
MCUXpresso fallback validation is deliberately deferred to the Monday/Tuesday onboarding
work. Do not make the Wednesday student path depend on an untested installer assembled at
the last minute.

## Constraints

- **The race is late August 2026.** The Rev A competition image must stay behaviourally
  unchanged and reproducible throughout.
- **DECIDED 2026-07-25: this is for the August 2026 event, starting now.** That makes it
  **race-critical event infrastructure with roughly four weeks**, not a cleanup exercise.
  Two consequences follow, and they pull in opposite directions:
  - The safety net matters more, not less. Keep the MCUXpresso path working until
    `byte-parity` passes, and treat the competition image as untouchable.
  - **Scope has to be cut to the critical path.** See the sequencing section below.
- Treat `bunny_vision_sw` as read-only reference material.
- Keep the MCUXpresso path working until `byte-parity` passes, so there is always a way
  back.

## Source Material

- `bunny_vision_sw/src/` — the reference: `setup.ps1`, `verify.ps1`, `CMakePresets.json`,
  `cmake/arm-none-eabi.cmake`, `bunny_cam/CMakeLists.txt`, and the two sim tools
- `build_cmake.ps1` and `src/avc/avc_core0/cmake/generate_mcuxpresso_cmake.py` — what is
  being replaced
- `src/avc/avc_core0/cmake/mcuxpresso_debug.cmake` — the 310-line generated source list
- `AGENTS.md` — the current documented build and flash flow
- `docs/research/AVC_Competition_Overview.md` — who the audience is and why it matters
