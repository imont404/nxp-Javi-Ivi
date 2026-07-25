# Camera Capture Backend — Can FlexIO Use the Existing EZH Pins?

**Question:** Rev A is wired for the EZH/SmartDMA camera backend. EZH caps PCLK and
consumes core1's usable bandwidth (shared code bus), and the EZH may be wanted for other
work. **Do the pins the camera is already routed to also carry FlexIO — so the backend
becomes a software choice rather than a board respin?**

**Answer: yes, with three jumper wires.** After them, `P1_4..P1_11` is *simultaneously*
contiguous `FLEXIO0_D12..D19` and contiguous `SMARTDMA_PIO0..PIO7` — EZH vs FlexIO
becomes **alt7 vs alt6** on identical wiring.

**Date:** 2026-07-25. Desk research against netlists and NXP signal data. Not yet built.

---

## 1. Silicon: every SmartDMA pin also carries FlexIO

From the authoritative NXP signal data for **MCXN947VDF** (184-pin VFBGA — the FRDM part),
**56 pins expose both**, consistently:

- **alt7 = `SMARTDMA_PIOn`**
- **alt6 = `FLEXIO0_Dn`**

Source: `MCXN947/json/ksdk2_0/MCXN947VDF/signal_configuration.json`, `pins[].@name`
mux lists.

**But capability is not the constraint — FlexIO *numbering* is.** FlexIO parallel shift
needs **contiguous** `D` pins. SmartDMA does not care which physical pin carries a given
`PIOn`, so the same wiring can be ideal for one and useless for the other.

## 2. The collision in the current wiring

Current assignment, from `src/avc/avc_core0/source/avc_io/bv_camera__interface.c:1438-1445`:

| Cam | Pin | SmartDMA | FlexIO | |
|---|---|---|---|---|
| D0 | P1_4 | PIO0 | `D12` | ok |
| D1 | P1_5 | PIO1 | `D13` | ok |
| D2 | P1_6 | PIO2 | `D14` | ok |
| D3 | P1_7 | PIO3 | `D15` | ok |
| **D4** | **P3_4** | PIO4 | **`D12`** | **collides with D0** |
| **D5** | **P3_5** | PIO5 | **`D13`** | **collides with D1** |
| D6 | P1_10 | PIO6 | `D18` | ok |
| D7 | P1_11 | PIO7 | `D19` | ok |

FlexIO set is `{12,13,14,15,12,13,18,19}` — a hard collision, and 16/17 missing.
**SmartDMA is contiguous today (PIO0..PIO7); FlexIO is not.** That asymmetry is the entire
problem.

### The stale comment named the answer

Lines 1442–1443 read `// EZH_PIO4, PIO1_8,P1_8/...` and `// EZH_PIO5, PIO1_9,P1_9/...`
while the code writes `PORT3->PCR[4]` and `PORT3->PCR[5]`. **The comments are wrong about
the code and right about the intent.** `SMARTDMA_PIO4/PIO5` exist on both pin pairs, so
EZH never noticed.

## 3. The fix

| Cam | Pin | SmartDMA | FlexIO |
|---|---|---|---|
| D0–D3 | P1_4…P1_7 | PIO0–3 | `D12`–`D15` |
| **D4** | **P1_8** | **PIO4** | **`D16`** |
| **D5** | **P1_9** | **PIO5** | **`D17`** |
| D6–D7 | P1_10, P1_11 | PIO6–7 | `D18`–`D19` |

Contiguous both ways. Verified programmatically against the signal dump.

## 4. Sync signals

Today the syncs use **no pin alt function at all** — they reach SmartDMA through INPUTMUX
(`bv_camera__interface.c:1430-1432`):

```c
INPUTMUX_AttachSignal(INPUTMUX0, 0, kINPUTMUX_GpioPort0Pin4ToSmartDma);  // VSYNC P0_4
INPUTMUX_AttachSignal(INPUTMUX0, 1, kINPUTMUX_GpioPort0Pin11ToSmartDma); // HSYNC P0_11
INPUTMUX_AttachSignal(INPUTMUX0, 2, kINPUTMUX_GpioPort0Pin5ToSmartDma);  // PCLK  P0_5
```

| Signal | Pin | FlexIO | Action |
|---|---|---|---|
| **PCLK** | P0_5 | **none** | **must reroute** |
| HSYNC | P0_11 | `FLEXIO0_D3` | **none needed** |
| VSYNC | P0_4 | none | none needed — GPIO IRQ, as the earlier FlexIO test used |

## 5. Board evidence

Netlists: `frdm-avc/tracks/A/output/design_review/design/` and
`avc/docs/FRDM-MCXN947/.../design_review/design/`.

