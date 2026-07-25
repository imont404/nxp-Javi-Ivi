# Neutron NPU — Toolchain and Workflow Reference

Practical companion to `AVC_Neutron_NPU_Assessment.md`. This is *how you would do it* if
the go/no-go lands on "go". Read the assessment first — the recommendation there is that
this stays off the critical path for August 2026.

---

## 1. Mental model

The NPU is **not** a DSP you call with a function pointer. There is no "convolve this
buffer" API. The only supported access path is:

```
  fixed weights you choose
            |
            v
  build a quantized int8 .tflite graph   (host, Python)
            |
            v
  neutron-converter --target mcxn94x     (host, offline CLI, NXP login to obtain)
            |
            v
  .tflite with NPU-mappable ops fused into a "NeutronGraph" custom node
  + optional C header (--dump-header-file-output)
            |
            v
  link into firmware: libtflm.a + libNeutronDriver.a + libNeutronFirmware.a
            |
            v
  TFLM interpreter Invoke() -> NeutronGraph kernel -> neutronRunBlocking()
```

Consequences worth internalizing:

- **No training is required.** The converter takes an *already-quantized* flatbuffer. You
  can hand-author Sobel or Gaussian weights and hand-pick quantization scales. There is no
  dataset, no calibration step, no GUI.
- **Everything the NPU can't do still runs** — as ordinary TFLM CPU kernels. A graph never
  fails for using an unsupported op; it just fragments and gets slower.
- **You pay a TFLM interpreter tax** on every frame regardless of how small the graph is.

---

## 2. Getting the converter

`neutron-converter` is a host CLI. It is **not on public PyPI** — verified:

```
pip download eiq-neutron-sdk    -> No matching distribution found
pip download neutron-converter  -> No matching distribution found
```

It ships inside the **eIQ Neutron SDK** zip (and historically the eIQ Toolkit), behind an
NXP account login at
`https://www.nxp.com/design/design-center/software/eiq-ai-development-environment/eiq-toolkit-for-end-to-end-model-development-and-deployment:EIQ-TOOLKIT#downloads`.

**Version pinning is not optional.** The converter must match the SDK's eIQ runtime
exactly. The mismatch failure is at *runtime*, not build time, and the exact message is
already in the shipped kernel (`sdk_extract/.../kernels/neutron/neutron.cpp:121`):

> `Incompatible Neutron NPU microcode and driver versions! Please, convert the model with Neutron converter tool intended for this SDK release.`

This project's SDK is **26.6.0** (`com.nxp.mcuxpresso.sdk.sdk_2.x_frdm-mcxn947_p2site_26.6.0.*`).
Record the converter version alongside it if you ever ship a model.

---

## 3. Converter invocation

From the SDK docs (`downloads/convert_model.html`, fetched 2026-07-25):

```bash
neutron-converter \
  --input  model_quant.tflite \
  --output model_quant_npu.tflite \
  --target mcxn94x \
  --dump-header-file-output
```

| Flag | Effect |
|---|---|
| `--target` | `mcxn94x` for this board. (`imxrt700` in NXP's own examples — don't copy blindly.) |
| `--dump-header-file-output` | Emits a `.h` with the model as a C array, for embedding in flash |
| `--dump-header-file-input` | Same for the input tensor, useful for bring-up |
| `--use-sequencer` | **RT700 only.** Neutron Sequencer bytecode mode. |
| `--fetch-constants-to-sram` | **RT700 only.** Weight prefetch from external flash. Mutually exclusive with `--use-sequencer`. |

The two RT700-only flags are worth knowing about precisely so nobody wastes time trying
them on MCX N.

---

## 4. Firmware-side integration

Three libraries and one kernel registration.

**CMake components** (paths relative to the SDK):

```
middleware/eiq/neutron/middleware_eiq_neutron.cmake
middleware/eiq/tensorflow-lite/middleware_eiq_tensorflow_lite_micro.cmake
middleware/eiq/tensorflow-lite/middleware_eiq_tensorflow_lite_micro_cmsis_nn.cmake
middleware/eiq/tensorflow-lite/tensorflow/lite/micro/kernels/neutron/middleware_eiq_tensorflow_lite_micro_neutron.cmake
```

**Op resolver** — model the shape on `tflm_cifar10`, which is the working reference for
this exact board. The whole thing:

```cpp
#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/kernels/neutron/neutron.h"

tflite::MicroOpResolver &MODEL_GetOpsResolver()
{
    static tflite::MicroMutableOpResolver<4> s_microOpResolver;
    s_microOpResolver.AddPad();
    s_microOpResolver.AddSoftmax();
    s_microOpResolver.AddDequantize();
    s_microOpResolver.AddCustom(tflite::GetString_NEUTRON_GRAPH(),
                                tflite::Register_NEUTRON_GRAPH());
    return s_microOpResolver;
}
```

**The resolver must list every op the converter left on the CPU.** Miss one and you get a
runtime failure to allocate tensors. The converter tells you which ops remain — read its
output rather than guessing.

**Note this project builds C, and TFLM is C++.** The eIQ integration is `.cpp`. The AVC
CMake build would need C++ enabled and a C-callable shim. Not hard, but not free either —
budget for it.

