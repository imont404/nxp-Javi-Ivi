+++
type = "plan"
id = "camera-sccb-robustness"
status = "pending"
created = "2026-07-24"

[[steps]]
id = "baseline-measure"
title = "Measure the current SCCB bus and record baseline timing"
status = "pending"

[[steps]]
id = "surface-failures"
title = "Stop discarding camera init status and report it over RTT"
status = "pending"
depends_on = ["baseline-measure"]

[[steps]]
id = "configurable-rate"
title = "Make the SCCB bit rate a configuration value and characterize slower rates"
status = "pending"
depends_on = ["surface-failures"]

[[steps]]
id = "retries"
title = "Add bounded retry and recovery to the SCCB register path"
status = "pending"
depends_on = ["surface-failures"]

[[steps]]
id = "on-screen-status"
title = "Show camera health on the LCD overlay"
status = "pending"
depends_on = ["surface-failures", "retries"]

[[steps]]
id = "design-doc-intent-audit"
title = "Audit design docs, ADRs, and requirements against implementation"
status = "pending"
depends_on = ["on-screen-status", "configurable-rate"]

[[steps]]
id = "test-runtime-impact-audit"
title = "Audit new test runtime impact"
status = "pending"
depends_on = ["on-screen-status", "configurable-rate"]

[[steps]]
id = "external-review"
title = "Obtain independent external review"
status = "pending"
depends_on = ["design-doc-intent-audit", "test-runtime-impact-audit"]

[[exit_criteria]]
id = "failures-are-visible"
title = "A failed camera bring-up produces an explicit RTT message and an on-screen indication"
status = "pending"

[[exit_criteria]]
id = "rate-configurable"
title = "SCCB bit rate is a named configuration value with a characterized safe setting"
status = "pending"

[[exit_criteria]]
id = "retries-bounded"
title = "SCCB retries are bounded, counted, and cannot hang or silently mask a dead bus"
status = "pending"

[[exit_criteria]]
id = "signoff"
title = "Focused signoff passes"
status = "pending"

[[exit_criteria]]
id = "design-doc-intent-audit"
title = "Design docs, ADRs, and requirements match implementation"
status = "pending"

[[exit_criteria]]
id = "test-runtime-impact-audit"
title = "New tests are listed and runtime impact is reviewed"
status = "pending"

[[exit_criteria]]
id = "external-review"
title = "Independent external review is complete"
status = "pending"
+++

# Camera SCCB rate, retries, and on-screen diagnostics

## Priority

**Lower priority. Sequence this after the `motor-encoder-qdc-bringup` work completes.**

## Purpose

Make OV5640 SCCB bring-up observable and resilient. Today a camera that fails to
configure produces **no message at all** — the failure is silent, and the only
symptom is a dead or wrong image. That is what turned the `avc-cam-rx` connector
seating issue into a suspected I2C problem during LVDS bring-up
(`docs/research/AVC_LVDS_Adapter.md` §0.6).

Three deliverables:

1. Confirm the SCCB bit rate is appropriate for the LVDS cable topology, and
   allow slowing it down.
2. Add retries and recovery so a marginal bus recovers instead of failing the
   whole camera.
3. Put camera health on the LCD so a bench operator sees the fault without RTT.

## Current State

Established by reading the tree on 2026-07-24:

- **Bus:** LPI2C7 through LP_FLEXCOMM7, functional clock FRO12M with
  `kCLOCK_DivFlexcom7Clk = 1` (12 MHz source).
  Set up in `camera__configure_i2c()` / `camera__i2c_init()`,
  `src/avc/avc_core0/source/avc_io/bv_camera__interface.c`.
- **Rate:** `camera__i2c_init()` uses `LPI2C_MasterGetDefaultConfig()` and does
  not override `baudRate_Hz`. The SDK default is **100 kHz**
  (`drivers/fsl_lpi2c.c`). The rate is therefore implicit — there is no
  `CONFIG__` knob for it.
- **Transfers:** `BOARD_Camera_I2C_SendSCCB` / `BOARD_Camera_I2C_ReceiveSCCB`
  in `board/board.c`, wired into `ov5640_resource_t` in
  `bv_camera__interface.c`.
- **Retries:** none. `video/fsl_sccb.c` (`SCCB_WriteReg`, `SCCB_ReadReg`,
  `SCCB_ModifyReg`, `SCCB_WriteMultiRegs`) is a straight pass-through with no
  retry, no backoff, and no recovery.
- **Status handling:** `camera__init_sensor()` calls
  `CAMERA_DEVICE_Init(&handle, &camconfig);` and **discards the return value**.
  This is the single highest-value fix in the plan — the sensor can fail to
  configure and the firmware proceeds as if it succeeded.
- **Signal path:** SCCB runs **single-ended** over the 50-pin flex — `SDA` on
  `J2.45`, `SCL` on `J2.47`, both reaching the FRDM-AVC through the 18-pin
  `J1` header (`J1.3` / `J1.4`). It is not LVDS-protected, so it is the most
  exposed camera signal on the link.

