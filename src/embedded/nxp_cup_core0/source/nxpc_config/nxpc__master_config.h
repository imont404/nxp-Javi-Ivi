
#ifndef NXPC_CAMERA__MASTER_CONFIG_H_
#define NXPC_CAMERA__MASTER_CONFIG_H_


#define CAMERA__OV7670							1
#define CAMERA__OV5640  						2
#define CAMERA__OV5460  						CAMERA__OV5640

//supported OV5640:
//FSL_VIDEO_RESOLUTION(320,200):
//FSL_VIDEO_RESOLUTION(320,102):
//FSL_VIDEO_RESOLUTION(160,120):
//FSL_VIDEO_RESOLUTION(320,240):

#define CONFIG__CAMERA_SELECT					(CAMERA__OV5640)

#define CAMERA_CAPTURE_BACKEND_SMARTDMA_EZH		1
#define CAMERA_CAPTURE_BACKEND_FLEXIO_DIAG		2
#define CAMERA_CAPTURE_BACKEND_FLEXIO_EDMA		3
#define CAMERA_CAPTURE_BACKEND_FLEXIO_PIPELINE_DIAG	4

#ifndef CONFIG__CAMERA_CAPTURE_BACKEND
#define CONFIG__CAMERA_CAPTURE_BACKEND			(CAMERA_CAPTURE_BACKEND_SMARTDMA_EZH)
#endif

/*
 * FlexIO camera pin group.
 *
 * Selects which set of pins the FlexIO eDMA capture path drives. Only meaningful
 * when CONFIG__CAMERA_CAPTURE_BACKEND selects a FlexIO backend.
 *
 * PORT4 is the original, proven group. It needs eleven fly-wires because Port 4
 * is not where the Rev A camera is routed.
 *
 * PORT1 reuses the Rev A camera wiring and needs only three jumpers on J9_EXT:
 *   J9_EXT.12 (P3_4 cam D4) -> J9_EXT.32 (P1_8)
 *   J9_EXT.11 (P3_5 cam D5) -> J9_EXT.30 (P1_9)
 *   J9_EXT.15 (P0_5 PCLK)   -> J9_EXT.2  (P1_14)
 * After those jumpers P1_4..P1_11 is contiguous FLEXIO0_D12..D19 and contiguous
 * SMARTDMA_PIO0..PIO7, so EZH versus FlexIO becomes alt7 versus alt6 on one
 * harness. See docs/research/AVC_Camera_FlexIO_Pin_Migration.md.
 *
 *                    | PORT4 group          | PORT1 group
 *   D0-D7            | P4_12..P4_19 D20..D27| P1_4..P1_11 D12..D19
 *   PCLK             | P4_20        D28     | P1_14        D22
 *   HREF             | P4_21        D29     | P0_11        D3
 *   VSYNC (GPIO IRQ) | P4_22        GPIO4   | P0_4         GPIO0
 */
#define CAMERA_FLEXIO_PIN_GROUP_PORT4			1
#define CAMERA_FLEXIO_PIN_GROUP_PORT1			2

#ifndef CONFIG__CAMERA_FLEXIO_PIN_GROUP
#define CONFIG__CAMERA_FLEXIO_PIN_GROUP			(CAMERA_FLEXIO_PIN_GROUP_PORT4)
#endif

#if CONFIG__CAMERA_FLEXIO_PIN_GROUP != CAMERA_FLEXIO_PIN_GROUP_PORT4 && \
    CONFIG__CAMERA_FLEXIO_PIN_GROUP != CAMERA_FLEXIO_PIN_GROUP_PORT1
#error Invalid CONFIG__CAMERA_FLEXIO_PIN_GROUP.
#endif

/*
 * The PORT1 group is only wired for the FlexIO eDMA capture path. The older
 * FlexIO signal/pipeline diagnostics remain Port 4 only.
 */
#if CONFIG__CAMERA_FLEXIO_PIN_GROUP == CAMERA_FLEXIO_PIN_GROUP_PORT1 && \
    CONFIG__CAMERA_CAPTURE_BACKEND != CAMERA_CAPTURE_BACKEND_FLEXIO_EDMA
#error CAMERA_FLEXIO_PIN_GROUP_PORT1 requires CONFIG__CAMERA_CAPTURE_BACKEND == CAMERA_CAPTURE_BACKEND_FLEXIO_EDMA.
#endif

#define CONFIG__CAMERA_RESOLUTION_X				(320)

#define CONFIG__CAMERA_RESOLUTION_Y				(200)

#define CONFIG__CAMERA_RESOLUTION				FSL_VIDEO_RESOLUTION(320, 200)

