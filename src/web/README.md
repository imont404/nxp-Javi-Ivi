# Shared Web Dashboard

This component owns the Formula One-style browser presentation shared by the direct
WebSerial viewer and Android Wi-Fi relay viewer. The transport implementations remain in
their consumer adapters.

From the repository root:

```powershell
.\src\web\build.ps1
.\src\web\build.ps1 -Check
```

The build deterministically generates and checks these committed standalone pages:

- `src/host/nxpc_usb_debug_viewer.html`
- `src/android/nxp_cup_bridge/app/src/main/res/raw/relay_viewer.html`

Neither output loads a CDN, remote font, package server, or repository-relative browser
asset. Edit the shared files under `dashboard`, the WebSerial adapter under
`src/host/web`, or the relay adapter under `src/android/web`; do not edit generated pages.
