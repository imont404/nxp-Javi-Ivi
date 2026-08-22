# AVC Competition Overview — Event, Track, Vehicle, and Constraints

**Purpose:** the missing context doc. Every plan in `docs/plans` and every research note in
`docs/research` implicitly assumes this, and none of it was written down until now.

**Written:** 2026-07-25, from the project owner's direct account.
**Next race:** late August 2026.

> **Verification note.** Track figures below are *nominal* and come from the NXP Cup rules
> as relayed by the organizer. The official rules PDF could not be retrieved by tooling —
> `nxpcup.nxp.com` and `www.nxp.com` serve a 746-byte block page to automated fetchers for
> the 2024, 2025, and 2026 editions. **If a copy is dropped into `docs/research/`, this doc
> should be reconciled against it.** As-built track dimensions deviate anyway (see §2).

---

## 1. What this event is

This is **not** the EU NXP Cup season, and the difference drives nearly every engineering
decision in this repository.

The organizer (NXP) was formerly heavily involved in the US Freescale Cup. US competition
wound down and the program is now mostly EMEA. A local race was authorized at Galileo in
2024 under the name **"AVC"**; it can now be run under the **NXP Cup** banner. Same idea,
local variant.

It runs inside **FIT**, a three-day student conference.

| | EU NXP Cup | This event |
|---|---|---|
| Preparation | Months | **~3 days** |
| Structure | Full season | Track within a 3-day conference |
| Students build | The car, from parts | **Nothing — the kit arrives assembled** |
| Students learn | Everything | **Software tuning** |
| Rules | Extensive | Deliberately minimal |

**Schedule shape:** students start **Tuesday evening**, race **Friday morning**. Day one
is mostly acclimatization — it is the first time they have ever seen the codebase. That is
precisely why reference material and a working starter image matter so much.

