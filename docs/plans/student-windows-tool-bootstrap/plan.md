+++
type = "plan"
id = "student-windows-tool-bootstrap"
status = "active"
created = "2026-08-25"

[[steps]]
id = "baseline-clean-export"
title = "Run the current setup, firmware build, host build, and self-test from a clean exported folder and record relative-path, download, duration, and artifact behavior"
status = "done"

[[steps]]
id = "freeze-release-contract"
title = "Freeze the anonymous Windows x64 core-tools asset contents, naming, versioning, licensing, manifest, checksum, compatibility, and explicit J-Link exclusion"
status = "done"
depends_on = ["baseline-clean-export"]

[[steps]]
id = "deterministic-host-package"
title = "Make the host package deterministic and versioned, with viewer, CLI, SDL2, pinned rblhost, licenses, source commit, file hashes, and bench-free self-test evidence"
status = "done"
depends_on = ["freeze-release-contract"]

[[steps]]
id = "one-command-release-script"
title = "Add one guarded maintainer script that compiles, tests, packages, checksums, and publishes a versioned GitHub Release asset with explicit dry-run and publish modes"
status = "done"
depends_on = ["deterministic-host-package"]

[[steps]]
id = "publish-first-core-tools-release"
title = "Publish and independently download-verify the first immutable, anonymously accessible Windows core-tools GitHub Release without changing repository remotes"
status = "active"
depends_on = ["one-command-release-script"]

[[steps]]
id = "harden-base-tool-provisioning"
title = "Pin and verify Arm GCC, require usable CMake and Ninja, refresh only the setup process environment after installs, show download progress, and fail visibly when required tools are unavailable"
status = "pending"
depends_on = ["freeze-release-contract"]

[[steps]]
id = "pinned-core-tools-install"
title = "Teach setup.ps1 to download the pinned release asset, verify its SHA-256 and manifest, stage extraction safely, install under out/artifacts/host, and run nxpc_tool selftest idempotently"
status = "pending"
depends_on = ["publish-first-core-tools-release", "harden-base-tool-provisioning"]

[[steps]]
id = "offline-cache"
title = "Support a preseeded or explicitly supplied offline archive and cache downloads under out without weakening checksum or manifest verification"
status = "pending"
depends_on = ["pinned-core-tools-install"]

[[steps]]
id = "rom-flash-recovery"
title = "Make ROM-HID the complete student flash path, preserve useful ROM errors, document physical SW3/reset recovery, and keep J-Link as a separately installed optional maintainer fallback"
status = "pending"
depends_on = ["pinned-core-tools-install"]

[[steps]]
id = "student-setup-docs"
title = "Replace stale setup instructions with the authoritative setup-build-flash-viewer flow, exact artifacts, offline use, preflash assumption, and physical recovery"
status = "pending"
depends_on = ["offline-cache", "rom-flash-recovery"]

[[steps]]
id = "host-contributor-docs"
title = "Document the advanced host contributor workflow for modifying, building, testing, packaging, and locally validating the native viewer and the separate generated WebSerial viewer"
status = "pending"
depends_on = ["deterministic-host-package", "one-command-release-script"]

[[steps]]
id = "automated-contract-tests"
title = "Add machine-checkable setup, archive-integrity, idempotency, documented-command, artifact-discovery, packaging, and release-script dry-run coverage without live publication"
status = "pending"
depends_on = ["student-setup-docs", "host-contributor-docs"]

[[steps]]
id = "clean-machine-and-board-acceptance"
title = "Prove online and offline setup on clean Windows, build competition firmware, program and reconnect through J11, and recover a nonfunctional application through physical ROM ISP without J-Link"
status = "pending"
depends_on = ["automated-contract-tests"]

[[steps]]
id = "design-doc-intent-audit"
title = "Audit setup, release, flash, host ownership, student boundaries, and active plan language against the implemented workflow"
status = "pending"
depends_on = ["clean-machine-and-board-acceptance"]

[[steps]]
id = "test-runtime-impact-audit"
title = "Record setup download sizes and durations, cache behavior, build and test costs, generated outputs, and hardware-dependent coverage"
status = "pending"
depends_on = ["automated-contract-tests"]

[[steps]]
id = "external-review"
title = "Obtain independent review of release integrity, licensing, setup failure behavior, offline handoff, ROM recovery, contributor documentation, and student usability"
status = "pending"
depends_on = ["design-doc-intent-audit", "test-runtime-impact-audit"]

