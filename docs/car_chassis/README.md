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

## ⚠️ Encoder counts per wheel revolution — vendor contradicts itself

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

**Do not hard-code either value.** Measure it on the actual car: rotate a wheel a known
number of turns and read the QDC delta. Both numbers are plausible until then.

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
