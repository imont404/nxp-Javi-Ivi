# USB CDC Stream Tools

These tools exercise the MCXN947 high-speed USB CDC stream proof used by the
AVC debug-display transport plan. This directory is intentionally outside
`docs/examples` so the same host tools can be used by the isolated NXP sample
and the active AVC firmware.

## Python Receiver

Use the Python receiver for quick iteration:

```powershell
uvx --from pyserial python .\src\usb_debug_host\usb_cdc_stream_read.py --port COM15 --seconds 10 --read-size 262144
```

The script requests a large host RX buffer when the platform exposes that API,
performs the framed `HELLO` exchange, requests frame/stat channels, and validates
the AVCU responses and frame-chunk headers. Use `--start-command START_SYNTH`
when measuring the synthetic maximum-rate source. `--legacy-ascii` retains the
old `START`/`STOP` path for comparison with older firmware.
Camera-mode streams include periodic `AVC_DBG_STATS_REPORT` packets with
firmware drop, endpoint-busy, send-error, and fixed-queue counters. The framed
receiver also subscribes to bounded UTF-8 device logs and typed named telemetry
and prints each record.

## Native Windows Receiver

Use the native receiver when measuring maximum sustained throughput. It avoids
Python parser overhead and uses Win32 serial APIs directly. It uses the same
framed HELLO/channel/close session as the browser and Python receiver; the
synthetic source is selected through `--start-command START_SYNTH`.

Build requirements:

- Visual Studio 2022 Build Tools with the C++ desktop workload.
- `vswhere.exe` under `C:\Program Files (x86)\Microsoft Visual Studio\Installer`.

Build and run:

```powershell
.\src\usb_debug_host\build_usb_cdc_stream_read.ps1
.\src\usb_debug_host\bin\usb_cdc_stream_read.exe --port COM15 --seconds 10 --read-size 262144
.\src\usb_debug_host\bin\usb_cdc_stream_read.exe --port COM15 --seconds 10 --start-command START_SYNTH
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
src/usb_debug_host/avc_usb_debug_viewer.html
```

Open it directly in Chrome or Edge. It contains all HTML, CSS, and JavaScript;
it makes no asset requests and needs no server, install, or build step.

Maintainers edit the three readable source files and regenerate the handoff:

```powershell
.\src\usb_debug_host\build_standalone_viewer.ps1
```

The optional development server regenerates and serves that same standalone
file:

```powershell
.\src\usb_debug_host\serve_webserial_viewer.ps1
```

If port 8765 is already in use, pass another port:

```powershell
.\src\usb_debug_host\serve_webserial_viewer.ps1 -Port 8766
```

Open the URL printed by the script in Chrome or Edge. WebSerial requires a user gesture
for port selection, so the receiver can only start after clicking `Connect` and
selecting the AVC CDC port. After the port is selected, the page establishes a
recognized telemetry session and requests camera-frame/stat channels. If frames
stop arriving, it reports the stale camera channel and repeats the framed
channel request after a short interval. The viewer also subscribes to the device
log channel, keeps the newest 200 records in its console, and automatically
discovers typed scalar telemetry in a live table keyed by name. Each row has a
plot selector. Plots retain at most 300 samples per discovered signal and at
most six signals may be selected at once. To preselect a dashboard, change only
the `DASHBOARD.defaultPlots` name list near the top of `webserial_viewer.js`;
firmware packet parsing does not need to change.

Automated Chrome coverage uses a simulated AVC Web Serial device to exercise the
framed handshake, mixed control/stat/log/telemetry traffic, a complete camera
frame, rolling plot updates and history bounds, DOM updates, and page errors. It
also verifies that Chrome exposes Web Serial for the direct `file://` origin and
that the standalone page makes no asset requests:

```powershell
npm install --prefix .\src\usb_debug_host
npm test --prefix .\src\usb_debug_host
```

This maintainer test dependency is not part of the student or race-week viewer
workflow.
