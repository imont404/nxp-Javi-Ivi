+++
type = "plan"
id = "usb-stream-optimization"
status = "active"
created = "2026-07-25"

[[steps]]
id = "measure-service-cost"
title = "Measure what avc_usb_debug_stream__service costs the main loop while actually streaming"
status = "pending"

[[steps]]
id = "host-build-root"
title = "Add the host-side CMake root so PC tools build from the same system as the firmware"
status = "pending"

[[steps]]
id = "receiver-benchmark"
title = "Build a host receiver that reports sustained throughput, drops, and latency against START_SYNTH"
status = "pending"
depends_on = ["host-build-root"]

[[steps]]
id = "sdl-viewer"
title = "Build the SDL live camera viewer on top of the benchmarked receiver"
status = "pending"
depends_on = ["receiver-benchmark"]

[[steps]]
id = "interrupt-driven-service"
title = "Drive the stream from the USB transfer-complete interrupt instead of the main loop"
status = "pending"
depends_on = ["measure-service-cost"]

[[steps]]
id = "buffer-ownership"
title = "Close the live-camera-buffer aliasing window in the streaming path"
status = "pending"
depends_on = ["measure-service-cost"]

[[steps]]
id = "always-on-runtime-gate"
title = "Retire CONFIG__USB_DEBUG_STREAM_ENABLE in favour of the runtime enumeration gate"
status = "pending"
depends_on = ["interrupt-driven-service", "buffer-ownership"]

[[steps]]
id = "sustained-throughput-truth"
title = "Establish real sustained throughput with camera, control loop, and display all running"
status = "pending"
depends_on = ["receiver-benchmark", "interrupt-driven-service"]

[[steps]]
id = "design-doc-intent-audit"
title = "Audit design docs, ADRs, and requirements against implementation"
status = "pending"
depends_on = ["sustained-throughput-truth"]

[[steps]]
id = "test-runtime-impact-audit"
title = "Audit new test runtime impact"
status = "pending"
depends_on = ["sustained-throughput-truth"]

[[steps]]
id = "external-review"
title = "Obtain independent external review"
status = "pending"
depends_on = ["design-doc-intent-audit", "test-runtime-impact-audit"]

[[exit_criteria]]
id = "service-off-the-main-loop"
title = "Streaming costs the main loop nothing measurable while a host is attached"
status = "pending"

[[exit_criteria]]
id = "no-build-variant"
title = "The race image and the development image are the same build, gated at runtime"
status = "pending"

[[exit_criteria]]
id = "aliasing-closed"
title = "A streamed frame cannot contain pixels from the following frame"
status = "pending"

[[exit_criteria]]
id = "throughput-known"
title = "Sustained throughput under realistic load is measured and recorded, not extrapolated from a synthetic burst"
status = "pending"

[[exit_criteria]]
id = "sdl-viewer-live"
title = "The SDL viewer shows a live camera feed and is built by the same CMake system as the firmware"
status = "pending"

[[exit_criteria]]
id = "design-doc-intent-audit"
title = "Design docs, ADRs, and requirements match implementation"
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

# Making the USB stream cheap, honest, and worth building tools on

## Scope, and what this is not

`usb-debug-display-transport` owns the transport itself - the framing, the
channels, the drop policy, and the open decisions about the Android path
(`android-path-decision`) and whether USB display can demote the LCD from the
BOM (`bom-display-decision`). **Those stay there.** This plan does not
re-litigate them.

This plan owns two things that are genuinely separate:

1. **Making the firmware side cost nothing.** Today the stream is serviced from
   the main loop, competing with the algorithm for the exact milliseconds the
   display work has been trying to protect.
2. **The host tooling**, so throughput claims can be measured rather than
   asserted, and so the SDL viewer finally gets built.

Most of it needs no board, which is the point - the car already emits the
stream and `START_SYNTH` generates data without the camera, so a receiver can be
developed and benchmarked on a PC alone.

## What is already known

From `AVC_USB_Debug_Display_Current_State.md`, measured:

| Quantity | Measured |
|---|---|
| Sustained, native receiver, 60.0 s | 2.870 MiB/s at 23.43 FPS, no drops |
| Sustained, Python receiver, 10.0 s | 2.868 MiB/s at 23.43 FPS |
| Synthetic ceiling (`START_SYNTH`) | ~34 MiB/s |

The sustained figure is **camera-limited, not link-limited** - it is simply what
23.43 FPS of 320x200 RGB565 amounts to. So the link has roughly 12x headroom
over what we use, and USB is not the bottleneck anywhere in this design.

And from reading the code during the display work:

- `avc_usb_debug_stream__publish_frame()` **does not copy the frame.** It stores
  a pointer and the bytes are pushed incrementally from `service()`. An earlier
  note describing a memcpy before `USB_DeviceCdcAcmSend()` was describing the
  send path, not a blocking copy per frame.
- It **early-outs on `!is_open()`**, so with nothing attached the per-frame cost
  is a couple of comparisons.

## The three things worth doing

### 1. Get `service()` off the main loop

It is called from the main loop, so streaming competes directly with the
student's algorithm. The USB stack already receives a transfer-complete
interrupt; chaining the next chunk from there makes the stream self-sustaining
at no main-loop cost.

A timer IRQ is the cruder alternative - simpler, but it polls and it picks a
rate rather than following the link.

**Measure first.** `measure-service-cost` exists because the expectation is that
this is small, and this project has been wrong about exactly that kind of
expectation twice in one session. The scope marker added for the display work
(`CONFIG__DISPLAY_SCOPE_MARKER_ENABLE`, P4_1) is the right instrument, since it
does not depend on the DWT cycle counter that `avc__next_frame()` resets.

### 2. Close the aliasing window

`s_streamFrameData` points at the **live camera buffer**, exactly as the LCD
dump does. If streaming a frame is still in progress when the camera swaps
buffers, the tail of that frame is the next frame's pixels.

Spread across `service()` calls over a whole frame period, **this window is
wider than the LCD's, not narrower.** It is the more likely of the two to be hit
and the harder to notice, because a torn frame on a PC viewer looks like a
network artifact.

Options, cheapest first: refuse the buffer swap while a stream is in flight and
drop the frame instead; or stream from a copy; or triple-buffer. The first is
almost free and makes the failure a dropped frame rather than a corrupt one.

### 3. Retire the compile-time switch

`CONFIG__USB_DEBUG_STREAM_ENABLE` does not need to exist. The cable is the
switch, and the code already behaves that way.

The value is not the microseconds - it is that **the race image stops being a
different build from the development image**. In a week where students flash
constantly, that removes a whole class of "works on my build" problems, and it
means the thing tested all week is the thing that races.

Gated on 1 and 2, because always-on is only safe once it is free and correct.

## Host tooling

`host-build-root` and the SDL viewer were deferred out of
`cmake-build-and-toolchain` before the race. They belong here now, and the order
matters:

**Benchmark before viewer.** A receiver that reports sustained throughput,
drops, and latency against `START_SYNTH` is what turns "34 MiB/s" from a
remembered number into a measurement anyone can repeat. The viewer is then built
on something already known to be correct, rather than being the thing that
discovers the receiver is dropping frames.

The same parsing serves the SDL viewer, the existing browser page, and any
Android app that comes out of `android-path-decision` - so it should be written
once, deliberately, as a reusable reader rather than three times.

## Deliberately out of scope

- **The Android app.** Belongs to `android-path-decision`.
- **Whether the LCD survives.** Belongs to `bom-display-decision`.
- **Moving work to core1.** A much larger idea, mentioned in
  `lcd-spi-throughput`, and it needs its own plan and a core1 bring-up first.
- **UVC compatibility.** Already a step in the transport plan.

## Files

- `src/avc/avc_core0/source/avc_io/avc_usb_debug_stream.c` - `publish_frame`,
  `service`, and the stream state
- `src/avc/avc_core0/source/main.c` - the main-loop call site and the camera
  buffer handoff
- `docs/research/AVC_USB_Debug_Transport_Protocol.md` - framing
- `docs/research/AVC_USB_Debug_Display_Current_State.md` - the measurements above
