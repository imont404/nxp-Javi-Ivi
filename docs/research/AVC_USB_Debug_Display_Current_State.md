# AVC USB Debug Display Current State

Status: native preview and one-cable ROM programming proven; recovery validation active

Last updated: 2026-08-23

## Summary

The AVC USB debug display path is viable. The running proof streams live
`320x200` RGB565 camera frames from the MCXN947 over high-speed USB CDC to a
Chrome/Edge WebSerial viewer. The browser renders the camera feed on a canvas,
shows byte rate, frame rate, parser counters, and firmware stream stats, and
displays bounded device log records.

Transport correctness, framed bidirectional control, fixed-memory arbitration,
generic logs, typed named telemetry, automatic browser discovery, and standalone
viewer packaging are implemented. The standalone viewer also has selectable,
independently auto-scaled rolling plots with fixed history and selection bounds.
The competition default now includes the session-gated transport; realistic
mode/disconnect validation remains active-plan work. Basic feasibility does not
need to be re-proven.

This is also the implemented host boundary for the Android bridge. The Moto claims the
CDC interface, completes the same framed session, and consumes the same packets as the
native, Python, and Web Serial hosts. No Android-specific firmware mode or packet type
was added.

A native SDL2/Dear ImGui Windows viewer now consumes the same protocol. The
camera is contained in its own responsive panel, with separate connection,
telemetry, and deliberate firmware-programming controls. The running firmware
can safe-stop and enter MCXN947 ROM USB-HS HID through the framed session, after
which the shared host backend validates, erases, writes, resets, reconnects, and
resumes preview using only J11.

## Implemented Path

Firmware:

- Active project: `src/avc/avc_core0`.
- Shared wire protocol: `src/common/avc_usb_debug/avc_usb_debug_protocol.h`.
- USB CDC stream module:
  `src/avc/avc_core0/source/avc_io/avc_usb_debug_stream.c`.
- Public stream API:
  `src/avc/avc_core0/source/avc_io/avc_usb_debug_stream.h`.
- Main-loop publish point:
  `src/avc/avc_core0/source/main.c`.
- NXP USB device stack and descriptors were ported into the AVC project from
  the local FRDM-MCXN947 CDC example under `docs/examples/frdmmcxn947`.

Host tools:

- Shared host tooling root: `src/usb_debug_host`.
- Native Windows receiver:
  `src/usb_debug_host/usb_cdc_stream_read.cpp`.
- Shared native host/parser and programmer:
  `src/usb_debug_host/avc_host_core.cpp` and
  `src/usb_debug_host/avc_programmer.cpp`.
- Native SDL2/Dear ImGui viewer and bounded CLI:
  `src/usb_debug_host/avc_viewer.cpp` and
  `src/usb_debug_host/avc_tool.cpp`.
- Python receiver:
  `src/usb_debug_host/usb_cdc_stream_read.py`.
- Static WebSerial viewer:
  `src/usb_debug_host/webserial_viewer.html`,
  `src/usb_debug_host/webserial_viewer.js`,
  `src/usb_debug_host/webserial_viewer.css`.
- Self-contained WebSerial handoff:
  `src/usb_debug_host/avc_usb_debug_viewer.html`.
- Native Android USB host and one-browser Wi-Fi relay:
  `src/android/avc_bridge`.

## Wire Protocol

Packets use a 32-byte `AVCU` envelope:

- `magic`: `"AVCU"` little-endian
- `version`: `1`
- `header_bytes`: `32`
- `flags`
- `msg_id`
- `sequence`
- `payload_length`
- `arg0`, `arg1`, `arg2`

Implemented messages:

