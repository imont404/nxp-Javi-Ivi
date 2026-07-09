# CoolFlux BSP32 (MCX N) — Consolidated Resources & Lessons Learned

Everything gathered on the NXP **CoolFlux BSP32** DSP as found on the **MCX N94x / N54x / N23x (incl. MCXN247, MCXN947)** parts.
Compiled 2026-06-16. Scope: what it is, how it's wired, the toolchain reality, the PLC story, and whether it's worth using.

> **TL;DR:** The CoolFlux BSP32 is an on-chip 32-bit DSP whose real purpose on MCX N94x is a **software-defined Power-Line-Communication (PLC) modem for smart metering** (G3-PLC / PRIME). It is genuinely capable, **but practically unusable for a hobby/AVC project**: there is **no public driver, no example, and no free compiler** — programming it requires the commercial **Synopsys "Chess" / ASIP Designer** toolchain (license) from the **NXP Leuven (Belgium)** team + CodecArt. The PLC firmware is delivered as an **NDA binary block**, not in the SDK. For offloading math on MCX N, use **PowerQuad** instead.

---

## 0. Files in this folder

| File | What it is |
|---|---|
| `sources/MCX N CoolFluxBSP32.pdf` | **NXP Confidential** overview deck (Leuven BE, codename "Nirvana", c.2009) — architecture + **toolchain/supply-chain** |
| `sources/Nirvana_IFR1_v2.00.xlsx` | Internal **IFR (Information Flash Region)** factory/customer config for the Nirvana device (1P/2P/3P02G meter SKUs, peripheral enables, GDET/TRNG, DEVICE_ID) |
| `sources/CoolFlux_RM_ch21_22.txt` | Extracted text of **MCX Nx4x RM Rev.5 Ch 21 (CoolFlux) + Ch 22 (Inter-CPU MAILBOX)**, pp.706-729 |
| `sources/PERI_BSP32.h` | CMSIS register header from the public MCUXpresso SDK (`mcux-devices-mcx`), the full `BSP32_Type` map |

Authoritative full RM (not copied here, 36 MB): `../avc/docs/MCXNx4xRM.pdf`.

---

## 1. What it is

- Peripheral **`BSP32_0`**, full name **"CoolFlux BSP32"**. Subsystem codename **"Nirvana"** (NXP Semiconductors, **Leuven, Belgium**).
- **32-bit data path + 32-bit data memories, dual-Harvard, dual-multiplier**, ultra-low-power CoolFlux architecture. 16/32-bit scalar + complex + SIMD. Built for **audio, sensor, and modem/baseband** work.
- It is an **ASIP** (Application-Specific Instruction-set Processor) — i.e., a customized DSP core, not a general MCU core.
- One instance on the device, alongside the **2x Cortex-M33** (+ PowerQuad, SmartDMA).
- **Present on:** MCX N94x (e.g. MCXN947), N23x (incl. **MCXN247**), N5xx family (per `PERI_BSP32.h` part list). **Removed on N54x** (which adds USB-HS instead) — the CoolFlux/PLC block is the N94x's metering differentiator.

---

## 2. How it's wired (RM Ch 21-22)

**Base addresses:** `BSP32_0 = 0x4003_2000` · `MAILBOX0 = 0x400B_2000`.

**Register map (`BSP32_0`, 56 bytes — see `sources/PERI_BSP32.h`):**
| Off | Reg | Notes |
|---|---|---|
| 0x00 | `OFFSET_PMEM` (6-bit) | windows DSP **Program** memory into system map |
| 0x04 | `OFFSET_XMEM` (8-bit) | windows DSP **X-data** memory |
| 0x08 | `OFFSET_YMEM` (8-bit) | windows DSP **Y-data** memory |
| 0x0C | `OFFSET_MAILBOX` (24-bit) | set `0x400B20` -> reaches mailbox via X-mem top 256 addr |
| 0x10 | `INTERRUPTS_EXTERNAL` (W) | host(M33) -> DSP IRQ |
| 0x14 | `INTERRUPTS_STATUS` (W1C) | DSP -> host IRQ status |
| 0x18 | `CF_GATING_OVERRIDE` | clock-gating override |
| 0x1C | `IVT_OFFSET` (24-bit) | interrupt-vector-table base |
| 0x20 | `SLEEP_MODE` (RO) | DSP sleep status |
| 0x24-0x30 | `IVT0..IVT3` (24-bit) | IVT contents (reset 0x8/0x9/0xA/0xB) |
| 0x34 | `IVT_DISABLE` | disable IVT |

**Memory reach (no dedicated "DSP RAM" — P/X/Y windowed into shared SRAM):**
- Program-mem IF: SRAM X + SRAM A..H, **+ flash + FlexSPI**.
- X & Y data IFs: SRAM A..H, **+ PowerQuad address space**; X-IF top 256 addr = Inter-CPU Mailbox.
- **3 AHB-Lite masters** (1 program, 2 data) — priorities via `SYSCON[AHBMATPRIO]` (`PRI_COOLFLUX_I/X/Y_ESPI`).

**Clock/reset:** dedicated `clk` + active-high async `reset`. SDK hooks: `kCLOCK_CoolFlux` (AHB_CLK_CTRL3.3), `kCLOCK_CoolFluxApb` (.20); `kCOOLFLUX_RST`, `kCOOLFLUX_APB_RST`. *RM: "CoolFlux needs to be properly programmed before the clock is enabled."*

