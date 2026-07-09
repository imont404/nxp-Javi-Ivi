# MCXN947 Deep Research (running notes)

Source unless noted: NXP SDK register/SVD metadata at `C:\Users\emh20\Desktop\usbhost\MCXN947`
(json + xml, `ksdk2_0` and `zephyr3_2`, package variants VAB/VDF/VKL/VNL/VPB/VKL).
Authoritative RM available: `avc/docs/MCXNx4xRM.pdf` (36 MB) for base addresses / full chapters.

---

## 1. CoolFlux DSP — PRESENT  ·  Power-line comms (PLC) — ABSENT

> **Consolidated:** all CoolFlux resources + lessons learned now live in **`../CoolFlux/`** (README.md + `sources/`: the internal deck, IFR xlsx, RM ch21-22 text, `PERI_BSP32.h`). That folder is the canonical/maintained copy; the notes below are the working detail.

### 1.1 Power-line communication: not a REGISTER block (but see 1.1b)
Zero matches for `power line` / `PLC` etc. across all MCXN947 register metadata — because PLC
is **not a hardware peripheral**. It is a **software modem that runs on the CoolFlux DSP** (below).

### 1.1b PLC nuance (IMPORTANT correction)
PLC is **not a register peripheral** (hence zero metadata hits) — it is a **software-defined modem that RUNS ON the CoolFlux BSP32**. On MCX N, the CoolFlux's primary purpose is exactly this: a software PLC modem for smart metering.
- **Product differentiation:** **N94x = CoolFlux "PLC DSP" present; N54x removes the CoolFlux/PLC** (and adds USB-HS). (eeNews launch coverage + NXP positioning.)
- **Standards targeted:** **G3-PLC and PRIME** (PRIME: 250 kHz sampling, 512-pt FFT, 42-89 kHz CENELEC band).
- **Public delivery status: MISSING / not in SDK.** Two NXP Community threads ("MCX N94x/54x PLC Application Notes" td-p/1832733; "Solved: MCXN947 CoolFlux BSP32 Use?" m-p/1849864) show customers asking for the PLC app note/software; NXP reps reply there are **no public tools to compile CoolFlux code, MCUXpresso IDE does not support the CoolFlux on MCXN947**, and point to the RM CoolFlux chapter. Org/SDK scan: no PLC/G3/PRIME code, no CoolFlux example, no PLC binary blob. `mcux-metering` repo = energy *measurement* math (active/reactive power), NOT PLC comms.
- **=> The PLC implementation the user recalls is an internal/NDA deliverable** (precompiled CoolFlux PLC firmware "binary block" + M33-side integration + app note) owned by the metering/PLC product line + CoolFlux team (EU), not published publicly. Metering-team contact (library owner): `pavel.rech@nxp.com`. CoolFlux toolchain: `coolflux.support@nxp.com`.
- **Public precedent collateral (not the MCXN deliverable):** DRM170 "Power-Line Communication Module for CENELEC Band" (older Kinetis PLC design ref), AN4946 "Powerline Communication - Analog Front End."

### 1.2 CoolFlux DSP: present as peripheral `BSP32_0` ("CoolFlux BSP32")
56-byte register block. NXP's CoolFlux is a Harvard-architecture audio/signal DSP; here it is
the secondary processor alongside the dual Cortex-M33s.

**Register map (`BSP32_0.xml`, offsets within the block):**
| Off | Reg | Notes |
|---|---|---|
| 0x00 | `OFFSET_PMEM` (6-bit) | Maps DSP **Program** memory window into system address map |
| 0x04 | `OFFSET_XMEM` (8-bit) | Maps DSP **X-data** memory window |
| 0x08 | `OFFSET_YMEM` (8-bit) | Maps DSP **Y-data** memory window |
| 0x0C | `OFFSET_MAILBOX` (24-bit) | Maps mailbox peripheral window |
| 0x10 | `INTERRUPTS_EXTERNAL` (WORZ) | Host -> DSP interrupt trigger |
| 0x14 | `INTERRUPTS_STATUS` (W1C) | DSP interrupt status |
| 0x18 | `CF_GATING_OVERRIDE` | Clock-gating override |
| 0x1C | `IVT_OFFSET` (24-bit) | Interrupt Vector Table base |
| 0x20 | `SLEEP_MODE` (RO) | DSP sleep status |
| 0x24-0x30 | `IVT0..IVT3` (24-bit) | IVT contents, reset 0x8/0x9/0xA/0xB |
| 0x34 | `IVT_DISABLE` | Disable IVT |

Key implication: **DSP P/X/Y code & data live in shared SRAM, windowed in via the OFFSET
registers** — there is no dedicated named "DSP RAM" region in the memory map.

