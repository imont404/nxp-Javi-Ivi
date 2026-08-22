# AVC Vision Pipeline — Lookup Tables, Edge Detection, and PowerQuad

Companion to [`neutron_npu/AVC_Neutron_NPU_Assessment.md`](neutron_npu/AVC_Neutron_NPU_Assessment.md).
That document concluded the NPU is the wrong accelerator for the 2026 color twist and
recommended a lookup-table approach instead. **This document is the design detail for that
recommendation**, plus two adjacent findings.

**Date:** 2026-07-25. No hardware measurement — all cycle figures are estimates, flagged
as such. Hardware facts are cited to source.

---

## Summary

| Finding | Consequence |
|---|---|
| **The MCXN947 flash cache is 64 bytes.** | A random-access LUT in flash gets ~zero cache benefit. Refines *where* tables live; does not kill the idea. |
| **The flash data cache used to be enabled only on the EZH camera path.** | Fixed in common `avc__init()` startup; EZH and FlexIO now get the same cache setup. |
| **Per-pixel HSV is impossible today at full frame; a LUT makes it routine.** | ~64 ms → ~3 ms. This is the whole argument. |
| **PowerQuad's native driver is fire-and-forget; the CMSIS wrapper is what blocks.** | 41 `PQ_WaitDone()` calls in the CMSIS layer, 0 in the native filter/matrix/transform drivers. Skip CMSIS to get overlap. |
| **PowerQuad has no interrupt.** | Completion is poll-only. Overlap must be statically structured, not event-driven. |
| **Edge detection is student-owned.** | Old float experiments were removed; the LUT exposes generic features without supplying the race solution. |

---

## 1. The flash cache is 64 bytes

From the MCX Nx4x Reference Manual Rev 5 §4.6.1, in the local extract at
`MCXN947/extracted/MCXNx4xRM/chapters/04_memory/text.md`:

> The FMC provides two separate mechanisms for accelerating read operations to the flash
> memory:
> - A 128-bit prefetch buffer, which can prefetch the next 128-bit flash memory location.
> - **A 64-Byte cache organized as a one set, four-way associative cache with 128-bit
>   (or 16-Byte) size entries.**

Four sixteen-byte lines. Both mechanisms are tuned for **sequential** access — the prefetch
buffer explicitly "requests the next sequential address after a read completes."

**A colour lookup table indexed by pixel value is the pathological case.** Consecutive
pixels index unrelated table entries, so essentially every lookup misses and pays raw flash
latency. Sequential prefetch actively works against you: it fetches the next table entry,
which you will never want.

**This does not kill the idea.** See the cost model in §2 — even paying flash latency on
every pixel, a full-frame LUT pass lands comfortably inside the frame budget. It changes
*where you put which table*, not *whether to use tables*.

### Correction, and a real finding underneath it

An earlier draft of this document claimed `NVM_CTRL[DIS_MBECC_ERR_DATA]` was left set and
was costing flash speculation. **That was wrong** — `SystemInit()` in
`device/system_MCXN947_cm33_core0.c` already clears it unconditionally at startup, and
that bit governs ECC bus-error reporting rather than the cache.

**What was actually true, and did matter:**

The cache enable originally lived inside the EZH camera path:

```c
SYSCON->LPCAC_CTRL &= ~1;                                  // instruction cache
SYSCON->NVM_CTRL   &= SYSCON->NVM_CTRL & ~(1 << 2 | 1 << 4);
```

Bit 2 is `DIS_FLASH_CACHE` and bit 4 is `DIS_FLASH_DATA`, so FlexIO builds previously ran
with flash data caching disabled. **Fixed:** `avc__flash_cache_init()` now runs from common
`avc__init()` startup before capture-backend selection. The table therefore has the same
cache configuration on EZH and FlexIO. Backend-specific target timing still needs to be
measured because the shared bus workload differs.

---

## 2. Why the LUT matters — the cost model

**Budget:** 150 MHz ÷ 24 FPS = 6.25 M cycles/frame. Frame is 320 × 200 = 64,000 px.

The current `rgb565_to_hsl()` in `avc__line_processor.c` does three float divides, several
branches, and a float normalise per pixel — call it ~150 cycles. **Estimates, not
measurements:**

| Work | Est. cycles | Est. time | % of frame |
|---|---|---|---|
| One line (320 px), float HSL — *what ships today* | ~48 k | ~0.32 ms | 0.8% |
| One line, LUT in flash | ~2.6 k | ~0.02 ms | ~0% |
| **Full frame, float HSL** | **~9.6 M** | **~64 ms** | **154% — impossible** |
| Full frame, LUT in flash (cache-miss every pixel) | ~600 k | ~4 ms | ~10% |
| Full frame, LUT in SRAM | ~260 k | ~1.7 ms | ~4% |

**The middle row is the argument.** Per-pixel HSV across a whole frame cannot be done at
all today; it exceeds the entire frame budget by 50%. A lookup table turns it into a ~10%
line item. That is the difference between "colour is a per-scanline trick" and "colour is
something you can do everywhere."

Note the flash-vs-SRAM gap is roughly 2×, not 10×. **Flash is a perfectly good home for
these tables.** Reserve SRAM for a table only if measurement shows you need it.

