# Rev B — Servo PWM Pin Options

**Question:** Rev A intended `P2_2/PWM1_A2` as the main servo PWM output, but `P2_2` is
also `CAMERA_CLKIN`, so the aux `P2_3/PWM1_B2` was used instead. Is there another PWM
available for Rev B, given four channels are committed to the H-bridges?

**Answer: yes — `PWM1` submodule 3 is completely unused, and `P3_20/PWM1_A3` already
reaches the shield on `J1.5` with nothing connected to it.**

**Date:** 2026-07-25. Desk research against both netlists and the NXP signal data.

---

## Result

| Option | Pin | Channel | Where | Verdict |
|---|---|---|---|---|
| **Recommended** | **`P3_20`** | **`PWM1_A3`** | shield `J1.5`, net `P3-20/SAI1_TXD0`, **unconnected** | **no rework, no conflict** |
| Alternate | `P2_1` | `PWM1_B3` | FRDM `J7.9` | free, but check shield header mapping |
| Keep | `P2_3` | `PWM1_B2` | shield `J3.5` → `U10` | works today; nothing wrong with it |
| Avoid | `P2_2` | `PWM1_A2` | shield `J3.7` → `U9` | the Rev A conflict |
| Needs rework | `P3_16` | `PWM1_A2` | FRDM `SJ11` | solder jumper per board — bad for kits |

---

## 1. Why Rev A collided

`P2_2` carries **two nets** on the FRDM board, joined by a series resistor:

```
P2_2/CAMERA_CLKIN   J11.16, R168.1
P2_2                J3.7, J7.8, R168.2, SJ7.3, TP5.1, U1.H3
```

So one MCU pin lands on **both** the camera header (`J11.16` → shield `J9.16`, driving
OV5640 `XCLK` from `CLKOUT`) **and** the Arduino-style header (`J3.7` → shield `J3.7` →
buffer `U9`, the intended servo output).

That is the "ports show up twice" problem exactly. `CLKOUT` and `PWM1_A2` cannot both own
the pin, and the camera needs `XCLK`, so the servo lost.

**`R168` is the decoupling point** if you ever wanted `P2_2` back for PWM — but the camera
still needs `XCLK` from somewhere, so this only moves the problem.

## 2. What is actually committed

Both servo buffers exist on Rev A — `U9` and `U10`, `SN74AHCT1G125DBVR` non-inverting
buffers to +5V — so the board was built expecting either to work.

| PWM1 submodule | Channel | Rev A pin | Use |
|---|---|---|---|
| 0 | `A0` | `P2_6` | H-bridge (`J3.15` → `R13`) |
| 0 | `B0` | `P2_7` | H-bridge (`J3.13` → `R14`) |
| 1 | `A1` | `P2_4` | H-bridge (`J3.11` → `R21`) |
| 1 | `B1` | `P2_5` | H-bridge (`J3.9` → `R22`) |
| 2 | `A2` | `P2_2` | **intended servo — lost to `CAMERA_CLKIN`** (`J3.7` → `U9`) |
| 2 | `B2` | `P2_3` | **servo as built** (`J3.5` → `U10`) |
| **3** | **`A3`** | — | **unused** |
| **3** | **`B3`** | — | **unused** |

**An entire PWM submodule is free.** The four H-bridge channels occupy submodules 0 and 1;
the servo sits on half of submodule 2. Submodule 3 was never touched.

## 3. Every PWM1 channel has two pin choices

From the MCXN947VDF signal data — this is the fact that would have avoided the Rev A
mistake:

| Channel | Pin option A | Pin option B |
|---|---|---|
| `PWM1_A0` | `P2_6` | `P3_12` |
| `PWM1_A1` | `P2_4` | `P3_14` |
| `PWM1_A2` | `P2_2` | **`P3_16`** |
| `PWM1_A3` | `P2_0` | **`P3_20`** |
| `PWM1_B0` | `P2_7` | `P3_13` |
| `PWM1_B1` | `P2_5` | `P3_15` |
| `PWM1_B2` | `P2_3` | `P3_17` |
| `PWM1_B3` | `P2_1` | `P3_21` |
| `PWM1_X0..X3` | `P2_8..P2_11` | `P3_18..P3_23` |