**Bus mastering (3 AHB masters, priorities in SYSCON0 `AHBMATPRIO`):**
- `PRI_COOLFLUX_I` (instruction), `PRI_COOLFLUX_X`, `PRI_COOLFLUX_Y_ESPI` — each 4 levels (LEVEL0-3).

**Clock / reset (SYSCON0):**
- `AHBCLKCTRL3`: `COOLFLUX` (core clock en), `COOLFLUX_APB` (APB clock en).
  SVD note: *"CoolFlux needs to be properly programmed before the clock enabled."*
- `COOLFLUX_RST`, `COOLFLUX_APB_RST` reset controls.

**ARM <-> DSP comms:** dedicated `MAILBOX` peripheral (252-byte block) + `OFFSET_MAILBOX` window.

**Security / debug:**
- AHBSC (secure access control) treats CoolFlux as a first-class master:
  `COOLFLUXI_{SECURE,NONSECURE}_{PRIV,NONPRIV}_MASTER`, plus user-allowed gates.
- Placed in a security domain (`trust_model.json`).
- Debug gated: `DSP_DBGEN`/`DSP_DBGDEN`; `SWD_ACCESS_DSP` unlock `SEC_CODE = 0x12345678`.

**Not captured in metadata (get from RM if needed):** absolute base address of `BSP32_0` and
`MAILBOX`, CoolFlux ISA/toolchain, P/X/Y memory sizes, MHz. See `MCXNx4xRM.pdf`.

> Relevance to AVC: the CoolFlux is a third programmable engine (besides 2x M33 + SmartDMA +
> PowerQuad). It's an audio/vector DSP with Harvard P/X/Y memory and a mailbox to the M33 —
> potentially usable to offload pixel/line math, but it needs the CoolFlux toolchain and the
> P/X/Y memory is windowed from shared SRAM (bandwidth contention to weigh). Not a quick win.

### 1.3 RM-confirmed details (Reference Manual Ch 21 + Ch 22)
Full chapter text extracted to `avc/docs/CoolFlux_RM_ch21_22.txt` (RM Rev.5, 06/2024, PDF pp.706-729).

