+++
type = "plan"
id = "usb-debug-display-transport"
status = "pending"
created = "2026-07-20"

[[steps]]
id = "sample-intake"
title = "Study local NXP HS USB examples and the active AVC frame path"
status = "done"

[[steps]]
id = "usb-only-throughput-proof"
title = "Prove high-speed USB connectivity and sustained bulk throughput without camera involvement"
status = "done"
depends_on = ["sample-intake"]

[[steps]]
id = "host-receiver-proof"
title = "Build a PC receiver and static browser viewer for framed RGB565 test data"
status = "done"
depends_on = ["usb-only-throughput-proof"]

[[steps]]
id = "transport-framing"
title = "Define the AVC debug transport framing, channels, drop policy, and host commands"
status = "done"
depends_on = ["usb-only-throughput-proof"]

[[steps]]
id = "avc-integration-slice"
title = "Integrate the nonblocking USB stream with the AVC camera/main-loop frame path"
status = "done"
depends_on = ["host-receiver-proof", "transport-framing"]

[[steps]]
id = "mcxn-cdc-porting-guide"
title = "Write a reusable guide for adding NXP MCXN HS USB CDC to another firmware project"
status = "pending"
depends_on = ["avc-integration-slice"]

[[steps]]
id = "android-path-decision"
title = "Select and validate the student phone path: WebSerial, WebUSB, or a small custom Android app"
status = "pending"
depends_on = ["host-receiver-proof", "transport-framing"]

[[steps]]
id = "uvc-compatibility-evaluation"
title = "Evaluate whether a UVC webcam-compatible mode is worth adding beside the debug transport"
status = "pending"
depends_on = ["usb-only-throughput-proof"]

[[steps]]
id = "bom-display-decision"
title = "Decide whether USB debug display can replace or demote the onboard LCD in the BOM"
status = "pending"
depends_on = ["avc-integration-slice", "android-path-decision", "uvc-compatibility-evaluation"]

[[steps]]
id = "design-doc-intent-audit"
title = "Verify design docs match current user intent and implemented behavior before closeout"
status = "pending"

[[steps]]
id = "test-runtime-impact-audit"
title = "Review validation runtime impact and record any slower-lane decisions before closeout"
status = "pending"

[[steps]]
id = "external-review"
title = "Record independent review of the parked or completed transport plan"
status = "pending"
depends_on = ["design-doc-intent-audit", "test-runtime-impact-audit"]

[[exit_criteria]]
id = "samples-understood"
title = "CDC VCOM, UVC virtual camera, and common EHCI device stack behavior are documented"
status = "met"

[[exit_criteria]]
id = "hs-enumeration"
title = "FRDM-MCXN947 enumerates as a high-speed USB device on a PC using the selected test firmware"
status = "met"

[[exit_criteria]]
id = "bulk-throughput"
title = "USB-only synthetic stream demonstrates at least 5 MB/s sustained receive throughput for 60 seconds, with 8 MB/s or better recorded as the 60 fps stretch target"
status = "met"

[[exit_criteria]]
id = "idle-when-disconnected"
title = "When no host has opened the stream, firmware skips frame generation/copy/cache-clean work beyond normal USB enumeration"
status = "pending"

[[exit_criteria]]
id = "host-render"
title = "A host tool renders a 320x200 RGB565 synthetic frame stream and displays frame rate, drop count, and byte rate"
status = "met"

[[exit_criteria]]
id = "framing-locked"
title = "Frame, overlay, log, stats, and control packet formats are documented with endian, sequencing, and recovery rules"
status = "pending"

[[exit_criteria]]
id = "avc-nonblocking"
title = "AVC integration streams frames from main-loop/service context without blocking camera capture or line processing"
status = "met"

[[exit_criteria]]
id = "mcxn-cdc-guide"
title = "A reusable MCXN HS USB CDC porting guide documents file layout, copied middleware, required .cproject metadata, config macros, clock/IRQ hooks, and validation steps"
status = "pending"

[[exit_criteria]]
id = "phone-path"
title = "Android phone connectivity path is validated on representative hardware or a custom app fallback is chosen"
status = "pending"

[[exit_criteria]]
id = "display-bom-recommendation"
title = "LCD removal, optional-population, or retention recommendation is recorded with measured bandwidth and usability tradeoffs"
status = "pending"

