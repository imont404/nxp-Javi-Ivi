# i.MX RT1064 / RT1176 as an AVC Camera Platform

Exploring NXP crossover MCUs that have a **dedicated parallel camera interface (CSI)** — the thing the MCXN947 lacks (forcing SmartDMA/FlexIO). A real CSI is a hardware frame-grabber with DMA: capture at high pixel clock, IRQ per frame/line, near-zero CPU.
Verified against NXP device data + Zephyr tree + JLCPCB/LCSC, 2026-06-17.

## Headline: both have CSI; OV5640 is turnkey
- **Parallel CSI driver in Zephyr:** `video_mcux_csi.c` (`nxp,imx-csi`). RT10xx SoC dtsi has `csi@402bc000`.
- **RT1176 also has MIPI-CSI2:** `video_mcux_mipi_csi2rx.c` (`nxp,mcux-mipi-csi2rx`), plus **PXP** 2D accel (rotate/scale/color-convert) and 2x LCDIF.
- **Your OV5640 is fully supported:** `drivers/video/ov5640.c` + ready camera shields **`nxp_btb44_ov5640`** and `arducam_cu450_ov5640`. NXP's RT EVKs literally ship with OV5640.
- Bonus: both have **USB HS host** (`nxp,uhc-ehci`) -> UVC-host option too.

## The two parts

| | **i.MX RT1064** (MIMXRT1064DVL6A) | **i.MX RT1176** (MIMXRT1176DVMAA) |
|---|---|---|
| Core | M7 @ 600 MHz | M7 @ 1 GHz + M4 @ 400 MHz |
| Flash | **4 MB internal** | **none** (external QSPI/HyperFlash) |
| RAM | 1 MB internal | 2 MB OCRAM (+ ext SDRAM typical) |
| Camera | **parallel CSI** | **parallel CSI + MIPI-CSI2** + PXP |
| Display | eLCDIF | 2x LCDIF + MIPI-DSI |
| Package | **MAPBGA-196** (10x10, 0.65 mm) | **LFBGA-289** (14x14) |
| Zephyr board | `mimxrt1064_evk` | `mimxrt1170_evk` |
| LCSC / JLC | C880325 / ~$14 | C1020237 / ~$17-20 |

## Why RT1064 is the better AVC fit
- **600 MHz M7 = ~5-10x the N947 M33** for vision.
- **Internal 4 MB flash + 1 MB RAM** -> a QVGA RGB565 frame is 153.6 KB, so **the whole capture+vision pipeline fits on-chip; NO external memory needed**. Board = BGA-196 + power + clock + a few passives. Far simpler than RT1176.
- **CSI ingests the OV5640 directly** with hardware DMA -> solves the SmartDMA pixel-clock ceiling *and* gives clean per-line/per-frame IRQs (what the edge detector wants), near-zero CPU. Strictly better than FlexIO.

## Why RT1176 is likely overkill for AVC
- 1 GHz + M4 + MIPI-CSI2 + PXP + dual display is more than a line-follower needs.
- **No internal flash** -> external QSPI/HyperFlash (+ usually SDRAM) -> **BGA-289 board with DDR-class routing**. Much bigger PCB effort.
- Justified only if you want MIPI cameras, heavy on-device ML, or 2x display/streaming.

## Stock reality (verify before committing!)
- The specific variants I checked show **thin JLCPCB in-house stock**: RT1064 DVL6A = **0**, RT1176 DVMAA = **2**. LCSC: RT1176 in stock ~$16.75; RT1064 listed ~$73 (inflated -> thin).
- "Lots of stock" is probably **JLCPCB global-sourcing** (LCSC/distributor-backed, adds lead time + sourcing fee/min-qty) or a **different variant**. **Action:** confirm the exact orderable P/N and whether it's in-house (no extra fee/lead) vs sourced. Fallback: the broader **RT106x** family (e.g. RT1062 + external QSPI flash) is usually better stocked but loses the internal-flash simplicity.

## Trade-offs vs staying on MCXN947
**For (RT):** real CSI solves camera ingest in silicon (no FlexIO/SmartDMA hacks); much more compute; OV5640 turnkey in Zephyr; USB host bonus.
**Against (RT):** **BGA** -> JLCPCB assembly + X-ray, no hand-rework (vs N947 LQFP); **leaves the MCX/eGFX/SmartDMA bare-metal AVC codebase** -> port to Zephyr (or MCUXpresso RT SDK); RT1176 adds external-memory board complexity.
**Unchanged:** the **cable signal-integrity** problem persists — CSI still ingests parallel signals over the cable, so the **LVDS adapter work still applies** (feed clean parallel into CSI). CSI replaces the *ingest engine*, not the *cable*.

