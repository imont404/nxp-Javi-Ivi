
#ifndef AVC_CAMERA__MASTER_CONFIG_H_
#define AVC_CAMERA__MASTER_CONFIG_H_


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
 * Both supported LCD panels use the same AVC LCD SPI pins in serial mode.
 * Change CONFIG__DISPLAY_PANEL to DISPLAY_PANEL_ER_TFT020_7 only for the
 * alternate BuyDisplay 2.0 inch 240x320 ST7789 panel.
 */
#define DISPLAY_PANEL_ER_TFT020_3				1
#define DISPLAY_PANEL_ER_TFT020_7				2

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

#if CONFIG__DISPLAY_TE_ENABLE && CONFIG__DISPLAY_PANEL != DISPLAY_PANEL_ER_TFT020_7
#error CONFIG__DISPLAY_TE_ENABLE is only valid for the alternate ER-TFT020-7 panel.
#endif

#if CONFIG__DISPLAY_PARALLEL_BITBANG_TEST_MODE && CONFIG__DISPLAY_PANEL != DISPLAY_PANEL_ER_TFT020_7
#error CONFIG__DISPLAY_PARALLEL_BITBANG_TEST_MODE is only valid for the alternate ER-TFT020-7 panel.
#endif

#ifndef CONFIG__USB_DEBUG_STREAM_ENABLE
#define CONFIG__USB_DEBUG_STREAM_ENABLE			(0)
#endif

/*
 * Motor encoder bring-up:
 * Default firmware leaves the encoder hardware disabled. The QDC path is
 * selected by a separate diagnostic build so the normal competition image keeps
 * the verified EZH camera + SPI LCD behavior.
 */
#define MOTOR_ENCODER_BACKEND_DISABLED			0
#define MOTOR_ENCODER_BACKEND_QDC				1

#ifndef CONFIG__MOTOR_ENCODER_BACKEND
#define CONFIG__MOTOR_ENCODER_BACKEND			(MOTOR_ENCODER_BACKEND_DISABLED)
#endif

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

#ifndef CONFIG__MOTOR_ENCODER_COUNTS_PER_WHEEL_REV
#define CONFIG__MOTOR_ENCODER_COUNTS_PER_WHEEL_REV	(0U)
#endif

#if CONFIG__MOTOR_ENCODER_BACKEND != MOTOR_ENCODER_BACKEND_DISABLED && CONFIG__MOTOR_ENCODER_BACKEND != MOTOR_ENCODER_BACKEND_QDC
#error Invalid CONFIG__MOTOR_ENCODER_BACKEND.
#endif

#if CONFIG__MOTOR_ENCODER_DIAG_ENABLE && CONFIG__MOTOR_ENCODER_BACKEND != MOTOR_ENCODER_BACKEND_QDC
#error CONFIG__MOTOR_ENCODER_DIAG_ENABLE requires CONFIG__MOTOR_ENCODER_BACKEND == MOTOR_ENCODER_BACKEND_QDC.
#endif

#if CONFIG__MOTOR_ENCODER_BACKEND == MOTOR_ENCODER_BACKEND_QDC && CONFIG__DISPLAY_PARALLEL_BITBANG_TEST_MODE
#error QDC encoder mode is mutually exclusive with alternate parallel LCD bit-bang mode.
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
