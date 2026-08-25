# Embedded Agent Guide

`nxp_cup_core0` is the active FRDM-MCXN947 firmware project. `../common`
contains linked shared source; keep `src/common` fixed because MCUXpresso linked
resources rely on this component remaining at the same directory depth.

The normal student-editable files are:

- `nxp_cup_core0/source/app/test_mode.c`
- `nxp_cup_core0/source/app/race_mode.c`

`test_mode.c` contains a small CAMERA / IO, VISION, and MOTORS dispatcher so
students can see representative input, graphics, telemetry, motor, and steering
API calls together. The framework still owns page navigation, status rendering,
arming, centered-pot interlock, output limits, command leases, and safe stops.
Additional top-level `.c` files under `source/app` are discovered automatically
by CMake and MCUXpresso; keep their headers beside them and include only the
public `nxp_cup.h` API from student modules.

Do not expose camera drivers, USB internals, PWM, QDC, framework-private headers,
or maintainer scripts as alternate student APIs. Preserve framework-owned motor
safe-stop, command leases, camera-buffer lifetime, mode transitions, and typed
telemetry.

Build with `.\src\embedded\build.ps1`. The adjacent `flash.ps1` defaults to
ROM-HID and falls back to J-Link Commander. Tools under `tools` are
maintainer-only. Keep the Rev A competition configuration as the default and
keep diagnostics explicit.
