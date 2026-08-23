# Build, Flash, and Consumer Layout

## Normal firmware build

Provision once, then use the canonical wrapper:

```powershell
.\setup.ps1
.\build.ps1
.\build.ps1 -Preset flexio-port1
.\build.ps1 -Clean
```

`build.ps1` validates the preset, configures CMake, and builds it. Direct
`cmake --preset <name>` plus `cmake --build --preset <name>` remains equivalent. The
competition outputs are:

- `build/cmake/competition/nxp_cup_core0.axf`
- `build/cmake/competition/nxp_cup_core0.bin`

The default competition preset is Rev A: EZH camera capture, ER-TFT020-3 SPI LCD,
session-gated USB telemetry, and QDC wheel-speed feedback.

## Flash evaluation

Until the clean-machine/hardware evaluation selects a default, `flash.ps1` requires an
explicit backend:

| Command | Purpose |
| --- | --- |
| `.\flash.ps1 -Backend Ozone` | Opens the competition AXF through the checked Ozone project; normal workshop route |
| `.\flash.ps1 -Backend Rom` | Uses the native host tool and J11 ROM-HID ISP with erase, write, full readback hash, reset, and runtime recovery |
| `.\flash.ps1 -Backend JLink` | Maintainer route with explicit or unambiguous probe selection |

The Ozone project is intentionally pinned to the competition artifact. Use ROM or J-Link
for another preset while backend evaluation is still open.

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
- Android bridge: `src/android/nxp_cup_bridge`, package `com.wavenumber.nxpc`.

All three consumers build after the rename. Protocol version 1 retains `AVCU` wire magic
`0x55435641`, runtime USB `1FC9:0094`, ROM HID `1FC9:014F`, packed message values, and
H.264 `video/avc`. Those compatibility values are independent of the NXP Cup product
labels and private source identifiers.
