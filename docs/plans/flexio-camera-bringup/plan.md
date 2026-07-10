+++
type = "plan"
id = "flexio-camera-bringup"
status = "active"
created = "2026-07-09"

[[steps]]
id = "tooling-flash-rtt"
title = "Establish reliable headless build, flash, and RTT observation"
status = "done"

[[steps]]
id = "reference-extraction"
title = "Extract and index MCXN947 reference material needed for FlexIO camera work"
status = "done"
depends_on = ["tooling-flash-rtt"]

[[steps]]
id = "pinmux-strategy"
title = "Choose candidate FlexIO camera pins and document wiring constraints"
status = "done"
depends_on = ["reference-extraction"]

[[steps]]
id = "implementation-strategy"
title = "Define firmware architecture for selectable SmartDMA/EZH and FlexIO capture backends"
status = "done"
depends_on = ["pinmux-strategy"]

[[steps]]
id = "signal-instrumentation"
title = "Add non-invasive camera signal instrumentation for XCLK, PCLK, VSYNC, and HSYNC"
status = "done"
depends_on = ["implementation-strategy"]

[[steps]]
id = "flexio-low-rate-capture"
title = "Prove low-rate FlexIO capture of camera data bytes"
status = "done"
depends_on = ["signal-instrumentation"]

[[steps]]
id = "flexio-frame-buffer"
title = "Capture usable image data into an AVC frame buffer"
status = "active"
depends_on = ["flexio-low-rate-capture"]

[[steps]]
id = "lcd-integration"
title = "Route FlexIO-captured frames through the existing LCD and line-processing path"
status = "pending"
depends_on = ["flexio-frame-buffer"]

[[steps]]
id = "comparison-signoff"
title = "Compare FlexIO capture against the current SmartDMA/EZH path"
status = "pending"
depends_on = ["lcd-integration"]

[[exit_criteria]]
id = "flash-rtt-ready"
title = "FRDM-MCXN947 can be built, flashed, reset, and observed from scripts"
status = "met"

[[exit_criteria]]
id = "pins-selected"
title = "FlexIO pin assignment is documented with board-header wiring and conflicts"
status = "met"

[[exit_criteria]]
id = "capture-architecture"
title = "Firmware has a documented backend strategy that can disable EZH/SmartDMA for FlexIO tests"
status = "met"

[[exit_criteria]]
id = "signal-counts"
title = "Instrumented firmware reports plausible VSYNC, HSYNC, and PCLK behavior over RTT"
status = "met"

[[exit_criteria]]
id = "image-path"
title = "FlexIO capture produces frame-buffer data that can be displayed or inspected"
status = "pending"

[[exit_criteria]]
id = "comparison"
title = "FlexIO feasibility is decided against the existing SmartDMA/EZH approach"
status = "pending"
+++

# FlexIO Camera Bringup

## Purpose

Determine whether the AVC project should replace the current SmartDMA/EZH camera
capture path with a FlexIO plus eDMA capture path on FRDM-MCXN947.

This plan deliberately starts with debug and flashing infrastructure. The FlexIO
camera work will involve repeated hardware tests, so the first milestone is a
known-good loop for build, flash, reset, and RTT observation.

## Source Material

- Current firmware: `src/avc/avc_core0`
- Current camera interface: `src/avc/avc_core0/source/avc_io/bv_camera__interface.c`
- Current app frame consumer: `src/avc/avc_core0/source/main.c`
- Current pin mux and clocks: `src/avc/avc_core0/board/pin_mux.c`,
  `src/avc/avc_core0/board/clock_config.c`
- Headless build wrapper: `build.ps1`
- CMake build wrapper and static linker scripts: `build_cmake.ps1`,
  `src/avc/avc_core0/link`
- Flash and RTT wrappers: `flash.ps1`, `rtt.ps1`,
  `scripts/tools/rtt_monitor.py`
- Flashing reference: `D:\projects\Gen2BLEController-main\w71_cruise_ble\flash.ps1`
- Board pin workbook:
  `docs/FRDM-MCXN947/FRDM-MCXN947/FRDM-MCXN947 board pin assignment.xlsx`
- NXP mux dump: `docs/research/MCXN947/json`
- Reference manuals: `docs/mcxn947/MCXNx4xRM.pdf`,
  `docs/mcxn947/MCXNx4xDS.pdf`
- Extracted reference chapters:
  `docs/research/MCXN947/extracted/MCXNx4xRM`
- Reference extraction script: `scripts/tools/extract_mcxn_reference.py`
- FlexIO pin candidate summary:
  `docs/research/MCXN947/flexio_pin_candidates`
- FlexIO pin summary script: `scripts/tools/summarize_flexio_pins.py`
- FlexIO examples:
  `docs/examples/frdmmcxn947/flexio_mculcd_edma_transfer_cm33_core0`
  and `docs/research/an-flexio_camera_rt1010`
