# AVC Motor Encoder QDC Research

Date: 2026-07-21. **Hardware-verified 2026-07-25.**

## Result — verified on hardware

The routing below works. Both channels count true quadrature, respond
independently, and now report calibrated wheel speed.

| Fact | Value |
|---|---|
| **Counts per wheel revolution** | **1320** (measured: 13188 over exactly 10 hand turns) |
| Gear ratio | **1:30** — the vendor spec table's 1:90 is wrong |
| Wheel diameter / circumference | 75 mm / 235.6 mm |
| M0 @ 10% duty | 678 counts/s, 30.8 rpm, 0.121 m/s |
| M1 @ 10% duty | 739 counts/s, 33.6 rpm, 0.132 m/s |
| M1 @ 20% duty | 1450 counts/s, 65.9 rpm, 0.259 m/s |
| Theoretical top speed | 0.432 m/s (110 rpm no-load) |
| Rate stability | sd under 0.5% of mean, no overflow or error flags |

### Three findings that change how the platform should be driven

**1. The motors are not matched.** M1 runs **8.8% faster than M0 at identical
duty**, consistently and well outside measurement noise. A car commanded
straight in open loop will curve. This is the concrete justification for
closed-loop wheel speed control, and it is worth showing students rather than
asserting.

**2. PWM duty is strongly non-linear in speed at the low end.** At 10% duty the
wheel turns ~31 rpm, about **28%** of the 110 rpm no-load rating, not the ~11 rpm
a linear reading predicts. Estimating wheel speed from duty will mislead you —
this actually produced a wrong counts-per-revolution inference before the
measurement corrected it.

**3. Encoder polarity was inverted.** With the QDC blocks in their default
direction, driving forward produced negative counts and position counted *down*
from zero, wrapping to ~4.29 billion. Fixed in hardware via `CTRL[REV]`
(`CONFIG__MOTOR_ENCODER_INVERT_M0` / `_M1`, both default 1) so forward reads
positive and position counts up. Fixing it in the peripheral rather than
negating in software keeps the raw position register meaningful.

### Channel independence — proven, not assumed

Driving M0 at 10% and M1 at 20% simultaneously: M0 held to within **0.4%** of
its baseline while M1 nearly exactly doubled (**+96.1%**), a rate ratio of 2.130
against a PWM ratio of 2.000. Driving both at one duty could not have
distinguished correct wiring from cross-wiring, or from two channels reading the
same source.

### Wheel-speed API

Available in the normal build when `CONFIG__MOTOR_ENCODER_BACKEND` selects QDC.
Sampled on a fixed 100 ms interval so the rate is measured over a known window
regardless of frame timing.

```c
float avc__wheel_rpm(avc_motor_encoder_id_t wheel);          /* + is forward */
float avc__wheel_velocity_mps(avc_motor_encoder_id_t wheel); /* + is forward */
```

### Student-facing API boundary

The organizer-supplied framework stops at a clean feedback and actuator boundary.
Students implement their own speed-control law. Framework/API cleanup may improve the
names, comments, timing contract, safety clamps, and telemetry around these functions,
but must not add a completed PID controller.

The intended teaching surface provides:

- independent left/right wheel speed with explicit units and positive-forward polarity;
- motor and servo commands with documented ranges and framework-owned safety limits;
- an explicit update cadence rather than control timing hidden in the camera frame loop;
- named USB telemetry so setpoint, measurement, output, and student-computed error can
  be plotted without placing the control law in platform code.

Potentiometers or host commands may later expose live tuning values, but mode ownership
must be explicit because the pots are also used by the existing local test mode. Any
motors-on teaching diagnostic remains a chassis-on-blocks activity.

Both wheels are shown on the LCD overlay. Per-wheel rather than combined,
deliberately — a single figure hides the mismatch above.

## Scope

Research the Rev A motor encoder wiring and the MCXN947 hardware path needed to
use the on-chip quadrature decoder blocks for per-motor speed feedback. Closed-loop
control is a student exercise; the platform supplies measurements, safe actuator APIs,
and diagnostics rather than a completed controller.

Primary local inputs:

- `D:\prj\wavenumber\avc\frdm-avc\tracks\A\output\design_review`
- `src/avc/avc_core0` device headers, drivers, pin mux, and motor-control code
- `docs/research/MCXN947/extracted/MCXNx4xRM`
- `docs/research/MCXN947/flexio_pin_candidates`

## Current Conclusion

The Rev A encoder pins can plausibly use real MCXN947 QDC hardware, but not by
routing arbitrary GPIO directly to QDC. The working route is:

1. Configure each encoder pin to its `ALT1` `TRIG_INx` function.
2. Use INPUTMUX to select those `TRIG_INx` signals as QDC phase inputs.
3. Initialize QDC0 and QDC1 as independent quadrature decoders.

This is stronger than a software edge-counting fallback, and it should be cheap
enough for future closed-loop motor speed work.

**Confirmed 2026-07-25.** This exact route works — see the verified result at the
top of this document.

## Rev A Connector Wiring

The design-review JSON reports both motor connectors as
`S6B-PH-SM4-TBT(LF)(SN)` six-pin JST connectors.

| Connector | Pin | Net | Role |
| --- | ---: | --- | --- |
| J17 | 1 | `M0-` | Motor 0 drive |
| J17 | 2 | `GND` | Encoder ground/reference |
| J17 | 3 | `P1-0/TSI0_CH0` | Encoder signal, proposed Phase A |
| J17 | 4 | `P1-1/TSI0_CH1` | Encoder signal, proposed Phase B |
| J17 | 5 | `+3v3` | Encoder power |
| J17 | 6 | `M0+` | Motor 0 drive |
| J11 | 1 | `M1-` | Motor 1 drive |
| J11 | 2 | `GND` | Encoder ground/reference |
| J11 | 3 | `P2-0/ENC_B` | Encoder Phase B |
| J11 | 4 | `P1-22/ENC_A` | Encoder Phase A |
| J11 | 5 | `+3v3` | Encoder power |
| J11 | 6 | `M1+` | Motor 1 drive |

## MCXN947 QDC Facts

Local device support shows two QDC instances:

- `FSL_FEATURE_SOC_QDC_COUNT (2)`
- QDC0/QDC1 clocks exist as `kCLOCK_Qdc0` and `kCLOCK_Qdc1`
- QDC0/QDC1 base pointers and IRQ vectors are present
- The device has QDC `CTRL3`, `LASTEDGE`, `POSDPER`, and filter prescaler
  features

The local INPUTMUX headers expose QDC phase destinations such as:

- `kINPUTMUX_TrigIn0ToQdc0Phasea`
- `kINPUTMUX_TrigIn1ToQdc0Phaseb`
- `kINPUTMUX_TrigIn3ToQdc1Phasea`
- `kINPUTMUX_TrigIn5ToQdc1Phaseb`

The important limitation is that QDC phase input selections include PINT,
SCT/timer/PWM/EVTG sources, and `TRIG_IN0..9`; they do not directly enumerate
all GPIO pins. That makes the pin alternate function selection critical.

## Pin-To-QDC Mapping

The MCXN947 signal mux table gives these alternate functions:

| Connector | MCU Pin | Pin ALT1 | Proposed QDC route |
| --- | --- | --- | --- |
| J17 pin 3 | `P1_0` | `TRIG_IN0` | `QDC0_PHASEA` |
| J17 pin 4 | `P1_1` | `TRIG_IN1` | `QDC0_PHASEB` |
| J11 pin 4 | `P1_22` | `TRIG_IN3` | `QDC1_PHASEA` |
| J11 pin 3 | `P2_0` | `TRIG_IN5` | `QDC1_PHASEB` |

Representative setup shape:

```c
PORT_SetPinMux(PORT1, 0U, kPORT_MuxAlt1);   /* P1_0  -> TRIG_IN0 */
PORT_SetPinMux(PORT1, 1U, kPORT_MuxAlt1);   /* P1_1  -> TRIG_IN1 */
PORT_SetPinMux(PORT1, 22U, kPORT_MuxAlt1);  /* P1_22 -> TRIG_IN3 */
PORT_SetPinMux(PORT2, 0U, kPORT_MuxAlt1);   /* P2_0  -> TRIG_IN5 */

INPUTMUX_Init(INPUTMUX);
INPUTMUX_AttachSignal(INPUTMUX, 0U, kINPUTMUX_TrigIn0ToQdc0Phasea);
INPUTMUX_AttachSignal(INPUTMUX, 0U, kINPUTMUX_TrigIn1ToQdc0Phaseb);
INPUTMUX_AttachSignal(INPUTMUX, 0U, kINPUTMUX_TrigIn3ToQdc1Phasea);
INPUTMUX_AttachSignal(INPUTMUX, 0U, kINPUTMUX_TrigIn5ToQdc1Phaseb);
```

The phase assignment is a starting point. Direction sign can be fixed after
bench observation by swapping A/B in INPUTMUX or setting the QDC reverse
direction control.

