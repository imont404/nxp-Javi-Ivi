# Wavenumber NXP Cup

This repository contains the organizer-supplied NXP Cup platform used at FIT:
MCXN947 firmware, Windows/browser host tools, an Android telemetry relay, shared
libraries, tests, and teaching material.

## Windows setup

From a PowerShell terminal at the repository root:

```powershell
.\setup.ps1
```

The setup script provisions the Arm GNU compiler, CMake, Ninja, `uv`, and
LLVM-MinGW. Toolchains and generated state stay under the ignored `out` directory;
the script does not persist environment variables.

Android SDK licenses require a separate explicit setup:

```powershell
.\src\android\setup.ps1 -AcceptLicenses
```

## Build entry points

```powershell
.\src\embedded\build.ps1   # Rev A competition firmware
.\src\embedded\flash.ps1   # ROM-HID first, then J-Link Commander
.\src\host\build.ps1       # Windows camera/telemetry viewer and CLI
.\src\android\build.ps1    # Android unit tests and debug APK
```

Generated artifacts are published under `out\artifacts`. The normal student
firmware loop is edit, build, and run `flash.ps1`. Flashing prefers ROM-HID and
automatically falls back to the command-line J-Link backend. RTT and lower-level
maintainer tools live under `src\embedded\tools`.

See [`src/README.md`](src/README.md) for the source-tree map. Students and LLM
assistants should read the nearest `AGENTS.md` before editing a component.