[[exit_criteria]]
id = "design-doc-intent-audit"
title = "Design docs match current user intent and implemented behavior"
status = "pending"

[[exit_criteria]]
id = "test-runtime-impact-audit"
title = "New tests are listed and runtime impact is reviewed"
status = "pending"

[[exit_criteria]]
id = "external-review"
title = "Independent review is recorded"
status = "pending"
+++

# USB Debug Display Transport

## Purpose

Evaluate replacing the onboard LCD debug display with a high-speed USB debug
display transport on the FRDM-MCXN947 AVC platform. The target user experience
is a student-friendly host display that can show raw camera frames, overlay
text/markers, debug prints, and optional controls on a laptop, PC, Android
phone, or small custom Android app.

The near-term goal is not to remove the LCD immediately. The near-term goal is
to measure whether the MCXN947 HS USB device path can stream AVC-sized frames
with low MCU overhead and clean backpressure behavior.

## Source Material

- Local CDC VCOM sample:
  `docs/examples/frdmmcxn947/dev_cdc_vcom_bm_cm33_core0`
- Local UVC virtual camera sample:
  `docs/examples/frdmmcxn947/dev_video_virtual_camera_lite_bm_cm33_core0`
- Common USB device stack files in both examples:
  `usb/device/usb_device_ehci.c`,
  `usb/device/usb_device_ehci.h`,
  `usb/device/usb_device_dci.c`,
  `usb/device/usb_device.h`,
  `usb/device/class/usb_device_class.c`
- CDC class files:
  `usb/device/class/usb_device_cdc_acm.c`,
  `source/virtual_com.c`,
  `source/usb_device_descriptor.c`,
  `source/usb_device_config.h`,
  `source/hardware_init.c`
- UVC class/application files:
  `source/virtual_camera.c`,
  `source/usb_device_video.c`,
  `source/usb_device_descriptor.c`,
  `source/usb_device_descriptor.h`
- Active AVC frame path:
  `src/avc/avc_core0/source/main.c`,
  `src/avc/avc_core0/source/avc_io/bv_camera__interface.c`,
  `src/avc/avc_core0/source/avc_config/avc__master_config.h`
- Shared host tooling:
  `src/usb_debug_host`
- Draft protocol document:
  `docs/research/AVC_USB_Debug_Transport_Protocol.md`
- Current parked state:
  `docs/research/AVC_USB_Debug_Display_Current_State.md`

## Current Findings

The local CDC and UVC examples both use the MCXN947 high-speed EHCI device
controller path. `USB1_HS_IRQHandler()` calls `USB_DeviceEhciIsrFunction()`.
The sample clock init enables the USBHS and USBHS PHY clocks, enables the
480 MHz USB PHY PLL from the 24 MHz crystal, and initializes the EHCI PHY.

The EHCI device driver is descriptor/DMA based. It allocates aligned queue
heads and dTDs, points those dTDs at the application buffer, sets the active
and IOC bits, then primes the endpoint. With cacheable buffers enabled, the
driver cleans the transmit buffer by range before priming and invalidates
received buffers before notification. This means frame payload movement is not
CPU byte pushing, but transfer setup, cache maintenance, and completion
callbacks still cost CPU time.

The examples configure `USB_DEVICE_CONFIG_EHCI_MAX_DTD = 16`. Each EHCI dTD
covers at most `0x4000` bytes, so a `320x200` RGB565 frame of `128000` bytes
needs eight dTDs if sent as one transfer. A `320x240` RGB565 frame of `153600`
bytes needs ten. The dTD pool is shared with control and other endpoints, so
the first proof should use 16 KiB chunks and only increase chunk size after
the busy/error behavior is measured.

The generic device API supports only one transfer request per endpoint at a
time. Application code must queue or drop at the application layer and start
the next request from the endpoint completion callback or a service function.

The CDC sample is the best first vehicle. It already provides high-speed bulk
IN/OUT endpoints with 512-byte HS packets. It also has the exact gating we
want: `attach` is set after configuration, and `startTransactions` is set when
the host opens the CDC port via control-line state. When those flags are not
set, the firmware can skip all stream generation work.