**Inter-CPU comms (Ch 22 MAILBOX):** `MAILBOX0`, AHB slave; **2 mailboxes x 3x 32-bit regs**; IRQ0/IRQ1 SET/CLR (CPU0/CPU1 M33s, reused for M33<->CoolFlux); `MUTEX[EX]` single-bit resource lock. Clock `AHBCLKCTRL0[MAILBOX]`, reset `PRESETCTRL0[MAILBOX_RST]`.

**Security/debug:** **No security of its own** (RM: "does not include any mechanism for supporting system security of the type implemented by the Cortex-M33"); bus access externally gated by AHBSC (`COOLFLUXI_{SECURE,NONSECURE}_{PRIV,NONPRIV}_MASTER`). Debug shared with M33 control-domain core; `SWD_ACCESS_DSP` unlock = `0x12345678`; cross-break for co-debug.

**M33-side bring-up sequence:** load P/X/Y memories -> set `ivt0..3` -> set APB config (offset regs, *before* reset release) -> enable clock -> de-assert reset.

---

## 3. The toolchain reality (the deal-breaker)

From the internal deck (`MCX N CoolFluxBSP32.pdf`):
- Programmed via the **ASIP Programmer tool flow** = **ChessDE IDE**: Compiler + Assembler + Linker + Simulator + HW debugger. **Licenses required.**
- "Chess/Checkers" = the **Synopsys ASIP Designer** toolchain (ex-Target Compiler Technologies) — a **commercial EDA tool, not a free embedded toolchain**. *This is exactly why it is, and always will be, absent from MCUXpresso.*
- **Supply chain:** tools from **Synopsys (+ CodecArt)**; software/codecs from **CodecArt** (3rd-party SW house, BE). Support: 1st/2nd line + CoolFlux team.

**Public SDK audit (whole `nxp-mcuxpresso` org):** ships only **register header + clock/reset enums + Kconfig IP marker + IRQ vectors**. **No `fsl_bsp32` driver, no example/demo, no firmware/toolchain, no component repo.** (The `mcux-metering` repo is energy *measurement* math, not PLC; the `tfa9xxx` codec "CoolFlux" hits are the TFA smart-amp's *own* embedded DSP — unrelated.)

---

## 4. The PLC story (why the CoolFlux exists here)

- On **MCX N94x the CoolFlux IS the software-defined PLC modem** for smart metering. **Standards: G3-PLC and PRIME** (PRIME: 250 kHz sampling, 512-pt FFT, 42-89 kHz CENELEC band). IFR meter SKUs `1P/2P/3P02G` = single/two/three-phase.
- **The PLC stack/app note was never delivered publicly.** NXP Community threads ([PLC App Notes](https://community.nxp.com/t5/MCX-Microcontrollers/MCX-N94x-54x-Power-Line-Communication-PLC-Application-Notes/td-p/1832733), [CoolFlux Use?](https://community.nxp.com/t5/MCX-Microcontrollers/MCXN947-Coolflux-BSP32-Use/m-p/1849864)) show customers asking; NXP replies "no public tools to compile CoolFlux code, MCUXpresso doesn't support it," pointing to the RM.
- **=> The PLC deliverable is an NDA "binary block"** (precompiled CoolFlux PLC firmware + M33-side integration + app note), built by the Leuven team / CodecArt with the Chess flow. Public precedent only: [DRM170](https://www.nxp.com/docs/en/reference-manual/DRM170.pdf) (CENELEC-band PLC module, Kinetis era), [AN4946](https://www.nxp.com/docs/en/application-note/AN4946.pdf) (PLC AFE).

---

## 5. Lessons learned / verdict

1. **PLC is software on the DSP, not a peripheral** — register/metadata scans correctly find "no PLC" because it's firmware, not silicon.
2. **The CoolFlux is real and capable**, but **gated behind a commercial Synopsys toolchain + NDA firmware + an EU (Leuven) team** — three independent walls. The public SDK deliberately gives you only enough to clock/reset/IRQ it and shuttle data.
3. **For the AVC car: don't.** Custom CoolFlux code needs a Synopsys ASIP Designer/Chess license and DSP firmware written from scratch with no driver/example to copy. Not worth it. **Use PowerQuad** (has `fsl_powerquad` driver + examples) for M33 math offload, and SmartDMA/FlexIO for the camera (see `../AVC_Camera_Interface_Research.md`).
4. **If the PLC binary is genuinely needed** (different project): it exists internally — pursue via the metering/PLC product line + Leuven CoolFlux team, expecting NDA.

### Internal contacts / next steps
- **`coolflux.support@nxp.com`** — EU CoolFlux team (toolchain + PLC firmware).
- **`pavel.rech@nxp.com`** — owner of `mcux-metering` repo; metering BU lead-in.
- **CodecArt** — third-party CoolFlux software house (Belgium).
- Ask specifically for: the **G3-PLC/PRIME CoolFlux binary for MCX N94x**, its **M33-side integration app note**, and **release/NDA status**.

---

*See also: `../MCXN947_Deep_Research.md` (running N947 notes) and `../AVC_Camera_Interface_Research.md`.*