- FlexIO camera implementation strategy:
  `docs/research/MCXN947/flexio_camera_implementation_strategy.md`
- Operational I/O tracking:
  `docs/research/MCXN947/flexio_camera_io_pin_map.md`

## Working Strategy

Keep the existing camera SCCB/I2C initialization and XCLK generation in place at
first. Add FlexIO capture beside the existing path, selected by configuration,
so the SmartDMA/EZH path remains available as a known-good baseline.

Initial tests should not try to capture full frames. Start by wiring the OV5640
module's returned PCLK and sync signals into candidate pins, then count edges
and line/frame events. Use GPIO/PINT interrupts for VSYNC and HSYNC unless the
FlexIO design needs those signals as timer/shifter gates. Use FlexIO first for
byte sampling from PCLK, then introduce eDMA and frame-buffer integration.

The active AVC camera configuration is OV5640 RGB565 at `320x200`, not full
QVGA `320x240`. The LCD path uses a `320x40` overlay band above a `320x200`
camera view, so the validated `p4_lines=200` RTT diagnostic matches the active
frame geometry. Track all wiring, mux, electrical, and diagnostic pin changes
in `docs/research/MCXN947/flexio_camera_io_pin_map.md` before relying on them
for FlexIO work.

## Step Notes

### tooling-flash-rtt

- Adapt the W71 `flash.ps1` pattern for `avc_core0.axf`.
- Target the FRDM-MCXN947 onboard Segger J-Link over SWD.
- Confirm the correct J-Link device string for MCXN947.
- Add or document an RTT monitoring command using Segger tooling.
- Prove a full loop: `.\build.ps1 -Clean`, flash, reset, boot banner visible
  over RTT.

### reference-extraction

- Add a Python script that can split or extract `MCXNx4xRM.pdf` text by chapter.
- Prefer structured output under `docs/research/MCXN947/extracted`.
- Include chapters needed for PORT/IOCON, INPUTMUX, GPIO/PINT, FLEXIO, DMA/eDMA,
  clocks, SmartDMA/EZH, and memory/bus behavior.
- If image extraction is practical, include relevant block diagrams and timing
  diagrams beside the chapter text.
- Completed with `scripts/tools/extract_mcxn_reference.py`. The generated
  `docs/research/MCXN947/extracted/MCXNx4xRM` bundle includes chapter text,
  per-chapter metadata, rendered key pages, and available embedded images.

### pinmux-strategy

- Parse or summarize `signal_configuration.json` for FlexIO0-capable pins,
  especially Port 4 candidates.
- Cross-check candidates against the FRDM workbook and current shield wiring.
- Preserve current camera I2C/SCCB pins and XCLK unless a conflict is proven.
- Identify pins for at least D0-D7, PCLK, VSYNC, HSYNC, and optional debug GPIOs.
- Record wiring assumptions for the OV5640 module, including any fly wires from
  camera PCLK/VSYNC/HSYNC back to the FRDM board.
- Completed with `scripts/tools/summarize_flexio_pins.py`. The generated
  `docs/research/MCXN947/flexio_pin_candidates` bundle selects `P4_12..P4_23`
  as the NXP-confirmed contiguous Port 4 FlexIO group (`FLEXIO0_D20..D31`) and
  flags the workbook `FLEXIO_LCD A18` camera map as stale or mismatched for
  `P4_2..P4_7`.
- Initial wiring strategy: keep current SCCB/I2C on `P3_2/P3_3`, keep XCLK on
  `P2_2`, keep reset and power-down on `P1_19/P1_18`, fly-wire camera D0-D7 to
  `P4_12..P4_19`, and route returned PCLK/HSYNC/VSYNC to the remaining Port 4
  FlexIO pins or GPIO/PINT pins during instrumentation.

### implementation-strategy

- Introduce a camera capture backend config such as `SMARTDMA_EZH` vs `FLEXIO`.
- Keep the current public frame handoff shape intact where possible:
  `avc__next_frame(uint16_t *buf)` and the existing LCD/line-processing flow.
- Isolate FlexIO setup into new source files rather than growing
  `bv_camera__interface.c` indefinitely.
- Pull in SDK FlexIO driver sources from local MCXN947 examples or from the
  RT1010 app note repo only after confirming they match MCXN947 SDK APIs.
- Disable bunny-build/EZH generation only for the FlexIO backend path, not for
  the current baseline.
- Completed with `docs/research/MCXN947/flexio_camera_implementation_strategy.md`.
  The selected architecture keeps SmartDMA/EZH as the default backend, adds
  diagnostic and FlexIO/eDMA backends behind compile-time configuration, keeps
  the existing public camera API and frame handoff, and stages the work from
  no-behavior-change configuration through signal counts, byte capture, DMA,
  and LCD integration.

### signal-instrumentation

- Add counters for VSYNC frame count, HSYNC lines per frame, PCLK activity, and
  missed/overflow events.