- **Base addresses:** `BSP32_0 = 0x4003_2000` · `MAILBOX0 = 0x400B_2000` (the `offset_mailbox` reg is set to `0x400B20` to window the mailbox into the DSP's X-memory top 256 addresses).
- **Core:** CoolFlux **BSP32** = "Baseband Signal Processor", **32-bit data path + 32-bit data memories, dual-Harvard, dual-multiplier**, ultra-low-power CoolFlux arch. 16/32-bit scalar + complex + SIMD. **Fully C-programmable, but with NXP's proprietary CoolFlux compiler/tool suite — obtained via coolflux.support@nxp.com (NOT public GCC).** This toolchain dependency is the main adoption barrier.
- **Memory reach (matters for offload):**
  - Program-memory IF: SRAM X + SRAM A..H, **plus flash and FlexSPI**.
  - X & Y data IFs: SRAM A..H, **plus PowerQuad addressing space**; X-IF top 256 addr = Inter-CPU Mailbox.
  - => DSP code/data live in shared SRAM (bus contention with M33/DMA to weigh); priorities via `SYSCON[AHBMATPRIO]`.
- **3 AHB-Lite masters:** 1 program + 2 data (X, Y).
- **No security of its own:** "CoolFlux BSP32 does not include any mechanism for supporting system security of the type implemented by the Cortex-M33." (Its bus access is still gated externally by AHBSC; SWD debug is shared with the M33 control-domain CPU.)
- **Interrupts:** 32 external inputs -> internal controller maps to 3 CoolFlux IRQ routines (edge-detected, SW-handled); DSP can also raise an IRQ to the M33.
- **Init sequence (M33-driven):** load P/X/Y memories -> set `ivt0..ivt3` -> set APB config (offset regs etc., *before* reset release) -> enable clock -> de-assert reset. Held in reset after device reset.
- **Clock/reset:** dedicated `clk` + active-high async `reset`; APB prog/debug interfaces have own clocks/resets. `SYSCON.AHBCLKCTRL3[COOLFLUX/COOLFLUX_APB]` clock en; `COOLFLUX_RST`.
- **Co-debug:** cross-break module transfers break events between CoolFlux and M33.
- **Compiler caveat (21.1.3):** long data-memory paths cap max frequency; compiler must avoid a data-mem load into a pointer reg immediately followed by an access through that pointer.
- **MAILBOX (Ch 22):** `MAILBOX0`, AHB slave; **two mailboxes x three 32-bit regs**; per-mailbox MUTEX + periph-ID; IRQ0/IRQ1 SET/CLR for CPU0(M33)/CPU1(M33) and reused for M33<->CoolFlux; `MUTEX[EX]` single-bit resource lock. Clock `SYSCON.AHBCLKCTRL0[MAILBOX]`, reset `PRESETCTRL0[MAILBOX_RST]`.

### 1.4 MCUXpresso SDK support audit (via mcuxsdk-manifests, 2026-06)
Searched the whole `nxp-mcuxpresso` org (manifest repo imports devices/middleware/rtos submanifests). **The SDK ships only bus-side plumbing for the CoolFlux — no driver, no example, no toolchain.**

What EXISTS in the public SDK:
- **Register map:** `mcux-devices-mcx/MCXN/periph/PERI_BSP32.h` — CMSIS `BSP32_Type` struct + bitfield macros, matches RM exactly (PMEM 6-bit/0x3F, XMEM/YMEM 8-bit/0xFF, MAILBOX 24-bit/0xFFFFFF). **Header explicitly covers MCXN247** (same family as N946/947/5xx) — so N247 has the BSP32/CoolFlux too.
- **Clock:** `fsl_clock.h` -> `kCLOCK_CoolFlux` (AHB_CLK_CTRL3 bit3), `kCLOCK_CoolFluxApb` (bit20).
- **Reset:** `fsl_reset.h` -> `kCOOLFLUX_RST`, `kCOOLFLUX_APB_RST`.
- **Kconfig IP marker:** `MCUX_HW_IP_DriverType_CoolFlux_BSP32` (declares the IP; no driver behind it).
- **Startup/IRQ vectors** for the CoolFlux->M33 external interrupt.

What does NOT exist (confirmed by org code search):
- **No `fsl_bsp32.c` / `fsl_coolflux.c` driver** (0 hits).
- **No CoolFlux/BSP32 example or demo app** (only `tzm_config.c` TrustZone region setups mention BSP32; not examples).
- **No DSP firmware / compiler / toolchain** (proprietary; RM says request via coolflux.support@nxp.com).
- **No dedicated component/middleware repo** (org repo search for coolflux/bsp32/dsp -> only "NatureDSP" = unrelated Cadence/Xtensa HiFi).

**Do NOT confuse with TFA smart-amp CoolFlux:** `mcux-component/codec/tfa9xxx` ("CFE = Enable CoolFlux DSP", firmware soft-mute, etc.) is the **TFA9xxx smart audio amplifier** (separate I2C chip embedding its own CoolFlux), NOT the MCXN on-chip BSP32.

**Net:** SDK lets you clock/reset/IRQ the core and read/write its config regs, but to actually *run code* on the CoolFlux you must obtain the proprietary CoolFlux compiler/tools + write DSP firmware + hand-roll the M33-side bring-up from raw registers (no driver/example to copy). High-friction; reinforces "not a quick win" for AVC.

### 1.5 CoolFlux toolchain & supply chain (from internal deck "MCX N CoolFluxBSP32.pdf", NXP Confidential, Leuven BE, c.2009)
Local files: `C:\Users\emh20\Desktop\usbhost\MCX N CoolFluxBSP32.pdf` + `Nirvana_IFR1_v2.00.xlsx`.
- **Subsystem codename "Nirvana"; owned by NXP Semiconductors Leuven, Belgium** (= the EU CoolFlux team).
- **CoolFlux BSP32 is an ASIP** (Application-Specific Instruction-set Processor); programmed once the architecture is "frozen" via the **ASIP Programmer tool flow**.
- **Toolchain = ChessDE IDE** = Compiler + Assembler + Linker + Simulator + Hardware debugger. **Licenses required.** "Chess/Checkers" is the **Synopsys ASIP Designer** toolchain (ex-Target Compiler Technologies) — a commercial EDA tool, NOT a free embedded toolchain. *This is why it will never be in MCUXpresso.*
- **Supply chain:** **tools from Synopsys (+ CodecArt); software/codecs from CodecArt** (third-party SW house, BE). Support tiers: 1st-line, 2nd-line, CoolFlux team. Existing SW = math libs + codecs; on-demand SW developed "when CoolFlux BSP32 is opened for customers."
- **`Nirvana_IFR1_v2.00.xlsx`** = Information-Flash-Region factory/customer config for the Nirvana device: IFR0/IFR1 fields, `Peripheral_EN0..3`, GDET/TRNG config, `DEVICE_TYPE`/`DEVICE_ID`, flash size — variants **1P02G / 2P02G / 3P02G = single/two/three-phase meter SKUs** (confirms metering/PLC purpose). Internal config data, not architecture.
- **Practical upshot:** running custom code on the CoolFlux requires a **Synopsys ASIP Designer / Chess license** (+ likely CodecArt engagement) — a commercial EDA procurement, not just an SDK download. The PLC firmware "binary block" was built with this flow by the Leuven team / CodecArt.
