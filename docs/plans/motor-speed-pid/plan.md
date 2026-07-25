+++
type = "plan"
id = "motor-speed-pid"
status = "pending"
created = "2026-07-25"

[[steps]]
id = "work"
title = "Execute plan work"
status = "pending"

[[steps]]
id = "design-doc-intent-audit"
title = "Audit design docs, ADRs, and requirements against implementation"
status = "pending"
depends_on = ["work"]

[[steps]]
id = "test-runtime-impact-audit"
title = "Audit new test runtime impact"
status = "pending"
depends_on = ["work"]

[[steps]]
id = "external-review"
title = "Obtain independent external review"
status = "pending"
depends_on = ["work", "design-doc-intent-audit", "test-runtime-impact-audit"]

[[exit_criteria]]
id = "signoff"
title = "Focused signoff passes"
status = "pending"

[[exit_criteria]]
id = "design-doc-intent-audit"
title = "Design docs, ADRs, and requirements match implementation"
status = "pending"

[[exit_criteria]]
id = "test-runtime-impact-audit"
title = "New tests are listed and runtime impact is reviewed"
status = "pending"

[[exit_criteria]]
id = "external-review"
title = "Independent external review is complete"
status = "pending"
+++

# Wheel Speed Control — Student Exercise Scaffolding

## Purpose

**PID is a student exercise. We are not writing the controller.**

This plan builds the *scaffolding* students write their control law into, in the
same spirit as `avc__line_processor.c` for vision: a clear place to put the
algorithm, feedback to close around, live tuning, visible telemetry, and safety
rails — with the interesting part deliberately left undone.

Carried out of `motor-encoder-qdc-bringup`, which verified the encoder hardware
and stopped short of control on purpose.

## Why the exercise motivates itself

**M1 runs 8.8 percent faster than M0 at identical PWM duty** — measured, well
outside noise. A car commanded straight in open loop visibly curves.

That is the whole lesson in one observable fact. A student can drive the car
straight, watch it veer, look at two different numbers on the screen, and
understand why feedback exists — before writing a line of control code. It is
worth building the exercise so that this is the first thing they see.

## What already exists

Hardware-verified and calibrated. See
`docs/research/AVC_Motor_Encoder_QDC_Research.md`.

| | |
|---|---|
| Per-wheel feedback | `avc__wheel_rpm()`, `avc__wheel_velocity_mps()`, positive is forward |
| Sampling | fixed 100 ms, independent of frame timing |
| Calibration | 1320 counts/wheel-rev measured, 75 mm wheel |
| Polarity | corrected in hardware, forward reads positive |
| Stability | sd under 0.5 percent of mean, no error flags |
| On-screen | both wheels, RPM and m/s |

## What the exercise needs

### A place to write the control law

A `avc__speed_controller.c` with a clearly marked function taking setpoint and
measurement per wheel and returning duty, called at a fixed rate, with the body
left as the exercise. Mirror the conventions of `avc__line_processor.c` — that
is the file students already learn to read.

### Live tuning without a rebuild

**The three pots are the obvious answer: alpha, beta, gamma as Kp, Ki, Kd.**
A student in a three-day event cannot afford a build-flash cycle per gain
change. Tuning a live loop with a physical knob and watching the wheel respond
is also a far better lesson than editing a constant.

This conflicts with the pots' current use for manual motor and servo control in
test mode, so the modes need separating.

### Telemetry that makes the loop visible

Setpoint against actual, per wheel, and the error. On screen at minimum. A
student who cannot see the error cannot tune the loop — and overshoot, steady
state error, and oscillation all need to be *visible* to be taught.

The USB debug stream and the Web Serial page are the natural home for a proper
scrolling plot, which teaches step response far better than a number.

### Safety rails

The bench diagnostic caps duty at 20 percent via `#error`. A student loop with
a large gain will command whatever it likes, so the output stage needs its own
clamp, plus a stop condition. Chassis-on-blocks stays the working mode.

## Things the measurements say the exercise must respect

- **PWM duty is strongly non-linear in speed at the low end** — 10 percent duty
  gives about 28 percent of no-load speed. A feed-forward term built on a linear
  duty-to-speed assumption is wrong exactly where it matters most. Worth
  deciding whether students should discover this or be told.
- **Noise varies with speed** — rate sd was 2.3 counts/sec at 10 percent duty
  and 58 at 20 percent. Derivative gain tuned at one speed may misbehave at
  another. This is a genuine teaching moment about D terms and noise.
- **The platform ceiling is about 0.432 m/s**, so the useful control range is
  narrow.

## Open Questions

- **Per-wheel loops, or one loop plus a differential correction?** The latter
  maps more directly onto "drive straight" and onto the active-differential
  steering idea. It may also be the easier thing to reason about first.
- **Does the loop run in the 41 ms frame cadence or on its own timer?** The
  encoder already samples at 100 ms independently of frames. Students learning
  about deadlines benefit from the control rate being explicit rather than
  incidental.
- **How much is left blank?** Too little and it is a fill-in-the-constant
  exercise; too much and three days is not enough. The line processor is the
  precedent for how much structure to give.
- **Is a worked example needed** — a deliberately badly tuned loop that
  oscillates, as a starting point to fix?

## Constraints

- **Race is late August 2026.** Must not destabilise the competition image;
  gate behind configuration as the encoder work is.
- Bench work is chassis-on-blocks.
- The audience is novices with about three days who have never seen this code.
