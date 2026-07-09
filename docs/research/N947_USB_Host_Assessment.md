# Can USB Host be added to the MCXN947? (AVC context)

Assessment for the AVC car (currently MCXN947). Verified against NXP device headers + the Zephyr tree, 2026-06-17.

## Verdict
**Yes — USB host is addable to the MCXN947, and the controller path is largely already done upstream.** The work is DT/board enablement + a VBUS hardware question + the class-driver maturity gap — *not* a new controller driver. **But for the AVC line-follower, a USB camera is still the wrong primary sensor** (whole-frame, latency, no per-line) — see `AVC_Camera_Interface_Research.md`. USB host on N947 makes sense as a *secondary/telemetry* path, or for non-realtime capture.

## What the N947 actually has (device headers)
The MCXN947 has **two** USB controllers:
| Controller | Type | Base | IRQ | In-tree Zephyr UHC driver |
|---|---|---|---|---|
| **USBHS1__USBC** | **High-Speed OTG (EHCI) + HS PHY** | `0x4010_B000`(NS)/`0x5010_B000`(S) | `USB1_HS_PHY_IRQn=66` (+ controller IRQ) | **`uhc_mcux_ehci.c`** (`nxp,uhc-ehci`) |
| USBFS0 | Full-Speed (KHCI) | `USBFS0_BASE` | `USB0_FS_IRQn=50` | `uhc_mcux_khci.c` (`nxp,uhc-khci`) |

- The **USBHS EHCI block is byte-identical to the MCXN236/MCXN247** (same `USBHS_Type`, same `0x4010_B000` base) and the **same IP RW612 / i.MX RT10xx / RW6xx use**. The in-tree `uhc_mcux_ehci` driver already targets it.
- For video (UVC, uncompressed), you **need the HS path** — Full-Speed can't carry uncompressed pixels.

## What's needed to enable host (modest)
1. **DT host node overlay** — the N9xx/N23x SoC dtsi exposes the controller only in *device* mode today (`nxp,ehci`). Add a `nxp,uhc-ehci` host node on the same controller (reg `0x10b000`, the HS IRQ, `phy-handle = <&usbphy1>`, `status="okay"`), enable `usbphy1`, disable the device node. (Same overlay shape as the MCXN236 port — see the USB-host research.)
2. **VBUS sourcing (the likely hardware blocker)** — host mode must supply 5 V VBUS to the device. The FRDM-MCXN947 target USB port is typically device/bus-powered; verify the schematic / add an external VBUS source or powered hub for bring-up.
3. **Clock/PHY** — the board already runs USB *device*, so the clock path exists; `uhc_mcux_ehci`/`uhc_mcux_common` handle host bring-up.
4. No new driver code — `uhc_mcux_ehci` is the same one RW612 uses for the `host_uvc` sample.

## Class-driver reality (the real gap)
- **UVC host class is merged** -> a USB camera will enumerate and stream (the `samples/subsys/usb/host_uvc` sample is the reference; only wired for `rd_rw612_bga` today, portable to MCXN via the overlay above).
- **MSC / HID / CDC-ACM host classes are NOT merged** (WIP/none) — so USB drives, HID, serial over host are not turnkey upstream.
- Whole stack is flagged **experimental, APIs unstable** by the maintainer.

## Effort & recommendation for AVC
- **Prototype effort:** small-to-moderate — DT overlay (mirrors MCXN236 port) + solve VBUS. Same `host_uvc` sample + a **YUYV** camera (no MJPEG — N947/RW612 have no JPEG decoder).
- **De-risk on RW612 first** (the sample's supported board), then port the overlay to the N947 — the controller IP is identical.
- **For the line-following control loop, keep the parallel OV5640 + FlexIO/SmartDMA path** (per-line, low latency). Treat N947 USB host as: (a) a way to use off-the-shelf USB cameras for non-realtime/secondary capture, or (b) a path you can demo, but not the primary track sensor.
- Note N947 has **no Wi-Fi** — so "USB camera + stream" still points to RW612 if streaming is the goal.

## References
- USB-host stack/state, N247 IP verification, `host_uvc` sample + MCXN236 overlay: see the Zephyr USB-host assessment (`../../../Zephyr_USB_Host_Assessment.md`) and notes.
- Camera architecture / why USB is wrong for the control loop: `AVC_Camera_Interface_Research.md` (this folder).
