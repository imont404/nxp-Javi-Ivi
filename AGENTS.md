# Agent Guide

This repository contains FRDM-MCXN947 firmware for the Wavenumber NXP Cup project.
Treat `src/nxp_cup/nxp_cup_core0` as the active firmware project and `src/common` as
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

**Student-facing setup and build documentation is `docs/setup.html`.** Read that
first; it covers provisioning a machine from nothing, the preset list, and the
conventions below.

**CMake presets are the default flow.** `build.ps1` is the canonical thin
wrapper; direct CMake remains supported.

```powershell
.\setup.ps1                              # once per machine; no MCUXpresso needed
.\build.ps1                              # competition preset
.\flash.ps1 -Backend JLink               # maintainer example; backend is explicit
.\rtt.ps1 -Reset -Seconds 10
```

**`flash.ps1` and `rtt.ps1` are maintainer tooling, not a student workflow.**
The student path is: build with a CMake preset, flash with **Segger Ozone**
(`src\nxp_cup\nxp_cup_core0\ozone__core0.jdebug`), observe through the on-board LCD and
the USB frame stream. Students are not given a J-Link and do not run these
scripts. Keep both out of `docs/setup.html` — an L0 test enforces it, because
they are the fastest way for an agent to put an image on a board and so they
drift into student docs on their own.

**VS Code is the editor and nothing more.** No tasks, no launch configurations,
no debug integration — one way to build and one way to flash, rather than two
that can disagree. `.vscode/` carries only `settings.json` and `extensions.json`.
Nothing here is load-bearing: if it is wrong the student gets unresolved
includes, not a broken build, so it is not worth a test.

`flash.ps1` and `rtt.ps1` take `-Preset <name>` to pick any other image, and
resolve it to `build\cmake\<preset>\nxp_cup_core0.axf`:

```powershell
.\flash.ps1 -Backend JLink -Preset flexio-port1
.\rtt.ps1   -Preset encoder-diag -Seconds 30
```

The flash command requires `-Backend Ozone`, `Rom`, or `JLink` until the
clean-machine evaluation selects a default. It and RTT refuse an unknown preset by name rather than reporting a missing file, and
they refuse an image that has not been built rather than silently flashing a
stale one. `cmake --list-presets` is the authoritative list. `-File` still takes
an explicit path, and `-Mcux` selects the MCUXpresso output directory for the
rare case of comparing the two build systems. `-CMake` is a deprecated no-op.

Both scripts find `arm-none-eabi-*` the same way the build does: the toolchain
`setup.ps1` provisioned under `out\toolchains`, before any MCUXpresso install.

`.\scripts\maintainer\build_cmake.ps1` remains for ad-hoc `-Define` builds and for `-CheckDrift`
and `-Regenerate` against MCUXpresso project metadata. The CMake flow is adapted
from the W71 project. It generates
`src\nxp_cup\nxp_cup_core0\cmake\mcuxpresso_debug.cmake` from durable MCUXpresso
project metadata: `.cproject` source roots/options plus `.project` linked
resources. Do not read or depend on generated `Debug` makefiles for scripted
build source lists; those files are transient and MCUXpresso recreates them.
The linker scripts are copied into `src\nxp_cup\nxp_cup_core0\link` so the scripted
build does not depend on MCUXpresso regenerating files under `Debug`.

MCUXpresso is **not required to build**. The headless wrapper exists only to
refresh generated makefiles after changing project settings in the IDE, whose
result is then carried into the committed source list by `-Regenerate`:

```powershell
.\scripts\maintainer\build_mcuxpresso.ps1
.\scripts\maintainer\build_mcuxpresso.ps1 -Clean -ResetWorkspace
.\flash.ps1 -Backend JLink -Mcux
```

The MCUXpresso wrapper defaults to
`C:\nxp\MCUXpressoIDE_25.6.136\ide\mcuxpressoidec.exe`, imports
`src\nxp_cup\nxp_cup_core0`, and builds `nxp_cup_core0/Debug` in a generated headless
workspace. Generated `.mcux_workspace*` folders are local build state and should
not be committed.