## Source Material

- `src/avc/avc_core0/source/avc_io/bv_camera__interface.c`
  (`camera__configure_i2c`, `camera__i2c_init`, `camera__init_sensor`)
- `src/avc/avc_core0/board/board.c`, `board/board.h`
  (`BOARD_Camera_I2C_*`, `BOARD_CAMERA_I2C_INSTANCE`)
- `src/avc/avc_core0/video/fsl_sccb.c`, `video/fsl_ov5640.c`
- `src/avc/avc_core0/drivers/fsl_lpi2c.c` (default `baudRate_Hz`)
- `src/avc/avc_core0/source/main.c` (`avc__update_overlay` — where on-screen
  status belongs)
- Hardware context and pinouts: `docs/research/AVC_LVDS_Adapter.md` §0
- Board repos: `avc-cam-tx` 11-10112, `avc-cam-rx` 11-10113,
  `avc-cam-flex` 11-10115 (see `AVC_LVDS_Adapter.md` §0.1 for URLs)

## Step Notes

### baseline-measure

- Scope `SCL`/`SDA` at the camera end with the LVDS link in place. Record actual
  bit rate, rise times, and whether the pull-ups are adequate over 12" of flex.
- Confirm where the SCCB pull-ups physically live (they are not on `avc-cam-rx`
  or `avc-cam-tx` per the as-built BOM — establish whether they are on the
  FRDM-AVC shield or the Adafruit OV5640 breakout). **This matters more than the
  bit rate**: a weak pull-up pulling a long single-ended line is the more likely
  root cause of marginal SCCB.
- Record the baseline before changing anything, so a slower rate can be shown to
  actually help rather than assumed to.

### surface-failures

- Check the `CAMERA_DEVICE_Init()` return in `camera__init_sensor()` and emit an
  explicit RTT error on failure. Do not proceed silently.
- Add a device-presence probe before full init — read the OV5640 chip ID
  (`0x300A`/`0x300B`, expect `0x56`/`0x40`) and log the value actually read.
  A wrong or `0xFF`/`0x00` read distinguishes "bus dead" from "sensor
  misconfigured", which is exactly the distinction that was missing during LVDS
  bring-up.
- Keep this step small and independent — it is useful on its own even if the
  rest of the plan is deferred.

### configurable-rate

- Add `CONFIG__CAMERA_SCCB_BAUD_HZ` to
  `source/avc_config/avc__master_config.h`, defaulting to the current effective
  100000 so the change is behavior-neutral until deliberately altered.
- Set `lpi2cConfig.baudRate_Hz` from it in `camera__i2c_init()`.
- Characterize at least 100 kHz and one slower rate against the baseline
  measurement. Record init time cost — the OV5640 register tables are long, so a
  slower bus directly lengthens boot.
- Decide and record whether the slower rate becomes the default, and why.

### retries

- Add bounded retry around the SCCB register accessors. Bounded is the
  requirement: retries must not be able to hang boot or mask a permanently dead
  bus.
- Count attempts and failures in module-scope counters so
  `avc_camera__service()` can report them, following the existing `cam_diag` /
  `cam_dma` RTT reporting style already in `bv_camera__interface.c`.
- Consider LPI2C bus recovery (clocking out a stuck slave) before a retry, but
  only if the baseline measurement shows stuck-bus events actually occur. Do not
  add recovery machinery speculatively.
- Distinguish transient (retry succeeded) from hard (retries exhausted) in both
  counters and messages.

### on-screen-status

- Extend the existing `top_info` overlay in `avc__update_overlay()`
  (`source/main.c`). The overlay already renders pot values, motor state, and
  CPU load in that band — camera status belongs alongside them.
- Show at minimum: camera init OK/FAIL, chip ID read, and SCCB retry/failure
  counts. Keep it terse; the band is 320x40 and already crowded.
- On a failed camera init the LCD currently shows whatever stale buffer content
  exists. Make the failure unambiguous on screen — that is the whole point for a
  bench operator without RTT attached.
- Watch the frame budget. This runs in the per-frame path that already reports
  CPU percentage; do not regress it.

## Open Questions

- Where are the SCCB pull-ups, and are they sized for 12" of single-ended flex?
  (Resolve in `baseline-measure` — this may make the rate change unnecessary.)
- Is 100 kHz actually marginal on this topology, or was the observed failure
  entirely the `J1` connector seating issue? The plan should be willing to
  conclude "rate was never the problem" and still keep the diagnostics.
- Should SCCB failure be fatal (halt with a clear message) or degraded
  (continue, display the fault, let the operator decide)? Competition use likely
  wants degraded-and-visible.
- Does the OV5640 need a re-init path at runtime if SCCB recovers, or is a
  power cycle the accepted remedy?
