# USB CDC Stream Tools

These tools exercise the MCXN947 high-speed USB CDC stream proof used by the
NXP Cup debug-display transport plan. This directory is intentionally outside
`docs/examples` so the same host tools can be used by the isolated NXP sample
and the active NXP Cup firmware.

## Python Receiver

Use the Python receiver for quick iteration:

```powershell
uvx --from pyserial python .\src\nxp_cup_host\usb_cdc_stream_read.py --port COM15 --seconds 10 --read-size 262144
```

The script requests a large host RX buffer when the platform exposes that API,
performs the framed `HELLO` exchange, requests frame/stat channels, and validates
the AVCU responses and frame-chunk headers. Use `--start-command START_SYNTH`
when measuring the synthetic maximum-rate source. `--legacy-ascii` retains the
old `START`/`STOP` path for comparison with older firmware.
Camera-mode streams include periodic `NXPC_DBG_STATS_REPORT` packets with
firmware drop, endpoint-busy, send-error, and fixed-queue counters. The framed
receiver also subscribes to bounded UTF-8 device logs and typed named telemetry
and prints each record.

## Native NXP Cup Host Probe

The developing one-cable host core has a small native protocol probe. It
enumerates Windows serial devices by VID/PID, refuses to guess when multiple
NXP Cup boards are present, completes the framed session, and can require one
complete camera frame:

```powershell
.\src\nxp_cup_host\build_nxpc_tool.ps1
.\src\nxp_cup_host\bin\nxpc_tool.exe selftest
.\src\nxp_cup_host\bin\nxpc_tool.exe devices
.\src\nxp_cup_host\bin\nxpc_tool.exe probe
.\src\nxp_cup_host\bin\nxpc_tool.exe probe --frame --seconds 3
.\src\nxp_cup_host\bin\nxpc_tool.exe enter-isp
```

`probe` auto-selects only an exact `VID_1FC9/PID_0094` match and only when
there is exactly one. `--port COMx` is an explicit override, but it is still
validated against that USB identity.

`enter-isp` is a deliberate maintainer command. It requires the firmware to
advertise the capability, sends the framed confirmation value after HELLO,
requires a correlated successful response, closes CDC, and succeeds only after
exactly one MCXN947 ROM HID (`VID_1FC9/PID_014F`) appears. It does not erase or
write flash by itself.

The bounded one-command programming path uses that same transition and shared
host core:

```powershell
.\build\host\nxp_cup_host\Release\nxpc_tool.exe program `
    --image .\build\cmake\camera-usb-bench\nxp_cup_core0.bin
