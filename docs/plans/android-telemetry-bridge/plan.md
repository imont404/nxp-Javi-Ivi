+++
type = "plan"
id = "android-telemetry-bridge"
status = "active"
created = "2026-08-21"

[[steps]]
id = "resume-gate"
title = "Confirm the USB protocol and race-week Web viewer are stable enough to support an independent Android host"
status = "done"

[[steps]]
id = "device-inventory"
title = "Inventory the Moto G Power 5G (2023), Android build, USB-host feature, developer access, adapters, and bench power topology"
status = "done"
depends_on = ["resume-gate"]

[[steps]]
id = "toolchain-bootstrap"
title = "Provision a reproducible command-line Android build, install, test, and adb toolchain on the maintainer workstation"
status = "done"
depends_on = ["resume-gate"]

[[steps]]
id = "protocol-fixtures"
title = "Create JVM conformance fixtures for fragmented AVCU control, frame, stats, log, and telemetry traffic"
status = "done"
depends_on = ["resume-gate"]

[[steps]]
id = "android-foundation"
title = "Create the minimal native app with a bounded parser, explicit session state, diagnostics, and no actuator controls"
status = "done"
depends_on = ["toolchain-bootstrap", "protocol-fixtures"]

[[steps]]
id = "wireless-adb-loop"
title = "Establish wireless adb build-install-run-log collection while the phone USB-C port is occupied by the car"
status = "done"
depends_on = ["device-inventory", "android-foundation"]

[[steps]]
id = "android-usb-host-proof"
title = "Open the AVC CDC interface from the phone and complete HELLO, SET_CHANNELS, PING, and CLOSE on real hardware"
status = "done"
depends_on = ["wireless-adb-loop"]

[[steps]]
id = "phone-preview"
title = "Parse and display live RGB565 camera frames plus connection and transport counters on the phone"
status = "done"
depends_on = ["android-usb-host-proof"]

[[steps]]
id = "wifi-relay-proof"
title = "Serve an embedded page and relay bounded camera and telemetry data to one browser over a controlled 5 GHz network"
status = "done"
depends_on = ["phone-preview"]

[[steps]]
id = "relay-backpressure"
title = "Prove slow or disconnected Wi-Fi clients cannot block USB reads, grow memory, or create unbounded display latency"
status = "done"
depends_on = ["wifi-relay-proof"]

[[steps]]
id = "vehicle-integration"
title = "Validate battery life, heat, mounting, cable retention, reconnects, RF behavior, and noninterference on the car"
status = "active"
depends_on = ["relay-backpressure"]

[[steps]]
id = "design-doc-intent-audit"
title = "Audit the Android design and protocol use against the implemented USB contract"
status = "pending"
depends_on = ["vehicle-integration"]

[[steps]]
id = "test-runtime-impact-audit"
title = "Audit app, host, hardware, and firmware validation coverage and runtime impact"
status = "pending"
depends_on = ["vehicle-integration"]

[[steps]]
id = "external-review"
title = "Obtain independent external review"
status = "pending"
depends_on = ["design-doc-intent-audit", "test-runtime-impact-audit"]

[[exit_criteria]]
id = "protocol-reuse"
title = "Android uses the same framed protocol as the Web viewer without a firmware fork"
status = "met"

[[exit_criteria]]
id = "unattended-loop"
title = "After one-time phone authorization and cable setup, an agent can build, test, deploy, start, inspect health, and collect logs without handling the phone"
status = "met"

[[exit_criteria]]
id = "portable-workstation"
title = "A clean Windows laptop can reproduce the pinned Android toolchain and build/deploy workflow without relying on this workstation's global state"
status = "pending"

[[exit_criteria]]
id = "usb-preview"
title = "The Moto G Power receives and displays the live AVC camera stream with observable parser and transport health"
status = "met"

[[exit_criteria]]
id = "browser-relay"
title = "One laptop browser receives a useful live camera and telemetry view over the phone's controlled 5 GHz network"
status = "met"

[[exit_criteria]]
id = "bounded-backpressure"
title = "A slow or absent Wi-Fi viewer cannot stall USB input or grow memory and always converges to the newest complete frame"
status = "met"

