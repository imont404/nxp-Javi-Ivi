# AVC USB Debug Display Current State

Status: viability proof complete; follow-up owned by `docs/plans/usb-debug-telemetry`

Last updated: 2026-08-21

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

This is also the stable host boundary for the Android bridge. Android feasibility
does not require a new firmware mode or protocol: it can claim the CDC interface,
complete the same framed session, and consume the same packets as the three existing
hosts.

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
- Python receiver:
  `src/usb_debug_host/usb_cdc_stream_read.py`.
- Static WebSerial viewer:
  `src/usb_debug_host/webserial_viewer.html`,
  `src/usb_debug_host/webserial_viewer.js`,
  `src/usb_debug_host/webserial_viewer.css`.

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

The active work is consolidated in
`docs/plans/usb-debug-telemetry/plan.md`. It owns:

- explicit frame-buffer ownership and measured main-loop cost;
- framed bidirectional hello, control, and response messages;
- bounded arbitration among frames, logs, telemetry, stats, and responses;
- separate USB-link, recognized-session, and vehicle-mode state;
- physical USB unplug/replug and additional host-only parser cases;
- remaining realistic disconnected/connected mode validation.

Native SDL tooling, an Android application, UVC, LCD removal, and general eGFX
replacement are intentionally outside that race-week plan.

The Android consumer is implemented separately under
`src/android/avc_bridge` and tracked by
`docs/plans/android-telemetry-bridge/plan.md`. On 2026-08-21 the Moto G Power 5G
(2023), Android 14/API 34, enumerated the real car as `WAVENUMBER AVC` (`1FC9:0094`)
through a USB-C OTG adapter and USB-A-to-C data cable. The native app claimed the CDC
bulk interface and repeatedly completed framed `HELLO`, `SET_CHANNELS(0)`, `PING`, and
`CLOSE`. Its pinned command-line toolchain, eight JVM protocol fixtures, wireless adb
deployment loop, structured health logging, and Android lint all pass. The bounded
three-buffer phone preview now displays the live RGB565 camera at about 23.42 FPS and
2.869 MiB/s with zero sequence or malformed-chunk errors in the recorded sustained run.
Graceful close and immediate reopen pass. The phone now also serves a standalone page on
port 8765 and preserves `AVCU` framing over a one-client binary WebSocket. Desktop Chrome
on the controlled `yellow` 5 GHz network rendered decimated live video and generic
`system.uptime` telemetry while the USB counters remained clean. The relay uses an
independent fixed three-buffer latest-frame mailbox, and the unattended verification
script validates complete contiguous frames without visual assistance. A two-second
send watchdog contains stalled TCP writes as client-local failures. Six consecutive
non-reading-client tests kept USB advancing, held warm app PSS around 56-59 MiB, and
successfully reconnected to a recent complete frame after every forced close. Physical
vehicle and race-network validation remain the next Android step. A separate six-cycle
abrupt app-process test recovered distinct firmware sessions 27-32, clean USB video,
telemetry, and recent relay frames without touching the cable. It also exposed and fixed
an Android restart case that created an IPv6-only wildcard listener behind an IPv4 URL;
the server now explicitly uses the IPv4 stack and active WLAN address.
The bridge also runs a connected-device foreground service with CPU and Wi-Fi locks while
the activity owns the session. With Android reporting `Dozing`, screen off, and light
idle, the laptop still received recent complete frames and USB remained at 23.42 FPS and
2.869 MiB/s. A 30-second loaded sample held 27 C, 49-60 MiB PSS, and approximately
427-588 mA discharge. Longer battery/thermal and physical vehicle tests remain open.

## Hardware and Event Implication

The board can stream camera debug frames to a host over high-speed USB with
substantial measured bandwidth margin and no observed drops in the 24 FPS camera
workload. Rev A remains the 2026 competition hardware. The LCD stays as the
cable-free camera check and local fallback; the Web Serial viewer is expected to
be the primary large-screen preview during the event.
