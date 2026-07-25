# Hiwonder Ackermann Chassis — Archived Vendor Page and Extracted Specs

Static dump of the vendor product page, kept because vendor pages rot and this one is now
load-bearing — it is the only source for the encoder and steering numbers the firmware
depends on.

- **Product:** Hiwonder Ackermann Intelligent Car — metal chassis, dual encoder motors,
  front-wheel servo steering
- **Live URLs (may rot):**
  `https://www.hiwonder.com/products/ackermann-steering-chassis`,
  docs `https://docs.hiwonder.com/projects/Ackermann-Chassis/en/latest/`
- **Archived:** page dump in `web/`, dimension drawing in `dimensions.png`

**The specs below are extracted from that dump so nobody has to open it.** If they
disagree with the HTML, the HTML wins — re-extract and fix this file.

---

## Chassis

| Property | Value |
|---|---|
| Length | ~270 mm |
| Width | ~197 mm |
| Ground to top mounting plate | 75 mm (= wheel diameter) |
| Steering | Ackermann, front |
| **Maximum steering angle** | **40°** |
| **Minimum turning radius** | **0.173 m** (vendor figure, at the 40° limit) |
| Drive | Independently driven rear wheels |
| Material | 5052 aluminum alloy, anodized |
| Suspension | None |

## Rear drive motors — `JGB37-520R90-12`

| Property | Value |
|---|---|
| Rated voltage | 12 V |
| Rated power | ~8.3 W |
| Type | Permanent magnet brushed |
| No-load speed | 110 rpm |
| Rated speed | 85 rpm |
| Rated current | 0.36 A |
| Stall current | 3.2 A |
| Stall torque | 15 kg·cm |
| Locked torque | 2.6 kg·cm |
| Shaft | 6 mm, D-shaped |
| Connector | PH2.0 6-pin |
| Encoder | AB dual-phase, 11 magnetic poles / lines |

## Steering servo — `LD-1501MG`

| Property | Value |
|---|---|
| Working voltage | 6–8.4 V |
| Control | PWM pulse-width |
| Precision | 0.3° |
| Stall current | 2.4–3 A |
| Weight | 61 g |
| Size | 40 × 20 × 40.5 mm |

---

## ✅ Encoder counts per wheel revolution — MEASURED: 1320

This feeds `CONFIG__MOTOR_ENCODER_COUNTS_PER_WHEEL_REV`, currently `0`, and the
`wheel-geometry-calibration` step of `motor-encoder-qdc-bringup`.

```
counts/wheel-rev = gear_ratio × 11 lines × 4 (quadrature decoding)
```

| Source (same product page) | Gear ratio | Implied counts/rev |
|---|---|---|
| Spec table | **1:90** | 3960 |
| Support answer in the Q&A | **1:30** | **1320** |

A customer measured **1320** over ten hand rotations, on both motors independently, and
Hiwonder support confirmed that arithmetic in writing — explaining that the common mistake
is forgetting the 4× quadrature multiplier. That is decent evidence for 1:30.

**But the spec table on the same page says 1:90, and the quoted output speeds (110 rpm
no-load from a motor of this class) fit the higher reduction better.** The contradiction is
unresolved.

### Measured 2026-07-25 — the support answer is right

A wheel was marked and turned **exactly 10 revolutions** by hand with motors off,
reading the QDC position delta:

```
13188 counts / 10 revolutions = 1318.8 per revolution
```

**1320 to within 0.09 percent.** The 12-count shortfall is not landing exactly back on the
mark. The other encoder read exactly zero throughout, confirming channel independence.

**The spec table's 1:90 is wrong; the support answer's 1:30 is correct.**
`CONFIG__MOTOR_ENCODER_COUNTS_PER_WHEEL_REV` is set to **1320**.

> **Do not estimate wheel speed from PWM duty.** A rated-speed argument initially pointed
> the wrong way: 110 rpm no-load suggested ~11 rpm at 10 percent duty, which would have
> implied 3960 counts. The wheel actually turns about **31 rpm at 10 percent duty**, some
> 28 percent of no-load. Duty is strongly non-linear in speed at the low end.

### Derived platform figures

Wheel diameter 75 mm, circumference 235.6 mm.

| | counts/sec | rpm | ground speed |
|---|---|---|---|
| 10% duty | ~678 (M0) / 739 (M1) | 30.8 / 33.6 | 0.121 / 0.132 m/s |
| 20% duty | ~1450 | 65.9 | 0.259 m/s |
| no-load rating | — | 110 | **0.432 m/s** |

The motors are **not matched**: M1 runs 8.8 percent faster than M0 at identical duty, so a
car commanded straight in open loop will curve.

## Vendor support links

Collected from the page Q&A; all are Google Drive folders and may rot:

- Tutorials / wiring diagrams: `https://drive.google.com/drive/folders/1Jcs9PcTw3cJdfZzUECddxVQQZ6stYKqJ`
- Steering angle calculation: same folder as above
- LD-1501MG servo: `https://drive.google.com/drive/folders/15HFyiXPlgDWr1AvTZWU-2RNtiiOSZnlk`
- Power/wiring diagrams: `https://drive.google.com/drive/folders/1O4BKZvg1j5O4--Iso-XlFvE_lz6UkY1M`

Vendor-provided example code covers STM32, Arduino, and Raspberry Pi only — nothing for
MCXN947, which is expected.

## Related

- [`../research/AVC_Competition_Overview.md`](../research/AVC_Competition_Overview.md) §3 — vehicle section
- [`../research/AVC_Motor_Encoder_QDC_Research.md`](../research/AVC_Motor_Encoder_QDC_Research.md) — QDC routing
- [`../plans/motor-encoder-qdc-bringup/`](../plans/motor-encoder-qdc-bringup/) — active plan