[[exit_criteria]]
id = "safe-reconnect"
title = "Connect, disconnect, app restart, and reconnect cannot enable motors or select a moving vehicle mode"
status = "pending"

[[exit_criteria]]
id = "vehicle-fit"
title = "The phone, USB cable, battery runtime, heat, RF link, and mounting are validated on the car"
status = "pending"

[[exit_criteria]]
id = "design-doc-intent-audit"
title = "Android design and protocol documentation match implementation"
status = "pending"

[[exit_criteria]]
id = "test-runtime-impact-audit"
title = "New tests are listed and runtime impact is reviewed"
status = "pending"

[[exit_criteria]]
id = "external-review"
title = "Independent external review is complete"
status = "pending"
+++

# Android Telemetry Bridge

## Current Status

Execution was authorized on 2026-08-21. The native USB host, phone preview, and first
one-browser Wi-Fi relay are complete on the real Rev A car. The Moto G Power serves its
embedded page at `http://<phone-address>:8765/`, preserves `AVCU` framing over a binary
WebSocket, and relays every fourth complete camera frame plus generic telemetry. Chrome
rendered live video and `system.uptime` while USB remained at about 23.42 FPS and
2.869 MiB/s with zero sequence or malformed-chunk errors. A two-second network-send
deadline now closes a client that stops reading. Six consecutive forced stalls kept USB
advancing, stayed near 56-59 MiB PSS after warm-up, and reconnected to a complete frame
within one source frame in the final recorded run. Six subsequent abrupt app-process
losses also recovered distinct firmware sessions 27-32, clean USB video, telemetry, and
recent Wi-Fi frames without cable handling. That test exposed and fixed Android choosing
an IPv6-only wildcard listener after restart; the relay now explicitly selects the IPv4
stack and binds the active WLAN address. A connected-device foreground service now keeps
CPU and Wi-Fi active through the secure lockscreen: live relay verification passed while
Android reported `Dozing`, screen off, and light idle. A 30-second loaded baseline held
27 C, 23.42 USB FPS, 2.869 MiB/s, 49-60 MiB PSS, and roughly 427-588 mA discharge. Active
work is now physical USB removal, vehicle power, mounting, cable retention, full-duration
thermal/runtime, and race-network validation.

## Concrete Hardware

The available handset is a **Moto G Power 5G (2023)**, not the older Moto G4 assumed in
early discussion. Motorola lists Android 13, a MediaTek Dimensity 930, 6 GB RAM, 256 GB
storage, a 5000 mAh battery, 185 g mass, USB-C with USB 2.0, and dual-band
802.11ac Wi-Fi with hotspot support:

- <https://en-ca.support.motorola.com/app/answers/detail/a_id/174789/~/moto-g-power-5g-%282023%29---specifications>
- <https://en-us.support.motorola.com/app/answers/detail/a_id/173296/~/wi-fi-hotspot---moto-g-power-5g-%282023%29>

Those facts remove the old Moto G4 concerns about micro-USB, Android 7, and 2.4 GHz-only
Wi-Fi. Bench inventory found the phone updated to Android 14/API 34 and verified its USB
host feature on the real car.

The car presents a conventional CDC ACM layout with NXP VID `0x1FC9`, PID `0x0094`,
bulk IN endpoint 2, bulk OUT endpoint 3, and 512-byte high-speed packets. The foundation
uses Android's USB-host APIs directly for discovery, permission, interface claims, CDC
control requests, and bounded endpoint transfers. It confirms the device by completing
the AVC framed `HELLO`, not by trusting VID/PID alone; no third-party serial dependency
was needed.

## Purpose

Use the phone as a wired in-car USB host and a Wi-Fi bridge through the same `AVCU`
protocol used by the PC viewer. The minimum useful product is:

```text
Rev A car -- USB CDC --> Android AVC parser --> phone preview
                                      |
                                      +--> latest complete frame + telemetry
                                            --> embedded HTTP/WebSocket server
                                            --> laptop browser
```

