# FlexIO Camera Implementation Strategy

## Decision

Build the FlexIO camera path as a selectable backend beside the current
SmartDMA/EZH implementation. Keep SmartDMA/EZH as the default baseline until the
FlexIO path has proven signal counts, byte capture, DMA capture, and LCD output.

The first firmware change should be a no-behavior-change backend selector. That
lets us verify the build, flash, and RTT loop before touching camera timing.

## Current Firmware Shape

- Public initialization entry point: `avc_camera__init()`.
- Public frame handoff into the application: `avc__next_frame(uint16_t *buffer)`.
- Current implementation file:
  `src/avc/avc_core0/source/avc_io/bv_camera__interface.c`.
- Current camera control that should be preserved initially:
  SCCB/I2C on `P3_2/P3_3`, XCLK on `P2_2`, reset on `P1_19`, and power-down on
  `P1_18`.
- Current capture backend: generated bunny/EZH code and SmartDMA, using the
  existing camera header data and sync pins.

The current file mixes sensor setup, frame buffer ownership, generated EZH
program construction, SmartDMA startup, and frame handoff. Splitting the capture
backend will make the FlexIO tests reversible and keep the known-good path
available.

## Backend Configuration

Add a compile-time capture backend selector in the AVC camera configuration.
The exact file can follow the existing config pattern, but the selector should
be visible from the camera implementation and default to SmartDMA/EZH.

```c
#define CAMERA_CAPTURE_BACKEND_SMARTDMA_EZH 1
#define CAMERA_CAPTURE_BACKEND_FLEXIO_DIAG  2
#define CAMERA_CAPTURE_BACKEND_FLEXIO_EDMA  3

#ifndef CONFIG__CAMERA_CAPTURE_BACKEND
#define CONFIG__CAMERA_CAPTURE_BACKEND CAMERA_CAPTURE_BACKEND_SMARTDMA_EZH
#endif
```

Also fix the camera model macro spelling while preserving compatibility with
existing references:

```c
#define CAMERA__OV5640 2
#define CAMERA__OV5460 CAMERA__OV5640
```

## Source Split

Keep `bv_camera__interface.c` as the public API and dispatcher. Move reusable
work out of the backend files only when it is needed by both paths.

Proposed files:

- `bv_camera__interface.c`: public `avc_camera__init()` dispatcher and shared
  frame handoff surface.
- `bv_camera__control.[ch]`: OV5640 reset/power-down, XCLK, SCCB/I2C, and
  sensor register initialization shared by all backends.
- `bv_camera__smartdma_ezh.[ch]`: current generated bunny/EZH and SmartDMA
  backend.
- `bv_camera__flexio_diag.[ch]`: diagnostic backend that keeps the sensor alive
  and counts PCLK, HSYNC/HREF, and VSYNC without capturing frames.
- `bv_camera__flexio_capture.[ch]`: FlexIO plus eDMA capture backend after the
  diagnostic stage proves the wiring and signal rates.

If frame buffer ownership becomes awkward during the split, introduce a small
`bv_camera__frame.[ch]` helper. Do not add that layer until the duplication is
real.

New C sources must be added to both supported build surfaces: the MCUXpresso
managed project files and the CMake build list used by `build_cmake.ps1`.

## Implementation Phases

### Phase 1: Backend Selector, Default SmartDMA/EZH

- Add the backend macros with SmartDMA/EZH as the default.
- Fix the `CAMERA__OV5460`/`CAMERA__OV5640` spelling mismatch.
- Keep current `bv_camera__interface.c` behavior intact.
- Build with `.\build_cmake.ps1`, flash with `.\flash.ps1 -CMake`, and confirm
  RTT output with `.\rtt.ps1 -CMake -Reset -Seconds 4`.

Exit condition: firmware behaves like the current baseline and the default
backend is explicitly SmartDMA/EZH.

### Phase 2: Shared Sensor Control

- Pull reset, power-down, XCLK, SCCB/I2C, and OV5640 register setup into a
  common module.
- Leave frame capture on SmartDMA/EZH.
- Keep the existing resolution and pixel-format setup unchanged.

Exit condition: the refactor builds and the baseline camera path still reaches
the existing frame consumer.

### Phase 3: FlexIO Diagnostic Backend

- Add `CAMERA_CAPTURE_BACKEND_FLEXIO_DIAG`.
- Do not boot SmartDMA and do not generate or load the bunny/EZH program in this
  backend.
- Keep I2C, XCLK, reset, and power-down on the existing pins.
- Configure the candidate Port 4 pins as GPIO/PINT inputs first, or as FlexIO
  inputs only where the diagnostic requires it.
