# AVC OV5640 LVDS Adapter — BOM & Design Notes

Adapter that solders on the back of the **Adafruit OV5640** breakout and runs the DVP bus over a cable as **LVDS differential pairs**, to fix the high-PCLK signal-integrity problem (see `AVC_Camera_Interface_Research.md`). Companion board at the **MCXN947** end converts back to single-ended.

Date: 2026-06-16 (design). Status: **built and working** — see §0 As-Built (2026-07-24).

---

## 0. As-Built Status (2026-07-24)

The design in §1-§6 below was built as **two boards plus a custom flex cable**, and it **works — clean at ~12"**. This section is the as-built record of record; §1-§6 are retained because they still explain *why* each choice was made.

Everything in §0 is extracted from the Altium design-review bundles
(`altium-cruncher dr <project>.PrjPcb` → `tracks/A/output/design_review/`), not from memory.

### 0.1 The three deliverables

| Board | CCA P/N | PCB P/N | Repository | Layers |
|---|---|---|---|---|
| **`avc-cam-tx`** | 10-10112 | 11-10112 | <https://github.com/wavenumber-eng/11-10112__avc-cam-tx> | 2 |
| **`avc-cam-rx`** | 10-10113 | 11-10113 | <https://github.com/wavenumber-eng/11-10113__avc-cam-rx> | 4 |
| **`avc-cam-flex`** | — | 11-10115 | <https://github.com/wavenumber-eng/11-10115__avc-cam-flex> | 2 |

Local working copies: `D:\prj\wavenumber\avc\avc-cam-{tx,rx,flex}`.
Rev **A**, status `PROTOTYPE`, schematic/PCB date 2026-07-10, golden fab data released 2026-07-13.

- **`avc-cam-tx`** mounts to the Adafruit OV5640 board. 3× LVDS quad **drivers** for the downstream bus + 1× LVDS **receiver** for the upstream clock.
- **`avc-cam-rx`** plugs into the FRDM-AVC shield. 3× LVDS quad **receivers** feeding **single-ended** into the MCXN947 + 1× LVDS **driver** for the upstream clock.
- **`avc-cam-flex`** is the custom **50-pin, 300.0 mm × 25.5 mm (11.811" × 1.004") 2-layer JLCPCB flex** joining them.

This is **Path B** from §5 (custom flex PCB), not the shielded-FFC prototype path. The as-built connector is **50-pin**, larger than the ~32-pin estimate in §5 — the extra conductors are grounds interdigitated between pairs, plus the single-ended and power lines.

### 0.2 As-built BOM (matches the §2 design BOM)

| Role | Part | JLCPCB | Qty | Designators |
|---|---|---|---|---|
| **tx** down driver | DS90LV047ATMTC | C3215927 | 3 | U1, U2, U3 |
| **tx** up receiver (XCLK) | SN65LVDS2DBVR | C38204 | 1 | U4 |
| **tx** XCLK pair termination | 100 Ω 0603 | C193336 | 1 | R7 |
| **tx** enable straps | 0 Ω 0402 | C242160 | 6 | R1–R6 |
| **rx** down receiver | DS90LV048ATMTCX | C3215940 | 3 | U1, U2, U3 |
| **rx** up driver (XCLK) | SN65LVDS1DBVR | C465731 | 1 | U4 |
| **rx** down pair termination | **330 Ω** 0603 | C278617 | 11 | R1,R2,R3,R6,R7,R8,R10,R12,R13,R14,R16 |
| **rx** enable straps | 0 Ω 0402 | C242160 | 6 | R4,R5,R9,R11,R15,R17 |
| **both** flex connector | Hirose **XF2M-5015-1A** 50-pos 0.5 mm R/A FPC | C188356 | 1 ea | J2 |
| **tx** board header | RS2BE-18-G-SMT, 18P 2.54 mm receptacle | C7298499 | 1 | J1 |
| **rx** board header | Adamtech **PM254-2-09-S-8.5**, 18P 2.54 mm receptacle (**bottom side**) | C3975154 | 1 | J1 |

The LVDS silicon matches §2 exactly. Enables are strapped active through 0 Ω jumpers (`EN`→+3V3, `EN*`→GND) rather than hard-wired, so they remain reworkable.

### 0.3 ⚠️ Termination discrepancy — 330 Ω, not 100 Ω

**The 11 downstream pairs are terminated with 330 Ω, not the 100 Ω this document specifies in §2 and §4.1.**