[[exit_criteria]]
id = "anonymous-release"
title = "A versioned immutable Windows x64 core-tools asset is anonymously downloadable and tied to a source commit, manifest, licenses, and SHA-256"
status = "pending"

[[exit_criteria]]
id = "one-command-release"
title = "One guarded maintainer command builds, tests, packages, checksums, and publishes the core-tools release while a dry run performs no external mutation"
status = "pending"

[[exit_criteria]]
id = "one-script-setup"
title = "From a clean folder, setup.ps1 provisions Arm GCC, CMake, and Ninja and installs verified prebuilt host tools under out without requiring LLVM, Rust, Visual Studio, MCUXpresso, or J-Link"
status = "pending"

[[exit_criteria]]
id = "artifact-discovery"
title = "The competition build publishes the normal BIN and AXF, the viewer selects the published BIN automatically, and flash uses the expected image without manual path entry"
status = "pending"

[[exit_criteria]]
id = "offline-handoff"
title = "The same setup completes from a preseeded cache with networking unavailable and rejects missing, modified, or incomplete archives"
status = "pending"

[[exit_criteria]]
id = "rom-only-student-recovery"
title = "A preflashed board programs and reconnects through J11, and a nonfunctional application is recovered through physical ROM ISP without J-Link"
status = "pending"

[[exit_criteria]]
id = "student-and-contributor-docs"
title = "A new student can set up, build, flash, view, and recover, while an advanced contributor can modify, test, package, and locally validate the host viewer from source"
status = "pending"

[[exit_criteria]]
id = "signoff"
title = "Focused setup, package, host, firmware, documentation, and clean-machine signoff passes"
status = "pending"

[[exit_criteria]]
id = "design-doc-intent-audit"
title = "Design documents, active plans, setup claims, and component ownership match the implemented workflow"
status = "pending"

[[exit_criteria]]
id = "test-runtime-impact-audit"
title = "Download, extraction, build, test, cache, generated-output, and hardware-dependent costs are recorded"
status = "pending"

[[exit_criteria]]
id = "external-review"
title = "Independent review of integrity, licensing, recovery, offline use, documentation, and student usability is complete"
status = "pending"
+++

# GitHub-Released Windows Core Tools and Student Setup

## Integration ownership

This plan owns the Windows student bootstrap, GitHub Release production, offline cache,
setup and contributor documentation, and clean-machine proof. It consumes the proven native
viewer and ROM-HID programmer from `one-cable-host-tool` and the standalone firmware build
from `cmake-build-and-toolchain`; it does not create another firmware build or flash entry
point. Product and wire compatibility remain owned by the framework plans.

The repository root retains one student setup entry point, `setup.ps1`. Component builds
remain under `src`. Repository-owned generated tools and artifacts remain under ignored
`out`; release archives are attached to GitHub Releases rather than committed to Git.

## Student outcome

The complete ordinary workflow is:

```powershell
.\setup.ps1
.\src\embedded\build.ps1
.\src\embedded\flash.ps1
.\out\artifacts\host\nxpc_viewer.exe
```

`setup.ps1` provisions Arm GNU Toolchain 14.2.Rel1, usable CMake, and usable Ninja. It then
downloads, verifies, and extracts a pinned Windows x64 core-tools release. Students do not
need LLVM-MinGW, Rust, Cargo, Visual Studio, MCUXpresso, Secure Provisioning, Ozone, or
J-Link for the normal path. Maintainer tooling may remain explicitly available.

All student boards are preflashed with an ENTER_ISP-capable competition image before
handoff. A running application safe-stops and transfers to ROM ISP. A broken application is
recoverable through the documented SW3/reset sequence on J11. J-Link remains a separately
downloaded optional maintainer fallback and is not redistributed in the core-tools asset.

## Core-tools release contract

The initial asset is named like:

```text
nxp-cup-core-tools-win-x64-1.0.0.zip
```

It contains exactly the runnable host surface and its required notices:

```text
nxpc_viewer.exe
nxpc_tool.exe
SDL2.dll
rblhost.exe
rblhost-LICENSE.txt
SDL2-LICENSE.txt
Dear-ImGui-LICENSE.txt
HOST-README.md
manifest.json
```