The firmware must not gain an Android-specific protocol or code path. The app owns its
USB session and relay policy; it never selects vehicle mode or enables an actuator.

## MVP Boundary

The first executable slice is deliberately small:

1. Detect the attached AVC CDC device, obtain user permission, open it, and complete
   `HELLO`, `SET_CHANNELS`, `PING`, and `CLOSE`.
2. Parse fragmented `AVCU` packets on a dedicated worker and show live RGB565 frames plus
   connection, parser, frame-rate, byte-rate, sequence, and drop counters on the phone.
3. Serve a static page embedded in the APK and forward a decimated latest-frame stream
   plus telemetry to one laptop browser over binary WebSocket.
4. Provide an in-app and machine-readable health view so the development loop can verify
   USB state, session state, last frame age, rates, drops, and connected browser count.

The relay should initially preserve the `AVCU` message format so the browser can reuse the
existing parser concepts. The phone may omit complete camera frames according to its
relay policy, but it must not forward partial frames as complete. Logs and telemetry are
small; camera frames use latest-complete-frame semantics.

Raw full-rate video is about 24.1 Mbit/s. The first Wi-Fi proof should send every fourth
frame, about 5.9 FPS and 6.0 Mbit/s, while continuing to drain USB at full rate. This is
enough to prove the bridge with useful visual feedback and provides margin on a crowded
network. Hardware H.264, recording, multi-client support, and a polished race dashboard
come only after this proof.

WebSocket uses TCP because an ordinary browser cannot consume arbitrary UDP datagrams.
Bounded application queues prevent TCP backpressure from becoming growing latency: keep
at most the newest complete relay frame, drop superseded frames with counters, and close
a client that cannot drain within a fixed limit. USB input must never wait for Wi-Fi.

## Network Shape

Development and race operation have different best first configurations:

- **Development:** put the workstation and phone on the same controlled 5 GHz access
  point or travel router. This gives the most reliable wireless `adb` connection while
  the phone's only USB-C port is connected to the car.
- **Race MVP:** manually enable the phone's 5 GHz hotspot and join the projector laptop
  to it. The app displays the local URL and, later, a QR code. This avoids venue Wi-Fi.
- **Optional follow-up:** Android's local-only hotspot API is available from API 26 and
  can create a no-Internet network for nearby clients. Android 13-targeted apps require
  `NEARBY_WIFI_DEVICES`. Programmatic hotspot control is not required for the first proof.

A travel router remains the fallback if Motorola's hotspot isolates clients, prevents
wireless debugging, or behaves poorly in the venue.

## Unattended Development Loop

The repository now provisions a maintainer-only reproducible toolchain without relying
on global `adb`, Android SDK, Java/JDK, Gradle, or Android Studio state:

- JDK 17;
- Android command-line tools, platform-tools, one pinned SDK platform, and build-tools;
- a checked-in Gradle wrapper so no global Gradle is required;
- scripted build, JVM tests, debug APK install/update, app start, health probe, and
  filtered log collection.

Prefer command-line provisioning in a documented maintainer location over making Android
Studio state part of the build. Android Studio may be installed for interactive work but
must not be required by automation or students.

The setup must also be portable to the maintainer laptop for travel to Guatemala:

- provide one PowerShell bootstrap entry point that starts from a normal clean Windows
  account and provisions the JDK and Android command-line tools into an ignored,
  repository-relative tool cache;
- pin and record every tool version, download source, and integrity hash rather than
  depending on whatever is globally installed on this PC;
- keep the Gradle wrapper and all required build configuration in the repository;
- provide a verification command that prints the resolved Java, SDK, build-tools,
  platform-tools, and Gradle versions before building;
- document the one-time Android SDK license step and the phone's separate RSA/wireless
  debugging pairing required on each workstation;
- once the online bootstrap works, prepare or document an optional offline cache/archive
  so poor event-site Internet does not prevent rebuilding or reinstalling the app.

Do not commit Android SDKs, JDKs, Gradle caches, signing secrets, phone RSA keys, or APK
build output. The repository carries the bootstrap recipe and version manifest; generated
tools remain replaceable local state.

One-time attended setup is unavoidable:

1. Enable developer options and USB debugging, authorize the workstation RSA key, and
   grant the app permission for the AVC USB device.
2. Establish wireless debugging. If Motorola's Android 13 wireless-pairing UI is not
   reliable, use the Android-documented fallback: connect to the workstation once,
   run `adb tcpip 5555`, then `adb connect <phone-address>:5555` after moving the phone's
   USB-C port to the car.
3. Connect the phone to the car with a known data-capable USB-C OTG adapter/cable, power
   the car independently, and leave the vehicle in a safe non-moving mode.

After that setup, `scripts/android/android_loop.ps1` builds, tests, installs, starts,
waits for a healthy framed session, verifies complete frames through the phone's
WebSocket, captures diagnostics, and exits nonzero on failure. Structured
`AVC_BRIDGE_HEALTH` logcat records and the HTTP `/health` endpoint make the proof
independent of a person looking at either screen. Use `-SkipRelay` only for a deliberate
USB-only diagnostic run.

Physical cable insertion, USB permission after app reinstall/reset, phone reboot,
hotspot permission dialogs, and car power cycling remain attended boundaries unless
additional switching hardware is deliberately added.

## Test Strategy

- Pure JVM tests feed the Kotlin parser synthetic and captured byte streams covering
  fragmentation, garbage recovery, unknown IDs, sequence gaps, `DROPPED_BEFORE`, mixed
  message classes, and reconnects.
- A protocol conformance fixture checks Kotlin constants and field decoding against
  packets produced from the C protocol definition. Do not introduce an NDK dependency
  merely to share a small packed header.
- Android tests cover USB permission/state transitions, lifecycle restart, bounded
  queues, phone preview model, WebSocket slow-client behavior, and `/health` output.
- Hardware tests use the existing competition image and record sustained USB and relay
  rates, last-frame age, all app drop counters, and firmware stats.
- Disconnect/reconnect tests prove that app or network state cannot select TEST/STUDENT
  mode or enable motors.

## Power and Mechanical Safety

In USB host mode the phone sources VBUS. The car is independently battery-powered, so the
first bench check must confirm the cable and board do not create an unintended back-power
path. For the MVP, run the phone from its own 5000 mAh battery and do not attempt to charge
it while it hosts the car. Powered hubs or charge-through OTG adapters add failure modes
and should be evaluated only if measured runtime requires them.

Before vehicle use, validate phone temperature, battery drain, cable retention, connector
strain relief, the 185 g mass and mounting location, and whether the relay car remains a
fair competition vehicle. A dedicated demonstration car remains the cleanest race-day
choice.

## Boundaries and Deferred Work

- No general remote motor, steering, servo, or vehicle-mode commands.
- No firmware fork and no Android-specific packet IDs for the MVP.
- No dependency on Android Chrome Web Serial or WebUSB.
- No app-store release, account system, cloud service, or venue network.
- No polished telemetry styling; the generic data path and health evidence matter first.
- No recording in the first relay proof. Add bounded recording after USB and Wi-Fi are
  proven, then evaluate hardware H.264 only if RF measurements justify it.
- No replacement of the standalone PC Web Serial viewer.

The inspected Bunny Vision firmware/software tree contains useful CDC host-side lineage
but no reusable Android/Gradle application or confirmed WebUSB spike. Treat claims of an
existing phone browser spike in older notes as stale unless an artifact is found later.

## Source Material

- `docs/plans/usb-debug-telemetry/plan.md`
- `docs/research/AVC_USB_Debug_Display_Current_State.md`
- `docs/research/AVC_USB_Debug_Transport_Protocol.md`
- `docs/research/AVC_RaceDay_Wireless_Frame_Relay.md`
- `src/common/avc_usb_debug/avc_usb_debug_protocol.h`
- `src/usb_debug_host/`
- Android USB host documentation:
  <https://developer.android.com/develop/connectivity/usb/host>
- Android USB-host debugging over network:
  <https://developer.android.com/develop/connectivity/usb>
- Android local-only hotspot documentation:
  <https://developer.android.com/develop/connectivity/wifi/localonlyhotspot>
