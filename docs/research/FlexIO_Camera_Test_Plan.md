# FlexIO Camera Capture — Test Plan (MCXN947 / OV5640)

Goal: replace the SmartDMA/EZH capture (pixel-clock-limited) with **FlexIO + eDMA** to free the OV5640 to run a high PCLK, while keeping clean **per-line DMA** for the edge detector. See `AVC_Camera_Interface_Research.md` for the why.
Date: 2026-06-17.

## Hardware facts (verified, MCXN947)
- **FlexIO: 1 instance**, **8 shifters**, **8 timers**, **8 shifter buffers (32-bit each)**, parallel-shift capable (`HAS_PARALLEL_WIDTH`), byte/nibble-swap buffers available.
- **eDMA: 16 channels**, up to 16-byte bursts.
- **SRAM ~480 KB** -> full QVGA RGB565 frame (320x240x2 = 153.6 KB) fits with room to spare; ping-pong line/stripe buffers trivially fit.

> **Note added 2026-07-25.** Two figures above have drifted from the shipped
> configuration, without changing the capacity argument being made. The firmware runs
> **320x200**, not QVGA 320x240, so a frame is **128 KB** and the ping-pong pair is
> **256 KB**. The linker defines 384 + 96 + 32 + 4 = **516 KB** of regions
> (`avc_core0_Debug_memory.ld`). RAM is in practice the *scarce* resource once both camera
> buffers are allocated — see `AVC_Competition_Overview.md` §7.

## Buffering model (two levels)
1. **In FlexIO: tiny elastic FIFO only** — 1 shifter fills 32 bits every 4 PCLKs (PWIDTH=8); chain shifters for ~16-32 B total. NOT a frame buffer; just hides DMA/bus latency.
2. **In SRAM via eDMA: the real buffer** — line / stripe / full-frame, your choice. **eDMA major-loop size = IRQ granularity** (set to one line -> 1 IRQ/line). Near-zero CPU during capture.

## FlexIO resource map (camera-dedicated)
| Resource | Use |
|---|---|
| Pins | D0-D7 (8) + PCLK + HREF (+ VSYNC optional) |
| Timer 0 | shift clock gated by PCLK |
| Timer 1 | (optional) gate shifting on HREF (active-line only) |
| Shifter(s) | 1 RX min; chain 2-4 for FIFO depth |
| VSYNC | GPIO interrupt for start-of-frame (do NOT spend a FlexIO timer) |
| Swap buffers | use SHIFTBUFBYS/etc. to fix pixel byte order, zero CPU |

> Single FlexIO instance -> camera consumes most of it. Keep the debug LCD on **SPI** (not FlexIO) during these tests.

## Suggested DMA buffer layout (start simple -> optimize)
- **Phase 1 (correctness):** capture a **full frame** into one SRAM buffer; 1 DMA IRQ/frame. Verify pixels/format/geometry by dumping over UART/SWO or to the SPI LCD.
- **Phase 2 (control-loop model):** **ping-pong line buffers** (2x one line, e.g. 320x2=640 B RGB565); eDMA major-loop = 1 line -> **1 IRQ/line**; edge-detect each line as it lands. Mirrors current EZH stripe model but at high PCLK, near-zero CPU.
- **Phase 3 (optimize):** grayscale/Y-only or subsample if bandwidth/RAM tight; tune burst size; measure CPU headroom.

## Test milestones
1. **PLL/clocks:** generate XCLK to OV5640; bring up SCCB/I2C; read sensor ID. (reuse existing AVC config)
2. **FlexIO RX bring-up:** capture a few lines into RAM at a LOW PCLK; verify byte order + HREF gating + line length.
3. **Frame capture (Phase 1):** full-frame DMA; visually verify.
4. **Raise PCLK:** push toward 50 MHz; confirm no dropped pixels (FlexIO FIFO + DMA keep up); find the ceiling.
5. **Per-line (Phase 2):** ping-pong line IRQ; integrate edge detector; measure latency + CPU%.
6. **Compare vs SmartDMA/EZH:** max stable PCLK, frame rate, CPU load.

## Open questions to answer during test
- Max stable PCLK on FlexIO+eDMA before FIFO overrun? (expect >> the ~10-20 MHz EZH ceiling; target ~50 MHz)
- eDMA latency/jitter vs FlexIO FIFO depth — how many shifters must be chained?
- RAM budget: full-frame vs ping-pong line vs stripe given other AVC buffers.
- Pin mux: are 8 contiguous FlexIO-capable pins + PCLK/HREF available on the FRDM-AVC shield routing?

## References
- AN12686 "Using FlexIO to emulate Parallel Camera Interface on i.MX RT" (same FlexIO IP; directly portable)
- AN5275 "Using FlexIO for parallel Camera Interface"
- App-code example: nxp-appcodehub/an-flexio_camera_rt1010 (FlexIO camera + LCD)
- Existing capture for comparison: `avc/.../ezh_camera.c` (SmartDMA), `frdmmcxn947/flexio_mculcd_polling` (FlexIO IP on this board)