- `AVC_DBG_RUI_WRITE_FRAME_BUFFER_RAW`: RUI-style raw framebuffer chunks.
- `AVC_DBG_STATS_REPORT`: firmware stream counters.
- `AVC_DBG_LOG_TEXT`: bounded UTF-8 severity/category/text records.
- `AVC_DBG_TELEMETRY_SCALAR`: typed named values with timestamps and units.
- `AVC_DBG_CONTROL_HELLO`: recognize a telemetry client and report capabilities.
- `AVC_DBG_CONTROL_SET_CHANNELS`: independently request frame, stats, and log
  output or stop them.
- `AVC_DBG_CONTROL_PING` and `AVC_DBG_CONTROL_CLOSE`: check or close a session.
- `AVC_DBG_CONTROL_ENTER_ISP`: confirmed, session-bound safe transition to the
  MCXN947 ROM USB-HS HID bootloader.

Reserved message classes already exist for:

- RUI display/UI operations
- RIO remote I/O
- STATS
- CONTROL

The log path uses eight fixed records, 15-byte category and 160-byte text limits,
and observable drop/high-water counters. It does no formatting unless a
recognized host session subscribes and never replaces the existing RTT path.

The telemetry path supports i32, u32, f32, and bool values with 31-byte names
and 15-byte units. Its sixteen fixed records coalesce repeated pending names to
the newest value and expose drop, high-water, and coalesce counters.

## Firmware Behavior

The Python receiver, native receiver, and Web Serial viewer use a recognized
framed session. Opening CDC alone produces no stream and does not change vehicle
mode. Legacy ASCII start/stop commands are no longer accepted; the receiver
options remain only for comparison with older firmware images.

The camera path is intentionally nonblocking from the control-loop point of
view:

- `avc__next_frame()` runs in interrupt context and only publishes a completed
  frame pointer plus a ready flag.
- The main loop consumes frames according to vehicle mode. TEST may add the
  existing eGFX overlay; race-waiting consumes only while the frame channel is
  subscribed; student mode invokes the student callback. Each path publishes
  the resulting frame without allowing USB to select that mode.
- `publish_frame()` does a short critical-section update, copies/builds/submits
  the first 16 KiB USB chunk, then returns.
- Remaining chunks are submitted from the USB CDC send-completion callback.
- If a previous USB frame is still active when a new camera frame is published,
  the new debug frame is dropped.

The current implementation uses one aligned 16 KiB USB TX staging buffer.
Camera data chunks are copied from the camera frame into that staging buffer
before `USB_DeviceCdcAcmSend()`. This is not zero-copy, but at 24 FPS the copy
load is small relative to the measured USB margin.

The same staging buffer is owned by one bounded dispatcher. Four queued control
responses have priority between packets, but yield after four replies when other
traffic is pending. Logs yield to a pending frame after two diagnostic packets.
Statistics coalesce and camera frames retain latest-frame/drop semantics.

The camera backend uses two ping-pong frame buffers. USB records the camera
generation when accepting a frame and checks the two-buffer reuse horizon both
before and after each staging copy. If capture catches up, USB aborts the
incomplete frame before another chunk is submitted. The next frame starts with
`DROPPED_BEFORE`, allowing hosts to discard partial assembly and resynchronize.

A 2026-08-21 backpressure run delayed host reads for two seconds with a 4 KiB
receive buffer. Firmware reported 46 dropped frame attempts; the Python parser
resumed with zero malformed packets, resync events, invalid headers, or sequence
errors. This establishes the deliberate drop rule rather than relying on the
normal measured bandwidth margin.

## Measurements

Synthetic ceiling:

- Native receiver measured roughly `34 MiB/s` using `START_SYNTH`.
- A high-rate synthetic run can occasionally show a Windows CDC/parser gap at
  the ceiling. Treat synthetic mode as a stress test, not the integration
  health indicator.

Camera stream:

- Native receiver for 60.017 s: `2.870 MiB/s`, `23.43 FPS`, 11248 frame
  chunks, 60 stats packets, zero malformed packets, zero sequence errors, zero
  firmware frame drops, zero endpoint-busy returns, zero send errors.
