# Agent Guide

This directory contains host-side tooling for the NXP Cup USB debug-display
transport. It is shared by the isolated FRDM-MCXN947 CDC sample and the active
NXP Cup firmware under `src/nxp_cup/nxp_cup_core0`.

## Scope

Keep reusable host receivers, viewers, and transport diagnostics here instead
of under `docs/examples`. Imported examples are reference material; this
directory is the durable host-tool home.

If protocol definitions need to be shared with firmware, prefer a small common
source location under `src/common` or a generated artifact with a documented
source of truth. Avoid duplicating packet constants in unrelated files once the
transport framing is locked.

## Python Receiver

Run from the repository root:

```powershell
uvx --from pyserial python .\src\nxp_cup_host\usb_cdc_stream_read.py --port COM15 --seconds 10 --read-size 262144
```

## Native Windows Applications

Build the normal viewer and CLI from the repository root:

```powershell
.\build_viewer.ps1
.\bin\host\nxpc_viewer.exe
```

Root `setup.ps1` provisions LLVM-MinGW, which is the preferred compiler. Visual
Studio C++ is not required for the normal viewer or CLI. The root wrapper copies
the complete runnable bundle to `bin\host`; authoritative CMake output remains
under `build\host\nxp_cup_host\Release`.

## Legacy Native Throughput Receiver

The native receiver is for maximum-throughput measurement. Build from the
repository root:

```powershell
.\src\nxp_cup_host\build_usb_cdc_stream_read.ps1
.\src\nxp_cup_host\bin\usb_cdc_stream_read.exe --port COM15 --seconds 10 --read-size 262144
```

This older, isolated throughput-receiver wrapper still uses Visual Studio 2022
Build Tools discovered through `vswhere.exe` and `vcvars64.bat`. It is not part
of the normal viewer build and is unnecessary for ordinary USB telemetry use.

Generated files under `bin` are local build output and must not be committed.
Use 256 KiB reads for sustained Windows CDC throughput measurements unless a
new host path is revalidated.

## Browser Viewer

`nxpc_usb_debug_viewer.html` is the standalone handoff. It must contain no
external assets and must work from a direct `file://` origin in Chrome/Edge.
Regenerate and test it after changing the readable HTML/CSS/JS sources:

```powershell
.\src\nxp_cup_host\build_standalone_viewer.ps1
npm test --prefix .\src\nxp_cup_host
```

The Playwright simulator covers handshake, control, stats, logs, a complete
camera frame, DOM updates, page errors, file-origin Web Serial availability,
rolling telemetry plots and their history bound, and absence of asset requests.
The localhost server remains optional maintainer tooling only.

Record execution findings against `docs/plans/usb-debug-telemetry` and move stable
protocol/measurement conclusions into the corresponding durable documents under
`docs/research`.