---

## 5. Memory planning

Current AVC footprint (`arm-none-eabi-size`, Rev A EZH build):

```
text 267,364   data 856   bss 324,740
```

Against `avc_core0_Debug_memory.ld`:

| Region | Origin | Size | Current use |
|---|---|---|---|
| `PROGRAM_FLASH0/1` | `0x0` | 2 × 1 MB | 267 KB — **lots of room** |
| `FRAME_BUFFERS` | `0x20000000` | 384 KB | 2 × 128 KB camera ping-pong + overlay + stack |
| `SRAMX` | `0x04000000` | 96 KB | |
| `SRAM_H` | `0x20060000` | 32 KB | |
| `USB_RAM` | `0x400ba000` | 4 KB | |

**Flash is fine. RAM is the problem.** Adding TFLM means:

- Interpreter + arena. A 320×200 int8 plane is **64 KB**; a three-stage graph with two
  live intermediates wants ~128–192 KB.
- Neutron microcode / weights / kernels buffers, all **16-byte aligned**.
- The NPU has only a **2 KB cache and no local memory** — it works out of shared SRAM over
  the AHB matrix, contending with EZH camera capture and LCD refresh.

**Therefore: process bands, not frames.** A 320×32 strip is a 10 KB tensor. This fits
comfortably, matches the fact that the car only needs a few scanlines, and keeps NPU
tensors out of contention with the camera buffers.

---

## 6. Hand-building a fixed-weight graph

Sketch, for the record — untested:

```python
import numpy as np, tensorflow as tf

# 3x3 Sobel-X as a fixed conv kernel, 1 in-channel, 1 out-channel, NHWC
kx = np.array([[-1,0,1],[-2,0,2],[-1,0,1]], np.float32).reshape(3,3,1,1)

inp = tf.keras.Input(shape=(32,320,1), batch_size=1)     # one band
out = tf.keras.layers.Conv2D(1,3,padding='same',use_bias=False,
                             kernel_initializer=tf.constant_initializer(kx))(inp)
m = tf.keras.Model(inp,out)

conv = tf.lite.TFLiteConverter.from_keras_model(m)
conv.optimizations = [tf.lite.Optimize.DEFAULT]
conv.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8]
conv.inference_input_type  = tf.int8
conv.inference_output_type = tf.int8
conv.representative_dataset = lambda: (
    [np.random.randint(-128,127,(1,32,320,1)).astype(np.int8).astype(np.float32)]
    for _ in range(10))
open('sobelx_int8.tflite','wb').write(conv.convert())
```

Then `neutron-converter --target mcxn94x --input sobelx_int8.tflite --output sobelx_npu.tflite --dump-header-file-output`.

Practical notes:

- Even with fixed weights, TFLite still wants a `representative_dataset` to pick
  *activation* scales. Feed it real camera bands, not noise, or the scales will be wrong
  and the int8 output will clip or lose resolution.
- **Cascaded convs requantize at every stage.** Characterize per-stage scales explicitly —
  this is where a hand-built filter graph silently degrades versus its float reference.
- Gradient magnitude has no `ABS` on the NPU. The construction
  `|g| ≈ ReLU(g) + ReLU(−g)` keeps it on-NPU using conv-fused activations plus `ADD`.
  Unverified against the converter.
- Validate numerically against a host reference **before** putting it on the car. Fixed
  weights make the expected output exact, modulo requantization — so a mismatch is a real
  bug, not noise.

---

## 7. Known-good starting point

Do not start from scratch. Start from `tflm_cifar10` for `frdmmcxn947`, extracted to
`sdk_extract/boards/frdmmcxn947/eiq_examples/tflm_cifar10/`. It is a working
NPU-accelerated build on this exact board. Get it running unmodified first, confirm the
NPU is actually engaged, *then* swap the model.

Also present in the SDK and worth knowing about:

| Example | Use |
|---|---|
| `tflm_cifar10` | Best starting point — small int8 CNN, NPU path |
| `tflm_label_image` | MobileNet; the source of NXP's ">26× vs CM33" datapoint |
| `tflm_modelrunner` | Runs models pushed over a link — useful for bench experiments |
| `tflm_kws` | Audio; not relevant here |

---

## 8. What is *not* retrievable by tooling

`nxp.com` returns a 746-byte block page to every automated fetcher regardless of user
agent. These all failed and must be downloaded through a browser session:

- `AN14700.pdf` — the Neutron application note (authoritative on operator support)
- `MCXNP184M150F70.pdf` — MCX N datasheet
- `MCXNFS.pdf` — MCX N fact sheet
- `NXP-Cup-2026-Rules.pdf` — and the 2024/2025 editions
- `community.nxp.com` attachment host returns 403 (lab guide PDFs, track configs)

What *did* work: `mcuxpresso.nxp.com/mcuxsdk/...` HTML docs via plain curl (saved in
`downloads/`), and the public GitHub mirror `nxp-mcuxpresso/mcuxsdk-middleware-eiq`.

**If you can grab AN14700 through a browser, drop it in `downloads/` — it is the single
most useful missing piece**, because it is authoritative on the NPU operator subset, which
is currently the weakest-confidence part of the assessment.
