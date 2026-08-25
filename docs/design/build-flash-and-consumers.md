# Build, Flash, and Consumer Layout

## Normal firmware build

Provision once, then use the canonical wrapper:

```powershell
.\setup.ps1
.\build.ps1
.\build.ps1 -Clean
```

`build.ps1` configures and builds the one supported competition image. Direct
`cmake --preset competition` plus `cmake --build --preset competition` remains
equivalent. The outputs are:

- `build/cmake/competition/nxp_cup_core0.axf`
- `build/cmake/competition/nxp_cup_core0.bin`

The root wrapper also publishes convenient copies under `bin/firmware/`.

The default competition preset is Rev A: EZH camera capture, ER-TFT020-3 SPI LCD,
session-gated USB telemetry, and QDC wheel-speed feedback.

QDC feedback is standard operating procedure for the competition image. It is
compiled and initialized unconditionally rather than selected as a backend.
`competition` is the only visible CMake preset. FlexIO and encoder diagnostics
remain explicit scripts under `scripts/maintainer` and use separate build trees.

## New-machine tool and output layout

Root `setup.ps1` provisions the Arm compiler under `out/toolchains` and installs
CMake, Ninja, uv, and LLVM-MinGW when needed. Firmware builds discover the local
Arm compiler automatically, and the native viewer uses LLVM-MinGW by default;
Visual Studio C++ is only an optional comparison toolchain.

Android has a separate license-gated setup:

```powershell
.\scripts\android\setup_android.ps1 -AcceptLicenses
.\build_android.ps1
.\build_android.ps1 -Offline
```

The Android setup installs pinned JDK 17, Android SDK Platform 35, Build Tools
35.0.0, ADB, and Gradle 8.9 under `out/toolchains/android`. Its Gradle dependency
cache is also repository-local. After one connected build fills that cache, a
clean offline build is supported.

| Consumer | Root build | Convenient output | Authoritative output |
| --- | --- | --- | --- |
| Firmware | `.\build.ps1` | `bin/firmware` | `build/cmake/competition` |
| Native viewer and CLI | `.\build_viewer.ps1` | `bin/host` | `build/host/nxp_cup_host/Release` |
| Android bridge | `.\build_android.ps1` | `bin/android/nxp_cup_bridge.apk` | `src/android/nxp_cup_bridge/app/build/outputs/apk/debug/app-debug.apk` |

Both `out` and `bin` are git-ignored. Neither setup script persists environment
variables. Keeping `out/toolchains` makes the tools and caches durable for that
checkout and avoids unnecessary downloads on poor connections.

## Flash evaluation

Until the clean-machine/hardware evaluation selects a default, `flash.ps1` requires an
explicit backend:

| Command | Purpose |
| --- | --- |
| `.\flash.ps1 -Backend Ozone` | Opens the competition AXF through the checked Ozone project; normal workshop route |
| `.\flash.ps1 -Backend Rom` | Uses the native host tool and J11 ROM-HID ISP with erase, write, full readback hash, reset, and runtime recovery |
| `.\flash.ps1 -Backend JLink` | Maintainer route with explicit or unambiguous probe selection |

The Ozone project is intentionally pinned to the competition artifact. Use an
explicit `-File` with ROM or J-Link for a maintainer diagnostic image.

## Maintainer tools

Non-normal entry points are under `scripts/maintainer`:

- `build_mcuxpresso.ps1` — fresh headless IDE comparison;
- `build_cmake.ps1` — generated-source drift and ad-hoc `-Define` builds;
- FlexIO and motor-encoder diagnostic wrappers.

The renamed MCUXpresso project is `src/nxp_cup/nxp_cup_core0` and emits
`Debug/nxp_cup_core0.axf`. A fresh headless import builds with zero errors and zero
warnings. A manual GUI import/navigation/Ozone smoke check remains for the owner.

## Host consumers

- Native CLI/viewer: `src/nxp_cup_host`, outputs under `build/host/nxp_cup_host/Release`.
- Standalone Web Serial viewer: `src/nxp_cup_host/nxpc_usb_debug_viewer.html`.
- Android bridge: `src/android/nxp_cup_bridge`, application ID
  `com.wavenumber.nxpc.bridge`.

All three consumers build after the rename. Protocol version 1 retains `AVCU` wire magic
`0x55435641`, runtime USB `1FC9:0094`, ROM HID `1FC9:014F`, packed message values, and
H.264 `video/avc`. Those compatibility values are independent of the NXP Cup product
labels and private source identifiers.

## 2026-08-24 clean-machine evidence

- The competition firmware rebuilt and published through the root wrapper.
- The flashed competition image produced a live camera stream in the native
  viewer over runtime USB.
- The native viewer built with LLVM-MinGW and ran without Visual Studio C++.
- Android provisioning completed under `out/toolchains/android`; a clean build,
  JVM unit tests, and a second clean offline build passed.
- The previously installed Android app worked with the hardware. Reinstalling
  the newly built APK and repeating USB-permission, relay-reconnect, and SoftAP
  tests remain pending; room Wi-Fi was unreliable and is not counted as a pass.
