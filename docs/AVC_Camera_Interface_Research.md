# AVC (NXP-Cup) Camera/Display Interface — Hardware Improvement Research

**Date:** 2026-06-16 | **Scope:** *Separate from the Zephyr USB-host work.* Exploration only — "can we make the MCXN947 design better, and is RW612 worth a pivot?"
**Current HW:** FRDM-MCXN947 + FRDM-AVC shield · OV5640 parallel (DVP) camera (Adafruit 5839) · SPI IPS TFT (ER-TFT020-3, 240×320) for debug overlays.
**Pipeline today:** OV5640 → **SmartDMA/EZH** ("bunny build" microcode) → per-frame or per-stripe(line) buffer in RAM → `avc__line_processor.c` edge detection on selected lines.

---

## 0. TL;DR

- **The frame-rate ceiling is the SmartDMA capture engine, not the OV5640.** The EZH microcode burns ~5–9 instructions per pixel clock, so PCLK is capped (~10–20 MHz) — that's why the code must "make PCLK disappear during HSYNC blanking" and ships `Div4`/`Div16` subsampled variants.
- **The single highest-leverage N947 upgrade: replace SmartDMA capture with FlexIO** (hardware shift registers + DMA). Lets the OV5640 run a normal/high PCLK at near-zero CPU, while **keeping the clean per-line DMA model your edge detector wants.** Proven by NXP app notes **AN12686 / AN5275** (FlexIO parallel camera).
- **Signal integrity over the jumpers is a *separate, electrical* problem.** FlexIO does **not** fix it — and a higher PCLK makes it worse. Fix it with cabling (short/shielded flex, series termination, or LVDS on PCLK), independent of MCU choice.
- **USB webcam is the wrong tool for a low-latency line-follower** (per-payload IRQs ~8k/s, software frame reassembly, multi-frame latency, immature host stack) — it solves cabling but degrades the control loop. Only sensible if bundled with a deliberate pivot to RW612 + Wi-Fi + Zephyr.
- **RW612 buys Wi-Fi streaming + a *dedicated* display controller (LCDIC), but makes the camera WORSE.** Per the NXP SDK manifest, **RW612 has NO FlexIO and NO SmartDMA/EZH** (and no CSI). So on RW612 you cannot do FlexIO *or* SmartDMA parallel capture at all — the OV5640 path is dead, leaving **USB webcam as the only camera option** (the worst choice for the control loop). Pick RW612 only if you fully commit to USB-cam + Wi-Fi streaming.

---

## 1. Why frame rate is limited today (evidence from `ezh_camera.c`)

The EZH/SmartDMA is a tiny programmable engine bit-banging the DVP bus. The inner per-pixel loop (`EZH_Camera_320240_Whole_Buf`) is roughly:

```
PCLK_0:  E_ACC_VECTORED_HOLD(PC,(1<<2))   ; wait for PCLK-edge change flag
         E_STRB_POST(R3, GPI, 1)          ; store one byte to RAM
         E_BSET_IMM(CFM,CFM,0)            ; clear vector flag
         E_GOSUB(VSYNC_0) / E_GOSUB(HSYNC_0)   ; poll frame/line sync
```

That's **multiple engine cycles per pixel**, all gated on detecting a *registered* PCLK edge. Consequences visible in the code itself:
- Header comment: *"the camera must be set that the pclk is disappeared during HSYNC blocking"* → the engine can't keep up unless PCLK is gated during blanking.
- `Div4` / `Div16` / grayscale variants exist purely to **throw away pixels** (store Y only, every 4th/16th) to survive the bandwidth.
- Whole-frame mode raises **one IRQ per frame** (`E_INT_TRIGGER` at VSYNC); subsampled modes raise **per-stripe IRQs** (16 stripes/frame) → this is your "per-line" path.

**Net:** the OV5640 wants a higher PCLK to scan its array at higher fps, but SmartDMA can't ingest it, so you've been forced into low PCLK + subsampling. **The capture engine is the bottleneck.**

---

## 2. The three capture architectures, compared (ties to the USB/IRQ discussion)

