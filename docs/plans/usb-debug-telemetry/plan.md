+++
type = "plan"
id = "usb-debug-telemetry"
status = "pending"
created = "2026-08-21"

[[steps]]
id = "lock-boundaries"
title = "Lock the system-mode, USB-link, telemetry-session, and student-code boundaries"
status = "done"

[[steps]]
id = "transport-correctness-audit"
title = "Measure the current USB service cost and re-audit callback-driven transmit behavior"
status = "done"
depends_on = ["lock-boundaries"]

[[steps]]
id = "buffer-ownership"
title = "Close the live-camera-buffer aliasing window with an explicit ownership/drop rule"
status = "done"
depends_on = ["transport-correctness-audit"]

[[steps]]
id = "control-plane"
title = "Replace ad-hoc commands with framed bidirectional hello, capability, command, and response messages"
status = "done"
depends_on = ["lock-boundaries"]

[[steps]]
id = "tx-arbitration"
title = "Add bounded nonblocking arbitration for frames, logs, telemetry, stats, and responses"
status = "done"
depends_on = ["buffer-ownership", "control-plane"]

[[steps]]
id = "system-dispatch"
title = "Reduce main to platform service plus explicit test, student, and safe-mode dispatch"
status = "done"
depends_on = ["lock-boundaries"]

[[steps]]
id = "log-channel"
title = "Implement bounded UTF-8 diagnostic log messages without replacing RTT"
status = "done"
depends_on = ["tx-arbitration"]

[[steps]]
id = "named-telemetry"
title = "Implement typed named scalar telemetry with timestamp, units, and drop accounting"
status = "done"
depends_on = ["tx-arbitration"]

[[steps]]
id = "web-viewer-model"
title = "Extend the Web Serial viewer with handshake, logs, discovered variables, and connection state"
status = "done"
depends_on = ["control-plane", "log-channel", "named-telemetry"]

[[steps]]
id = "web-plots"
title = "Add bounded rolling plots and declarative dashboard configuration by telemetry name"
status = "done"
depends_on = ["web-viewer-model"]

[[steps]]
id = "standalone-web-viewer"
title = "Bundle the complete viewer into one self-contained HTML file"
status = "done"
depends_on = ["web-plots"]

[[steps]]
id = "runtime-gating"
title = "Enable the transport in the race image only when disconnected and connected costs are proven safe"
status = "done"
depends_on = ["system-dispatch", "tx-arbitration", "web-viewer-model"]

[[steps]]
id = "realistic-validation"
title = "Validate camera preview, telemetry, logs, disconnects, and control behavior under realistic load"
status = "pending"
depends_on = ["standalone-web-viewer", "runtime-gating"]

[[steps]]
id = "student-example"
title = "Document a minimal student telemetry example without supplying a race or control solution"
status = "pending"
depends_on = ["realistic-validation"]

[[steps]]
id = "design-doc-intent-audit"
title = "Audit design documents and protocol documentation against implementation"
status = "pending"
depends_on = ["student-example"]

[[steps]]
id = "test-runtime-impact-audit"
title = "Audit new tests and their runtime impact"
status = "pending"
depends_on = ["realistic-validation"]

[[steps]]
id = "external-review"
title = "Obtain independent external review"
status = "pending"
depends_on = ["design-doc-intent-audit", "test-runtime-impact-audit"]

[[exit_criteria]]
id = "host-neutral-protocol"
title = "The same framed protocol supports the Web viewer and a future Android bridge without firmware forks"
status = "met"

[[exit_criteria]]
id = "mode-boundaries"
title = "USB link state, recognized telemetry session, and vehicle operating mode are explicit and separate"
status = "met"

[[exit_criteria]]
id = "safe-disconnect"
title = "USB disconnect cannot start the student algorithm or enable motors"
status = "met"

[[exit_criteria]]
id = "nonblocking"
title = "Host commands and all outbound channels use bounded work and documented drop/backpressure rules"
status = "met"

[[exit_criteria]]
id = "frame-integrity"
title = "A transmitted frame cannot alias camera writes from a later frame"
status = "met"

[[exit_criteria]]
id = "generic-debug"
title = "The browser shows camera frames, log text, arbitrary named values, and selected rolling plots"
status = "met"

[[exit_criteria]]
id = "standalone-viewer"
title = "The complete viewer is one self-contained HTML file requiring no server, external assets, package install, or build step"
status = "met"

[[exit_criteria]]
id = "race-image"
title = "The competition image is validated both disconnected and connected under realistic load"
status = "pending"

[[exit_criteria]]
id = "student-boundary"
title = "Examples expose framework services without supplying the student algorithm or controller"
status = "pending"

[[exit_criteria]]
id = "design-doc-intent-audit"
title = "Design documents and protocol documentation match implementation"
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

# USB Debug Telemetry and Web Viewer

## Priority

Implementation is feature-complete for camera-assembly testing and for the Android host.
Park this plan while the operator screens the 24 camera assemblies. Resume it before
student handoff for the remaining competition-image safety cases, the student-facing
example, documentation/test audits, and external review. Viewer styling and dashboard
polish remain parked.

