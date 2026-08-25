# Android Telemetry Relay

This maintainer-only component hosts the Moto G Power USB telemetry and Wi-Fi
relay. It is not required for the student firmware build.

```powershell
.\src\android\setup.ps1 -AcceptLicenses
.\src\android\build.ps1
```

For a parallel worktree, `NXPC_ANDROID_TOOLS_DIR` may point at an already
provisioned `out\toolchains\android` directory instead of downloading a second
copy.

The Gradle project remains under `nxp_cup_bridge`. Bench, deploy, reconnect,
compression, and relay-verification commands live under `tools`. The APK is
published to `out\artifacts\android\nxp_cup_bridge.apk`.

The currently proven browser resource is
`nxp_cup_bridge/app/src/main/res/raw/relay_viewer.html`, loaded as bytes by
`MainActivity`. It is still authoritative until the shared-dashboard build described in
`docs/design/shared-web-dashboard.md` is implemented. That work may share markup,
styling, and display logic with the Formula One WebSerial page, but the Android adapter
continues to own WebSocket connection, JPEG/H.264/raw selection, and
`AVCJ`/`AVC4`/`AVCR` decoding. Do not replace it with the WebSerial script.

A possible viewer-only APK is a later structural decision, not a second implementation
of the existing relay. First extract a reusable Kotlin core for `AVCU`, USB session
ownership, frame assembly, and bounded latest-frame delivery; then choose product
flavors or separate app modules as recorded in the Android plan.