The local UVC sample is useful for webcam compatibility research but is not the
best primary debug path. It advertises MJPEG, `176x144`, 30/25/20/15/10/5 fps
intervals, defaults to 15 fps, and uses a small 512-byte isochronous payload
buffer. It does not directly stream raw AVC RGB565 frames or provide an easy
debug/log/control side channel.

The active AVC camera configuration is `320x200` RGB565. The LCD path composes
a `320x40` overlay plane above a `320x200` camera view. `avc__next_frame()`
currently runs in interrupt context and only publishes a frame pointer plus a
ready flag. The USB stream should not send from that ISR. It should run from
the main loop or a USB service function after the frame pointer is published.

The first AVC integration slice now uses `START` for real camera-frame
streaming and `START_SYNTH` for the previous synthetic maximum-rate source.
Camera frames are published from the main loop after the existing marker and
overlay drawing. The CDC path copies each chunk into the aligned USB TX staging
buffer and drops a newly published frame if a previous camera frame is still
active. On 2026-07-20, native receive measured the synthetic source at
34.257 MiB/s with clean framing and the camera source at 2.866 MiB/s,
23.40 FPS, with no malformed packets or sequence errors.

The stats-report slice adds `AVC_DBG_STATS_REPORT` packets to camera mode.
Native camera receive for 60.017 seconds measured 2.870 MiB/s, 23.43 FPS,
11248 frame chunks, 60 stats packets, zero dropped-frame flags, zero malformed
packets, zero sequence errors, zero firmware frame drops, zero endpoint-busy
returns, and zero send errors. Python camera receive for 10.030 seconds
measured 2.868 MiB/s, 23.43 FPS, 1878 frame chunks, 10 stats packets, and the
same zero-error counters.

On 2026-07-21, the Chrome WebSerial viewer was observed rendering the live AVC
camera feed from the running board. This confirms the desktop browser path
from MCU camera frame publication through high-speed USB CDC, WebSerial, and
canvas rendering. The same CDC/WebSerial live feed was then observed running
for about 15 minutes during bench use.

The plan is now parked so the PCB revision can move forward. Resume work from
`docs/research/AVC_USB_Debug_Display_Current_State.md`, the shared protocol
header, and the host-tool directory rather than replaying the bringup thread.

## Working Strategy

Start with a USB-only speed test, isolated from camera, FlexIO, line
processing, and display work. This is the right first test because it answers
the highest-risk transport questions independently:

- Does the board enumerate as high-speed through the intended connector,
  cable, and host path?
- What is the sustained CDC bulk throughput to a native PC receiver?
- What is the sustained throughput to a browser receiver?
- Does Android Chrome support the chosen path on representative phones, or do
  we need WebUSB or a custom app?
- How much endpoint busy/backpressure occurs at AVC frame sizes?
- Does the firmware truly do near-zero stream work when no host is listening?

The first firmware proof should be a fork or small adaptation of the CDC VCOM
example, not the active AVC firmware. Generate synthetic frame payloads from a
static buffer or simple counter pattern, then send them as framed chunks over
CDC bulk IN. Keep USB OUT active for simple host commands such as start, stop,
set rate, set payload size, request stats, and select test pattern.

Use a native PC receiver first because it gives the cleanest USB ceiling and is
our backup path. Then test a static browser app using WebSerial. If Android
WebSerial is unreliable on the target phones, evaluate WebUSB/vendor bulk or a
small custom Android app.

## Proposed Transport

Use a binary framed stream over the selected bulk transport. Keep the MCU side
simple and host-side composition flexible.

Initial packet types:

- `HELLO`: protocol version, board ID, stream capabilities, build tag.
- `FRAME_START`: frame ID, timestamp/cycle count, width, height, pixel format.
- `FRAME_CHUNK`: frame ID, byte offset, payload length, RGB565 payload.
- `FRAME_END`: frame ID, bytes sent, dropped-frame count, timing stats.
- `OVERLAY_TEXT`: text lines, marker descriptions, or compact JSON/CBOR.
- `LOG_TEXT`: debug strings that can replace or supplement RTT for students.
- `STATS`: counters, byte rate, endpoint busy count, drops, camera FPS.
- `CONTROL`: host-to-device commands.