Confirmed from the netlist: each resistor sits directly across a pair at the receiver, e.g. `VSYNC_P: J2.39, R2.2, U1.3` / `VSYNC_N: J2.40, R2.1, U1.4`, with R2 = ERJ-3EKF3300V (330 Ω).

The **upstream `CAM_CLK` pair is correctly terminated at 100 Ω** (tx R7), so the two directions are terminated inconsistently within the same design.

Why this matters: LVDS drivers are current-mode, so termination sets both the received swing and the line match. 330 Ω across a ~100 Ω pair produces roughly 3× the intended differential swing and a substantial impedance mismatch. **At the current PCLK this is harmless and measurably clean** — §5's reasoning holds, the bit period vastly exceeds the ~1.7 ns cable delay, so reflections settle well within a bit, and the larger swing arguably adds noise margin.

**It becomes a real risk if PCLK goes up.** The whole point of the FlexIO work (`AVC_Camera_Interface_Research.md` §6.1) is to stop trading frame rate against PCLK. Before raising PCLK, decide deliberately whether 330 Ω was intentional; if not, 100 Ω is a drop-in 0603 change on `avc-cam-rx`.

**Status: not yet resolved with the designer** — flagged here, not corrected.

### 0.4 Connector pinouts

**J1 — 18-pin 0.100" header (identical on both boards).** On `avc-cam-rx` this is the connector that mates to the FRDM-AVC shield.

| Pin | Signal | Pin | Signal | Pin | Signal |
|---|---|---|---|---|---|
| 1 | +3V3 | 7 | PCLK | 13 | D3 |
| 2 | GND | 8 | CAM_CLK (XCLK) | 14 | D2 |
| 3 | SDA | 9 | D7 | 15 | D1 |
| 4 | SCL | 10 | D6 | 16 | D0 |
| 5 | VSYNC | 11 | D5 | 17 | CAM_RST |
| 6 | HSYNC | 12 | D4 | 18 | CAM_PDWN |

**J2 — 50-pin 0.5 mm FPC (identical on both boards, straight-through over the flex).**

| Pins | Signal |
|---|---|
| 1, 2, 3, 48, 49, 50 | +3V3 |
| 4, 6, 8, 11, 14, 17, 20, 23, 26, 29, 32, 35, 38, 41, 44, 46 | GND (interdigitated between pairs) |
| 5 / 7 | CAM_RST / CAM_PDWN |
| 45 / 47 | SDA / SCL |
| 9,10 · 12,13 · 15,16 · 18,19 | D0 · D1 · D2 · D3 pairs |
| 21,22 · 24,25 · 27,28 · 30,31 | D4 · D5 · D6 · D7 pairs |
| 33,34 | CAM_CLK (XCLK, **upstream**) |
| 36,37 | PCLK |
| 39,40 | VSYNC |
| 42,43 | HSYNC |

The `G S S G S S G` interdigitation described in §5 is present as built. Note the P/N ordering **alternates** pair-to-pair (e.g. D0 is N,P at 9,10 but D1 is P,N at 12,13) — routing-driven, and harmless because polarity is fixed at both ends by the same flex.

**Spare capacity:** one quad receiver channel is unused (`U1` channel 4, outputs left floating as `NetU1_7/8/10`), matching the "12 channels, 1 spare" plan in §1.

### 0.5 Why this exists — the problem it replaces

For competitions, students wired the Adafruit OV5640 to the FRDM-AVC shield with **dupont/jumper wires into headers**. Because of the high pixel clock, those runs could only be so long before the DVP bus degraded, producing **HSYNC/VSYNC errors and image artifacts**. Cable length was effectively a hard constraint on the mechanical design of the car.

The LVDS link removes that constraint: 12" of clean, mechanically robust flex instead of a length-limited bundle of jumpers.

### 0.6 Bring-up note — first power-up did not work

⚠️ **Read this before debugging a dead camera on this stack.**

On first bring-up the camera did not come up. The initial suspicion was **I2C/SCCB communication** with the OV5640 — a reasonable first guess, because SCCB is one of the few signals that still runs single-ended over the flex and it fails silently: no image, no sync, nothing to scope.

That was wrong. **The problem was the 18-pin 0.100" female header (`J1`) on `avc-cam-rx` mating to the FRDM-AVC** — it was not properly seated. A power cycle and reseating the connectors fixed it. No firmware change, no design defect.