/*
 * Rev A competition baseline:
 * - camera capture through the original SmartDMA/EZH path
 * - original ER-TFT020-3 LCD over the existing SPI display path
 * - alternate ER-TFT020-7 TE and parallel bit-bang experiments disabled
 * - USB remote display stream disabled unless explicitly selected and retested
 *   with the SPI LCD frame dump path active
 *
 * Both supported LCD panels use the same NXP Cup LCD SPI pins in serial mode.
 * Change CONFIG__DISPLAY_PANEL to DISPLAY_PANEL_ER_TFT020_7 only for the
 * alternate BuyDisplay 2.0 inch 240x320 ST7789 panel.
 */
#define DISPLAY_PANEL_ER_TFT020_3				1
#define DISPLAY_PANEL_ER_TFT020_7				2

#ifndef CONFIG__DISPLAY_ENABLE
#define CONFIG__DISPLAY_ENABLE					(1)
#endif

#ifndef CONFIG__DISPLAY_PANEL
#define CONFIG__DISPLAY_PANEL					(DISPLAY_PANEL_ER_TFT020_3)
#endif

#ifndef CONFIG__DISPLAY_TE_ENABLE
#define CONFIG__DISPLAY_TE_ENABLE				(0)
#endif

#ifndef CONFIG__DISPLAY_TEST_MODE
#define CONFIG__DISPLAY_TEST_MODE				(0)
#endif

#ifndef CONFIG__DISPLAY_PARALLEL_BITBANG_TEST_MODE
#define CONFIG__DISPLAY_PARALLEL_BITBANG_TEST_MODE	(0)
#endif

#ifndef CONFIG__DISPLAY_PARALLEL_BITBANG_WR_DELAY_CYCLES
#define CONFIG__DISPLAY_PARALLEL_BITBANG_WR_DELAY_CYCLES	(16U)
#endif

#ifndef CONFIG__DISPLAY_PARALLEL_BITBANG_FRAME_DELAY_MS
#define CONFIG__DISPLAY_PARALLEL_BITBANG_FRAME_DELAY_MS		(250U)
#endif

#if CONFIG__DISPLAY_PANEL != DISPLAY_PANEL_ER_TFT020_3 && CONFIG__DISPLAY_PANEL != DISPLAY_PANEL_ER_TFT020_7
#error Invalid CONFIG__DISPLAY_PANEL.
#endif

#if CONFIG__DISPLAY_ENABLE != 0 && CONFIG__DISPLAY_ENABLE != 1
#error CONFIG__DISPLAY_ENABLE must be 0 or 1.
#endif

#if !CONFIG__DISPLAY_ENABLE && (CONFIG__DISPLAY_TEST_MODE || CONFIG__DISPLAY_TE_ENABLE || \
                                CONFIG__DISPLAY_PARALLEL_BITBANG_TEST_MODE || \
                                CONFIG__DISPLAY_TIMING_DIAG_ENABLE || \
                                CONFIG__DISPLAY_SCOPE_MARKER_ENABLE)
#error Display diagnostics require CONFIG__DISPLAY_ENABLE=1.
#endif

#if CONFIG__DISPLAY_TE_ENABLE && CONFIG__DISPLAY_PANEL != DISPLAY_PANEL_ER_TFT020_7
#error CONFIG__DISPLAY_TE_ENABLE is only valid for the alternate ER-TFT020-7 panel.
#endif

#if CONFIG__DISPLAY_PARALLEL_BITBANG_TEST_MODE && CONFIG__DISPLAY_PANEL != DISPLAY_PANEL_ER_TFT020_7
#error CONFIG__DISPLAY_PARALLEL_BITBANG_TEST_MODE is only valid for the alternate ER-TFT020-7 panel.
#endif

/*
 * Servo PWM output selection.
 *
 * Rev A intended P2_2/PWM1_A2 as the main servo output, but P2_2 is also
 * CAMERA_CLKIN - one MCU pin landing on both the camera header and the
 * Arduino-style header - so the aux P2_3/PWM1_B2 was used instead. Both buffers
 * (U9 and U10) are fitted.
 *
 * P3_20/PWM1_A3 is a candidate for Rev B: PWM1 submodule 3 is entirely unused,
 * and P3_20 already reaches the shield at J1.5 with nothing connected to it.
 * Selecting it here drives that header pin so it can be verified with a scope
 * before committing to a board respin.
 *
 * See docs/research/AVC_RevB_Servo_PWM_Options.md.
 *
 *   P2_3_B2  PWM1 submodule 2, channel B, alt5 - as built on Rev A (shield J3.5)
 *   P3_20_A3 PWM1 submodule 3, channel A, alt5 - Rev B candidate (shield J1.5)
 */
