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
id = "compression-inventory"
title = "Inventory the Moto hardware video encoders and define bounded full-rate JPEG and H.264 measurements against the existing RGB565 stream"
status = "done"
depends_on = ["relay-backpressure"]

[[steps]]
id = "full-rate-jpeg"
title = "Benchmark full-camera-rate JPEG from RGB565 without blocking USB or growing latency"
status = "done"
depends_on = ["compression-inventory"]

[[steps]]
id = "hardware-h264"
title = "Prove and benchmark hardware-accelerated H.264 encoding from the existing RGB565 camera stream"
status = "done"
depends_on = ["compression-inventory"]

[[steps]]
id = "compressed-browser-delivery"
title = "Select and integrate the simplest compressed browser delivery path supported by measured phone performance"
status = "done"
depends_on = ["full-rate-jpeg", "hardware-h264"]

[[steps]]
id = "vehicle-integration"
title = "Validate battery life, heat, reconnects, RF behavior, and noninterference on the car; keep mounting and cable retention outside the software track"
status = "active"
depends_on = ["compressed-browser-delivery"]

[[steps]]
id = "design-doc-intent-audit"
title = "Audit the Android design and protocol use against the implemented USB contract"
status = "done"
depends_on = ["compressed-browser-delivery"]

[[steps]]
id = "test-runtime-impact-audit"
title = "Audit app, host, hardware, and firmware validation coverage and runtime impact"
status = "done"
depends_on = ["compressed-browser-delivery"]

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
id = "full-rate-compression"
title = "The phone sustains the camera frame rate through a bounded compressed-video path without degrading USB capture"
status = "met"

[[exit_criteria]]
id = "rf-bitrate"
title = "Measured compressed bitrate and latency are suitable for smooth one-viewer race-day use on a controlled 5 GHz link"
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
status = "met"

[[exit_criteria]]
id = "test-runtime-impact-audit"
title = "New tests are listed and runtime impact is reviewed"
status = "met"

[[exit_criteria]]
id = "external-review"
title = "Independent external review is complete"
status = "pending"
+++

# Android Telemetry Bridge

## Current Status

Execution was authorized on 2026-08-21. The native USB host, phone preview, and full-rate
one-browser Wi-Fi relay are complete on the real Rev A car. The Moto G Power serves its
embedded page at `http://<phone-address>:8765/`, sends each JPEG as one bounded `AVCJ`
WebSocket message, and preserves generic telemetry as normal `AVCU` messages. A 240-frame
end-to-end run delivered 23.493 FPS at 1.972 Mbit/s with a most-recent-frame age of about
24 ms while USB remained at 23.42 FPS and 2.870 MiB/s with zero sequence or malformed-
chunk errors. Headless Chrome decoded 120 frames in five seconds with no page errors and
nonblack canvas pixels. A two-second network-send
deadline now closes a client that stops reading. Six consecutive forced stalls kept USB
advancing, stayed near 56-59 MiB PSS after warm-up, and reconnected to a complete frame
within one source frame in the final recorded run. Six subsequent abrupt app-process
losses also recovered distinct firmware sessions 27-32, clean USB video, telemetry, and
recent Wi-Fi frames without cable handling. The relay explicitly selects the IPv4 stack
and binds the active WLAN address. A connected-device foreground service now keeps
CPU and Wi-Fi active through the secure lockscreen: live relay verification passed while
Android reported `Dozing`, screen off, and light idle. A 30-second loaded baseline held
27 C, 23.42 USB FPS, 2.869 MiB/s, 49-60 MiB PSS, and roughly 427-588 mA discharge. Physical
integration remains parked; the active software work is documentation and test/runtime
audit of the now-complete compressed relay.

The Android codec inventory reports the MediaTek `c2.mtk.avc.encoder` as a hardware,
vendor AVC encoder. At 320x200 it accepts planar, semiplanar, and flexible YUV420 byte
buffers as well as surface input, so the first H.264 proof can use a bounded RGB565-to-
YUV420 conversion without adding EGL or changing the camera firmware. The vendor codec
table advertises roughly 125-129 FPS at 320x240; the live measurement remains decisive
because it includes conversion, codec queueing, USB capture, preview, and relay load.

