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
then sends `STOP`, drains stale bytes, sends `START`, and validates the AVCU
frame-chunk headers. `START` selects the camera-frame stream. Use
`--start-command START_SYNTH` when measuring the synthetic maximum-rate source.
Camera-mode streams include periodic `AVC_DBG_STATS_REPORT` packets with
firmware drop, endpoint-busy, and send-error counters.

## Native Windows Receiver

Use the native receiver when measuring maximum sustained throughput. It avoids
Python parser overhead and uses Win32 serial APIs directly.

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

Serve the static WebSerial viewer from localhost:

```powershell
.\src\usb_debug_host\serve_webserial_viewer.ps1
```

If port 8765 is already in use, pass another port:

```powershell
.\src\usb_debug_host\serve_webserial_viewer.ps1 -Port 8766
```

Open the URL printed by the script in Chrome. WebSerial requires a user gesture
for port selection, so the receiver can only start after clicking `Connect` and
selecting the AVC CDC port. After the port is selected, the page immediately
sends `START` and begins rendering the camera stream. If frames stop arriving,
the page reports `No frames; check test mode` and sends a fresh `START` after a
short stale-data interval.