#define SERVO_PWM_OUTPUT_P2_3_B2				1
#define SERVO_PWM_OUTPUT_P3_20_A3				2

#ifndef CONFIG__SERVO_PWM_OUTPUT
#define CONFIG__SERVO_PWM_OUTPUT				(SERVO_PWM_OUTPUT_P2_3_B2)
#endif

#if CONFIG__SERVO_PWM_OUTPUT != SERVO_PWM_OUTPUT_P2_3_B2 &&     CONFIG__SERVO_PWM_OUTPUT != SERVO_PWM_OUTPUT_P3_20_A3
#error Invalid CONFIG__SERVO_PWM_OUTPUT.
#endif

#ifndef CONFIG__USB_DEBUG_STREAM_ENABLE
#define CONFIG__USB_DEBUG_STREAM_ENABLE			(1)
#endif

/* Bench-only DWT timing report for nxpc_usb_debug_stream__service(). */
#ifndef CONFIG__USB_DEBUG_PROFILE_ENABLE
#define CONFIG__USB_DEBUG_PROFILE_ENABLE			(0)
#endif

#if CONFIG__USB_DEBUG_PROFILE_ENABLE && !CONFIG__USB_DEBUG_STREAM_ENABLE
#error CONFIG__USB_DEBUG_PROFILE_ENABLE requires CONFIG__USB_DEBUG_STREAM_ENABLE.
#endif

/* EZH and FlexIO camera capture both use the same ping-pong frame storage. */
#define CONFIG__CAMERA_FRAME_BUFFER_COUNT		(2U)

/*
 * Display SPI timing diagnostic:
 * Instruments eGFX_DumpRaw with the cycle counter and reports where the frame
 * dump time goes. Measurement only - it changes no behavior - but it costs a
 * few cycles per phase and prints over RTT, so it stays off in the competition
 * image. See docs/research/AVC_LCD_SPI_Design.md.
 */
#ifndef CONFIG__DISPLAY_TIMING_DIAG_ENABLE
#define CONFIG__DISPLAY_TIMING_DIAG_ENABLE		(0)
#endif

/*
 * Scope marker for the frame dump, on P4_1.
 *
 * Drives P4_1 high for the whole per-frame LCD dump - both eGFX_DumpRaw calls -
 * and low between. Pulse width is the total dump time, pulse period is the
 * frame period, both read directly off a scope.
 *
 * This exists because the software instrumentation cannot be trusted: it reads
 * the DWT cycle counter, and nxpc__next_frame() zeroes that counter on every
 * camera frame. A GPIO edge does not care.
 *
 * P4_1 is already configured as an output in pin_mux.c, and P4_0 is used the
 * same way by the memcpy DMA path.
 */
#ifndef CONFIG__DISPLAY_SCOPE_MARKER_ENABLE
#define CONFIG__DISPLAY_SCOPE_MARKER_ENABLE		(0)
#endif

/* Report once per this many eGFX_DumpRaw calls. Two calls per frame. */
#ifndef CONFIG__DISPLAY_TIMING_DIAG_REPORT_CALLS
#define CONFIG__DISPLAY_TIMING_DIAG_REPORT_CALLS	(48)
#endif

/*
 * Force test mode regardless of the TEST switch (P3_18).
 *
 * The LCD refresh path only runs in test mode, so measuring it otherwise needs
 * someone at the bench to hold a switch. This is for diagnostic builds run
 * remotely.
 *
 * It does not make the car move: testmode__motors_enable starts at 0 and is
 * only set by a center-button press, so the motor branch stays unreachable
 * without someone physically at the board.
 */
#ifndef CONFIG__FORCE_TEST_MODE
#define CONFIG__FORCE_TEST_MODE				(0)
#endif

/*
 * Bench-only deterministic non-moving mode for a bare FRDM board. The shield's
 * TEST and center-button inputs are otherwise electrically absent and may float.
 * This keeps the system in RACE_WAITING while a recognized USB session previews
 * the camera. It must never be selected by the competition preset.
 */
#ifndef CONFIG__FORCE_RACE_WAITING_MODE
#define CONFIG__FORCE_RACE_WAITING_MODE		(0)
#endif

#if CONFIG__FORCE_TEST_MODE && CONFIG__FORCE_RACE_WAITING_MODE
#error Test mode and race-waiting mode cannot both be forced.
#endif

