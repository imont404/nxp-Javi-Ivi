# eIQ Neutron NPU on MCXN947 — Assessment for the AVC Student Platform

**Question asked:** Can the MCXN947's eIQ Neutron NPU be used to accelerate AVC camera
preprocessing — with NXP precompiling a fixed graph that students consume, rather than
students touching the ML toolchain?

**Date:** 2026-07-25
**Status:** Research complete. No hardware prototype run.
**Scope:** The late-August 2026 FIT race. See `../AVC_Competition_Overview.md`.

---

## Bottom line

**The NPU is technically usable and the toolchain is real, but it is the wrong tool for
the 2026 color twist, and it cannot be made ready and de-risked before late August.**

Three findings drive that conclusion, in order of importance:

1. **NPU execution blocks the calling core.** `neutronRunNonBlocking()`, `neutronWait()`,
   and `neutronIsReady()` are *stubs* in the shipped SDK — verified by disassembly, not
   inferred from docs. Offloading to the NPU therefore never buys you concurrency on the
   core that issues it. It only buys wall-clock, and only if the NPU beats the CPU at the
   same job. See `evidence/blocking_execution_proof.md`.
2. **Tensor marshalling costs more than the work itself.** The NPU eats int8 NHWC
   tensors. The camera produces packed RGB565. Unpacking 64,000 pixels into planar int8
   is an O(pixels) CPU pass that costs *more than simply doing the color classification
   on the CPU with a lookup table*. This is decisive for pointwise color work — which is
   exactly what the 2026 twist needs.