| | **SmartDMA/EZH (today)** | **FlexIO (proposed N947 upgrade)** | **USB UVC webcam** |
|---|---|---|---|
| Mechanism | SW bit-bang, ~5–9 cyc/pixel | **HW shift register clocked by PCLK + DMA** | USB host iso transfers |
| Max PCLK | ~10–20 MHz (the ceiling) | **~50 MHz+** (pin/timer limited) | n/a (camera-internal) |
| CPU load | Engine saturated per pixel | **Near zero** (DMA moves words) | ~8k IRQs/s + per-payload memcpy |
| Data layout | Row-major image in RAM ✅ | Row-major image in RAM ✅ | Frame reassembled in SW from payloads |
| **Per-line delivery** | Yes (stripe IRQs) ✅ | **Yes (DMA half/full + line IRQ)** ✅ | **No** — whole frames only |
| **Latency** | Low / deterministic ✅ | **Low / deterministic** ✅ | **High** (USB scheduling + frame buffering) ❌ |
| Cabling | Parallel DVP (SI-sensitive) | Parallel DVP (SI-sensitive) | **Standard USB cable** ✅ |
| Stack effort | Have it | Port AN12686 (~moderate) | Heavy; Zephyr host UVC immature, bare-metal complex |

**Key point for a line-following control loop:** the parallel-camera + FlexIO path gives you **per-line data, in image order, with deterministic low latency** — exactly what edge detection on selected lines needs. **USB UVC gives the opposite** (whole frames, software-assembled, latency of one-or-more frames). So USB "fixes" cabling at the cost of the very properties your controller depends on. (See the companion USB-host research for the per-payload IRQ detail — same conclusion.)

---

## 3. The real artifact cause: signal integrity, not the MCU

"Strange artifacts from missed sync" over long jumpers at high PCLK is a **transmission-line / edge-integrity** problem on PCLK, HSYNC, and the 8 data lines. This is **orthogonal** to whether you use SmartDMA, FlexIO, or switch MCUs — and **FlexIO at higher PCLK can make it worse.** Address it electrically:

1. **Shorten / shield the cable** — biggest ROI. A short shielded flex or a properly-grounded ribbon (ground every other conductor) beats long loose jumpers.
2. **Series termination** (~22–47 Ω) on PCLK and data at the OV5640 side; reduce the sensor's IO drive strength; ensure solid GND return next to PCLK.
3. **Differential/LVDS on PCLK** (and optionally data) over the long run with transceivers at each end — robust, but board work.
4. **Lower PCLK** — what you do now; trades frame rate. FlexIO lets you stop trading.

> If you keep the parallel camera, do #1–#2 regardless of MCU. If the run is unavoidably long, #3 is the durable fix.

---

## 4. Display: SPI TFT → 8080 parallel

- Current SPI TFT (ER-TFT020-3) is fine for debug overlays but bandwidth-limited.
- **8080 parallel via FlexIO on N947** is a clean upgrade — and you already have the template: `frdmmcxn947/flexio_mculcd_polling_cm33_core0` (FlexIO MCU-LCD 8080). NXP **AN12685** covers FlexIO 8080 LCD. The candidate display (buydisplay 8080 MCU 240×320) matches.
- ⚠️ **FlexIO resource budget:** MCXN947 FlexIO has a finite set of shifters/timers. Using FlexIO for **both** camera-in *and* 8080-LCD-out simultaneously may exhaust shifters/timers or pin-mux. Options: (a) camera on FlexIO + keep LCD on SPI (slow but adequate for debug), or (b) time-multiplex, or (c) verify shifter/timer/pin budget before committing to both on one FlexIO. **Check this early** — it's the main gotcha of the "all-FlexIO" plan.

---

## 5. RW612 vs MCXN947 — what actually changes

RW612 = 260 MHz Cortex-M33, **Wi-Fi 6 + BLE + 802.15.4**, FLEXCOMM (serial), FLEXSPI, **ENET (RMII)**, DMIC, I2S, **LCDIC (dedicated LCD controller)**, SCT0, GDMA, **USB OTG (HS)**. **Verified against the NXP SDK peripheral manifest (`C:\Users\emh20\Desktop\usbhost\RW612`): RW612 has NO FlexIO and NO SmartDMA/EZH** — and no MIPI-CSI. MCXN947, by contrast, has **both FlexIO and SmartDMA**.