- Python receiver for 10.030 s: `2.868 MiB/s`, `23.43 FPS`, 1878 frame chunks,
  10 stats packets, zero parser errors and zero firmware drop/busy/send-error
  counters.
- Framed-session Python receiver on 2026-08-21 for 10.050 s: `2.876 MiB/s`,
  `23.48 FPS`, two correlated control responses, and zero malformed packets,
  resyncs, sequence errors, firmware drops, endpoint-busy returns, or send errors.
- Mixed frame/stat/log/control run on 2026-08-21 for 10.015 s: `2.875 MiB/s`,
  `23.47 FPS`, four PINGs in one host write, all six expected responses, response
  queue high-water four, one system log, and zero queue drops, malformed packets,
  resyncs, sequence errors, firmware drops, endpoint-busy returns, or send errors.
- Mixed frame/stat/log/telemetry/control run on 2026-08-21 for 10.019 s:
  `2.873 MiB/s`, `23.45 FPS`, one discovered `system.uptime` u32 sample with `ms`
  units, all six expected control responses, and zero transport, parser, queue,
  or firmware errors.
- Competition-default hardware run on 2026-08-21 for 10.039 s: `2.867 MiB/s`,
  `23.41 FPS`, framed control/log/telemetry present, and zero transport, parser,
  queue, or firmware errors. RTT confirmed the physical TEST input selected
  `TEST` during this run; race-waiting mode still needs a separate observation.
- Closed-session/reopen validation on 2026-08-21 wrote legacy `START` with DTR
  low and observed zero returned bytes, then completed two immediate framed
  open/stream/stop/close cycles at 23.5 FPS with zero resync, sequence, drop,
  busy, or send errors. This exposed and fixed a bulk-OUT re-arm fault. Both
  Python and native receivers now wait for stop and close responses.
- The updated framed native receiver measured `34.472 MiB/s` from the synthetic
  source for one second with zero parser or sequence errors.
- Chrome WebSerial viewer rendered the live camera feed on 2026-07-21.
- The WebSerial/CDC path was then observed running live for about 15 minutes.
- The native viewer sustained 113 complete frames in five seconds with zero
  malformed packets after its final UI build.
- The native viewer worker completed application preview, safe ISP entry,
  ROM query/erase/write/reset, application reconnect, and resumed preview with
  186 complete frames and zero malformed packets.
- The hardened CLI repeated that cycle with a 370,740-byte image. Its packaged
  NXP `rblhost` 0.2.0 backend performs a full readback and requires the byte
  count and SHA-256 to match before reset; installed SPSDK `blhost` 3.10.0
  remains a proven fallback.
- Missing, wrong-extension, invalid-vector, and nonexistent-port program
  requests were rejected before ISP; a complete camera frame arrived after each
  refusal. Physical application-stream unplug/replug remains pending bench input.

RTT remains available while USB/WebSerial is running. A live attach without
reset showed the RTT control block and camera telemetry around `23.39 FPS`.

## WebSerial Viewer State

The race-week viewer is the self-contained
`src/usb_debug_host/avc_usb_debug_viewer.html`. Open it directly in Chrome or
Edge; no server, install, build step, or external asset is required. The local
server remains optional maintainer tooling for source-file iteration.

Chrome WebSerial requires a user gesture:

1. Click `Connect`.
2. Select the AVC CDC port, observed as `USB Serial Device (COM31)` on the
   current bench.
3. The viewer negotiates a telemetry session and requests frame/stat/log/telemetry channels.

The viewer has a stale-data watchdog. If bytes stop arriving, it reports a stale
camera channel and repeats the framed channel request. Camera publication now
exists in TEST, race-waiting (only while the frame channel is active), and the
student frame-service path. USB session state never selects a vehicle mode.