`PWM0_A0..B3` and `PWM0_X0..X3` exist too, but **only on `P3_0..P3_11`** — see §5 for why
those are unavailable.

**`PWM1_A2` was also available on `P3_16`.** Routing the intended servo signal there would
have avoided the camera entirely. (`P3_16` is not header-accessible without rework — see
§5 — but it shows the constraint was pin choice, not silicon.)

## 4. Recommendation for Rev B

**Route the servo to `P3_20` / `PWM1_A3`.**

- It already reaches the shield at **`J1.5`**, on net `P3-20/SAI1_TXD0`, whose only
  terminals are `J1.5` and `J1_EXT.5` — **nothing is connected to it.**
- On the FRDM board `P3_20` reaches `J1.5` and `J13.6` with no series parts and no second
  net, so there is no repeat of the `P2_2` double-landing.
- It is a fresh PWM submodule, so it cannot interact with H-bridge timing, complementary
  pairing, or fault handling on submodules 0 and 1.
- **No board rework and no solder-jumper change** — this matters for a kit that students
  receive assembled.

Feed it through a `SN74AHCT1G125` buffer to +5V as `U9`/`U10` already do.

**Alternate: `P2_1` / `PWM1_B3`**, at FRDM `J7.9`. Free on the FRDM side, but the shield's
`J7.9` is `P0-25/FC1-SCK`, so shield and FRDM connector numbering differ there — **confirm
the physical mapping before relying on it.** `P3_20` needs no such check because shield
`J1.5` and FRDM `J1.5` agree.

### What to do with the old pins

- **Keep `P2_3`/`PWM1_B2`** wired to `U10`. It works, it costs nothing to leave, and it is
  a ready fallback.
- **Drop the `P2_2` → `U9` connection**, or repurpose `U9` for the new pin. Leaving a
  buffer driven by a pin that must stay `CLKOUT` is a trap for the next person.

## 5. Why the `P3_0..P3_16` PWM pins are not options

`PWM0` is attractive on paper — twelve channels, entirely unused — but on the FRDM board:

- **`P3_0` through `P3_15` are all committed to `FLEXSPI0`** (the on-board external flash,
  `U7`/`U8`). They have test points but **reach no connector.**
- **`P3_16`** goes only to `SJ11`, a three-way solder jumper selecting whether `J1.1`
  connects to `P3_16` or `P4_5`. Usable only by reworking a solder jumper **on every
  board**, which is unacceptable for student kits.
- `P3_17`, `P3_19`, `P3_21` are the centre, right, and left buttons.
- `P3_2`/`P3_3` are the SCCB `I2C`, and `P3_4`/`P3_5` are camera `D4`/`D5`.

So despite `PWM0` looking free, **`PWM1` submodule 3 is the only clean path.**

## 6. Interaction with the FlexIO camera migration

If Rev B also adopts the FlexIO camera routing
([`AVC_Camera_FlexIO_Pin_Migration.md`](AVC_Camera_FlexIO_Pin_Migration.md)), camera
`D4`/`D5` move off `P3_4`/`P3_5` onto `P1_8`/`P1_9`.

That frees **`P3_4`/`P3_5` = `PWM0_X2`/`PWM0_X3`** — but they are still only useful if the
shield routes them, and `PWM0_X` channels are submodule outputs rather than a
complementary `A`/`B` pair. Worth knowing, not worth depending on.

**More usefully: the two changes do not conflict.** The FlexIO migration touches
`P1_8`, `P1_9`, `P1_14`; this touches `P3_20`. They can be adopted independently or
together.

## 7. Open items

- Confirm on the physical board that shield `J1.5` is genuinely spare and mechanically
  reachable for the servo connector's placement.
- Decide whether `U9` is repurposed for `P3_20` or removed.
- Confirm `P3_20` has no default pull or boot-strap behaviour that would glitch the servo
  at reset — a servo twitching on power-up is worth avoiding on a car that students
  handle.
- If `P2_1`/`PWM1_B3` is preferred instead, map shield `J7` to the FRDM connector properly
  first.
