# FRDM-MCXN947 J12 — SmartDMA/EZH Pin Availability

**Question:** `J12` on the FRDM-MCXN947 was used for the earlier FlexIO camera test and is
now unused. Are any EZH/SmartDMA I/O pins available there?

**Answer: yes, but only six distinct channels, and they cannot form an 8-bit bus.**
The best contiguous run is **4 bits (`PIO28..PIO31`) on J12.13–16**.

**Date:** 2026-07-25. Desk research against the FRDM netlist and the MCXN947VDF signal
data. Nothing built or tested.

**Context:** relevant because moving camera capture to FlexIO frees the EZH
([`AVC_Camera_FlexIO_Pin_Migration.md`](AVC_Camera_FlexIO_Pin_Migration.md)), and the
question becomes what the freed EZH can usefully reach.

---

## Result

`J12` is a 2×14 header with 26 signal pins (plus 3V3 and GND). **Nine carry a
`SMARTDMA_PIO` function**, but they collapse to **six distinct channels** because three
appear twice:

| Channel | Pin option A | Pin option B | Note |
|---|---|---|---|
| `PIO24` | `P4_0` — J12.4 | — | firmware uses as `CAM_DBG` |
| `PIO25` | `P4_1` — J12.3 | — | firmware uses as `FRAME_DBG` |
| `PIO28` | `P2_8` — J12.13 | — | free |
| `PIO29` | `P2_9` — J12.14 | `P4_5` — J12.6 | free, **duplicated** |
| `PIO30` | `P2_11` — J12.16 | `P4_6` — J12.5 | free, **duplicated** |
| `PIO31` | `P2_10` — J12.15 | `P4_7` — J12.7 | free, **duplicated** |

**`PIO26` and `PIO27` are the gap.** They exist only on:

- `P2_6` / `P2_7` — committed to H-bridge PWM (`PWM1_A0` / `PWM1_B0`)
- `P3_10` / `P3_11` — committed to `FLEXSPI0` external flash, reaching no connector

So **an 8-bit contiguous SmartDMA group cannot be assembled on J12**, and not anywhere
else on this board either without giving up motor drive.

## The usable group

**`PIO28..PIO31` — a contiguous 4-bit nibble on four adjacent header pins.**

| J12 pin | MCU | SmartDMA | FlexIO |
|---|---|---|---|
| 13 | `P2_8` | `PIO28` | `FLEXIO0_D16` |
| 14 | `P2_9` | `PIO29` | `FLEXIO0_D17` |
| 15 | `P2_10` | **`PIO31`** | `FLEXIO0_D18` |
| 16 | `P2_11` | **`PIO30`** | `FLEXIO0_D19` |

> ⚠️ **Header order does not match channel order.** J12.15 is `PIO31` and J12.16 is
> `PIO30` — the last two are swapped relative to the pin sequence. Harmless if you know,
> confusing if you assume. Note the FlexIO numbering on the same pins *is* sequential,
> which makes the discrepancy easy to miss.

`PIO24`/`PIO25` are also on J12 (pins 4 and 3) but are separated from the nibble by the
26/27 gap, and both are currently driven as debug outputs by firmware
(`BOARD_LCDPINS_CAM_DBG` / `FRAME_DBG` on `P4_0` / `P4_1`).

## What is not on J12

The other seventeen signal pins carry no `SMARTDMA_PIO` function at all:

- **`P4_12..P4_23`** (J12.17–28) — `FLEXIO0_D20..D31` only. This is the pin group the
  earlier FlexIO camera test used, and it is FlexIO-capable but **not** SmartDMA-capable.
- **`P0_7`, `P0_8`, `P0_9`, `P0_12`, `P0_13`** (J12.8–12) — the FXIO LCD control signals;
  some carry low FlexIO channels (`D0`, `D1`, `D4`, `D5`), none carry SmartDMA.

That asymmetry is worth internalising: **`P4_12..P4_23` is FlexIO-only, and the SmartDMA
channels on Port 4 are `P4_0..P4_7`**, which are mostly elsewhere on the board.

## AVC shield interaction

Of the J12 SmartDMA pins, the shield only touches `P4_0` and `P4_1`, as pass-throughs on
`J2.18` / `J2.20`. **`P4_5`, `P4_6`, `P4_7`, and `P2_8..P2_11` are not on the shield at
all** — they are FRDM-only and reachable solely at J12.

That is convenient: the whole `PIO28..PIO31` nibble is free of shield entanglement, so
using it costs nothing on the AVC board.

## What this is good for

Six channels, four of them contiguous, on a header with nothing else on it. Reasonable
uses for a freed EZH:

- **A 4-bit parallel bus** — enough for a nibble-wide interface, or a bit-banged protocol
  with clock and data.
- **Several independent fast I/O lines** — the EZH is an I/O coprocessor, and driving or
  sampling a handful of signals deterministically without CPU involvement is its natural
  job.
- **Not a second camera port.** An 8-bit DVP bus is out of reach here.

Remember the EZH's existing claim: the camera program currently uses `PIO0..PIO7`
(`P1_4..P1_11`) plus `PIO13` (`P1_17`). Those free up if capture moves to FlexIO, and they
*are* a contiguous 8-bit group — so **if the EZH needs a wide bus, the pins it already has
are better than anything J12 offers.**

## Method

- J12 pinout from `docs/FRDM-MCXN947/.../design_review/design/SCH-90818_A_design.json`.
- `SMARTDMA_PIO` and `FLEXIO0_D` functions from
  `docs/research/MCXN947/json/ksdk2_0/MCXN947VDF/signal_configuration.json`, parsed from
  the per-pin mux name lists.
- Shield claims from `frdm-avc/tracks/A/output/design_review/design/`.

## Caveats

- Availability here is **pin-level only**. Whether the EZH's programming model can use an
  arbitrary `PIO` bit range depends on the `bunny_build` program, which currently targets
  `PIO0..PIO7`. A different bit range is a software question this document does not answer.
- `P4_0` / `P4_1` are usable only if the `CAM_DBG` / `FRAME_DBG` debug outputs are given up
  or moved.