Keep fields little-endian. Start without payload CRC for throughput work, but
reserve a header flag for optional CRC if corrupted or misaligned streams show
up during testing.

## MCU Policy

The stream must never stall camera capture or AVC control behavior.

- If USB is not configured/open, do no frame serialization work.
- If the IN endpoint is busy, drop the next frame rather than waiting.
- Prefer latest-frame semantics over preserving every frame.
- Keep send requests out of camera ISR context.
- Use 16 KiB chunks at first so each chunk maps to one EHCI dTD.
- Track drop, busy, send-error, and max-latency counters from the first proof.
- Avoid copying camera buffers unless buffer lifetime requires it.

The active camera backend has two frame buffers. Direct USB DMA from a camera
buffer is only safe while the camera code will not overwrite that buffer. The
AVC integration must either guarantee ownership until USB completion, send only
while the frame is still stable, or copy into an explicit USB staging buffer.
If none of those is cheap enough, the correct behavior is to drop the frame.

## Test Phases

### Phase 1: USB-only CDC throughput

Build and flash the local CDC VCOM example or a minimal derivative. Confirm
high-speed enumeration. Replace echo behavior with a synthetic frame streamer:

- `320x200 RGB565` payload: `128000` bytes per frame.
- Optional `320x240 RGB565` payload: `153600` bytes per frame.
- 16 KiB chunks with frame/chunk headers.
- Host command to select continuous, fixed-count, or idle mode.
- RTT or USB stats reporting for bytes/sec, frames/sec, endpoint busy, and
  send callback timing.

Minimum useful result: 5 MB/s sustained to a native PC receiver for 60 seconds.
Stretch result: 8 MB/s or better, enough for `320x200` RGB565 near 60 fps
before overlay/log overhead.

### Phase 2: Host display proof

Implement the host receiver in two layers:

- Native PC receiver for reliable measurement and competition fallback.
- Static browser app that opens the transport, decodes RGB565, draws to canvas,
  and overlays text/log/stats.

The browser proof should start with desktop Chrome. Then test Android phones
using WebSerial if available. If Android support is inconsistent, keep the same
framing and move the phone path to WebUSB/vendor bulk or a small Android app.

### Phase 3: AVC integration

Port only the proven transport pieces into `src/avc/avc_core0`. The first AVC
slice streams synthetic frames from inside AVC for ceiling tests and the latest
camera frame after `next_frame_ready` is consumed in `main.c`.

Do not stream from `avc__next_frame()` or the FlexIO/eDMA callback. Add a
small `avc_usb_debug_stream__service()` style module called from the main loop.

The preferred user-facing model is raw `320x200` camera RGB565 plus separate
overlay text/markers. Host composition should draw the top status bar and any
student markers, so the MCU does not have to spend LCD-style drawing time when
USB debug output is enabled.

### Phase 4: WebUSB/vendor or custom Android app

If WebSerial is not good enough on representative Android phones, evaluate a
vendor-specific bulk interface. The NXP stack already has BOS descriptor and
vendor/class request plumbing, but the local examples do not include a complete
WebUSB app. This is a second path after the CDC throughput baseline, not the
first proof.

A custom Android app remains an acceptable fallback. It can use the same binary
framing and can be simpler than fighting browser support if the phones are
known and competition deployment can tolerate installing an APK.

### Phase 5: UVC compatibility branch

Run the stock UVC sample only to validate camera-class enumeration and host
behavior. Continue with UVC only if "shows up as a webcam" becomes a concrete
requirement. UVC likely needs descriptor and format work for `320x200` or
`320x240`, and raw debug side channels would still need CDC/vendor composite
support.

## Immediate Next Steps

1. Decide whether overlay text should remain drawn into the camera buffer for
   the near term or move to host-side text messages.
2. Write the reusable MCXN HS USB CDC porting guide for the second agent.

## Open Questions

- Does the intended FRDM/shield USB connector route to the same HS EHCI device
  instance used by the examples?
- Which Android phones and Chrome versions should define the competition
  compatibility target?
- Will the final student workflow allow an installed Android app, or must it
  work from a static web page?
- Should the AVC firmware expose one composite USB device eventually, such as
  CDC/vendor debug plus optional UVC?
- What buffer ownership rule is acceptable for direct USB DMA from AVC camera
  frame buffers?