3. **A 64 KB lookup table solves the actual problem better.** Per-pixel color
   classification via a LUT indexed by chroma is faster than the NPU path, needs no new
   toolchain, has zero version-lock risk, is more expressive than a linear classifier,
   and is dramatically more teachable. Details in [Recommendation](#recommendation).

**What the NPU *is* good for here:** multi-stage spatial convolution over a full frame,
and small genuine CNNs. Neither is on the critical path for this year's race.

**Recommended disposition:** Ship the LUT-based color pipeline for the race. Pursue
Neutron as a *post-race, off-critical-path* exploration, or as an opt-in demo build that
cannot affect the competition image.

---

## 1. What is actually on this machine

Established by inspecting the installed SDK package, not from documentation:

`C:\Users\EliHughes\mcuxpresso\02\SDKPackages\com.nxp.mcuxpresso.sdk.sdk_2.x_frdm-mcxn947_p2site_26.6.0.*.zip`

| Component | Path in SDK | Size | Meaning |
|---|---|---|---|
| Neutron driver | `middleware/eiq/neutron/mcxn/libNeutronDriver.a` | 21 KB | MCX N build, present |
| Neutron firmware | `middleware/eiq/neutron/mcxn/libNeutronFirmware.a` | 135 KB | NPU microcode runtime |
| TFLM runtime | `middleware/eiq/tensorflow-lite/lib/cm33/armgcc/libtflm.a` | 1.7 MB static | Links in far smaller |
| Neutron TFLM kernel | `.../micro/kernels/neutron/neutron.cpp` | 8 KB | The `NeutronGraph` custom op |
| Working example | `boards/frdmmcxn947/eiq_examples/tflm_cifar10/` | — | Runs on **this exact board** |
| Also present | `tflm_kws`, `tflm_label_image`, `tflm_modelrunner`, `tflm_lib` | — | |

**The entire runtime side is already available offline.** Extracted to `sdk_extract/` in
this folder for reference.

**What is missing:** `neutron-converter`, the *host* tool. It is not on public PyPI
(`pip download eiq-neutron-sdk` and `neutron-converter` both return "no matching
distribution"). It ships in the eIQ Neutron SDK / eIQ Toolkit behind an NXP account
login. As an NXP employee this is a non-issue for Eli, but it is a hard blocker for any
student or external contributor.

### Proof that the runtime is a solved path

`tflm_cifar10/model/model_cifarnet_ops_npu.cpp` — the entire op resolver for an
NPU-accelerated model on this board is four lines:

```cpp
static tflite::MicroMutableOpResolver<4> s_microOpResolver;
s_microOpResolver.AddPad();
s_microOpResolver.AddSoftmax();
s_microOpResolver.AddDequantize();
s_microOpResolver.AddCustom(tflite::GetString_NEUTRON_GRAPH(), tflite::Register_NEUTRON_GRAPH());
```

Everything else in the CIFAR-10 graph collapsed into one `NeutronGraph` node on the NPU.
Mixed NPU/CPU execution is the designed behavior, not a fallback.

---

## 2. The blocking-execution finding

This is the most important architectural fact, and it is not stated plainly in any NXP
documentation I could reach.

`NeutronDriver.h` declares non-blocking execution but annotates it *"only available for
Neutron-S."* The symbols nonetheless exist in the MCX N library, so the comment alone is
not conclusive. Disassembling the shipped archive resolves it:

| Function | Instructions | Reality |
|---|---|---|
| `privateNeutronRunBlocking` | 111 | Real implementation |
| `privateNeutronRunNonBlocking` | 3 | `movw r0, #0x8c0b; bx lr` |
| `privateNeutronWait` | 3 | `movw r0, #0x910b; bx lr` |
| `privateNeutronIsReady` | 3 | `movw r0, #0x960b; bx lr` |

Decoding those constants against `NeutronErrors.h`:

```
0x8c0b -> component=DRIVER  category=UNSUPPORTED
0x910b -> component=DRIVER  category=UNSUPPORTED
0x960b -> component=DRIVER  category=UNSUPPORTED
```

**Verification against a second target.** I checked the RT700 build of the same driver
from the public `nxp-mcuxpresso/mcuxsdk-middleware-eiq` repo. It has the *identical*
stubs. So this is not an MCX N deficiency — non-blocking execution is simply unimplemented
in the embedded driver builds generally. ("Neutron-S" appears to mean the Linux/applications-
processor variant.) The conclusion is unchanged, but state it accurately: **on any embedded
Neutron target in this SDK, `neutronRunBlocking` is the only working execution path.**

### Why this matters so much for AVC

The attractive framing — *"let the NPU prepare masks in the background while the CPU runs
control"* — **does not work as stated.** The core that calls into the NPU sits inside
`neutronRunBlocking` until the graph finishes.

**There is a workaround, and it is a good one:** the MCXN947 is dual-core and **core1 is
currently unused**. Running the NPU from core1 restores concurrency — core1 blocks, core0
keeps its 41 ms control loop. Results hand over through shared SRAM plus the mailbox.

But note what that implies: *if core1 is free anyway, core1 can just do the preprocessing
in plain C, with no TFLite, no converter, no version-lock, and no quantization.* For a
student platform that is a much better trade. The NPU only becomes compelling if the work
is genuinely convolution-heavy.

**Open question requiring hardware:** whether the Neutron driver initializes and runs
cleanly from core1 (interrupt routing, clock gating, `neutronInit()` ownership). Nothing
in the SDK examples demonstrates it — every eIQ example is `cm33_core0`.

---

## 3. Operator support — a correction worth flagging

The MCUXpresso SDK page `middleware/eiq/tensorflow-lite/docs/topics/supported_operators`
lists ~170 operators as supported. **That table is TFLM CPU-kernel coverage, not the NPU
subset.** It is easy to misread — I nearly did, and prior research in the bunny_vision
project cites the same page for NPU claims. Saved to `downloads/supported_operators.html`.

The NPU-accelerated subset is much smaller. From NXP KB material and corroborated by the
CIFAR-10 example's residual CPU ops, the Int8 per-channel-quantized set is approximately:

`CONV_2D`, `DEPTHWISE_CONV_2D`, `ADD`, `AVERAGE_POOL_2D`, `MAX_POOL_2D`,
`FULLY_CONNECTED`, `LOGISTIC`, plus (newer releases) `RESHAPE`, `PAD`, `SLICE`.
`MUL` support is version-dependent and disputed across sources.

**This list must be re-verified against the exact pinned SDK before designing any graph.**
NXP's own guidance is that only the eIQ Toolkit User Guide matching your SDK version is
authoritative. Anything not on the list silently falls back to the CM33 — correct results,
but a fragmented graph and a full feature-map round trip through shared SRAM at every
boundary.

**Quantization requirement:** int8 **per-channel** (symmetric int8 weights, int32 biases,
asymmetric int8 activations). Float and uint8-per-tensor are not accelerated. For
fixed-function kernels there is no calibration dataset — you choose the scales by hand.

---

## 4. Cost model against the 41 ms budget

**Budget:** 150 MHz ÷ 24 FPS = **6.25 M cycles/frame**.
**Frame:** 320 × 200 = **64,000 pixels** → **~98 cycles/pixel** if you touch every pixel
once and do nothing else. That is a *generous* budget, and it is the crux of this whole
assessment.

Current firmware footprint for reference (`arm-none-eabi-size`, Rev A EZH build):
`text 267 KB / 2048 KB flash`, `bss 325 KB / 512 KB RAM`. Flash is wide open; **RAM is
the constrained resource**, with the 2 × 128 KB camera ping-pong buffers dominating.

All figures below are **engineering estimates**, not measurements. They are order-of-
magnitude and intended to rank options, not to predict.

### Candidate A — per-pixel color classification (this is the 2026 twist)

| Approach | Cost | Notes |
|---|---|---|
| **CPU, 64 KB LUT** | **~3 ms** (~7% budget) | One `ldrh` + one `ldrb` per pixel |
| CPU, naive float HSL | ~60–100 ms | **Blows the budget** — current starter code only survives because it does one line |
| NPU, 1×1 conv 3→N | ~0.4 ms compute, **but ~4–5 ms CPU just to build the input tensor** | Marshalling dominates |

**The NPU loses outright.** Unpacking RGB565 into planar int8 costs more than the entire
LUT solution. The accelerator cannot help with bit-unpacking, which is the actual work.

### Candidate B — spatial convolution over the full frame (blur, Sobel, morphology)

| Approach | Cost | Notes |
|---|---|---|
| CPU, separable + SMLAD | ~8 ms per 3×3 stage | Fits, but stacks up fast |
| NPU | ~1–3 ms per stage | Plus one-time ~2 ms input marshalling |

**Roughly a tie for one filter; the NPU pulls ahead at three or more stacked stages**,
because marshalling amortizes. Note the utilization caveat: a 1-channel 3×3 convolution
cannot fill the 16-MAC array's input-channel parallelism, so the theoretical 4.8 GOPS is
not remotely achievable on this shape.

### Candidate C — a real CNN (steering regression or obstacle classification)

This is what Neutron is actually built for: multi-channel, multi-layer, high MAC
utilization, plausibly 10–25× over the CM33. It is also the only option that requires a
**training dataset, a training pipeline, and generalization to a track whose materials,
geometry, and lighting change every year and are not known until you are in the hall in
Guatemala.** For a 3-day event with novice tuners, this is a research project, not a
deliverable.

---

## Recommendation

### For the 2026 race — do this instead of the NPU

**Switch the sensor to YUY2 and classify color from a chroma-indexed lookup table.**

This is a better answer on every axis that matters, and it directly uses the YUV idea
already under consideration.

1. **YUY2 is a two-register change.** The in-tree OV5640 driver already supports it —
   `OV5640_SetPixelFormat()` in `video/fsl_ov5640.c` writes `0x4300=0x3F, 0x501f=0x00`
   for `kVIDEO_PixelFormatYUYV`. Capture is byte-moving and format-agnostic.
2. **Y comes for free.** Edge detection currently pays a full RGB565→luminance conversion
   per pixel. In YUY2 the Y plane is *already there* at stride 2. The black-edge detection
   that the whole track depends on gets cheaper and more accurate at the same time.
3. **Chroma classification is illumination-invariant.** Index a LUT by the `(U,V)` byte
   pair — 65,536 entries × 1 byte = **64 KB, in flash**, where there is ample room. One
   load per pixel returns a class. Because U/V are already brightness-normalized, a green
   lane reads as green under the hall's lighting *and* under a shadow, which an RGB or
   raw-HSL threshold will not reliably do.
4. **It is more expressive than anything the NPU would give you.** A LUT encodes
   *arbitrary* decision regions in chroma space. A 1×1 convolution can only encode a
   linear boundary.
5. **It is a better teaching artifact.** Students design the classifier visually on the
   PC — which is exactly what the existing USB high-speed / Web Serial frame path is for
   — and generate a header. The feedback loop is: point the camera at the real track,
   see the chroma scatter, paint the class regions, regenerate, flash. That teaches color
   spaces, quantization, and calibration far better than an opaque precompiled graph.

**Cost check:** ~3 ms of a 41 ms budget for a full frame, or well under 1 ms for the
handful of scanlines actually needed. No new toolchain, no version-lock, no NPU.

**The one real tradeoff to flag:** RGB565 was chosen because it blits straight to the LCD.
YUY2 means the live camera view needs a YUV→RGB565 conversion for display. Options:
convert only the displayed region, convert at reduced rate, or keep display on a
downsampled path. **This needs to be measured before committing** — it could cost more
than the color classification saves, and the live LCD view is a debugging feature students
depend on.

### For the NPU specifically

**Do not put it on the critical path for August.** Reasons, in order:

- The converter must *exactly* version-match the SDK's eIQ runtime, or the model is
  rejected at runtime. The failure string is already in the shipped kernel source:
  *"Incompatible Neutron NPU microcode and driver versions! Please, convert the model with
  Neutron converter tool intended for this SDK release."* Pinning that across an event
  where boards may get reflashed is an avoidable risk.
- It adds TFLM to the link (flash is fine; **RAM is not** — the tensor arena competes with
  256 KB of camera buffers, and a 320×200 int8 plane is 64 KB apiece).
- Driving it from core1 — necessary to avoid stalling control — is undemonstrated in any
  SDK example.
- The workload the race actually needs is pointwise, which is the NPU's worst case.

**If you want to pursue it anyway** (and there is legitimate value — it is the marquee
MCX N feature and students would find it compelling), the lowest-risk shape is:

- **An opt-in build flag** — `CONFIG__VISION_BACKEND_NPU` — defaulting off, physically
  unable to affect the competition image. This matches how `CONFIG__CAMERA_CAPTURE_BACKEND`
  and `CONFIG__MOTOR_ENCODER_BACKEND` already gate risky subsystems.
- **Band/tile processing, not full frame.** Process 320×32 strips → 10 KB tensors. This
  sidesteps the RAM squeeze and matches the fact that only a few scanlines matter.
- **A multi-stage fixed-weight graph, not a single filter** — blur → Sobel x/y →
  magnitude — so marshalling amortizes across stages and the NPU actually wins.
- **Validate numerically first.** Fixed weights make the output exactly predictable, so
  diff the NPU result against a host reference implementation before trusting it on the
  car.

### Decision point

**Set a go/no-go around 2026-08-07.** If a Neutron prototype has not demonstrated
(a) init and run from core1, (b) end-to-end frame cost inside budget, and (c) a stable
converter/SDK pin by that date, ship the LUT pipeline and treat Neutron as a post-race
demo. The race is the deliverable; the NPU is not.

---

## Open questions that only hardware answers

1. Does `neutronInit()` / `neutronModelPrepare()` / `neutronRunBlocking()` work when
   called from **core1**? Interrupt routing and clock ownership are unverified.
2. What is the real per-invoke `NeutronGraph` overhead at 24 Hz? Fixed cost per invoke
   may dominate everything for small tensors.
3. Does `neutron-converter` accept and efficiently map a **1-channel, non-square,
   320×200** (or 320×32 band) input with 3×3 convs? All published examples are square
   multi-channel classifier inputs.
4. Is `MUL` NPU-accelerated in the pinned SDK? Sources conflict.
5. What does YUV→RGB565 display conversion actually cost, and does it erase the YUY2 win?
6. Does NPU AHB traffic contend measurably with EZH camera capture and LCD refresh, which
   share the bus matrix?

---

## Provenance and confidence

**High confidence (verified locally on this machine):** SDK contents and paths; the
blocking-execution stubs and their error codes (disassembly); the CIFAR-10 op resolver;
OV5640 YUYV support; current flash/RAM footprint; frame buffer layout and the fact that
`camera_view.Data` aliases the live camera buffer.

**Medium confidence (documentation, version-dependent):** the NPU operator subset;
quantization requirements; converter CLI flags.

**Estimates, explicitly not measured:** every cycle-count and millisecond figure in
§4. They are intended for ranking options only.

**Not retrievable:** `nxp.com` serves a 746-byte block page to all automated fetchers.
AN14700, the MCXN datasheet, the MCX N fact sheet, and the NXP Cup rules PDF all failed
this way regardless of user agent, as did the community.nxp.com attachment host (403).
Anything needed from those must be downloaded manually through a browser session.

**Prior art reused:** `D:\prj\wavenumber\bunny_vision\bunny_vision_firmware-west\bunny_vision_sw\docs\research\`
— `neutron_npu_edge_detect_research.md` and its synthesis JSON. Different project, same
silicon. Its silicon and toolchain findings transfer directly; its *recommendation* does
not, because bunny_vision's edge-detect would own the NPU outright with no hard frame
deadline, while AVC has a 41 ms control loop and a competing CPU solution that is simply
better for pointwise work. That report also did not identify the blocking-execution
constraint.