## NXP SDK Sanity Check

NXP's current MCUXpresso SDK GitHub delivery is release `26.06.00` as of
2026-07-21. The upstream `mcuxsdk-core` repository includes a QDC driver
(`drivers/qdc/fsl_qdc.h`, driver version 2.0.1), but this AVC project does not
currently include `fsl_qdc.c` or `fsl_qdc.h`.

NXP's FRDM-MCXN947 QDC basic and index-interrupt example board notes route:

- `J3.3(P1_22)` -> `kINPUTMUX_TrigIn3ToQdc0Phasea`
- `J3.1(P2_0)` -> `kINPUTMUX_TrigIn5ToQdc0Phaseb`

That directly validates the J11 encoder pair route. The J17 pair uses the same
mechanism with `TRIG_IN0` and `TRIG_IN1`.

Reference links:

- <https://www.nxp.com/design/design-center/software/development-software/mcuxpresso-software-and-tools-/mcuxpresso-software-development-kit-sdk%3AMCUXpresso-SDK>
- <https://github.com/nxp-mcuxpresso/mcuxsdk-core/tree/main/drivers/qdc>
- <https://raw.githubusercontent.com/nxp-mcuxpresso/mcuxsdk-examples/main/_boards/frdmmcxn947/driver_examples/qdc/basic/example_board_readme.md>

## Firmware Conflicts And Remaining Risks

- `P2_0` is also named `EZH_LCD_WR`; `P1_22` is also named `EZH_LCD_DC`.
  Encoder mode and alternate parallel LCD/EZH display experiments must be
  mutually exclusive at build/profile level.
- Old SPI LCD and USB CDC display streaming do not need `P2_0`/`P1_22`, so they
  remain compatible with encoder work.
- `P1_0` and `P1_1` are currently not used by active AVC firmware, but they are
  also PMOD/MikroE/TSI-labelled pins. Do not enable TSI on these pins in an
  encoder build.
- Existing motor PWM uses PWM1 submodules 0 and 1 and muxes outputs on
  `PORT2_3..PORT2_7`. That does not collide with the encoder phase pins, but
  `P2_0` also has a `PWM1_A3` alternate function and should stay reserved for
  `TRIG_IN5` in encoder builds.
- Bench testing confirmed clean 3.3 V quadrature signals with the implemented
  QDC path. Keep filter changes evidence-driven; do not retune them during race
  week without a captured signal-quality problem.
- The measured conversion is 1320 counts per wheel revolution. The framework
  exposes that feedback but deliberately does not provide a finished PID
  controller; control design and tuning remain student work.

## Historical Bring-Up Sequence

The hardware bring-up below is complete and retained as durable rationale for the
implemented path. It is not an open PID implementation plan.

1. Add an explicit motor-encoder configuration profile:
   `CONFIG__MOTOR_ENCODER_BACKEND_DISABLED`,
   `CONFIG__MOTOR_ENCODER_BACKEND_GPIO_DIAG`, and
   `CONFIG__MOTOR_ENCODER_BACKEND_QDC`.
2. Add a pin ownership guard so `QDC` encoder mode cannot be built with the
   alternate parallel LCD/EZH pin mode that drives `P2_0` or `P1_22`.
3. Decide whether to import NXP `fsl_qdc` or implement a narrow register-level
   `avc__motor_encoder_qdc` module. Importing the SDK driver is lower risk for
   feature use; the thin module is smaller and easier to audit.
4. First bench test without motor drive: spin each wheel by hand and report raw
   QDC signal monitor bits, position, direction flag, and delta counts over RTT
   and/or USB debug stream.
5. Next test with the chassis on blocks: enable a low-speed motor diagnostic
   loop with conservative PWM limits, run each motor independently, and publish
   per-motor counts per control interval.
6. Once counts-per-revolution, gear ratio, and wheel diameter/circumference are
   measured or confirmed, convert QDC rate to wheel RPM and vehicle speed.
7. Keep the framework at the feedback/actuator boundary. Students may add fixed-rate
   closed-loop control in their algorithm code, using generic telemetry for tuning.

## EZH/FlexIO Camera Carry-Forward Note

The current camera backend remains compile-time selectable through
`CONFIG__CAMERA_CAPTURE_BACKEND`. The Rev A EZH camera path is restored as the
default with `CAMERA_CAPTURE_BACKEND_SMARTDMA_EZH`; FlexIO capture remains
available as an explicit override for future experiments. The display default
is the original ER-TFT020-3 SPI panel, with TE and parallel bit-bang tests
disabled unless explicitly selected.