- Emit compact RTT status lines so tests can run without a debugger UI.
- Use this phase to answer whether VSYNC/HSYNC need to be FlexIO timer inputs
  or can remain GPIO/PINT instrumentation/control signals.
- Pre-wiring checkpoint: firmware now has an explicit
  `CONFIG__CAMERA_CAPTURE_BACKEND` selector with SmartDMA/EZH as the default and
  a dormant `FLEXIO_DIAG` backend that can configure `P4_20`, `P4_21`, and
  `P4_22` as diagnostic GPIO inputs. The default CMake image was built, flashed,
  and RTT-verified before wiring changes.
- Completed with guarded Port 4 sync diagnostics. `P4_21` HSYNC/HREF counts
  about 6040 rising edges/sec, `P4_22` VSYNC counts about 30 to 31 rising
  edges/sec, `P4_20` PCLK edge activity is present, and `p4_lines=200` matches
  the active `320x200` OV5640 configuration. The `P4_22` VSYNC guard did not
  trip after shortening the jumper and adding the 330 ohm pulldown/termination.

### flexio-low-rate-capture

- Start from the current `320x200` OV5640 mode unless a lower-rate mode is
  needed for first byte-capture debugging.
- Use the tracked Port 4 map: data `D0..D7` planned for `P4_12..P4_19`
  (`FLEXIO0_D20..D27`), PCLK on `P4_20`, HSYNC/HREF on `P4_21`, VSYNC on
  `P4_22`, and `P4_23` spare.
- Add a sync-qualified synthetic-frame backend before first byte capture. It
  should use the current GPIO IRQ sync instrumentation to accept plausible
  VSYNC/HSYNC timing and feed deterministic RGB565 test frames through the
  existing `avc__next_frame()` and LCD path with no data lines required.
- RTT-verified the `FLEXIO_PIPELINE_DIAG` backend after flashing on
  2026-07-10. It accepts frames at 30/sec with `p4_lines=200`, PCLK activity
  present, and only one rejected startup VSYNC. LCD visual confirmation remains
  the next check.
- Keep SysTick available as a future timing source, but do not take ownership
  of `SysTick_Handler` until the existing `e_tick` delay/timing behavior is
  reviewed.
- Move next to real data movement rather than spending more time on synthetic
  image generation. With data lines disconnected and pulled down, the first
  FlexIO/eDMA capture should produce a reliable black frame. If needed, tie one
  data pin high manually to prove bit position and RGB565 packing before
  restoring real camera data wiring.
- Completed the first real data-movement proof with `FLEXIO_EDMA` as the active
  backend. The firmware configures `P4_12..P4_19` as FlexIO data
  `D20..D27`, uses `P4_20/FLEXIO0_D28` as PCLK, uses
  `P4_21/FLEXIO0_D29` as HREF gating, and keeps `P4_22` as the GPIO VSYNC
  start trigger.
- RTT after flashing reported `cam_dma done` increasing by 30 frames/sec,
  no submit errors, no callback errors, no timeouts, and pulled-low sample data
  `first=0000,0000,0000,0000 sample_nz=0`.
- Byte order and pixel packing still need validation with at least one tied-high
  data bit or real camera data wiring.

### flexio-frame-buffer

- Add eDMA only after low-rate byte capture is understood.
- Prefer line-buffer or ping-pong buffering if full-frame DMA has timing or RAM
  pressure issues.
- Keep memory placement explicit because current frame buffers already consume a
  large fraction of `FRAME_BUFFERS`.
- First implementation uses DMA1 channel 0, FlexIO shifter 0 request 61, and
  32-byte minor transfers from `SHIFTBUF0` into the existing 32-byte-aligned
  ping-pong camera frame buffers.
- Current proof is a pulled-low black frame. Next validation is a controlled
  nonzero data bit, then restoring real camera D0-D7 to `P4_12..P4_19`.

### lcd-integration

- Convert captured data into the existing RGB565 frame representation.
- Reuse the existing LCD output path to inspect capture quality.
- Keep line-processing behavior available so AVC logic can run on the captured
  frame.

### comparison-signoff

- Compare boot reliability, capture reliability, CPU load, memory use, frame
  rate, and code complexity against the SmartDMA/EZH path.
- Decide whether FlexIO becomes the primary path, remains an experiment, or is
  deferred in favor of EZH.

## Open Questions

- Exact OV5640 module pins available for PCLK, VSYNC, HSYNC, and data D0-D7.
- Whether the physical header and solder-jumper state make `P4_12..P4_23`
  immediately usable, or whether fly wires need to land elsewhere.
- Whether FlexIO can use the camera PCLK directly as a timer clock on MCXN947 or
  needs a routed/gated signal through a specific FlexIO pin or INPUTMUX path.
- Whether VSYNC/HSYNC should gate capture in FlexIO hardware, trigger GPIO/PINT
  control logic, or both.
- Whether the capture target should be whole-frame, per-line ping-pong, or
  smaller diagnostic buffers for the first working implementation.
