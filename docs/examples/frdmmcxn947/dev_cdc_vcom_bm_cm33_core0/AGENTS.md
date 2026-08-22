# Agent Guide

This folder is the isolated FRDM-MCXN947 high-speed USB CDC project that was used
for the completed transport proof. It is now comparison/reference material for
`usb-debug-telemetry`.

Do not treat it as an active build target unless a plan explicitly adds that work.
It is still derived from the imported NXP `dev_cdc_vcom_bm_cm33_core0` sample,
so keep changes focused on USB enumeration, CDC bulk throughput, host-control
commands, and synthetic-frame streaming. Do not integrate AVC camera or display
logic here; that belongs in `src/avc/avc_core0` after the USB-only proof is
measured.

## Build

Use the local wrapper from this directory:

```powershell
.\build.ps1
.\build.ps1 -Clean
.\build.ps1 -Clean -ResetWorkspace
```

The wrapper imports this MCUXpresso project into the sibling
`..\.mcux_workspace_cdc_vcom` headless workspace and builds
`frdmmcxn947_dev_cdc_vcom_bm_cm33_core0/Debug`. Generated `Debug`,
`Release`, and `.mcux_workspace*` folders are local build state and must not be
committed.

## Flash

Use the local flash wrapper:

```powershell
.\flash.ps1
.\flash.ps1 -File .\Debug\frdmmcxn947_dev_cdc_vcom_bm_cm33_core0.axf
```

The wrapper defaults to the FRDM-MCXN947 onboard J-Link serial observed in the
main AVC scripts, device `MCXN947_M33_0`, SWD at 4 MHz, and auto-detects the
newest installed SEGGER `JLink_V*` path when the configured default is missing.
Pass `-UsbSerial ""` only when exactly one J-Link is connected or interactive
selection is acceptable.

## Bench Notes

Keep the debug USB connection attached for J-Link flash and debug UART. Use the
separate FRDM-MCXN947 HS USB device connector for CDC enumeration and throughput
tests.

The first proof should remain camera-free:

- confirm high-speed enumeration,
- confirm the host sees a CDC ACM COM port,
- replace echo behavior with synthetic RGB565 frame streaming only after stock
  enumeration works,
- measure native PC receive throughput before testing browser or Android paths.

Use the shared host receiver for the synthetic stream:

```powershell
uvx --from pyserial python ..\..\..\..\src\usb_debug_host\usb_cdc_stream_read.py --port COM15 --seconds 10
```

Use the shared static WebSerial viewer from a localhost server:

```powershell
..\..\..\..\src\usb_debug_host\serve_webserial_viewer.ps1
```

Record execution findings against `docs/plans/usb-debug-telemetry`, not in this
sample folder, and preserve stable conclusions under `docs/research`.
