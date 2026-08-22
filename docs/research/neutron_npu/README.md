# eIQ Neutron NPU Research — MCXN947 / AVC

Dedicated research folder for the question: **can the MCXN947's eIQ Neutron NPU accelerate
AVC camera preprocessing for the 2026 color-track challenge?**

Reference material, not an active plan. Created 2026-07-25.

## Read in this order

| File | What it is |
|---|---|
| **[`AVC_Neutron_NPU_Assessment.md`](AVC_Neutron_NPU_Assessment.md)** | **Start here.** Findings, cost model against the 41 ms budget, recommendation, decision point. |
| [`Neutron_Toolchain_Workflow.md`](Neutron_Toolchain_Workflow.md) | How the toolchain actually works — converter, integration, memory planning, hand-built graphs. |
| [`evidence/blocking_execution_proof.md`](evidence/blocking_execution_proof.md) | Reproducible binary-level proof of the single most important constraint. |

## Answer in one paragraph

The toolchain is real and the runtime is already on this machine, but **the NPU is the
wrong tool for this year's problem.** NPU execution blocks the calling core (verified by
disassembly — the non-blocking API is stubbed), and the int8 tensor marshalling needed to
feed it costs more than doing per-pixel color classification on the CM33 outright. The
implemented competition alternative is a packed **RGB565-to-Y/H/S/V flash lookup
table**: one generic table access exposes luminance plus teachable hue/saturation/value
features without adding a toolchain or a finished classifier. The earlier YUY2/chroma-LUT
recommendation in the assessment is retained as historical analysis but is superseded for
2026 because RGB565 is already proven through the LCD, USB viewer, and Android relay.
Keep Neutron as an opt-in, off-critical-path post-race exploration.

## Folder contents

```
AVC_Neutron_NPU_Assessment.md     main deliverable
Neutron_Toolchain_Workflow.md     practical how-to
evidence/
  blocking_execution_proof.md     disassembly + falsification check
downloads/
  supported_operators.html        SDK docs (NOTE: TFLM CPU coverage, NOT the NPU subset)
  convert_model.html              neutron-converter CLI reference
sdk_extract/                      pulled from the installed SDK 26.6.0 package
  middleware/eiq/neutron/         driver + firmware libs, headers, cmake
  .../micro/kernels/neutron/      the NeutronGraph TFLM custom op
  boards/frdmmcxn947/eiq_examples/tflm_cifar10/       working NPU example, this board
  boards/frdmmcxn947/eiq_examples/tflm_label_image/   MobileNet reference
```

## Provenance

- **Local, high confidence:** everything in `sdk_extract/`, pulled from
  `com.nxp.mcuxpresso.sdk.sdk_2.x_frdm-mcxn947_p2site_26.6.0.*.zip`. The blocking-execution
  finding is binary analysis of the shipped `libNeutronDriver.a`.
- **Fetched:** `downloads/*.html` via curl from `mcuxpresso.nxp.com`.
- **Blocked:** `nxp.com` serves a 746-byte block page to all automated fetchers.
  AN14700, the MCX N datasheet and fact sheet, and the NXP Cup rules PDFs could not be
  retrieved and need a manual browser download. **AN14700 is the highest-value gap** —
  it is authoritative on the NPU operator subset.
- **Prior art:** `D:\prj\wavenumber\bunny_vision\...\docs\research\neutron_npu_*` —
  different project, same silicon. Silicon and toolchain findings transfer; the
  recommendation does not, and that report missed the blocking constraint.

## Related

- [`../AVC_Competition_Overview.md`](../AVC_Competition_Overview.md) — the constraints this
  assessment is judged against.
- [`../AVC_Vision_Pipeline_Design.md`](../AVC_Vision_Pipeline_Design.md) — implemented
  RGB565 Y/H/S/V LUT and the current competition disposition.
