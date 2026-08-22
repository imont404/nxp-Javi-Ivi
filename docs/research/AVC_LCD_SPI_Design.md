# AVC Rev A LCD SPI Design

Date: 2026-08-21. Durable closeout of the `lcd-spi-throughput` execution plan.

## Competition Decision

The Rev A competition image keeps the ER-TFT020-3/ST7789 display on LPSPI1 at the
verified **37.5 MHz SCK** setting:

- `CONFIG__DISPLAY_SPI_PLLCLKDIV = 2`
- `CONFIG__DISPLAY_SPI_SCKDIV = -1`

This is the configuration observed producing a correct live camera image. Do not change
the PLL divider for the 2026 race image. Builds using `PLLCLKDIV = 1` produced a black
screen even when `SCKDIV = 2` nominally restored the same 37.5 MHz SCK. That discriminates
the failure from a simple panel-frequency limit; the cause was not established.

The LCD remains useful for a cable-free camera check, boot/fault status, and lightweight
student visualization. The expected event workflow uses the direct USB Web Serial viewer
for development and the proven Android Wi-Fi relay for the race display. Complex plots,
logs, dashboards, and overlays belong on those host surfaces.

## Durable Implementation Result

The original display path called `lpspi1_init()` twice for each region dump, once for
8-bit control transfers and once for 32-bit pixel transfers. Each call reinitialized
LPSPI1 and called `EDMA_Init(DMA0, ...)`. Because DMA0 is shared, repeatedly resetting the
controller while the system is running was a correctness risk independent of display
speed.

The committed implementation now performs the expensive peripheral and DMA setup once.
Later calls change `TCR[FRAMESZ]` and refresh the eDMA-lite transfer preparation. The
working 37.5 MHz display was visually verified after this change. Preserve this one-time
initialization behavior.

The bulk RGB565 transfer is already dominated by wire time: a 320x240 frame is 153,600
bytes, or about 32.77 ms at 37.5 MHz before command/setup overhead. Further small-transfer
or TCD optimization cannot recover the time that is physically on the SPI wire.

The LCD DMA currently reads directly from the live camera ping-pong buffer. That is safe
only while the display finishes before capture reuses that buffer. If algorithm or overlay
work delays the dump past the next swap, the camera can overwrite pixels still being sent
and produce a torn frame. Treat this as an explicit ownership/deadline constraint; a future
fix may drop a display frame while its source buffer is busy rather than copying 128 KB or
stalling camera capture.

## Measurement Caveat

The execution plan initially reported approximately 37.20 ms per full display frame and
34.14 ms after the initialization fix. Those values agree closely with the independently
calculated wire time and correctly identified repeated initialization as waste, but they
are **not authoritative timing measurements**.

The instrumentation used `DWT->CYCCNT`, while `avc__next_frame()` resets that same counter
on every camera frame. Any measurement spanning the callback can therefore be corrupted.
Future timing work must use an independently owned counter such as OSTIMER or the existing
P4_1 display-dump scope marker before publishing new measurements.

Similarly, LPSPI divider readback was not a reliable source of truth during the experiment.
Measured transfer duration and visual output contradicted some register-derived claims.

## Display/Host Boundary

Do not remove the working LCD or eGFX dependency immediately before the competition.
Instead, future API cleanup should put a narrow AVC-owned RGB565 interface in front of the
current implementation. The MCU-side requirement is intentionally small:

- clear/fill a region;
- put a pixel;
- draw horizontal and vertical lines or simple markers;
- draw a small fixed-font status string;
- transfer a camera/status region to the panel.

Complex graphs, scrolling logs, configurable dashboards, and rich overlays belong in the
USB host application. If MCU primitives remain performance-relevant, specialize them for
RGB565: clip once per primitive, write pixels directly, use packed stores for horizontal
spans, and avoid the generic per-pixel format dispatch in the existing eGFX path.

## Deferred Questions

- Why changing `PLLCLKDIV` at runtime breaks the display path even when nominal SCK is
  restored. `clock_config.c`'s `PLLCLKDIV_HALT` boot setting is a specific diagnostic lead,
  not an established cause.
- Whether a different, correctly initialized clock source can safely run LPSPI1 above
  37.5 MHz.
- Whether the remaining per-region command transfers or DMA descriptors are worth
  optimizing after host preview becomes the normal workflow.
- Whether eGFX should eventually be removed from the firmware build after the narrow AVC
  display API exists and the USB workflow is proven in student use.

None of these questions blocks the Rev A 2026 competition image.

## Related

- [`AVC_Competition_Overview.md`](AVC_Competition_Overview.md)
- [`AVC_USB_Debug_Display_Current_State.md`](AVC_USB_Debug_Display_Current_State.md)
- [`AVC_USB_Debug_Transport_Protocol.md`](AVC_USB_Debug_Transport_Protocol.md)
- `src/avc/avc_core0/source/avc_io/lpspi1.c`
- `src/avc/avc_core0/source/avc_io/eGFX_Driver_ER-TFT020-3.c`
