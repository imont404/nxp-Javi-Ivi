# Android Agent Guide

This component consumes the existing `AVCU` v1 session and must not independently
rename wire bytes, USB identities, or public firmware fields. Keep USB reading,
compression, and network delivery bounded; slow clients must never block the USB
reader or grow memory without limit.

Use `setup.ps1` only with explicit Android license acceptance and `build.ps1` for
unit tests plus APK assembly. Hardware/deploy scripts under `tools` are
maintainer-only and require an explicitly selected phone when ambiguity exists.

The current `res/raw/relay_viewer.html` is a proven monolithic relay client. During the
planned shared-dashboard extraction, preserve its WebSocket, video-mode, and bounded
decode behavior in an Android-owned adapter; share presentation only. Do not create a
second relay implementation or split APK roles until the plan's shared-dashboard,
lifecycle, and app-role structure dependencies are satisfied. See
`docs/design/shared-web-dashboard.md`.
