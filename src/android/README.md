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
