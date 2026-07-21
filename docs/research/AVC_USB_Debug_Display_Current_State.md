# AVC USB Debug Display Current State

Status: parked after viability proof

Last updated: 2026-07-21

## Summary

The AVC USB debug display path is viable. The running proof streams live
`320x200` RGB565 camera frames from the MCXN947 over high-speed USB CDC to a
Chrome WebSerial viewer. The browser renders the camera feed on a canvas and
shows byte rate, frame rate, parser counters, and firmware stream stats.

This work was paused so the PCB revision can move forward. The next phase is
cleanup and productization, not proof of basic feasibility.

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

Reserved message classes already exist for:

- RUI display/UI operations
- RIO remote I/O
- LOG debug text
- STATS
- CONTROL

The next protocol feature should be `AVC_DBG_LOG_TEXT`, carried as UTF-8 log
payloads interleaved between frame and stats packets. It should use a small
firmware ring buffer and drop logs instead of blocking.

## Firmware Behavior

Host commands are still ASCII during bringup:

- `START`: stream camera frames.
- `START_SYNTH` or `SYNTH`: stream synthetic frames as a maximum-rate stress
  source.
- `STOP` or `0`: stop either source.

The camera path is intentionally nonblocking from the control-loop point of
view:

- `avc__next_frame()` runs in interrupt context and only publishes a completed
  frame pointer plus a ready flag.
- The main loop consumes the frame, performs the existing eGFX marker/overlay
  drawing, and calls `avc_usb_debug_stream__publish_frame(frame)`.
- `publish_frame()` does a short critical-section update, copies/builds/submits
  the first 16 KiB USB chunk, then returns.
- Remaining chunks are submitted from the USB CDC send-completion callback.
- If a previous USB frame is still active when a new camera frame is published,
  the new debug frame is dropped.

The current implementation uses one aligned 16 KiB USB TX staging buffer.
Camera data chunks are copied from the camera frame into that staging buffer
before `USB_DeviceCdcAcmSend()`. This is not zero-copy, but at 24 FPS the copy
load is small relative to the measured USB margin.

The camera backend uses ping-pong frame buffers. At 24 FPS there is about
41.67 ms per frame. The measured USB transport time is comfortably below that
period, so the current pointer lifetime policy has worked in bench testing.
If the camera frame rate is increased substantially, revisit explicit buffer
ownership or a full-frame staging policy.

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
- Chrome WebSerial viewer rendered the live camera feed on 2026-07-21.
- The WebSerial/CDC path was then observed running live for about 15 minutes.

RTT remains available while USB/WebSerial is running. A live attach without
reset showed the RTT control block and camera telemetry around `23.39 FPS`.

## WebSerial Viewer State

The viewer is served locally:

```powershell
.\src\usb_debug_host\serve_webserial_viewer.ps1
```

Default URL:

```text
http://127.0.0.1:8765/webserial_viewer.html
```

Chrome WebSerial requires a user gesture:

1. Click `Connect`.
2. Select the AVC CDC port, observed as `USB Serial Device (COM15)` on the
   current bench.
3. The viewer immediately sends `START` and begins rendering.

The viewer has a stale-data watchdog. If bytes stop arriving, it reports
`No frames; check test mode` and sends a fresh `START` after a short interval.
The current firmware only publishes camera frames from the test-mode frame path.

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

## Near-Term Backlog

Parked work to resume after the PCB revision:

- Code cleanup around USB stack file ownership, naming, and config.
- Add `AVC_DBG_LOG_TEXT` and host display of debug prints.
- Decide whether overlay text stays drawn into the camera buffer or moves to
  host-composed text/RUI packets.
- Build an SDL-based C host viewer with auto-connect/reconnect behavior. This
  is expected to be the robust student laptop fallback when cables are yanked
  during floor testing.
- Evaluate a dedicated Android app using the same framing if Android Chrome
  WebSerial is not enough for the competition phones.
- Add side-channel messages for potentiometer values and other UI/state.
- Consider nonvolatile storage for settings after the transport/UI shape is
  stable.
- Write the reusable MCXN HS USB CDC porting guide for the second agent.
- Revisit zero-copy or explicit buffer ownership if frame rate or payload size
  increases.

## PCB Implication

The fundamental debug-display transport is proven well enough to inform the
PCB revision: the board can stream camera debug frames to a host over high-speed
USB with substantial bandwidth margin and no observed drops in the 24 FPS
camera workload. The LCD can be reconsidered as optional, demoted, or retained
for non-host use cases, but it is no longer the only viable debug display path.
