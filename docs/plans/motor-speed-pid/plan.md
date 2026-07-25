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

# Closed-Loop Wheel Speed Control

## Purpose

Carried out of `motor-encoder-qdc-bringup`, which verified the encoder hardware
and stopped deliberately short of control.

**The motivating fact is measured, not assumed: M1 runs 8.8 percent faster than
M0 at identical PWM duty.** A car commanded straight in open loop will curve.
Closed-loop wheel speed control is what fixes that, and PID is being taught to
students this year for the first time.

## What is already in place

Everything the loop needs to close around exists and is hardware-verified:

| | |
|---|---|
| Per-wheel RPM and m/s | `avc__wheel_rpm()`, `avc__wheel_velocity_mps()`, positive is forward |
| Sampling | fixed 100 ms interval, independent of frame timing |
| Calibration | 1320 counts/wheel-rev, measured; 75 mm wheel |
| Polarity | corrected in hardware via QDC `CTRL[REV]` |
| Telemetry stability | sd under 0.5 percent of mean, no error flags |
| On-screen readout | both wheels, RPM and m/s, on the LCD overlay |

See `docs/research/AVC_Motor_Encoder_QDC_Research.md`.

## Things measured that the controller must respect

- **PWM duty is strongly non-linear in speed at the low end.** 10 percent duty
  gives about 28 percent of no-load speed, not 10 percent. A feed-forward term
  built on a linear duty-to-speed assumption will be wrong where it matters
  most, at low speed.
- **Noise is not constant across the speed range.** Rate standard deviation was
  2.3 counts/sec at 10 percent duty and 58 at 20 percent. Derivative gain tuned
  at one speed may misbehave at another.
- **Platform ceiling is about 0.432 m/s** at the 110 rpm no-load rating, so the
  useful control range is narrow.

## Open Questions

- Per-wheel independent loops, or one loop plus a differential correction? The
  latter maps more directly onto "drive straight" and onto the active
  differential idea for steering assistance.
- Where does the loop run - in the 41 ms frame cadence, or on its own faster
  timer? The encoder already samples at 100 ms independently of frames.
- What do students actually tune? Exposing raw PID gains to a novice in three
  days may be worse than exposing one or two shaped parameters.

## Constraints

- **Race is late August 2026.** Must not destabilise the competition image.
  Gate behind configuration as the encoder work is.
- Bench work is chassis-on-blocks with the existing 20 percent duty cap.