Typed telemetry is discovered automatically in a name-keyed table. Selected
signals retain at most 300 samples and render as independently auto-scaled
rolling plots, with at most six plots active. Browser tests exercise fragmented
reads, mixed packets, log and telemetry DOM updates, two full frames, Stop/Start,
history truncation, direct file-origin Web Serial, and zero external asset loads.

## eGFX Relationship

eGFX drawing already operates on RAM-backed `eGFX_ImagePlane` buffers. The LCD
driver's `eGFX_DumpRaw()` is a physical-output step that writes ST7789 pixels
over LPSPI/eDMA. It is DMA-assisted but main-loop paced.

The USB path can become an eGFX-style virtual display backend later:

- eGFX draws into a framebuffer exactly as it does today.
- A USB dump function publishes framebuffer or dirty-rect chunks over CDC.
- The host renders those chunks instead of the MCU writing LCD pins.

This is not implemented yet. The current proof publishes the `camera_view`
buffer after the existing drawing operations.

## Follow-Up Ownership

Generic browser telemetry work remains in
`docs/plans/usb-debug-telemetry/plan.md`. It owns:

- explicit frame-buffer ownership and measured main-loop cost;
- framed bidirectional hello, control, and response messages;
- bounded arbitration among frames, logs, telemetry, stats, and responses;
- separate USB-link, recognized-session, and vehicle-mode state;
- remaining browser-specific unplug/replug and host-only parser cases;
- remaining realistic disconnected/connected mode validation.

Native SDL tooling and the one-cable ROM programmer are implemented and tracked
by `docs/plans/one-cable-host-tool/plan.md`. UVC, LCD removal, and general eGFX
replacement remain outside the race-week transport work. The Android application
is implemented and tracked by its separate plan.

The Android consumer is implemented separately under `src/android/avc_bridge` and tracked
by `docs/plans/android-telemetry-bridge/plan.md`. On the real Rev A car, the Moto G Power
5G (2023), Android 14/API 34, enumerates `WAVENUMBER AVC` (`1FC9:0094`) through the proven
OTG-adapter/A-to-C cable topology. Persistent USB association reopens sequential sessions
after physical detach/attach without another prompt. The app repeatedly completes framed
`HELLO`, `SET_CHANNELS`, `PING`, and `CLOSE`; sustained input remains about 23.42 FPS and
2.869 MiB/s with zero sequence or malformed-chunk errors.

The phone serves a self-contained one-browser page on port 8765 and forwards generic
telemetry as normal `AVCU`. The browser selects full-rate JPEG (`AVCJ`), fragmented-MP4
H.264 (`AVC4`), or diagnostic RGB565 (`AVCR`) without restarting the app; JPEG is the
default. A same-process 120-frame run measured 23.706 FPS / 3.135 Mbit/s JPEG, 22.696 FPS
/ 0.740 Mbit/s H.264, and 23.347 FPS / 23.907 Mbit/s raw while the USB parser stayed
clean. These three downstream envelopes are phone-to-browser representations, not
firmware protocol extensions.

The phone UI is now limited to the live image, connection/mode, usable URL, and a large
disconnected overlay. Detailed health remains in logcat and `/health`. Independent fixed
mailboxes, latest-frame semantics, and a two-second send watchdog keep Wi-Fi work from
blocking USB. Raw and compressed slow-client proofs, repeated app restarts, physical USB
reconnects, Soft AP plus wireless adb, and screen-off `Dozing` operation have passed.
Remaining Android work is secured 5 GHz/venue RF, race-duration battery and thermal
validation, stale-session recovery after a power-only car restart, and the parked move of
long-lived ownership from the activity into a foreground service.

## Hardware and Event Implication

The board can stream camera debug frames to a host over high-speed USB with
substantial measured bandwidth margin and no observed drops in the 24 FPS camera
workload. Rev A remains the 2026 competition hardware. The LCD stays as the
cable-free camera check and local fallback; the Web Serial viewer is expected to
be the primary large-screen preview during the event.