- Report compact RTT status lines containing at least:
  VSYNC count, HSYNC/HREF count, PCLK activity, lines per frame when measurable,
  and overflow/missed-event counters.

Exit condition: with the OV5640 wired to Port 4 candidates, RTT reports
plausible frame, line, and pixel-clock activity without using SmartDMA/EZH.

### Phase 4: Low-Rate FlexIO Byte Capture

Start from the RT1010 FlexIO camera driver model and adapt it to MCXN947 SDK
APIs instead of hand-rolling the shifter/timer programming.

Initial configuration target:

- Data D0-D7: `P4_12..P4_19` mapped to `FLEXIO0_D20..D27`.
- `datPinStartIdx = 20`.
- PCLK: `P4_20` mapped to `FLEXIO0_D28`.
- `pclkPinIdx = 28`.
- HSYNC/HREF: `P4_21` mapped to `FLEXIO0_D29`.
- `hrefPinIdx = 29`.
- VSYNC: keep as GPIO/PINT initially, with `P4_22` available as
  `FLEXIO0_D30` if FlexIO hardware gating becomes necessary.
- Start with a small diagnostic capture buffer, not a full frame.
- Use the lowest practical OV5640 pixel clock and resolution during this phase.

Exit condition: captured bytes change predictably with a stable test scene or
camera color-bar mode, and byte order can be explained.

### Phase 5: FlexIO eDMA Capture

- Use MCXN947 SDK eDMA APIs and local examples as the model.
- Start with `kDma0RequestMuxFlexIO0ShiftRegister0Request`.
- Use a known free DMA0 channel and configure the request mux with
  `EDMA_SetChannelMux()` if the MCXN947 SDK path matches the local FlexIO LCD
  example.
- Confirm whether any INPUTMUX DMA request enable is also required for the
  selected request path.
- Keep the first DMA target small, then move to line-buffer or ping-pong
  buffering before whole-frame capture.

Exit condition: eDMA fills a bounded buffer from FlexIO without CPU polling and
without corrupting the existing frame buffers.

### Phase 6: Frame Buffer and LCD Integration

- Convert or route captured data into the existing RGB565 frame representation.
- Reuse the existing LCD display path for visual inspection.
- Preserve `avc__next_frame(uint16_t *buffer)` as the main application handoff
  until there is a reason to change it.

Exit condition: the LCD can show image data captured through FlexIO/eDMA.

## Hardware Strategy

Initial wiring stays aligned with the pin candidate summary:

| Camera signal | MCU pins | Role |
| --- | --- | --- |
| D0-D7 | `P4_12..P4_19` | FlexIO data `D20..D27` |
| PCLK | `P4_20` | FlexIO timer clock candidate `D28` |
| HSYNC/HREF | `P4_21` | FlexIO gate or GPIO/PINT count `D29` |
| VSYNC | `P4_22` or GPIO/PINT | Frame boundary, optional FlexIO `D30` |
| Spare/debug | `P4_23` | Scope/debug `D31` |
| SCCB/I2C | `P3_2/P3_3` | Existing sensor control |
| XCLK | `P2_2` | Existing SCG CLKOUT |
| RESET/PWDN | `P1_19/P1_18` | Existing GPIO control |

Before enabling capture, verify with a scope or logic analyzer that XCLK reaches
the camera and that returned PCLK, HSYNC/HREF, and VSYNC are present at the MCU
pins being tested.

## Risks and Open Checks

- The FRDM workbook has stale FlexIO labels for `P4_2..P4_7`; use the NXP
  MCXN947VDF signal dump as the routing authority.
- Confirm that the SDK FlexIO camera model accepts `datPinStartIdx = 20`.
  The MCXN947 FlexIO API uses pin indexes and local LCD examples already use
  high FlexIO data indexes, so this is plausible but still needs a build test.
- Confirm whether VSYNC belongs in FlexIO hardware or remains a GPIO/PINT frame
  boundary interrupt.
- Confirm DMA request setup on MCXN947: eDMA channel mux alone may be enough,
  but INPUTMUX request enables need to be checked during implementation.
- Avoid full-frame DMA until smaller transfers prove alignment, byte order, and
  frame boundary control.

## First Firmware Patch Checklist

- Add backend macros with SmartDMA/EZH as the default.
- Fix the OV5640 macro spelling compatibility.
- Add a tiny RTT print identifying the selected capture backend during
  `avc_camera__init()`.
- Build with `.\build_cmake.ps1`.
- Flash with `.\flash.ps1 -CMake`.
- Capture boot RTT with `.\rtt.ps1 -CMake -Reset -Seconds 4`.