`manifest.json` records the package version, source commit, platform and architecture,
build-tool versions, programmer provenance, and every file's size and SHA-256. The release
also publishes the outer ZIP SHA-256. The setup pin records an immutable release tag,
explicit asset URL, and expected outer hash; it never resolves `latest`.

The asset must be anonymously downloadable. If the main repository is private, publish it
from an organizer-controlled public distribution repository rather than requiring student
GitHub credentials or tokens.

## One-command maintainer release

Add one durable maintainer entry point, named along these lines:

```powershell
.\src\host\release.ps1 -Version 1.0.0          # local build/test/package dry run
.\src\host\release.ps1 -Version 1.0.0 -Publish # create and publish the GitHub release
```

The script performs preflight, native host build, host tests, generated-WebSerial drift
check, CLI self-test, deterministic packaging, manifest and checksum verification, GitHub
authentication checks, release creation, asset upload, and download verification. It
refuses a dirty source tree, an existing tag or release, absent licenses, failed tests,
unexpected dependency versions, or a mismatched downloaded asset. Dry run is the default
and performs no external mutation. Publishing requires explicit `-Publish`; the agent does
not change remotes or publish while implementing and validating this plan.

The first release may be produced manually through this script. CI publication is optional
after the local release path is proven; do not make GitHub Actions a second release process.

## Setup behavior

Keep version, URL, and hash pins in one checked-in data file consumed by setup and tests.
Downloads are retained under `out/downloads`; installed build tools stay under
`out/toolchains`; the host runtime is staged and then installed under
`out/artifacts/host`. Never replace a known-good installed runtime until the new archive,
manifest, files, versions, and self-test all pass.

Online setup downloads from the pinned release. Offline setup accepts an explicit local
archive or finds the exact pinned archive in the cache, then applies the same verification.
`-Force` may redownload, while a matching installed manifest makes ordinary reruns fast and
network-free. Required failures return nonzero and state the next corrective action. Setup
must not print completion when a required compiler, build tool, archive, or runtime is
missing.

The current Arm archive is 305,393,359 bytes (305.4 MB decimal, 291.2 MiB). The baseline
test must record download and extraction time, extracted size, visible progress, and cache
behavior so classroom expectations are accurate. The current long silent BITS interval is
a known usability issue to address.

## Documentation boundary

The authoritative student setup documentation covers prerequisites, setup, build, flash,
viewer launch, artifact paths, one-board selection, online and offline operation, the
preflash assumption, physical ROM recovery, and an optional official SEGGER download link.
Every documented command is checked against the tree.

An advanced host contributor section separately explains CMake, Ninja, LLVM-MinGW, native
source ownership, Dear ImGui and SDL2 dependencies, host tests, artifact discovery,
packaging, local release dry runs, and the generated WebSerial viewer rule. It must remain
clear that ordinary students consume the prebuilt host bundle and do not need those tools.

## Validation strategy

The first test runs an exported tracked-file snapshot outside the repository root so its
duplicated `docs/plans` tree cannot confuse plan auditing. It runs the current setup and both
normal builds, confirms the embedded compiler resolves inside that exported repository,
runs `nxpc_tool selftest`, and records artifact paths and durations. This is a relative-path
baseline, not clean-machine proof, because workstation-global tools remain visible.

Automated tests use local archives and dry-run publication; they do not require live
networking or mutate GitHub. They cover valid install, corrupt outer hash, corrupt inner
manifest, missing runtime files, wrong version, interrupted staging, known-good rollback,
idempotent rerun, offline cache, setup failure codes, documented commands, and host firmware
discovery.

Final acceptance uses clean Windows 10/11 with no NXP or SEGGER software and no LLVM, Rust,
or Visual Studio. Run online setup, build the competition image, program through J11,
verify full readback and reconnect, then deliberately install a nonfunctional application
and recover through physical ROM ISP. Repeat setup from the offline cache with networking
disabled. Record every prompt, duration, download, artifact, and recovery step.

## Constraints

- Preserve the competition firmware behavior, `AVCU` v1 bytes, and USB VID/PID.
- Do not add line following, PID, active differential, or a race solution.
- Do not redistribute SEGGER J-Link software without written authorization.
- Do not require GitHub credentials for student setup.
- Do not commit generated release ZIPs or restore root component build wrappers.
- Do not change repository remotes or publish as an incidental implementation step.
- Keep runtime behavior bounded and failures explicit enough for first-time students.