> ⚠️ **This is the decisive difference.** N947 can capture a parallel camera (SmartDMA today, FlexIO as the upgrade). **RW612 physically cannot** — with no FlexIO and no SmartDMA, there is no engine to shift in a DVP bus. On RW612 the OV5640 parallel path is simply unavailable.

| Concern | MCXN947 | RW612 |
|---|---|---|
| Parallel camera ingest | **SmartDMA (have) / FlexIO (port)** ✅ | **None** — no FlexIO, no SmartDMA, no CSI ❌ → USB only |
| Keep the OV5640? | Yes | **No** — forced to a USB webcam |
| Camera cabling/SI problem | present (fixable, §3) | n/a if USB, but inherits USB latency instead |
| Wi-Fi real-time streaming | ❌ (none) | ✅ **the real reason to switch** |
| Display | SPI / FlexIO-8080 (emulated) | **LCDIC — dedicated 8080/SPI display controller** ✅ |
| USB host (off-the-shelf cams) | FS/HS, bare-metal | **HS, in-tree `nxp,uhc-ehci` host node; UVC host builds on RW612** |
| Ecosystem | MCUXpresso bare-metal (current) | leans Zephyr |

**Takeaways:**
- **RW612 doesn't just "not fix" the camera — it removes your good options.** No FlexIO/SmartDMA means the parallel OV5640 can't be captured at all, so moving to RW612 **forces** the USB-UVC path (the worst one for a tight control loop, §2).
- **RW612's genuine wins are Wi-Fi telemetry/streaming, a dedicated display controller (LCDIC), and a USB-HS host with the in-tree `nxp,uhc-ehci` controller driver.** Correction: the Zephyr USB-host **reference/CI test platforms are MAX3421E + native_sim virtual**, NOT RW612. `rd_rw612_bga` is a *build target* for the UVC host (PR #94590 built it "but not run"); it's the plausible NXP HS dev platform but I have no source confirming host hardware bring-up on it. So "USB webcam + stream over Wi-Fi" is a coherent RW612 + Zephyr vision, but RW612's USB-host maturity is "builds + controller present," not "proven reference" — on top of USB UVC's latency and stack immaturity.
- **For the camera specifically, MCXN947 is the better silicon** — it's the only one of the two that can do high-rate parallel capture with low-latency per-line DMA.

### 5b. If you DO go single-SoC RW612: making the USB camera viable

You can't carry the parallel OV5640 over (no FlexIO/SmartDMA), so the camera becomes a USB-UVC module. Two rules:

1. **There is no "parallel-camera → USB adapter" to buy.** Bridge chips (Sonix SN9C2xx, Generalplus, Cypress/Infineon FX3/CX3) only ship *inside* complete modules, pre-configured for one sensor. Building your own (FX3+GPIF) reintroduces the parallel-capture + timing problems you're escaping. Don't. If going USB, **replace** the sensor with a complete module (the sensor↔bridge link becomes a 2 mm trace inside the module → kills the long-jumper SI problem).
2. **Buy a module with a DOCUMENTED YUY2/uncompressed mode** — RW612 has **no hardware JPEG decoder**, so an MJPEG-only "1080p" webcam means 30 fps of software JPEG decode on the M33 (heavy + latency + competes with edge detection). Bandwidth is NOT the limiter (640×480 YUY2@30 ≈ 18 MB/s vs USB-HS 480 Mbps); **format, latency, and exposure control are.**

**Vetted candidate — ELP-USB100W03M-L36 (OmniVision OV9712), board-level ~32×32 mm, USB2.0 HS, UVC driver-free:**

| Format | Resolution | FPS |
|---|---|---|
| **YUY2 (uncompressed)** | **640×480** | **30** ✅ |
| **YUY2 (uncompressed)** | **320×240** | **30** ✅ |
| YUY2 | 800×600 | 20 |
| YUY2 | 1280×720 | 10 (uncompressed collapses above VGA) |
| MJPEG | 1280×720 ↓ | 30 |