**Debug order for a dead camera on this link — do these before touching firmware:**

1. **Reseat `J1`, the 18-pin 0.100" header between `avc-cam-rx` and the FRDM-AVC.** This is the known failure mode. Confirm it is fully seated across all 18 positions and not tilted or offset by a row.
2. Reseat both 50-pin ZIF flex connectors (`J2`) at `avc-cam-tx` and `avc-cam-rx`; confirm the tail is fully home and each latch is closed.
3. **Full power cycle** — not just a reset or a re-flash. The camera rails and the LVDS drivers need to come up cleanly.
4. Only then suspect SCCB / firmware.

A partially-seated 18-pin header is deceptive: it makes contact on enough pins to look plausible while dropping SCCB, a sync line, or a data bit. The symptom is a **dead or corrupt camera with no obvious firmware fault**, which is exactly what invites a long software hunt. Per §0.4, `J1` carries +3V3, GND, SCCB, all three sync signals, XCLK, and all eight data bits — every class of failure is reachable from that one loose connector.

### 0.7 Still open

- **§0.3 termination:** confirm whether 330 Ω was intentional; revisit before raising PCLK.
- Flex **stackup / measured differential impedance** is not recorded here (2-layer, signals over ground plane per §5).
- The OV5640 `XCLK` divider is unchanged — firmware still runs `CLOCK_SetClkDiv(kCLOCK_DivClkOut, 12U)` for the 320x200 mode (`bv_camera__interface.c`, `camera__configure_xclk()`). Now that the cable is clean, **the cable is no longer what caps frame rate** — the SmartDMA/EZH capture engine is (`AVC_Camera_Interface_Research.md` §1).
- **SCCB robustness** is planned but not implemented: I2C rate reduction, retries, and on-screen diagnostics. See plan `camera-sccb-robustness`.

---

## 1. Architecture (signal directions)

- **UP — MCU -> camera, 1 channel: `XCLK` (XVCLK master/system clock).** The OV5640 needs this to run its internal PLL. *(Not PCLK — PCLK is generated by the camera and returns downstream.)*
  - If the camera board already has its own oscillator, this channel can be dropped (or put a local 24 MHz oscillator on the adapter instead). Most bare OV5640 boards need host XCLK.
- **DOWN — camera -> MCU, 11 channels: `PCLK`, `VSYNC`, `HREF`, `D0..D7`.** PCLK is the most timing-critical; MCU samples the data bus on PCLK edges (source-synchronous).
- **Single-ended / slow (no LVDS):** I2C/SCCB (`SCL`,`SDA`), `RESET`, `PWDN`.

11 down channels -> **3x quad (12 ch, 1 spare)**. 1 up channel -> 1x single pair.

---


## 2. BOM (JLCPCB stock confirmed 2026-06-16)

| Role | Part | JLCPCB # | LCSC | Pkg | Stock | Rate | Qty/board |
|---|---|---|---|---|---|---|---|
| **Down driver** (on camera adapter) | DS90LV047ATMTCX/NOPB | C87097 | C87097 | TSSOP-16 | 1,200 | 400 Mbps | **3** |
| **Down receiver** (on MCU board) | DS90LV048ATMTCX/NOPB | C87137 | C87137 | TSSOP-16 | 2,999 | 400 Mbps | **3** |
| **Up driver / XCLK** (on MCU board) | SN65LVDS1DBVR | C465731 | C465731 | SOT-23-5 | 12,771 | 630 Mbps | **1** |
| **Up receiver / XCLK** (on camera adapter) | SN65LVDS2DBVR | — | C38204 | SOT-23-5 | in stock | 400 Mbps | **1** |

- DS90LV047A/048A = TI "flow-through" quad LVDS (inputs one side, LVDS pairs the other) — best pinout for a tight adapter; TSSOP-16 (~4.4x5 mm).
- All four are JLCPCB **"Extended"** parts (no Basic LVDS exists) -> one-time ~$3 feeder fee each (~$12 total). 400 Mbps/ch >> ~50 MHz PCLK need.
- Passives: **100 ohm 0402 termination resistor per differential pair at the receiver end** (11x on MCU board + 1x on camera adapter), plus 100 nF decoupling per IC.