### Diagnostic build variants

The default build is the **Rev A competition image** and should stay that way:
EZH camera capture, ER-TFT020-3 SPI LCD, session-gated USB telemetry available,
and QDC wheel feedback enabled. USB enumerates when attached, but sends nothing until a
recognized framed host session explicitly subscribes.
Everything else is a variant behind `CONFIG__` selection with `#error` guards.

Prefer a preset:

```powershell
cmake --build --preset competition           # Rev A competition default
cmake --build --preset camera-usb-bench      # bare FRDM, direct EZH camera,
                                             #   USB preview, no shield/LCD
cmake --build --preset flexio-port1          # FlexIO capture on the Rev A camera
                                             #   pins (needs 3 J9_EXT jumpers),
                                             #   frees the EZH
cmake --build --preset flexio-port4          # original FlexIO group, 11 fly-wires
cmake --build --preset encoder-diag          # encoders, motors off, RTT only
cmake --build --preset encoder-diag-motors   # spins the wheels. Car on blocks.
```

The wrapper scripts remain where they do something a fixed preset cannot,
namely tuning the diagnostic at build time:

```powershell
.\scripts\maintainer\build_motor_encoder_diag.ps1 -EnableMotors -PwmPercentM0 10 -PwmPercentM1 20 -AutoStartMs 4000
.\scripts\maintainer\flash_motor_encoder_diag.ps1 -EnableMotors       # must match how it was built
.\scripts\maintainer\rtt_motor_encoder_diag.ps1 -EnableMotors -Seconds 30
```

Driving the two motors at *different* duties is deliberate: equal duty would
hide a cross-wiring or duplicate-read fault between the two QDC channels.

Ad-hoc variants go through `-Define`, e.g.:

```powershell
.\scripts\maintainer\build_cmake.ps1 -BuildDir "build\cmake\nxp_cup_core0-Encoders" `
    -Define "CONFIG__MOTOR_ENCODER_BACKEND=MOTOR_ENCODER_BACKEND_QDC"
```

Motors-on encoder builds go to a **separate output directory** from motors-off,
so the tree tells you which image is there. Duty is capped at 20 percent by an
`#error`. Car on blocks.

The wrapper proliferation is known debt — see `docs/plans/build-system-cleanup`.

### Probe selection

The flash and RTT wrappers default to SEGGER J-Link, device `MCXN947_M33_0`,
and SWD at 4 MHz.

**Probe selection is not hardcoded** — every kit has a different J-Link serial.
`scripts\tools\jlink_common.ps1` resolves it in this order:

1. An explicit `-UsbSerial` argument.
2. `$env:NXPC_JLINK_SERIAL`.
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

Hardware findings, all verified on the bench 2026-07-25 unless noted:

- `docs/research/AVC_Camera_FlexIO_Pin_Migration.md` — FlexIO capture on the Rev A camera
  wiring with three jumpers; frees the EZH and with it core1
- `docs/research/AVC_Motor_Encoder_QDC_Research.md` — encoder feedback, 1320 counts/rev
  measured, wheel-speed API, and the 8.8% motor mismatch
- `docs/research/AVC_RevB_Servo_PWM_Options.md` — the Rev A servo PWM conflict and the
  verified `P3_20` replacement
- `docs/research/AVC_Vision_Pipeline_Design.md` — LUT colour processing, edge detection,
  PowerQuad overlap
- `docs/research/AVC_J12_SmartDMA_Availability.md` — what the unused J12 header can reach
- `docs/research/neutron_npu/` — NPU assessment (conclusion: wrong tool for this year)

Start FlexIO camera work by reading:

- `docs/research/FlexIO_Camera_Test_Plan.md`
- `docs/research/FlexIO_Camera_External_Resources.md`
- `docs/research/an-flexio_camera_rt1010`
- `docs/examples/frdmmcxn947`

The imported examples under `docs/examples/frdmmcxn947` are comparison/reference
projects. Do not treat them as active build targets unless a plan explicitly
adds that work.