---

## 3. Recommended table design

### Pack the channels into one 32-bit entry

Because the cache will not help you, **the number of lookups matters more than the size of
the table.** Four separate `uint8` tables means four independent flash misses per pixel.
One `uint32` table means one:

```c
/* 65,536 entries x 4 bytes = 256 KiB in flash. */
extern const uint32_t avc_rgb565_yhsv_lut[65536];

static inline void avc_color__unpack(uint16_t px, uint8_t *y, uint8_t *h, uint8_t *s, uint8_t *v)
{
    uint32_t e = avc_rgb565_yhsv_lut[px];   /* one access -> all four channels */
    *y =  e        & 0xFF;
    *h = (e >>  8) & 0xFF;
    *s = (e >> 16) & 0xFF;
    *v = (e >> 24) & 0xFF;
}
```

### Use `uint8`, not float

Eight-bit hue is 1.4°/step — far finer than any threshold a student would set, and finer
than RGB565 quantisation justifies in the first place. Integer compares also avoid FPU
loads. Float tables would be 4× the flash for no usable precision.

### Keep hue generic and circular

The generated hue uses 256 equal circular bins, with no color-specific rotation. Red still
crosses the zero point, which is useful in the first-day color-space lesson and keeps the
framework table generic. A later application-specific table may rotate hue, but must name
that convention rather than hiding it from students.

### Keep the teaching structure

The point of exposing H, S, and V separately rather than baking in a class byte is
**pedagogical** — hue as an angle and S/V as a confidence gate is the lesson. Students still
write:

```c
if (v > v_min && s > s_min) {          /* enough colour information to trust hue */
    if (h >= red_lo && h <= red_hi) ... /* now hue is meaningful */
}
```

The table replaces the *arithmetic*, not the *reasoning*. A pre-baked class LUT would be
marginally faster and would destroy the teachable structure — the collapse to a single
class byte is the natural "now make it fast" follow-up exercise, not the starting point.

### Generation

`scripts/tools/generate_rgb565_color_lut.py` generates the committed
`avc_rgb565_yhsv_lut.inc`. The generator uses RGB bit replication, BT.601 Y, and Python's
HSV reference conversion; it exhaustively validates all 65,536 entries and supports
`--check` for CI. The artifact is deterministic and contains no tuned color classes.

```powershell
uv run python scripts/tools/generate_rgb565_color_lut.py
uv run python scripts/tools/generate_rgb565_color_lut.py --check
```

**Implemented 2026-08-21:** the competition build now uses this table for the existing
scanline test view. The old float HSL conversion was removed. The table occupies exactly
`0x40000` bytes in `.rodata` and consumes no SRAM. A clean staged LUT-only competition
snapshot uses 354,180 bytes of the first 1 MiB flash bank; the current integrated working
tree with pending state-machine/telemetry changes uses 383,368 bytes. Both leave ample
headroom.

---

## 4. Edge detection remains student-owned

The earlier line processor contained unused float-HSL gradient, adaptive threshold, and
hybrid segment experiments. They were removed with the LUT integration: they duplicated
color work, used variable-length float arrays, and risked handing students too much of the
solution. The remaining binary edge/segment helpers are intentionally simple; the
student-API plan still needs to decide whether even those belong in the handoff.

For teaching and future student work, the relevant Canny comparison remains:

| Canny stage | Status in current code |
|---|---|
| Gaussian smoothing | **Missing** |
| Gradient | Student implementation |
| Non-maximum suppression | Student implementation |
| Double threshold + hysteresis | Student implementation |

### Add Gaussian pre-smoothing

Without a smoothing pass, an implementation differentiates noise. A 5-tap
`[1,4,6,4,1]/16` pass over the line is the single biggest robustness win available, and it
matters specifically for this track: tape edges under conference-hall lighting produce
glare speckle that a raw central difference will happily report as an edge.

### Add hysteresis

Two thresholds — a high one to seed an edge, a low one to extend it. Perhaps fifteen lines
in 1-D. It buys **both** failure modes at once: faded or dusty tape stops being missed, and
isolated glare specks stop being promoted. It is also a genuinely good teaching question:
*why would two thresholds beat one?*

### Use gradient orientation to survive crossings

The track has 45° and 90° crossings (see
[`AVC_Competition_Overview.md`](AVC_Competition_Overview.md) §2), and a naive follower
chases the transverse line off the track.

If several adjacent scanlines are processed, a vertical difference gives **edge
orientation**. At a crossing, the transverse line's gradient direction differs sharply from
the lane edges'. That converts the crossing hazard from "hope it survives" into an explicit,
testable condition — and it is the strongest argument for processing a band of rows rather
than a single line.

The LUT makes multi-row experiments affordable without prescribing their implementation.

---

## 5. PowerQuad — the native driver overlaps, the CMSIS wrapper does not

### The evidence

`PQ_WaitDone()` call counts in `devices/MCXN947/drivers/`:

| File | `PQ_WaitDone()` calls |
|---|---|
| `fsl_powerquad_cmsis.c` | **41** |
| `fsl_powerquad_filter.c` | **0** |
| `fsl_powerquad_matrix.c` | **0** |
| `fsl_powerquad_transform.c` | **0** |