**Per-board population:**
- **Camera adapter (back of OV5640):** 3x DS90LV047A + 1x SN65LVDS2 + 1x 100 ohm (XCLK term) + decoupling (+ optional local 3.3 V LDO / 24 MHz oscillator).
- **MCU board (MCXN947):** 3x DS90LV048A + 1x SN65LVDS1 + 11x 100 ohm (per down pair) + decoupling.

---

## 3. Channel allocation (3x quad down, 12 ch)

| Quad | Ch0 | Ch1 | Ch2 | Ch3 |
|---|---|---|---|---|
| Driver/Rx #1 | PCLK | VSYNC | HREF | D0 |
| Driver/Rx #2 | D1 | D2 | D3 | D4 |
| Driver/Rx #3 | D5 | D6 | D7 | *(spare)* |

(Keep PCLK on its own quad-channel and length-match its pair to the data pairs.)

---

## 4. Design caveats (lock in before layout)

1. **Termination:** 100 ohm across each diff pair **at the receiver** (DS90LV047A/048A and SN65LVDS1/2 need external 100 ohm). 11x at MCU + 1x at camera.
2. **Input-level compatibility:** DS90LV047A = VCC 3.0-3.6 V, TTL inputs (VIH ~2.0 V). **Verify OV5640 DOVDD (I/O) on the Adafruit board** — 2.8 V is fine; if set to 1.8 V the driver inputs are marginal -> run DOVDD at 2.8 V or add a translator. Receiver outputs 3.3 V CMOS -> MCXN947 3.3 V I/O OK.
3. **Skew (source-synchronous):** length-match the **PCLK pair to the 8 data pairs** through the whole channel (chip pin -> trace -> cable -> trace -> pin). Keep inter-pair skew well under the PCLK period (20 ns @ 50 MHz).
4. **Power:** 3.3 V on the adapter for the LVDS drivers + camera rails; local decoupling per IC.
5. **Direction is fixed per signal** — drivers at the source end, receivers at the sink end. Adapter = 3 quad DRIVERS + 1 single RECEIVER; MCU = 3 quad RECEIVERS + 1 single DRIVER.
6. **Package:** TSSOP-16 + SOT-23-5 are the small-package sweet spot with good stock. QFN LVDS exists but JLCPCB stock is much worse.

---

## 5. Cabling — ~12" (305 mm) flat flex

**Feasibility:** 12" LVDS over flat flex is fine. At ~50 MHz PCLK the bit period (~20 ns) >> cable delay (~1.7 ns one-way), so reflections settle within a bit — flat flex is forgiving at this length (cf. laptop LVDS panel cables). Limiters are **crosstalk + return path + skew**, not attenuation. Camera is fixed -> **static bend** (no dynamic flex-life concern).

**Path A — off-the-shelf shielded FFC (fast prototype):**
- 0.5 mm pitch, ~300 mm, with shield/ground layer (Molex Premo-Flex shielded, Wurth WR-FFC shielded). Plain FFC works if grounds interspersed: `G S+ S- G S+ S- G ...`.
- Uncontrolled diff impedance; ground-interspersing -> ~40-45 conductors -> 40/45-pin FFC. OK at 12".

**Path B — custom 2-layer flex PCB (preferred, JLCPCB-cheap):** ⭐
- Signal traces top, **solid ground plane bottom** -> controlled ~100 ohm differential, low crosstalk, no wasted ground conductors. Length-match PCLK pair to the 8 data pairs (serpentine); edge ground-stitch.
- Terminate in **ZIF flex tails** (stiffener + contacts) straight into 0.5 mm FFC connectors on both boards (phone/camera-flex style) -> one part, no separate cable+connector.
- ~28-30 top conductors + plane -> **~32-pin 0.5 mm ZIF** each end.

**Connector:** 0.5 mm-pitch FFC/FPC ZIF (Hirose FH12-style or JLCPCB-stocked generic). Mind **contact side** (type A same / type D opposite) so the tail mates at each end.

**Plan:** proto-1 with shielded FFC for fast bring-up; move to custom 2-layer flex for the robust/thin version (matters more if PCLK -> 50+ MHz via FlexIO).

## 6. Open / next
- Confirm Adafruit OV5640 DOVDD voltage and whether an onboard XCLK oscillator exists.
- Pick a specific JLCPCB-stocked 0.5 mm ZIF connector + finalize pin count; define custom-flex stackup/impedance.
- (Alt to reduce wires: ChannelLink/FPD-Link SerDes -> ~1 pair, but poor JLCPCB stock + more complex; discrete-LVDS is the pragmatic choice.)