```

It validates the `.bin` size, MCXN947 stack/reset vectors, and SHA-256; requires
exactly one runtime CDC or ROM HID target; reports query, erase, write, full
readback verification, and reset separately; then reconnects and requires a
complete camera frame. It never invokes fuse, CMPA, security, or program-once
commands.

The default backend is the colocated NXP `rblhost` 0.2.0 executable. After
writing, it reads the selected byte range back from flash and requires the size
and SHA-256 to match before reset. NXP SPSDK `blhost` 3.10.0 remains a proven
fallback and requires its write response to report the exact image length.
`--programmer <path>` or `NXPC_PROGRAMMER_PATH` selects either pinned tool;
`--blhost` and the older backend-specific environment variables remain accepted
for maintainer compatibility.

## Native Camera Viewer

Build the first SDL2/Dear ImGui viewer with:

```powershell
.\src\nxp_cup_host\build_nxpc_viewer.ps1
.\build\host\nxp_cup_host\Release\nxpc_viewer.exe
```

The viewer establishes the framed session, requests raw RGB565 camera frames
plus stats/logs/telemetry, and reconnects after a runtime disconnect. The SDL
workspace clears to black. Its `Camera`, `NXP Cup status`, `Program firmware`, and
`Debug log` Dear ImGui panels are independently movable and resizable. The
debug panel shows retained `NXPC_DBG_LOG_TEXT` records with device timestamp,
severity, category, and text; named scalar telemetry remains in the status
panel. The video is never painted behind the host UI.

The `Program firmware` panel selects an existing `nxp_cup_core0.bin` with a native
file dialog. Programming remains disabled until exactly one supported runtime
CDC or ROM HID target is present and the erase confirmation is checked. The
background connection worker validates the image and pinned programmer, requests
safe ISP entry through its existing CDC session, reports query/erase/write/reset
stages, and reconnects the preview after reset. It uses the same programmer
backend and exact-device/no-guessing rules as `nxpc_tool`; the GUI does not build
firmware and exposes no fuse, CMPA, security, or program-once operation.
On disconnect, the camera panel hides its last texture and shows a prominent
message; after re-enumeration the worker negotiates a new session and increments
the successful-connection count before displaying new frames.
Repeated physical J11 detach/reattach was bench-validated on 2026-08-23: the
same running viewer recovered the re-enumerated `COM34` session and resumed live
preview, reaching four successful connections and 821 complete frames with zero
malformed packets.

A bounded hidden bench smoke test exercises the same GUI receive/render path
and exits nonzero unless it receives complete, well-formed frames:

```powershell
.\build\host\nxp_cup_host\Release\nxpc_viewer.exe --test-seconds 5
```

The build uses the repository's existing SDL2 package and the locally retained
Dear ImGui `v1.91.9b` sources under `vendor/imgui`; configuration makes no
network request and does not depend on Bunny Vision or another checkout. ImGui's
license is retained with its sources. The build copies the pinned repository
`SDL2.dll` beside the executable so the preview runs without a separate SDL
installation.

Create a portable, checksummed runtime zip after a successful build with:

```powershell
.\src\nxp_cup_host\package_nxpc_host.ps1
```

The zip contains `nxpc_viewer.exe`, `nxpc_tool.exe`, `SDL2.dll`, the pinned
standalone `rblhost.exe`, the relevant third-party licenses, and a SHA-256
manifest. It does not contain a compiler or build the student firmware. The zip
is generated under `build/dist` and is intentionally not tracked by Git; copy it
separately to another machine or regenerate it after building there.

## Native Windows Throughput Receiver

Use the native receiver when measuring maximum sustained throughput. It avoids
Python parser overhead and uses Win32 serial APIs directly. It uses the same
framed HELLO/channel/close session as the browser and Python receiver; the
synthetic source is selected through `--start-command START_SYNTH`.

Build requirements:

- Visual Studio 2022 Build Tools with the C++ desktop workload.
- `vswhere.exe` under `C:\Program Files (x86)\Microsoft Visual Studio\Installer`.

Build and run:

```powershell
.\src\nxp_cup_host\build_usb_cdc_stream_read.ps1
.\src\nxp_cup_host\bin\usb_cdc_stream_read.exe --port COM15 --seconds 10 --read-size 262144
.\src\nxp_cup_host\bin\usb_cdc_stream_read.exe --port COM15 --seconds 10 --start-command START_SYNTH
```

LLVM/Clang is not required for this host machine because Visual Studio Build
Tools are already installed. If a future machine uses LLVM instead, document
the install command and update the build wrapper before relying on it for
plan results.

On the current Windows CDC path, 256 KiB reads have been the stable high-rate
setting. Larger multi-megabyte reads can lower throughput or create apparent
serial-driver gaps even when the firmware and protocol framing are valid.

## Browser Viewer

The race-week handoff is the self-contained file:

```text
src/nxp_cup_host/nxpc_usb_debug_viewer.html
```

Open it directly in Chrome or Edge. It contains all HTML, CSS, and JavaScript;
it makes no asset requests and needs no server, install, or build step.

Maintainers edit the three readable source files and regenerate the handoff:

```powershell
.\src\nxp_cup_host\build_standalone_viewer.ps1
```

The optional development server regenerates and serves that same standalone
file:

```powershell
.\src\nxp_cup_host\serve_webserial_viewer.ps1
```

If port 8765 is already in use, pass another port:

```powershell
.\src\nxp_cup_host\serve_webserial_viewer.ps1 -Port 8766
```

Open the URL printed by the script in Chrome or Edge. WebSerial requires a user gesture
for port selection, so the receiver can only start after clicking `Connect` and
selecting the NXP Cup CDC port. After the port is selected, the page establishes a
recognized telemetry session and requests camera-frame/stat channels. If frames
stop arriving, it reports the stale camera channel and repeats the framed
channel request after a short interval. The viewer also subscribes to the device
log channel, keeps the newest 200 records in its console, and automatically
discovers typed scalar telemetry in a live table keyed by name. Each row has a
plot selector. Plots retain at most 300 samples per discovered signal and at
most six signals may be selected at once. To preselect a dashboard, change only
the `DASHBOARD.defaultPlots` name list near the top of `webserial_viewer.js`;
firmware packet parsing does not need to change.

Automated Chrome coverage uses a simulated NXP Cup Web Serial device to exercise the
framed handshake, mixed control/stat/log/telemetry traffic, a complete camera
frame, rolling plot updates and history bounds, DOM updates, and page errors. It
also verifies that Chrome exposes Web Serial for the direct `file://` origin and
that the standalone page makes no asset requests:

```powershell
npm install --prefix .\src\nxp_cup_host
npm test --prefix .\src\nxp_cup_host
```

This maintainer test dependency is not part of the student or race-week viewer
workflow.