The CMSIS-compatible layer has no choice — `arm_fir_q15()` promises results on return, so
the wrapper fires the engine and immediately spins. **The CMSIS interface is exactly what
destroys the overlap.** The native driver never waits:

```c
void PQ_FIR(POWERQUAD_Type *base, const void *pAData, uint16_t ALength,
            const void *pBData, uint16_t BLength, void *pResult, uint32_t opType)
{
    base->INABASE = (uint32_t)pAData;
    base->INBBASE = (uint32_t)pBData;
    base->LENGTH  = ((uint32_t)BLength << 16U) + (uint32_t)ALength;
    base->OUTBASE = (uint32_t)pResult;
    base->CONTROL = (CP_FIR << 4U) | opType;   /* fires and returns */
}
```

Five register writes. The engine is a bus master that reads `INABASE`/`INBBASE` and writes
`OUTBASE` under its own steam.

**Contrast with the eIQ Neutron NPU, which is blocking-only** — its non-blocking API is
stubbed to return `DRIVER/UNSUPPORTED`
([proof](neutron_npu/evidence/blocking_execution_proof.md)). For overlapping work with the
control loop, PowerQuad is the right engine and Neutron is the wrong one. This **inverts**
the ranking reached in the bunny_vision research, which assessed PowerQuad for 2-D
whole-image filtering and dismissed it.

### Two access paths, different characters

- **Coprocessor path** — `__arm_mcr` / `__arm_mrc`, ~107 macros. `PQ_SqrtF32`, `PQ_InvF32`,
  `PQ_Arctan2Fixed`, `PQ_Vector8*`. Synchronous by construction: the instruction issues,
  you read the result back. Low latency, no overlap possible or needed. This is the scalar
  math path.
- **AHB register path** — FIR, matrix, FFT/DCT. Asynchronous, completion via
  `CONTROL & INST_BUSY`. **This is where overlap lives.**

PowerQuad also has 4 × 4 KB of private RAM for coefficients and temporaries, so FIR taps do
not re-traverse the system bus per sample.

### No interrupt

There is no `POWERQUAD_IRQn` in the device header. Completion is **poll-only**, and
`PQ_WaitDone()` spins on `__WFE()`. The usable pattern is therefore:

```c
PQ_FIR(POWERQUAD, line_q15, 320, gauss_taps, 5, smoothed_q15, PQ_FIR_FIR);
/* ... do a known chunk of CPU work here: LUT colour pass, control math ... */
while (POWERQUAD->CONTROL & INST_BUSY) { }   /* test the bit yourself */
```

Do **not** call `PQ_WaitDone()` in the overlap path — it blocks immediately, which is the
thing you are trying to avoid. Overlap has to be **statically structured** (fire, do a known
amount of work, then check) rather than event-driven.

### The honest caveat: is there enough work to hide?

At one scanline per frame the entire vision pass is ~0.3 ms out of 41 ms. **There is
nothing to overlap with.** PowerQuad only starts paying when the workload grows to many
lines or a full frame — which is exactly what the LUT makes affordable.

**So the two ideas are coupled: the LUT is what creates enough work for PowerQuad overlap to
be worth structuring.** Sequence accordingly.

Also note PowerQuad needs Q15/Q31/float, not `uint8`. Y values need widening — trivial for
320 samples, a real per-pixel cost at full frame. And PowerQuad, EZH capture, LCD DMA, and
the CM33 all share the bus matrix; at 320 samples that is noise, at full frame it needs
measuring.

---

## 6. Recommended sequence

1. **Done:** flash cache enable now runs from common `avc__init()` startup, so FlexIO and
   EZH builds get the same setup.
2. **Done 2026-08-21:** build the packed `uint32` RGB565 → Y/H/S/V table in flash and use
   it in the existing scanline test path. Target timing remains to be measured against the
   ~4 ms full-frame estimate; flash-cache behaviour is still the main unknown.
3. Keep Gaussian pre-smoothing and hysteresis as student-facing teaching material rather
   than adding a finished edge detector to the framework.
4. **Only then** evaluate PowerQuad overlap, once step 2 has produced a workload large
   enough to hide anything behind. Use the native driver, not the CMSIS wrapper. The SDK
   ships `powerquad_benchmark_filter` and a paired software version for FRDM-MCXN947 —
   measure with those before designing anything.
5. **Gradient orientation for crossing rejection** once multi-row processing is affordable.

## 7. Open questions

- What is the actual measured cost of a random-index flash LUT read? The 64-byte cache says
  "expect a miss every time," but raw flash latency at 150 MHz is not documented in the
  material reachable here (the datasheet is blocked to tooling — see the NPU assessment).
- Measure LUT timing on both capture backends now that cache setup is common; bus workload
  can still make the results differ.
- RGB565 is the competition format. YUY2 is deferred; it would require LCD conversion and
  changes across the already-proven USB/Android paths.
- Real PowerQuad FIR throughput for a 320-sample line including setup overhead, and how
  much bus contention it introduces against EZH capture.