Detailed SCCB and signal diagnostics remain deferred; once implemented, they should
publish through this generic telemetry path.

## Completion Assessment

The transport objective is complete enough to be consumed by another host. The Rev A
competition image and all three current hosts use the same framed session; real hardware
has delivered camera, stats, logs, telemetry, correlated responses, stop/close, and rapid
reopen with zero reported transport or parser errors in the recorded runs. The standalone
browser viewer is also accepted for current use.

The formal plan is **not yet closed**. Real-phone work has now exercised the same framed
session through repeated app loss/restart, locked-screen operation, JPEG and H.264 load,
slow-client backpressure, and USB-device reconnects while camera input stayed at about
23.42 FPS with zero sequence or malformed-packet errors. `realistic-validation` still
owns explicit competition-image observation without a host, race-waiting/student load,
partial-frame physical unplug/replug, and motor-safe disconnect behavior. The student
example, complete design/test audit, and independent review also remain. None requires a
wire-contract change.

## Purpose

Turn the proven high-speed USB CDC camera stream into the common debug transport for the
Rev A competition firmware. The immediate host is the existing static Web Serial viewer.
The durable protocol must also support a future wired Android bridge without a separate
firmware implementation.

The completed plan leaves USB device support enabled in the normal competition image.
Disconnected operation must be effectively dormant, and opening CDC alone must not start
traffic: a recognized telemetry session explicitly requests its output channels.

This plan consolidates the completed proof in `usb-debug-display-transport` with the
correctness work previously placed in `usb-stream-optimization`. The proof is not being
repeated: live 320x200 RGB565 streaming, browser rendering, and sustained throughput are
already established in `docs/research/AVC_USB_Debug_Display_Current_State.md`.

The organizer supplies infrastructure and clear APIs, not a completed autonomous-driving
or speed-control solution. Students decide what algorithm and telemetry values to publish.

## Current State

- The device uses high-speed USB CDC with a versioned 32-byte `AVCU` envelope.
- Live camera receive was measured at about 2.87 MiB/s and 23.43 FPS with no observed
  drops; `START_SYNTH` reached about 34 MiB/s.
- The browser viewer renders 320x200 RGB565 frames and reports stream statistics.
- Frame, stream-statistics, framed control, bounded UTF-8 logs, and typed named scalar
  telemetry are implemented end to end. The viewer discovers values by name and renders
  bounded, selectable rolling plots.
- Host-to-device control is framed; legacy ASCII start/stop commands are no
  longer accepted by the competition firmware.
- USB readiness is currently `attach && startTransactions`: configured/enumerated and a
  host that has opened the CDC interface.
- `avc_usb_debug_stream__publish_frame()` retains a pointer to the live camera buffer,
  but records its camera generation. Before and after every 16 KiB staging copy, the
  transport checks the two-buffer reuse horizon. If capture catches up, the incomplete
  USB frame is aborted and the next frame carries explicit resynchronization evidence;
  the host never presents the partial frame.
- The transfer-complete callback already calls the bounded transmit dispatcher.
  `avc_usb_debug_stream__service()` handles received commands and also attempts transmit
  progression. Both paths submit through the same one-transfer ownership rule; no interrupt
  rewrite is currently justified.
- `main.c` uses explicit TEST, race-waiting, student-running, and safe/startup dispatch.
  USB camera publication exists in each frame-consuming operational path without hiding
  or selecting the student algorithm.
- The self-contained browser viewer has been accepted for current camera work. Additional
  styling is deferred; the generic transport, parser, and discovered-data model are the
  durable deliverables.
- The host-neutral slice required by `android-telemetry-bridge` is stable. Android must
  reuse the current `AVCU` envelope and framed session without a firmware fork.

## Architecture Boundaries

Keep these concepts separate even if the first implementation uses a simple policy:

1. **USB link state:** disconnected, enumerated/configured, or CDC open.
2. **Telemetry session state:** no recognized client, hello/capability exchange, active
   subscriptions. A generic terminal opening the port is not the same as the AVC app.
3. **Vehicle operating mode:** safe, local/host test, or student algorithm.

Opening CDC never selects a vehicle operating mode. The physical TEST input selects local
test through the system state machine. The PC or future Android telemetry application
identifies itself with a framed hello/session message and explicitly requests output
channels; that session remains independent while TEST or student code runs.

Never transition from a disconnected test session directly into a moving student mode.
Mode transitions disable motors unless the destination deliberately and safely re-enables
them. Telemetry may remain active while the student algorithm runs; a recognized app must
not permanently imply test mode.

The intended top-level shape is deliberately obvious:

```c
avc_system__init();

while (1)
{
    avc_system__service();

    switch (avc_system__mode())
    {
        case AVC_SYSTEM_MODE_TEST:
            avc_test__service();
            break;

        case AVC_SYSTEM_MODE_STUDENT:
            avc_student_algorithm__service();
            break;

        case AVC_SYSTEM_MODE_SAFE:
        default:
            avc_system__safe_stop();
            break;
    }
}
```

