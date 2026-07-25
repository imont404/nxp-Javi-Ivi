# Agent Guide

This repository contains FRDM-MCXN947 firmware for the Wavenumber AVC project.
Treat `src/avc/avc_core0` as the active firmware project and `src/common` as
linked shared source used by the MCUXpresso project.

## Setup

Use `uv` for Wavenumber dev-standard tooling:

```powershell
uvx --from wn-dev-std==2026.7.18 dev-std --version
uvx --from wn-dev-std==2026.7.18 dev-std audit . --scope docs.plans
```

The root `pyproject.toml` exists for tooling and dev-standard configuration.
This firmware repository is not packaged as a Python distribution.

## Plans

Use dev-std active plans under `docs/plans`:

```powershell
uvx --from wn-dev-std==2026.7.18 dev-std plan list
uvx --from wn-dev-std==2026.7.18 dev-std plan create <plan-id> --title "<title>"
uvx --from wn-dev-std==2026.7.18 dev-std plan show <plan-id>
uvx --from wn-dev-std==2026.7.18 dev-std log create <plan-id> <step-id> --body "<note>"
```

Do not create new active plans in `docs/research` or `docs/examples`. Existing
research files are durable/reference material during the current migration.

## Build

Use the root CMake wrapper for normal scripted builds:

```powershell
.\build_cmake.ps1
.\flash.ps1 -CMake
.\rtt.ps1 -CMake -Reset -Seconds 10
```

The CMake flow is adapted from the W71 project. It generates
`src\avc\avc_core0\cmake\mcuxpresso_debug.cmake` from durable MCUXpresso
project metadata: `.cproject` source roots/options plus `.project` linked
resources. Do not read or depend on generated `Debug` makefiles for scripted
build source lists; those files are transient and MCUXpresso recreates them.
The linker scripts are copied into `src\avc\avc_core0\link` so the scripted
build does not depend on MCUXpresso regenerating files under `Debug`.

Use the MCUXpresso headless wrapper as the fallback and to refresh generated
makefiles after project setting changes:

```powershell
.\build.ps1
.\build.ps1 -Clean -ResetWorkspace
.\flash.ps1
.\rtt.ps1 -Seconds 10
```

The MCUXpresso wrapper defaults to
`C:\nxp\MCUXpressoIDE_25.6.136\ide\mcuxpressoidec.exe`, imports
`src\avc\avc_core0`, and builds `avc_core0/Debug` in a generated headless
workspace. Generated `.mcux_workspace*` folders are local build state and should
not be committed.

The flash and RTT wrappers default to SEGGER J-Link, device `MCXN947_M33_0`,
and SWD at 4 MHz.

**Probe selection is not hardcoded** — every kit has a different J-Link serial.
`scripts\tools\jlink_common.ps1` resolves it in this order:

1. An explicit `-UsbSerial` argument.
2. `$env:AVC_JLINK_SERIAL`.
3. Auto-detect, when exactly one SEGGER probe is attached.

If more than one probe is attached and none was specified, the wrappers **throw
and list what they found** rather than guessing. Without that, J-Link Commander
picks whichever probe enumerates first, which on a workstation driving two
boards will silently flash the wrong one. A student with a single kit needs no
configuration.

If the configured SEGGER path is missing, the
wrappers auto-detect the newest installed `C:\Program Files\SEGGER\JLink_V*`
tool path. `rtt.ps1` defaults to a PyLink monitor
under `scripts\tools\rtt_monitor.py`; it derives `_SEGGER_RTT` from the current
AXF with `arm-none-eabi-nm`, starts RTT at that address, and can reset the
target with `-Reset`. Use `-Backend Logger` to fall back to SEGGER
`JLinkRTTLogger`.

If a headless build reports undefined references to `bq__*`, `SEGGER_RTT_*`,
`FONT_*`, or `bunny_build*`, rerun with `-ResetWorkspace`. That failure means
the Eclipse workspace import is stale and omitted linked shared-source objects.

## Reference Material

**Read `docs/research/AVC_Competition_Overview.md` first.** It covers what this project is
for: the NXP Cup / FIT student race, the 55 cm track, the chassis and electronics, the
41 ms frame budget, and the constraint that this is an organizer-supplied platform tuned by
novices in about three days. Every plan assumes that context.

For NPU / camera-processing questions, see `docs/research/neutron_npu/`.

Start FlexIO camera work by reading:

- `docs/research/FlexIO_Camera_Test_Plan.md`
- `docs/research/FlexIO_Camera_External_Resources.md`
- `docs/research/an-flexio_camera_rt1010`
- `docs/examples/frdmmcxn947`

The imported examples under `docs/examples/frdmmcxn947` are comparison/reference
projects. Do not treat them as active build targets unless a plan explicitly
adds that work.