**Connector identity:** FRDM-AVC **J9** (2×16, 2.54 mm) mates to FRDM-MCXN947 **J11**
(the FRDM board's `J9` carries no nets). The shield also has **`J9_EXT`** — an identical
2×16 header on the *same nets*. **`J9_EXT` is the jumper point**: every camera signal is
reachable without disturbing the stack.

### P1_8 / P1_9 loading

| Net | Shield | FRDM board |
|---|---|---|
| `P1_8` (J9.32) | J9, J9_EXT only | J11.32, **R173 → `LPC_MCU_LINK_TX`**, U1.A1 |
| `P1_9` (J9.30) | J9, J9_EXT only | J11.30, **R172 → `LPC_MCU_LINK_RX`**, U1.B1 |

**R172/R173 are 330 Ω** series isolation into the MCU-Link VCOM UART.

- **P1_9: harmless.** MCU-Link only listens.
- **P1_8: contention.** MCU-Link TX drives it through 330 Ω. Nothing is damaged — that
  resistor exists for exactly this, limiting the fight to ~10 mA — and TX idles high.
  **Remove R173 (0603, FRDM board) if D4 misbehaves.** The VCOM is unused: debug is
  SEGGER RTT (`E_LOG` → `SEGGER_RTT_LOG`) and `BOARD_InitDebugConsole()` is never called.

### PCLK reroute candidates

| Pin | J9 | FlexIO | Verdict |
|---|---|---|---|
| **P1_14** | **2** | **`D22`** | **use this** — J9/J9_EXT + J11/U1 only |
| P1_15 | 1 | `D23` | equally clean, alternate |
| P1_12 | 28 | `D20` | **avoid** — drives Q2 (FDV301N MOSFET) + R3 (100 k) on the shield |
| P1_13 | 27 | `D21` | **avoid** — R163 0 Ω to `ENET_RXDV/MODE2`, an Ethernet PHY strap |

P1_14/P1_15 are nominally `EZH_LCD_D10/D11` — parallel-LCD signals, unused with the
shipped SPI panel (`CONFIG__DISPLAY_PANEL = DISPLAY_PANEL_ER_TFT020_3`).

## 6. Minimum change

**Hardware — three short dupont jumpers on `J9_EXT`:**

| # | From | To |
|---|---|---|
| 1 | pin 12 (`P3_4`, cam D4) | pin 32 (`P1_8`) |
| 2 | pin 11 (`P3_5`, cam D5) | pin 30 (`P1_9`) |
| 3 | pin 15 (`P0_5`, PCLK) | pin 2 (`P1_14`) |

Contingent: remove **R173**.

**No cuts.** The camera signal stays on its original net; the old pin is simply muxed off
in firmware, so both pins see the signal and only the new one is read. Each jumper adds a
stub — acceptable at the current PCLK for a bench fixture, and a reason not to raise PCLK
until Rev B routes it properly.

**Firmware:**
- `P1_4..P1_11` → **alt6** (`FLEXIO0_D12..D19`)
- `P1_14` → alt6 (`FLEXIO0_D22`, PCLK)
- `P0_11` → alt6 (`FLEXIO0_D3`, HSYNC)
- `P0_4` VSYNC → GPIO IRQ, unchanged
- Mux `P3_4`, `P3_5`, `P0_5` to disabled
- Drop the three `INPUTMUX_AttachSignal` calls on the FlexIO path

## 7. Why this beats the previous FlexIO attempt

`FlexIO_Camera_Test_Plan.md` and `MCXN947/flexio_camera_io_pin_map.md` target
`P4_12..P4_23` → `FLEXIO0_D20..D31`, which needs **all eight data lines fly-wired** plus
the syncs. The `P1_4..P1_11` group **is the existing camera wiring** — three wires instead
of eleven, and it keeps EZH working on the same harness.

## 8. Rev B

Route camera **D4/D5 to P1_8/P1_9** instead of P3_4/P3_5, and **PCLK to P1_14** instead of
P0_5. Then both backends work natively with zero jumpers, PCLK can be routed for signal
integrity, and the EZH is freed for other use.

## 9. Open items

- Confirm `J9_EXT` is unpopulated/accessible on the physical test board.
- Confirm nothing else on the shield needs `P1_14` (`EZH_LCD_D10`) while the SPI panel is
  selected.
- Whether MCU-Link TX on P1_8 disturbs D4 in practice, or R173 must come off.
- Whether stub length on the three jumpers limits usable PCLK — measure before concluding
  anything about FlexIO's frame-rate ceiling.
- Actual FlexIO frame rate achieved vs the ~24 FPS EZH ceiling. That number is the whole
  justification for Rev B.

## 10. Related

- [`FlexIO_Camera_Test_Plan.md`](FlexIO_Camera_Test_Plan.md) — prior FlexIO plan (Port 4 group)
- [`MCXN947/flexio_camera_io_pin_map.md`](MCXN947/flexio_camera_io_pin_map.md) — prior pin map
- [`MCXN947/flexio_pin_candidates/`](MCXN947/flexio_pin_candidates/) — earlier derived candidates
- [`AVC_Competition_Overview.md`](AVC_Competition_Overview.md) §4 — capture path context
- [`AVC_Vision_Pipeline_Design.md`](AVC_Vision_Pipeline_Design.md) — what the frame budget is for