Both live compression probes now sustain the 23.42 FPS source while USB remains at
2.869-2.870 MiB/s with zero sequence and malformed-chunk errors. JPEG quality 70 measured
23.47 FPS, 1.956 Mbit/s, about 4.0 ms mean end-to-end encode latency, no drops after
startup, and 48 MiB PSS. MediaTek hardware H.264 at a 750 kbit/s target measured 23.38
FPS, 0.752 Mbit/s, about 50.8 ms mean latency, four bounded startup drops, and 74 MiB PSS.
The measured RF saving from H.264 is real, but full-rate JPEG is already only about eight
percent of raw RGB565 bandwidth and is much simpler for an ordinary browser to consume.
Use JPEG for the next one-browser MVP; retain H.264 as a proven option if race-network
measurements justify its added framing, initialization, and browser-decoder complexity.
That JPEG browser path is now implemented. An initial 75 ms inbound WebSocket poll capped
delivery at 13.2 FPS; reducing the bounded client-control poll to 5 ms removed the cap and
produced the 23.493 FPS result above. A repeated slow-reader test still closed the client
at the two-second watchdog, kept USB healthy, and remained bounded at about 59 MiB PSS.

The design audit confirms that firmware and the USB contract did not change: Android
still sends only `HELLO`, `SET_CHANNELS`, `PING`, and `CLOSE`, and contains no actuator or
vehicle-mode command. `AVCJ` exists only between the phone and its embedded browser;
generic telemetry remains `AVCU`. The test/runtime audit covers new JVM framing and
RGB565-to-I420 fixtures plus real-phone JPEG, H.264, WebSocket payload, slow-reader,
forced-restart, locked-screen, and Chrome decode proofs. With a live JPEG browser, a spot
sample showed about 50 MiB PSS, 25.9 percent process CPU in Android's eight-core `top`,
27 C battery temperature, and roughly 483 mA discharge. These are healthy development
measurements, not a substitute for a race-duration battery/thermal run.

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
                                      +--> latest complete frame --> JPEG
                                           + generic telemetry
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
3. Serve a static page embedded in the APK and forward a full-rate, latest-frame JPEG
   stream plus generic `AVCU` telemetry to one laptop browser over binary WebSocket.
4. Provide an in-app and machine-readable health view so the development loop can verify
   USB state, session state, last frame age, rates, drops, and connected browser count.

The USB side remains the existing `AVCU` contract. On Wi-Fi, telemetry retains `AVCU`
framing while each independently decodable JPEG uses one small `AVCJ` envelope carrying
frame ID, dimensions, byte count, capture timestamp, and a dropped-before flag. The phone
may omit complete camera frames under backpressure, but it never forwards a partial frame
as complete. Camera frames use latest-complete-frame semantics throughout.

Raw full-rate video is about 24.1 Mbit/s. The completed first Wi-Fi proof sends every
fourth frame, about 5.9 FPS and 6.0 Mbit/s, while continuing to drain USB at full rate.
The next experiment keeps the firmware and RGB565 camera mode fixed and compares bounded
full-rate JPEG with Android hardware H.264. Measure achieved encode FPS, encoded bitrate,
frame age/latency, drops, USB health, CPU time, memory, temperature, and battery current.
The measured result selected JPEG: it sustains the full 23.4 FPS in the browser at about
1.97 Mbit/s. H.264 remains the lower-bitrate option, but does not justify its additional
browser transport and decoder lifecycle for the current controlled 5 GHz link. Recording,
multi-client support, and a polished race dashboard remain deferred.

WebSocket uses TCP because an ordinary browser cannot consume arbitrary UDP datagrams.
Bounded application queues prevent TCP backpressure from becoming growing latency: keep
at most the newest complete relay frame, drop superseded frames with counters, and close
a client that cannot drain within a fixed limit. USB input must never wait for Wi-Fi.

## Network Shape

Development and race operation have different best first configurations:

- **Development:** put the workstation and phone on the same controlled 5 GHz access
  point or travel router. This gives the most reliable wireless `adb` connection while
  the phone's only USB-C port is connected to the car.
- **Race candidate:** manually enable the phone's 5 GHz hotspot, restart the bridge so it
  binds the hotspot address, and join the projector laptop to it. Device inventory reports
  no concurrent STA+AP support, so this disconnects `yellow` and wireless adb. The hotspot
  client path and post-switch address must still be tested before relying on it.
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
- No recording in the compression proof. Evaluate JPEG and hardware H.264 directly from
  the live RGB565 stream before changing camera format or committing to a browser video
  protocol.
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