`avc_system__service()` may hide bounded platform maintenance: camera service, USB
enumeration/RX/TX, encoder sampling, input updates, battery/health checks, telemetry
scheduling, and safe mode-transition requests. It must not hide the student algorithm,
blocking waits, or unexpected actuator commands.

## Bidirectional Protocol

Retain the existing `AVCU` envelope and little-endian recovery rules. Add framed control
messages rather than extending the ASCII command parser indefinitely. The minimum control
plane should cover:

- hello, protocol version, build identity, and capabilities;
- command request plus correlated ACK/NACK response;
- start/stop or subscribe/unsubscribe for frames, logs, stats, and telemetry;
- requested update rate where meaningful;
- ping/health and explicit session close.

The first protocol must not expose a general remote motor-enable command. Host control is
for debug-output selection and safe mode requests until a separate safety review says
otherwise.

Use two outbound diagnostic concepts:

- **Log text:** bounded UTF-8 records with severity/category, intended for people. RTT
  remains available; USB logging supplements it rather than redefining every existing
  debug macro.
- **Telemetry:** named typed scalar samples with timestamp or frame number and optional
  units, intended for automatic discovery and plotting. Key/value strings may be useful
  during a proof but are not the durable typed representation.

Example API shape, subject to the `lock-boundaries` design step:

```c
AVC_DBG_LOG_INFO("camera initialized");
AVC_DBG_VALUE_F32("servo.position", servo_position, "normalized");
AVC_DBG_VALUE_F32("wheel.left.speed", left_speed, "m/s");
```

Calls must be cheap when no telemetry session subscribes, bounded when connected, and
safe to drop with counters. Do not format strings or block inside camera/USB ISRs.

## Transmit Policy

One bulk IN endpoint must carry responses, logs, telemetry, stats, and frame chunks.
Define a bounded scheduler before adding side channels:

- protocol responses and safety/status messages cannot be trapped behind an entire frame;
- bulk frames use latest-frame semantics and may be dropped;
- repetitive telemetry may be coalesced or dropped, with counters;
- logs are bounded/truncated and may be dropped rather than delaying control;
- memory use is fixed at build time; no heap allocation in the stream path;
- buffer ownership makes corruption impossible, even when that means dropping a frame.

The exact priority order and queue depths are deliverables of `tx-arbitration`, not hidden
implementation details.

## Web Viewer

Keep the browser as the race-week host because it already works and is the fastest surface
for students and LLM-assisted customization. Build on one parser/state model:

- clear disconnected, enumerated/open, and recognized-session status;
- existing camera preview and transport statistics;
- scrolling log console with severity filtering;
- automatically discovered telemetry table;
- bounded history buffers and selectable rolling plots;
- declarative dashboard configuration that names telemetry channels rather than editing
  the byte parser;
- graceful display of unknown message IDs and forward-compatible protocol errors.

The race-week deliverable is one self-contained HTML file with its CSS and JavaScript
inlined. It must open directly in a Web Serial-capable browser without a local HTTP server,
external assets, package installation, or a build step. Separate source files may remain
during development only if the checked-in standalone file is the authoritative handoff and
is validated directly from the filesystem.

Parser and model behavior should have host-only tests using recorded or synthetic byte
streams, including partial packets, garbage recovery, unknown IDs, reconnects, and mixed
message classes.

## Validation

Board-independent work can cover protocol structures, parser tests, Web UI behavior, and
synthetic streams. Final signoff requires the Rev A board:

- competition image with no host attached;
- host enumerated but CDC not open;
- Web viewer connected with camera, logs, and telemetry active;
- unplug/replug during a partial frame;
- realistic camera, LCD, and algorithm load;
- explicit observation that disconnect leaves motors safe;
- sustained counters for drops, endpoint busy, queue high-water marks, and send errors.

Performance statements must be measured. In particular, measure the current service call
before deleting or moving it, because transmit completion is already callback-driven.

Measured disconnected and connected runtime costs passed, and `runtime-gating`
changed the competition default to `CONFIG__USB_DEBUG_STREAM_ENABLE=1`. A
separate USB-only race firmware is not part of the architecture.

## Explicitly Out of Scope

- A native SDL/ImGui viewer.
- Implementing the Android application; this plan only keeps the protocol host-neutral.
- UVC/webcam mode.
- Removing the onboard LCD or deciding its future BOM status.
- General eGFX replacement or MCU graphics optimization.
- A completed student race, line-following, or PID algorithm.
- Broad API/code cleanup outside the narrow system-dispatch seam required here.

## Source Material

- `docs/research/AVC_USB_Debug_Display_Current_State.md`
- `docs/research/AVC_USB_Debug_Transport_Protocol.md`
- `src/common/avc_usb_debug/avc_usb_debug_protocol.h`
- `src/avc/avc_core0/source/avc_io/avc_usb_debug_stream.c`
- `src/avc/avc_core0/source/main.c`
- `src/usb_debug_host/webserial_viewer.html`
- `src/usb_debug_host/webserial_viewer.js`
- Bunny Vision host architecture, read-only reference:
  `D:/prj/wavenumber/bunny_vision/bunny_vision_firmware-west/bunny_vision_sw`
- `docs/plans/android-telemetry-bridge/plan.md`