## Recommendation / where this sits
- This is the **"solve it in silicon"** option vs the **"improve N947 in place"** option (FlexIO).
- **Immediate step is unchanged:** do the **FlexIO camera test on the existing N947** (cheap, current board) — see `FlexIO_Camera_Test_Plan.md`.
- **RT1064 is the strongest next-gen-board candidate** if FlexIO proves marginal or you want a real frame-grabber + headroom. Pair with the LVDS cable for the remote camera, OV5640 + Zephyr video for software.
- Decision gate: are you willing to move to **BGA + Zephyr** to get a proper CSI? If yes -> RT1064. If staying LQFP/MCUXpresso -> FlexIO on N947 is the pragmatic path.

## Display / 8080 support (verified)
- **Both have the eLCDIF** (`nxp,imx-elcdif`): RT1064 `lcdif@402b8000`; RT1176 `lcdif@40804000` **+** `lcdifv2` (RGB/DPI) **+** `mipi-dsi`. The eLCDIF **hardware-supports 8/16-bit MPU/8080 ("Smart LCD") mode** -> so **both can drive 8080 displays**. RT1176 is the more capable display part (adds parallel-RGB LCDIFv2 + MIPI-DSI).
- **Caveat:** stock drivers are **RGB/DOTCLK-only** — Zephyr `display_mcux_elcdif.c` uses `elcdif_rgb_mode_config_t` (no MPU); MCUXpresso `fsl_elcdif.c`/EVK examples are RGB-focused. eLCDIF-8080 works but is DIY/poorly documented.
- **Easy path = FlexIO-8080** (AN12685, FlexIO MCU-LCD driver) on either part — same as the N947 `flexio_mculcd` route.
- **Advantage over N947:** camera is on **dedicated CSI**, so **FlexIO is fully free for the 8080 LCD** — no camera/LCD FlexIO contention (which was the N947 gotcha). CSI->camera, FlexIO->8080 LCD, both unencumbered.

## PXP graphics engine (bare-metal; AVC overlay pipeline)
- **Both RT1064 and RT1176 have PXP** (2D Pixel Pipeline): **YUV->RGB CSC, scale, rotate/flip, alpha-blend/composite**. Mature bare-metal driver `fsl_pxp.c` + examples (`pxp_csc`, `pxp_rotate`, `pxp_queue`) + **eIQ MPP middleware** (`mcux-sdk-middleware-mpp`, does CSI->PXP->display). **Bare-metal MCUXpresso exposes PXP fully** (Zephyr only uses it for limited CFB rotation) -> "no Zephyr" is the right call.
- **MCXN947 has NO PXP** (only PowerQuad DSP math) -> PXP is a real RT advantage.
- **PXP is display-interface-agnostic** (memory-to-memory). Flow: CSI -> YUYV frame (RAM) -> PXP (CSC + scale-to-240x320 + rotate + blend overlay) -> RGB565 framebuffer (RAM) -> ship to ST7789 over **SPI** (ER-TFT020-2/3) or **FlexIO-8080** (ER-TFT020-7). PXP's direct-to-display mode targets eLCDIF (big RGB panels); for small SPI/8080 use PXP mem-to-mem + DMA panel transfer.
- **AVC value:** offload to hardware -> camera YUYV->RGB565, scale/crop, rotate, and **composite the eGFX debug overlay onto the live camera image** (replaces CPU compositing). M7 freed for vision/control.
- **Display = ST7789 240x320** (the AVC car already uses ER-TFT020-3 SPI). **Bottleneck is the panel transfer, not PXP:** 240x320 RGB565 = 153.6 KB; ~20 ms over 60 MHz SPI (~50 fps). **8080 (ER-TFT020-7) is faster** for high overlay refresh. PXP processes a frame in <1 ms.
- Parts: PXP confirmed in `mcux-devices-rt/RT1064` and `.../MIMXRT1176` (fsl_pxp present for both).

## References
- Zephyr: `drivers/video/video_mcux_csi.c`, `video_mcux_mipi_csi2rx.c`, `ov5640.c`; shields `nxp_btb44_ov5640`, `arducam_cu450_ov5640`; boards `mimxrt1064_evk`, `mimxrt1170_evk`.
- NXP: i.MX RT1064 product page; i.MX RT1170 family (RT1176).
- Camera architecture rationale: `AVC_Camera_Interface_Research.md` (this folder).
