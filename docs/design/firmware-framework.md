# Firmware Framework

The competition image keeps platform initialization, camera-buffer handling, display,
USB telemetry, wheel-speed sampling, and actuator safety inside the framework. The two
normally edited implementation files are:

- `src/nxp_cup/nxp_cup_core0/source/app/test_mode.c`
- `src/nxp_cup/nxp_cup_core0/source/app/race_mode.c`

Both include the single public header
`src/nxp_cup/nxp_cup_core0/source/nxp_cup.h`. They do not include board, SDK,
camera-driver, USB, display, PWM, QDC, or framework-private headers.

## Mode flow

| Mode | Entry and behavior | Actuators |
| --- | --- | --- |
| `STARTUP` | One-time board, camera, display, USB, encoder, and framework initialization | Disabled |
| `TEST` | Selected by the installed test jumper; each accepted frame calls `test_mode_on_frame()` | Disabled until EXE is released and all three pots are centered; then command lease applies |
| `RACE_WAITING` | Selected without the jumper; LCD says `RACE MODE` and `PRESS EXE TO START` after a camera frame | Disabled |
| `RACE_RUNNING` | EXE release after a valid camera frame; each accepted frame calls `race_mode_on_frame()`; another EXE release safe-stops and returns to `RACE_WAITING` | Enabled at zero for the audible armed cue, then subject to command lease and faults |
| `ENTERING_ISP` | Confirmed USB control request transfers execution to ROM ISP | Disabled first |
| `SAFE_FAULT` | Camera loss, repeated callback overrun, or unexpected bootloader return | Disabled and steering centered |

EXE toggles between race waiting and race running. Starting requires a valid camera frame.
Stopping disables both motors, centers steering, and returns to `RACE_WAITING`. The test
jumper is monitored by the framework: insertion safe-stops and enters TEST; removal
safe-stops and returns to race waiting.

## Frame contract

The callback receives one writable `uint16_t *` RGB565 frame. It is valid only until the
callback returns. Code may inspect pixels and draw temporary test overlays but must not
retain the pointer, free it, or expose it to asynchronous work. Buffer ownership and the
camera producer remain private.

The foreground consumes the latest completed frame. If another frame arrives before the
previous one is accepted, the older pending frame is replaced and the drop counter is
incremented. This bounds latency and avoids a public queue or ownership protocol. USB
publication occurs after the callback; RACE mode does not perform the full LCD frame dump.

## Public API

`nxp_cup.h` exposes only:

- RGB565 geometry, row access, RGB565/YHSV LUT conversion, and simple overlay primitives;
- normalized alpha, beta, and gamma controls plus left/right buttons;
- clamped left/right motor duty and steering position;
- left/right wheel speed in RPM or m/s, measurement age, and availability;
- milliseconds, callback time, and frame-drop count;
- named scalar or 48-byte text telemetry and bounded formatted log messages.

The framework publishes `system.mode`, `system.state`, motor enable/commands, and
steering command independently of participant callbacks. Participant telemetry cannot
suppress these protected diagnostics.

It deliberately supplies no lane-center decision, steering controller, motor PID, active
differential, completed edge detector, or race strategy.

## Safety and timing

Entering race running enables zero-duty PWM without starting a lease, providing an audible
armed cue without requesting motion. The first `motors_set_duty()` call starts a 100 ms
countdown, and every later call refreshes it. If participant code stops sending commands for
100 ms, the 10 ms tick interrupt zeros PWM even if a callback is stalled; foreground service
then disables the motor outputs. This lease is a dead-man timer for commanded motion, not a
race-duration timer. Three consecutive callbacks over the 41 ms frame budget enter
`SAFE_FAULT`. In `RACE_RUNNING`, 250 ms without a camera frame also enters the fault state.

These paths compile in both CMake and a fresh MCUXpresso headless workspace. Physical
stall, TEST/RACE, camera-loss, and actuator tests remain part of the next Rev A bench
regression because the board is currently in transit.