/*
 * Display SPI clock.
 *
 * The LPSPI source is PLLCLKDIV, which is PLL0 (150 MHz) divided by
 * CONFIG__DISPLAY_SPI_PLLCLKDIV. LPSPI cannot divide by less than two, so:
 *
 *   PLLCLKDIV 2 -> 75 MHz source  -> SCK ceiling 37.5 MHz
 *   PLLCLKDIV 1 -> 150 MHz source -> SCK ceiling 75 MHz
 *
 * The source tree attaches PLLCLKDIV only to FLEXCOMM1 for this display, but a
 * runtime change to divider 1 produced a black panel even when SCK was divided
 * back to 37.5 MHz. The cause is unresolved. Divider 2 at 37.5 MHz is the only
 * competition-verified configuration; do not change it without bench evidence.
 * See docs/research/AVC_LCD_SPI_Design.md.
 */
#ifndef CONFIG__DISPLAY_SPI_PLLCLKDIV
#define CONFIG__DISPLAY_SPI_PLLCLKDIV			(2)
#endif

/*
 * Requested SCK. The driver reports a register-derived diagnostic estimate
 * using srcClock / (2^PRESCALE * (SCKDIV + 2)). Validate timing claims with
 * measured transfer duration or the P4_1 scope marker.
 */
#ifndef CONFIG__DISPLAY_SPI_BAUD_HZ
#define CONFIG__DISPLAY_SPI_BAUD_HZ			(50000000)
#endif

/*
 * Explicit LPSPI SCKDIV. -1 leaves the SDK baud-rate search in charge.
 * From a 150 MHz source: 0 -> 75 MHz, 1 -> 50 MHz, 2 -> 37.5 MHz.
 */
#ifndef CONFIG__DISPLAY_SPI_SCKDIV
#define CONFIG__DISPLAY_SPI_SCKDIV			(-1)
#endif

/*
 * Rev A QDC wheel feedback is standard firmware hardware, not a selectable
 * backend. It was bench-proven with the EZH camera, SPI LCD, and USB telemetry.
 */

#ifndef CONFIG__MOTOR_ENCODER_DIAG_ENABLE
#define CONFIG__MOTOR_ENCODER_DIAG_ENABLE		(0)
#endif

#ifndef CONFIG__MOTOR_ENCODER_DIAG_MOTOR_ENABLE
#define CONFIG__MOTOR_ENCODER_DIAG_MOTOR_ENABLE	(0)
#endif

#ifndef CONFIG__MOTOR_ENCODER_DIAG_PWM_PERCENT
#define CONFIG__MOTOR_ENCODER_DIAG_PWM_PERCENT	(12)
#endif

/*
 * Per-motor diagnostic PWM. Default to the shared value so existing behaviour
 * is unchanged. Setting them differently is a deliberate test: if M0 and M1 are
 * driven at different duties and the two QDC channels report proportionally
 * different rates, the channels are provably independent and correctly
 * associated - a cross-wiring or duplicate-read fault cannot survive it, where
 * driving both at one duty would hide exactly that.
 */
#ifndef CONFIG__MOTOR_ENCODER_DIAG_PWM_PERCENT_M0
#define CONFIG__MOTOR_ENCODER_DIAG_PWM_PERCENT_M0	(CONFIG__MOTOR_ENCODER_DIAG_PWM_PERCENT)
#endif

#ifndef CONFIG__MOTOR_ENCODER_DIAG_PWM_PERCENT_M1
#define CONFIG__MOTOR_ENCODER_DIAG_PWM_PERCENT_M1	(CONFIG__MOTOR_ENCODER_DIAG_PWM_PERCENT)
#endif

/*
 * Optional auto-start for the motors-on diagnostic, in milliseconds after the
 * diagnostic loop begins. 0 means button-only, which is the default.
 *
 * This exists because a scripted RTT capture window and a human pressing a
 * button race each other, and the capture usually wins. Auto-start makes the
 * measurement repeatable. It is still gated behind
 * CONFIG__MOTOR_ENCODER_DIAG_MOTOR_ENABLE and the 20 percent duty cap, and the
 * button still toggles afterwards. Car on blocks.
 */
#ifndef CONFIG__MOTOR_ENCODER_DIAG_AUTOSTART_MS
#define CONFIG__MOTOR_ENCODER_DIAG_AUTOSTART_MS	(0U)
#endif

#ifndef CONFIG__MOTOR_ENCODER_DIAG_REPORT_MS
#define CONFIG__MOTOR_ENCODER_DIAG_REPORT_MS	(250U)
#endif

