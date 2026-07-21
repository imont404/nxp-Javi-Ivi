# AVC Motor Encoder QDC Research

Date: 2026-07-21

## Scope

Research the Rev A motor encoder wiring and the MCXN947 hardware path needed to
use the on-chip quadrature decoder blocks for future per-motor speed feedback
and PID speed control.

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

## Firmware Conflicts And Risks

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
- Need to confirm encoder electrical behavior on the bench: output type,
  voltage level, idle state, pull-ups/pull-downs, and noise. The QDC input
  filter should be enabled only after seeing the real signal quality.
- Counts-per-revolution and gear ratio are still unknown. PID work should not
  start until those are measured or specified.

## Bring-Up Recommendation

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
7. Only after speed telemetry is stable, add fixed-rate PID speed control and
   keep tunables/reporting on the existing debug transport path.

## EZH/FlexIO Camera Carry-Forward Note

The current camera backend remains compile-time selectable through
`CONFIG__CAMERA_CAPTURE_BACKEND`. The Rev A EZH camera path is restored as the
default with `CAMERA_CAPTURE_BACKEND_SMARTDMA_EZH`; FlexIO capture remains
available as an explicit override for future experiments. The display default
is the original ER-TFT020-3 SPI panel, with TE and parallel bit-bang tests
disabled unless explicitly selected.