- **Exposure is a UVC-adjustable parameter** (+ gain/brightness/white-balance) → can pin exposure for consistent track detection (most cheap webcams can't).
- A **120° wide-FOV variant** exists in the same OV9712 family (ELP B01DRJXDEA) if matching the original 120° Amazon pick.
- Stay **≤ VGA for uncompressed**. Standing caveats remain: whole frames only (no per-line), UVC/host-stack latency, and the immature RW612 UVC host must work.
- **Avoid generic Amazon "1080p 120°" listings** that don't disclose a YUY2 mode / UVC descriptor — likely MJPEG-only (bad on RW612) and may not enumerate on the early host stack. Prefer vendors that publish format tables (ELP-class).

> **Better than single-SoC RW612 for most cases:** keep N947 + FlexIO + parallel OV5640 for the camera/control loop and add Wi-Fi as a **companion radio** (ESP32 or an RW612 module over UART/SPI) just for streaming/telemetry — Wi-Fi without sacrificing the low-latency per-line camera path.

Sources: [ELP-USB100W03M-L36 spec (OV9712, YUY2+MJPEG)](http://www.elpcctv.com/elp-720p-usb-camera-module-usb20-ov9712-color-sensor-support-yuy2-and-mjpeg-with-36mm-lens-p-194.html) · [ELP 120° YUY2 variant](https://www.amazon.com/ELP-megapixel-Camera-Module-120degree/dp/B01DRJXDEA) · [ELP robot-vision YUY2 listing](https://www.elpcctv.com/elp-720p-cmos-ov9712-sensor-mjpeg-yuy2-dual-lens-stereo-usb-camera-module-with-uvc-for-robot-vision-p-135.html)

---

## 6. Recommendation

**To make the N947 design better (lowest risk, highest leverage), in order:**

1. **Port FlexIO parallel-camera capture (AN12686 / AN5275) to MCXN947.** Removes the SmartDMA PCLK ceiling, restores frame rate, near-zero CPU, and **keeps per-line DMA** for the edge detector. No N947 sample exists, but the FlexIO IP is the same as the i.MX RT app notes + you already run FlexIO MCU-LCD on this board → portable. **This is the single biggest improvement.**
2. **Fix the cable electrically** (short/shielded flex + series termination; LVDS on PCLK if the run must stay long). This is what actually kills the artifacts; do it regardless of MCU.
3. **(Optional) 8080 display via FlexIO** using your existing `flexio_mculcd` sample — but first **budget FlexIO shifters/timers** against the camera use (§4).

**Treat as separate, deliberate tracks (not quick fixes):**
- **USB webcam** — only as part of a pivot to RW612 + Wi-Fi + Zephyr, accepting higher latency and host-stack immaturity. Wrong tool for a tight line-following loop on its own.
- **RW612** — adopt for **Wi-Fi streaming/telemetry** (and its dedicated **LCDIC** display controller). But note RW612 has **no FlexIO and no SmartDMA**, so choosing it **forces a USB camera** — you can't carry the parallel OV5640 over. Don't pick RW612 expecting to improve the camera; pick it for Wi-Fi and accept the USB-cam tradeoff.

**One-line answer to "can we make N947 better?":** Yes — **FlexIO capture + a proper camera cable** get you higher frame rate and cleaner sync on the hardware you already have, without giving up the low-latency per-line model. And note the camera is a reason to **stay on N947**: RW612 lacks both FlexIO and SmartDMA, so it can't capture the parallel OV5640 at all. Reserve RW612 for when Wi-Fi streaming is the priority and you're ready to go USB-camera.

---

### References
- Current capture microcode: `avc/src/common/bunny_build/test/camera_reference_generator__320_240_whole_buf/.../ezh_camera.c`
- FlexIO display template you own: `frdmmcxn947/flexio_mculcd_polling_cm33_core0`
- [AN12686 — FlexIO parallel camera on i.MX RT](https://www.nxp.com/docs/en/application-note/AN12686.pdf) · [AN5275 — FlexIO parallel camera](https://www.nxp.com/docs/en/application-note/AN5275.pdf) · [AN12685 — FlexIO 8080 LCD](https://www.nxp.com/docs/en/application-note/AN12685.pdf)
- [FlexIO camera+LCD example (RT1010)](https://github.com/nxp-appcodehub/an-flexio_camera_rt1010)
- [RW612 block diagram](https://www.nxp.com/assets/block-diagram/en/RW612.pdf) · [RD-RW612-BGA Zephyr board](https://docs.zephyrproject.org/latest/boards/nxp/rd_rw612_bga/doc/index.html)
- See companion: `Zephyr_USB_Host_Assessment.md` (USB UVC IRQ/latency detail)
