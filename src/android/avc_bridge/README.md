# AVC Android Bridge

This maintainer-only Android project is the phone-side USB telemetry and Wi-Fi relay.
It is not part of the student firmware build and does not contain a vehicle algorithm.

From a clean Windows checkout:

```powershell
.\scripts\android\setup_android.ps1 -AcceptLicenses
.\scripts\android\build_android.ps1
.\scripts\android\phone_inventory.ps1
.\scripts\android\enable_wireless_adb.ps1
.\scripts\android\android_loop.ps1 -Serial <phone-ip>:5555
```

The setup script provisions pinned, verified tools only under
`out\toolchains\android`. Nothing is installed globally. The phone requires one-time
USB debugging authorization on each workstation. Wireless adb is used because the
phone's USB-C port is occupied by the car during hardware development.

The verified bench topology is:

```text
PC -- Wi-Fi "yellow" --> Moto G Power 5G (2023)
PC -- USB-C ----------> FRDM J17 (debug/power)
phone -- USB-C OTG adapter -- USB-A-to-C data cable --> FRDM J11 (MCX HS USB)
```

The OTG adapter makes the phone host/source deterministically. The phone then
enumerates `WAVENUMBER AVC / AVC USB DEBUG CEDC`, VID/PID `1FC9:0094`.
The first run requires one attended Android USB-device permission approval;
subsequent `install -r` loops retain it while the device remains authorized.

A plain C-to-C cable can negotiate the wrong direction because the FRDM board's
PTN5150A defaults to dual-role mode. For development, wireless adb can request
the phone's host role after the cable is connected:

```powershell
.\scripts\android\set_phone_usb_host.ps1 -Serial <phone-ip>:5555
```

That fallback was verified on this Moto G Power, but it requires adb and is not
a race-day solution. The board's documented R50 10 kOhm UFP strap is the direct
C-to-C hardware option; evaluate that as separate vehicle hardware work before
changing a board.

The checked-in app provides a bounded `AVCU` stream parser, packed-payload decoders,
framed control-packet builder, JVM fixtures, and an explicit USB session state machine.
On launch it completes `HELLO`, subscribes to camera/stats/log/telemetry channels,
performs `PING`, and displays only complete 320x200 RGB565 frames. A fixed three-buffer
mailbox keeps only the newest preview, so display work cannot block USB or grow memory.
Graceful exit sends `SET_CHANNELS(0)` and `CLOSE`.

`AVC_BRIDGE_HEALTH` logcat records expose FPS, MiB/s, packet and frame counts, sequence
errors, malformed chunks, preview drops, and diagnostic record counts. The unattended
deploy check requires at least one complete frame with zero sequence and malformed-chunk
errors. On the verified Rev A car, the sustained preview runs at about 23.42 FPS and
2.869 MiB/s.

The app also serves a standalone viewer from the phone:

```text
http://<phone-address>:8765/
```

The binary WebSocket at `/stream` preserves normal `AVCU` frame and telemetry packets.
The relay copies every fourth complete source frame into a separate fixed three-buffer
latest-frame mailbox; browser or network work never runs on the USB reader. The embedded
page has no server-side asset dependencies and renders generic named telemetry alongside
the live camera. `/health` exposes both USB and relay counters.

`android_loop.ps1` now verifies this path without a person watching either screen. To
check an already running app directly, use:

```powershell
.\scripts\android\verify_android_relay.ps1 -Serial <phone-ip>:5555
.\scripts\android\test_android_relay_backpressure.ps1 -Serial <phone-ip>:5555
```

The verified `yellow`-network proof rendered live video and `system.uptime` in desktop
Chrome. A separate fixed-buffer relay mailbox prevents network work from stalling USB,
and a two-second send deadline closes a client that stops draining TCP. The automated
backpressure test deliberately opens a WebSocket and stops reading, checks USB progress,
parser health, and app PSS, then proves a new client receives a recent complete frame.
Six consecutive real-hardware stalls passed with warm PSS around 56-59 MiB and a final
source-to-sent frame gap of one. The viewer remains intentionally one-client and unstyled.
