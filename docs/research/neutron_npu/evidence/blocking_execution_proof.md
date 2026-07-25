# Evidence: Neutron NPU execution is blocking-only in the shipped SDK

**Claim:** On MCXN947 (and every embedded Neutron target in MCUXpresso SDK 26.6.0), the
only working NPU execution path is `neutronRunBlocking()`. The non-blocking API exists as
declared symbols but is stubbed to return `DRIVER / UNSUPPORTED`.

**Why it matters:** you cannot overlap NPU work with CPU work on the core that issues it.
Any "NPU prepares masks in the background" design must instead run the NPU from the
otherwise-idle **core1**.

**Verified:** 2026-07-25, locally, against the shipped binary. This is a binary-level
finding, not a documentation reading.

---

## Step 1 — the header hints but does not settle it

`sdk_extract/middleware/eiq/neutron/driver/include/NeutronDriver.h`:

```c
/// - Perform Neutron execution in blocking mode.
NeutronError neutronRunBlocking(NeutronModelHandle hdl, const NeutronDataConfig *dcfg);

/// - Perform Neutron execution in non-blocking mode.
/// - This functionality is only available for Neutron-S.
NeutronError neutronRunNonBlocking(NeutronModelHandle hdl, const NeutronDataConfig *dcfg);

/// - Wait (block) for Neutron completion.
/// - This functionality is only available for Neutron-S.
NeutronError neutronWait(NeutronModelHandle hdl, const NeutronDataConfig *dcfg);

/// - Query if the job is done by Neutron.
/// - This functionality is only available for neutronRunNonBlocking.
NeutronError neutronIsReady(NeutronModelHandle hdl, bool *isReady);
```

"Neutron-S" is undefined in the header, and the MCXN947's NPU is an N1-16. Inconclusive
on its own — and the symbols *are* exported from the MCX N library:

```bash
arm-none-eabi-nm -g --defined-only libNeutronDriver.a | grep -i 'run\|ready\|wait'
#   T neutronIsReady
#   T neutronRunBlocking
#   T neutronRunNonBlocking
#   T neutronWait
```

## Step 2 — instruction counts expose the stubs

```bash
cd docs/research/neutron_npu/sdk_extract/middleware/eiq/neutron/mcxn
arm-none-eabi-objdump -d libNeutronDriver.a | awk '
/^[0-9a-f]+ <[a-zA-Z]/ {name=$2; gsub(/[<>:]/,"",name); cur=name; cnt[name]=0; next}
cur && /^ *[0-9a-f]+:/ {cnt[cur]++}
END {for (k in cnt) if (k ~ /^privateNeutron(Run|Wait|IsReady)/) printf "%-40s %d\n", k, cnt[k]}' | sort
```

```
privateNeutronIsReady                    3
privateNeutronRunBlocking              111
privateNeutronRunNonBlocking             3
privateNeutronWait                       3
```

## Step 3 — the stub bodies

```
00000000 <privateNeutronRunNonBlocking>:
   0:	f648 400b 	movw	r0, #35851	@ 0x8c0b
   4:	4770      	bx	lr

00000000 <privateNeutronWait>:
   0:	f249 100b 	movw	r0, #37131	@ 0x910b
   4:	4770      	bx	lr

00000000 <privateNeutronIsReady>:
   0:	f249 600b 	movw	r0, #38411	@ 0x960b
   4:	4770      	bx	lr
```

Load a constant, return. No hardware access.

## Step 4 — decode the constants

Per `NeutronErrors.h`: 3 bits component, 5 bits category, 23 bits code. The driver builds
these with `GEN_NEUTRON_DRIVER_ERROR(category, __LINE__)`, so the code field is the source
line number.

| Value | Component | Category | Code (= source line) |
|---|---|---|---|
| `0x8c0b` | `DRIVER` (3) | `UNSUPPORTED` (1) | 140 |
| `0x910b` | `DRIVER` (3) | `UNSUPPORTED` (1) | 145 |
| `0x960b` | `DRIVER` (3) | `UNSUPPORTED` (1) | 150 |

Those line numbers land exactly on the `neutronRunNonBlocking` / `neutronWait` /
`neutronIsReady` declarations in the public header. Consistent and conclusive.

## Step 5 — falsification check against a second target

If these were MCX-N-specific gaps, the RT700 build should differ. It does not:

```bash
gh api repos/nxp-mcuxpresso/mcux-sdk-middleware-eiq/contents/neutron/rt700/cm33/libNeutronDriver.a \
  --jq '.download_url' | xargs curl -sSL -o rt700_libNeutronDriver.a
# same analysis ->
privateNeutronIsReady                    3
privateNeutronRunBlocking              111
privateNeutronRunNonBlocking             3
privateNeutronWait                       3
```

**Identical.** So the correct statement is *not* "MCX N lacks non-blocking NPU execution"
— it is **"non-blocking Neutron execution is unimplemented in the embedded driver builds
in this SDK release; `Neutron-S` refers to a different (Linux/applications-processor)
variant."**

The practical consequence for AVC is the same either way, but the accurate framing matters
if anyone later asks NXP about it or moves to a different part.

## Corroboration from the TFLM integration

`sdk_extract/.../micro/kernels/neutron/neutron.cpp` — the only execution call in the
TFLite Micro Neutron kernel's `Eval()`:

```cpp
error = neutronRunBlocking(neutron->model_handle, &neutron->data_config);
```

The framework itself only ever uses the blocking path. Nothing in the SDK exercises the
non-blocking API.

---

## Reproduction

Tools used (already installed with MCUXpresso IDE 25.6.136):
`C:\nxp\MCUXpressoIDE_25.6.136\ide\tools\bin\arm-none-eabi-{nm,objdump}.exe`

Source archive:
`C:\Users\EliHughes\mcuxpresso\02\SDKPackages\com.nxp.mcuxpresso.sdk.sdk_2.x_frdm-mcxn947_p2site_26.6.0.2019112514465708596082553675375.zip`

Extracted copies of the analyzed binaries and headers are in `../sdk_extract/`.

**Re-check this after any SDK upgrade.** It is a property of the shipped binary, not of
the silicon, and NXP could implement the non-blocking path in a later release — which
would meaningfully change the architecture recommendation in
`../AVC_Neutron_NPU_Assessment.md`.