/*
 * Encoder counts per wheel revolution.
 *
 * MEASURED 2026-07-25: 13188 counts over exactly 10 hand-turned revolutions,
 * i.e. 1318.8/rev, which is 1320 to within 0.09 percent. Consistent with
 * gear_ratio 30 x 11 encoder lines x 4 (quadrature) = 1320.
 *
 * This settles a vendor contradiction. Hiwonder's spec table states 1:90,
 * which would give 3960; their support answer states 1:30 giving 1320. The
 * measurement backs the support answer.
 *
 * Note the rated-speed argument points the wrong way and should not be trusted
 * here: the motor is rated 110 rpm no-load, and at 10 percent duty the wheel
 * turns about 31 rpm rather than the ~11 rpm a linear reading would predict.
 * PWM duty is strongly non-linear in speed at the low end.
 */
#ifndef CONFIG__MOTOR_ENCODER_COUNTS_PER_WHEEL_REV
#define CONFIG__MOTOR_ENCODER_COUNTS_PER_WHEEL_REV	(1320U)
#endif

/*
 * Encoder direction polarity, per motor.
 *
 * MEASURED 2026-07-25: with both QDC blocks in their default direction, driving
 * the car FORWARD produces NEGATIVE counts. Position also counts down from zero
 * and wraps to ~4.29 billion, which is both confusing and awkward for anything
 * position-based.
 *
 * Setting these applies the QDC CTRL[REV] bit so forward motion counts up and
 * reads positive. Fixing it in hardware rather than negating in software keeps
 * the raw position register meaningful.
 *
 * Per-motor because a mirrored motor mounting is a normal thing to encounter;
 * on this chassis both need inverting.
 */
#ifndef CONFIG__MOTOR_ENCODER_INVERT_M0
#define CONFIG__MOTOR_ENCODER_INVERT_M0				(1)
#endif

#ifndef CONFIG__MOTOR_ENCODER_INVERT_M1
#define CONFIG__MOTOR_ENCODER_INVERT_M1				(1)
#endif

/*
 * Wheel diameter, for converting encoder rate to ground speed.
 * 75 mm, equal to the chassis ground-to-top-plate height.
 * See docs/car_chassis/README.md.
 */
#ifndef CONFIG__MOTOR_ENCODER_WHEEL_DIAMETER_MM
#define CONFIG__MOTOR_ENCODER_WHEEL_DIAMETER_MM		(75U)
#endif

#if CONFIG__DISPLAY_PARALLEL_BITBANG_TEST_MODE
#error Standard QDC wheel feedback is mutually exclusive with alternate parallel LCD bit-bang mode.
#endif

#if CONFIG__MOTOR_ENCODER_DIAG_PWM_PERCENT < 0 || CONFIG__MOTOR_ENCODER_DIAG_PWM_PERCENT > 20
#error CONFIG__MOTOR_ENCODER_DIAG_PWM_PERCENT must stay between 0 and 20 for the bench diagnostic.
#endif

#if CONFIG__MOTOR_ENCODER_DIAG_PWM_PERCENT_M0 < 0 || CONFIG__MOTOR_ENCODER_DIAG_PWM_PERCENT_M0 > 20
#error CONFIG__MOTOR_ENCODER_DIAG_PWM_PERCENT_M0 must stay between 0 and 20 for the bench diagnostic.
#endif

#if CONFIG__MOTOR_ENCODER_DIAG_PWM_PERCENT_M1 < 0 || CONFIG__MOTOR_ENCODER_DIAG_PWM_PERCENT_M1 > 20
#error CONFIG__MOTOR_ENCODER_DIAG_PWM_PERCENT_M1 must stay between 0 and 20 for the bench diagnostic.
#endif


//#define CONFIG__OV7670_IS_160x120 (1)           // This only applies to the OV7670

#if CONFIG__CAMERA_SELECT == CAMERA__OV7670

#if CONFIG__OV7670_IS_160x120 == 1
#define DEMO_BUFFER_WIDTH 120
#define DEMO_BUFFER_HEIGHT 160
#define DEMO_CAMERA_RESOLUTION kVIDEO_ResolutionQQVGA /* 160x120 */
#define DEMO_SMARTDMA_API kSMARTDMA_CameraWholeFrameQVGA
#else
#define DEMO_BUFFER_WIDTH 240
#define DEMO_BUFFER_HEIGHT 320
#define DEMO_CAMERA_RESOLUTION kVIDEO_ResolutionQVGA /* 320*240 */
#define DEMO_SMARTDMA_API kSMARTDMA_CameraWholeFrameQVGA
#endif

#elif CONFIG__CAMERA_SELECT == CAMERA__OV5640

#else

#error Invalid camera selection.

#endif

#endif