**History:** ~2024 first run (subject of an NXP Smarter World blog post, *"The Power of
Transformation"* — that URL now 404s). 2025 run again, with strong results. Late August
2026 is the third.

### What this implies for the firmware

This repository is an **organizer-supplied student platform**, not a competitor's entry.
The design bar is therefore *not* "fastest possible car". It is:

> A novice with roughly 60 hours, who did not write this code and cannot debug it deeply,
> can understand it, tune it, and see clearly when something is wrong.

Concretely, that is why the codebase looks the way it does: `CONFIG__*` compile-time knobs
with `#error` guards, RTT and on-screen diagnostics, a live LCD camera view, a small
tunable starter algorithm rather than a clever opaque one. **Silent failure is the enemy**
— the student cannot read the firmware to find out what went wrong. That framing is the
whole justification for the `camera-startup-diagnostics` plan.

---

## 2. The track

Nominal geometry, borrowed from the official NXP Cup rules — **the only part of those
rules this event follows**:

- **Width: 55 cm**
- **Surface: matte white**
- **Edges: a continuous 2 cm black line along _each_ edge**
- **Crossings: at 45° and 90°**

### Read this carefully — it is *edge*-line following

There are **two black rails and the car drives between them**. This is not a center-line
follower. It is the classic Freescale/NXP Cup arrangement, and it changes the vision
problem:

- The car tracks a *lane*, not a stripe. Lane center is inferred from two edges.
- **Crossings are the hazard.** At a 45°/90° intersection both edge lines break *and*
  transverse lines appear. A naive follower chases the transverse line off the track.
  Handling crossings is a real part of the challenge.
- Losing one edge (curve, glare, saturation) still leaves one edge — a robust algorithm
  degrades rather than fails.

### As-built reality

The track is built **from locally sourced materials in Guatemala**, so:

- Width comes out **close to 55 cm, not exactly**.
- Edges are **tape**, and are **sometimes made a little wider** than 2 cm.
- "Matte" matters more than it sounds. Gloss under conference-hall lighting produces
  specular glare that can blow out a camera row and read as "no line anywhere". This is a
  materials decision with direct firmware consequences.

**Do not hard-code 55 cm or 2 cm anywhere.** Treat both as tunable, and expect the actual
numbers to be measured on-site.

### 2026 twist — color

New this year, and **not yet finalized**. Two candidate directions:

1. **A narrower colored lane** inside the track — bonus points for staying in it.
2. **Colored obstacles** — bonus for avoiding them.

Either way the car must distinguish **green and red** from the white surface and black
edges. This is the driver behind the current camera-format and processing work. See
[`neutron_npu/AVC_Neutron_NPU_Assessment.md`](neutron_npu/AVC_Neutron_NPU_Assessment.md)
for the analysis of how to implement it, including the recommended chroma-LUT approach.

---

## 3. The vehicle

### Chassis

Hiwonder **Ackermann steering chassis**
(`https://www.hiwonder.com/products/ackermann-steering-chassis`,
docs at `https://docs.hiwonder.com/projects/Ackermann-Chassis/en/latest/`).

**A static dump of the vendor product page is archived in [`../car_chassis/`](../car_chassis/)**
(`web/*.html` plus `dimensions.png`). It is the source for the spec tables below and is
worth keeping, since vendor pages rot.

| Property | Value |
|---|---|
| Length | ~270 mm |
| Width | ~197 mm |
| Ground to top mounting plate | 75 mm (= wheel diameter) |
| Steering | Ackermann, front, **40° maximum angle** |
| **Minimum turning radius** | **0.173 m** (vendor figure, at the 40° limit) |
| Drive | **Independently driven rear wheels** |
| Chassis material | 5052 aluminum, anodized |

**Rear drive motors** — `JGB37-520R90-12`:

| Property | Value |
|---|---|
| Rated voltage | 12 V |
| Rated power | ~8.3 W |
| Type | Permanent magnet brushed |
| No-load speed | 110 rpm |
| Rated speed | 85 rpm |
| Stall / rated current | 3.2 A / 0.36 A |
| Stall / locked torque | 15 kg·cm / 2.6 kg·cm |
| Shaft | 6 mm D-shaped |
| Connector | PH2.0 6-pin |

**Steering servo:** LD-1501MG, 6–8.4 V, PWM pulse-width control, 0.3° precision, 61 g.

### Encoder counts per wheel revolution — measured

- Encoder: **AB dual-phase, 11 lines** (11 base pulses per motor-shaft revolution).
- With 4× quadrature decoding, which is what the MCXN947 QDC does:

```
counts/wheel-rev = gear_ratio x 11 lines x 4 (quadrature)
                 = 30 x 11 x 4 = 1320
```

**Measured on this car 2026-07-25:** 13188 counts over exactly 10 hand-turned
revolutions = 1318.8/rev, i.e. **1320 to within 0.09%**.

The vendor contradicts itself — the spec table says **1:90** (which would give 3960) and
the support answer says **1:30**. **The measurement backs 1:30.**
`CONFIG__MOTOR_ENCODER_COUNTS_PER_WHEEL_REV` is set to **1320**.

> Note the no-load speed argument points the *wrong* way and should not be trusted: 110 rpm
> no-load suggests ~11 rpm at 10% duty, which would imply 3960. The wheel actually turns
> ~31 rpm at 10% duty. **PWM duty is strongly non-linear in speed at the low end.**

Derived: wheel circumference **235.6 mm**, theoretical top speed **0.432 m/s**.

**Two consequences that shape the control problem:**

- **Turning radius is slightly tighter than the track requires — but only slightly.**
  Students frequently **saturate the steering angle** (the servo hits the 40° limit). An
  algorithm that assumes proportional steering authority will misbehave at the limit.
- **Independent rear wheels allow an active differential.** Torque-vectoring the rear
  wheels can assist steering beyond what the servo alone provides — a natural answer to
  the saturation problem, and largely unexploited so far.

**Motor speed is limited by the 12 V gear motors.** This is a real ceiling: in 2025 the
strongest teams essentially ran **flat out**, with the control algorithm — not the
powertrain — deciding the outcome.

### Camera mounting

The **OV5640** mounts **off the chassis on a raised mast**, deliberately high, looking far
down the track. This is a deliberate design choice with several effects:

- **Perspective.** On a straight, the lane appears as a white wedge narrowing toward the
  vanishing point, black edges converging. Apparent lane width is a function of image row.
- **Geometry shifts with camera angle**, so any row→distance mapping must be calibrated,
  not assumed.
- **Lookahead compensates for latency.** Seeing far ahead is what makes ~24 FPS adequate —
  the car reacts to track it will reach several frames from now. This is the single reason
  the frame rate is not a binding constraint.

---

## 4. Electronics and firmware platform

**NXP FRDM-MCXN947** (donated by NXP) plus the custom **FRDM-AVC shield**, which provides
motor/PWM drive and the camera interface.

### MCXN947

- Dual **Cortex-M33 @ 150 MHz**. **Only core0 is used today.** Core1 has been impractical
  because the EZH shares a code bus with it, and the EZH runs camera capture. Moving
  capture to FlexIO removes that contention and makes core1 genuinely available — see
  [`AVC_Camera_FlexIO_Pin_Migration.md`](AVC_Camera_FlexIO_Pin_Migration.md).
- eIQ **Neutron N1-16 NPU** (see the assessment doc; not currently used).
- PowerQuad DSP coprocessor, CoolFlux BSP32 (both effectively unusable — no toolchain).
- 2 MB flash, ~512 KB SRAM.

### Camera path

| Aspect | Current state |
|---|---|
| Sensor | OV5640 |
| Capture backend | **EZH / SmartDMA** coprocessor (`CONFIG__CAMERA_CAPTURE_BACKEND`) |
| Alternate backend | FlexIO + eDMA — implemented and hardware-proven, not the Rev A default |
| Format | RGB565 |
| Resolution | **320 × 200** |
| Frame rate | **~24 FPS** |
| Buffering | Double-buffered ping-pong, 128 KB each |

- **Resolution is 320×200 to preserve RAM**, not because of sensor or bus limits. PSRAM
  over QuadSPI is possible on the board but deliberately avoided — the project stays
  within on-chip RAM.
- **24 FPS is limited by the EZH**, which is an I/O coprocessor and can only register the
  pixel clock so fast. It is not a sensor or algorithm limit.
- **RGB565 is locked for the 2026 competition path.** It blits straight to the LCD and is
  already proven through the PC and Android transports. A generated 65,536-entry flash
  LUT maps each pixel to eight-bit Y/H/S/V with one table access; YUY2 is deferred rather
  than destabilizing the display and host paths during race week.
- **Rev A is the 2026 competition hardware.** The working default remains EZH; a
  locally modified Rev A board can also use the proven FlexIO Port 1 path below.
  Rev B boards were not available in time for this event.

**FlexIO capture is proven on the Rev A camera wiring** (2026-07-25). Three jumpers on
`J9_EXT` — camera D4 and D5 onto `P1_8`/`P1_9`, PCLK onto `P1_14` — make `P1_4..P1_11`
simultaneously contiguous `FLEXIO0_D12..D19` and contiguous `SMARTDMA_PIO0..PIO7`.
Backend selection then becomes **alt7 versus alt6 in firmware on one harness**, verified
by flashing both alternately with no hands on the hardware. FlexIO captured at
**23.39 fps** against the EZH baseline of 23.43, with zero errors.

Rev B needs only two routing changes: camera D4/D5 to `P1_8`/`P1_9`, and PCLK to `P1_14`.
See [`AVC_Camera_FlexIO_Pin_Migration.md`](AVC_Camera_FlexIO_Pin_Migration.md).

### Display and debug

- On-board **LCD** showing the live camera view with lightweight graphic overlays. It is
  the cable-free camera check, boot/fault surface, and local fallback.
- **USB high-speed** streaming of frame data to a PC, tested against a Chrome **Web
  Serial** page. The self-contained viewer is the direct-PC debug surface for camera,
  logs, named telemetry, and rolling plots.
- The **Moto G Power 5G (2023)** is a proven USB-host/Wi-Fi bridge for the large-screen
  race view. Its one-browser page defaults to JPEG and can select H.264 or raw RGB565;
  it reuses the same session-gated `AVCU` firmware stream without a phone-specific mode.
- **eGFX** (Wavenumber graphics library, older version): currently supplies the local
  frame-buffer, line, and text operations. Keep it for the working race image, but put a
  small RGB565 AVC display API in front of it later and move complex visualization to the
  host. Pixel, text, and line hot paths need specialization if they remain on the MCU.
- Note the memory trick: `camera_view.Data` **aliases the live camera frame buffer**, so
  student overlays are drawn directly into the frame that gets DMA'd to the LCD. No extra
  128 KB buffer.

---

## 5. The real-time budget

**This is the core teaching device of the whole event.**

```
150 MHz / 24 FPS  =  6.25 M cycles per frame
                  =  ~41.7 ms wall clock
320 x 200         =  64,000 pixels
                  →  ~98 cycles/pixel if you touch every pixel exactly once
```

Students are told plainly: **you have 1/24 s — about 41 ms — to determine a steering
angle.** The firmware measures and displays this (`algorithm_timer`, `frame_timer` via
`CYCLE_COUNTER`, CPU % on the overlay).

The classic student failure is **doing too much graphics work and blowing the frame time**
— which is exactly the lesson. Deadlines are real, and the debug visualization competes
with the work it is visualizing.

Useful anchor: only **a few scanlines** actually need processing. Full-frame work is
affordable if written carefully, but is rarely necessary.

---

## 6. Starter code and what students do

Students receive a framework, not a completed line-following solution. The platform
initializes the hardware, captures frames, exposes actuator/input APIs, and provides test
and visualization paths. Students build the track interpretation and vehicle-control
algorithm themselves.

The framework demonstrates the ingredients they can use:

- Reading a chosen scanline from the RGB565 frame (`line_to_process`, selectable live via
  the alpha pot — a nice touch for bench exploration).
- Color-space conversion through the generic RGB565→Y/H/S/V lookup table. The table
  removes floating-point arithmetic without baking in a track or obstacle classifier.
- Thresholding luminance to a black/white mask and using saturation/value as confidence
  gates for a circular hue filter. The current test view demonstrates the ingredients;
  students choose their own detection logic.
- Drawing the detected mask back onto the live view as an overlay.

**Students decide for themselves** how to find track center and what control algorithm to
use. That is the point of the exercise.

**2026 teaching addition: PID — as a student exercise.** It was not taught in 2025, though
some teams worked it out independently and did very well. This year it is introduced
explicitly, and students write the control law themselves; the platform supplies the
scaffolding, not the controller.

The motivating fact is already measured and observable: **M1 runs 8.8% faster than M0 at
identical duty**, so a car commanded straight in open loop visibly curves. A student can
see that before writing any control code. The independent rear wheels and the
active-differential idea pair naturally with it.

See [`AVC_Motor_Encoder_QDC_Research.md`](AVC_Motor_Encoder_QDC_Research.md), especially
the student-facing API boundary.

Three tuning potentiometers (alpha, beta, gamma) and a center button give live, no-rebuild
adjustment — with a pots-at-midpoint safety interlock before motors will enable.

---

## 7. Constraints summary

Judge any proposed change against these:

| Constraint | Value |
|---|---|
| **Hard deadline** | Race is **late August 2026** |
| **Student time** | ~3 days, first exposure to the code |
| **Frame budget** | 41 ms / 6.25 M cycles |
| **RAM** | ~512 KB, of which 256 KB is camera ping-pong — **RAM is the scarce resource** |
| **Flash** | 2 MB, ~267 KB used — ample |
| **Frame rate** | ~24 FPS, EZH-limited |
| **Competition hardware** | Rev A; EZH default, modified-board FlexIO option proven |
| **Track** | Nominal 55 cm, built from local materials, dimensions approximate |
| **Design bar** | Tunable by a novice; fails visibly, never silently |

## 8. Open items

- **Finalize the 2026 color twist** — narrower colored lane vs. colored obstacles. The
  vision implementation branches on this.
- **Obtain the NXP Cup rules PDF** through a browser and reconcile §2 against it.
- **Resolve how the 55 cm is measured** — line-center to line-center, inner edge to inner
  edge, or outer to outer. A ±4 cm swing that matters for track construction.
- **Confirm as-built track dimensions on-site** and record them here.
- **Measure LUT timing** on EZH and FlexIO if full-frame color processing becomes useful.
  RGB565 remains the competition format; YUY2 is post-competition work.
- **Rev B decision** — routing recommended and measured; needs a build decision. Also
  decide whether the MCU-Link VCOM should stay connected to `P1_8` at all.
- **What the freed EZH is actually for** — capture no longer needs it, but nothing has
  yet demonstrated it running unrelated work alongside FlexIO capture.

## 9. Related documents

- [`neutron_npu/AVC_Neutron_NPU_Assessment.md`](neutron_npu/AVC_Neutron_NPU_Assessment.md)
  — NPU feasibility and the recommended color-processing approach
- [`AVC_Vision_Pipeline_Design.md`](AVC_Vision_Pipeline_Design.md) — LUT design, edge
  detection upgrades, and PowerQuad overlap
- [`AVC_Camera_FlexIO_Pin_Migration.md`](AVC_Camera_FlexIO_Pin_Migration.md) — proving
  FlexIO capture on the Rev A camera pins, and the Rev B routing recommendation
- [`AVC_RevB_Servo_PWM_Options.md`](AVC_RevB_Servo_PWM_Options.md) — the Rev A servo PWM
  conflict and the free PWM channel for Rev B
- [`AVC_J12_SmartDMA_Availability.md`](AVC_J12_SmartDMA_Availability.md) — which EZH/SmartDMA
  channels the unused J12 header can reach
- [`AVC_Camera_Interface_Research.md`](AVC_Camera_Interface_Research.md) — camera interface options
- [`AVC_LVDS_Adapter.md`](AVC_LVDS_Adapter.md) — camera cabling; §0.6 documents the connector-seating
  incident that motivated the SCCB robustness work
- [`AVC_Motor_Encoder_QDC_Research.md`](AVC_Motor_Encoder_QDC_Research.md) — encoder feedback for PID
- [`AVC_USB_Debug_Transport_Protocol.md`](AVC_USB_Debug_Transport_Protocol.md) — common
  session-gated USB transport used by PC and Android hosts
- [`AVC_RaceDay_Wireless_Frame_Relay.md`](AVC_RaceDay_Wireless_Frame_Relay.md) — proven
  Moto USB-host/Wi-Fi relay and remaining race-network checks
- `../plans/` — active execution plans
