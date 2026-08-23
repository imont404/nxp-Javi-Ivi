

#include "fsl_ov7670.h"
#include "fsl_ov5640.h"
#include "fsl_lpi2c.h"
#include "fsl_lpflexcomm.h"
#include "fsl_smartdma.h"
#include "fsl_inputmux.h"
#include "fsl_gpio.h"
#include "fsl_port.h"
#include "fsl_edma.h"
#include "board.h"
#include "bunny_build.h"
#include "cr_section_macros.h"
#include "pin_mux.h"
#include "bv_camera__interface.h"
#include "nxpc__master_config.h"
#include "e_tick.h"

#define EZH_STACK_SIZE 64   /* stack size for EZH, see smart_dma driver recommendation */

#define DEBUG__PCLK (1)
#define DEBUG__VSYNC (1)
#define DEBUG__HSYNC (1)

#define CAMERA_DIAG_PCLK_PIN (20U)
#define CAMERA_DIAG_HSYNC_PIN (21U)
#define CAMERA_DIAG_VSYNC_PIN (22U)
#define CAMERA_DIAG_PCLK_MASK (1UL << CAMERA_DIAG_PCLK_PIN)
#define CAMERA_DIAG_HSYNC_MASK (1UL << CAMERA_DIAG_HSYNC_PIN)
#define CAMERA_DIAG_VSYNC_MASK (1UL << CAMERA_DIAG_VSYNC_PIN)
#define CAMERA_DIAG_IRQ_MASK (CAMERA_DIAG_HSYNC_MASK | CAMERA_DIAG_VSYNC_MASK)
#define CAMERA_DIAG_VSYNC_IRQ_MAX_PER_SECOND (120U)

#define CAMERA_REF_PCLK_PIN (5U)
#define CAMERA_REF_HSYNC_PIN (11U)
#define CAMERA_REF_VSYNC_PIN (4U)
#define CAMERA_REF_PCLK_MASK (1UL << CAMERA_REF_PCLK_PIN)
#define CAMERA_REF_HSYNC_MASK (1UL << CAMERA_REF_HSYNC_PIN)
#define CAMERA_REF_VSYNC_MASK (1UL << CAMERA_REF_VSYNC_PIN)
#define CAMERA_REF_IRQ_MASK (CAMERA_REF_HSYNC_MASK | CAMERA_REF_VSYNC_MASK)

#define CAMERA_FRAME_PIXELS (FSL_VIDEO_EXTRACT_WIDTH(CONFIG__CAMERA_RESOLUTION) * \
                             FSL_VIDEO_EXTRACT_HEIGHT(CONFIG__CAMERA_RESOLUTION))
#define CAMERA_FRAME_BYTES (CAMERA_FRAME_PIXELS * sizeof(uint16_t))
#define CAMERA_FRAME_BUFFER_COUNT CONFIG__CAMERA_FRAME_BUFFER_COUNT
#define CAMERA_PIPELINE_DIAG_LINE_MARGIN (20U)
#define CAMERA_PIPELINE_DIAG_LINES_MIN (CONFIG__CAMERA_RESOLUTION_Y - CAMERA_PIPELINE_DIAG_LINE_MARGIN)
#define CAMERA_PIPELINE_DIAG_LINES_MAX (CONFIG__CAMERA_RESOLUTION_Y + CAMERA_PIPELINE_DIAG_LINE_MARGIN)
/*
 * FlexIO camera pin group.
 *
 * Everything below is expressed per group so the shifter, timer, and eDMA setup
 * stays identical between them. See CONFIG__CAMERA_FLEXIO_PIN_GROUP in
 * nxpc__master_config.h and docs/research/AVC_Camera_FlexIO_Pin_Migration.md.
 *
 * Naming: *_GPIO_PIN is a PORT/GPIO bit index, *_PIN (and *_PIN_START) is a
 * FLEXIO0_Dnn index. That split already existed; it is now applied per signal
 * because the PORT1 group does not keep data, PCLK, and HREF adjacent on one
 * port the way the PORT4 group does.
 */
#if CONFIG__CAMERA_FLEXIO_PIN_GROUP == CAMERA_FLEXIO_PIN_GROUP_PORT1

/* Rev A camera wiring plus three J9_EXT jumpers. */
#define CAMERA_FLEXIO_DATA_PORT           PORT1
#define CAMERA_FLEXIO_DATA_PORT_CLOCK     kCLOCK_Port1
#define CAMERA_FLEXIO_DATA_GPIO_START_PIN (4U)   /* P1_4  */
#define CAMERA_FLEXIO_DATA_PIN_START      (12U)  /* FLEXIO0_D12 */

#define CAMERA_FLEXIO_PCLK_PORT           PORT1
#define CAMERA_FLEXIO_PCLK_PORT_CLOCK     kCLOCK_Port1
#define CAMERA_FLEXIO_PCLK_GPIO_PIN       (14U)  /* P1_14 */
#define CAMERA_FLEXIO_PCLK_PIN            (22U)  /* FLEXIO0_D22 */

#define CAMERA_FLEXIO_HREF_PORT           PORT0
#define CAMERA_FLEXIO_HREF_PORT_CLOCK     kCLOCK_Port0
#define CAMERA_FLEXIO_HREF_GPIO           GPIO0
#define CAMERA_FLEXIO_HREF_GPIO_PIN       (11U)  /* P0_11 */
#define CAMERA_FLEXIO_HREF_PIN            (3U)   /* FLEXIO0_D3 */

#define CAMERA_FLEXIO_VSYNC_PORT          PORT0
#define CAMERA_FLEXIO_VSYNC_PORT_CLOCK    kCLOCK_Port0
#define CAMERA_FLEXIO_VSYNC_GPIO          GPIO0
#define CAMERA_FLEXIO_VSYNC_GPIO_CLOCK    kCLOCK_Gpio0
#define CAMERA_FLEXIO_VSYNC_GPIO_PIN      (4U)   /* P0_4 */
#define CAMERA_FLEXIO_VSYNC_IRQN          GPIO00_IRQn
#define CAMERA_FLEXIO_PIN_GROUP_NAME \
    "D0-D7=P1_4..P1_11/FLEXIO0_D12..D19 PCLK=P1_14/FLEXIO0_D22 HREF=P0_11/FLEXIO0_D3 VSYNC=P0_4/GPIO IRQ"

#else /* CAMERA_FLEXIO_PIN_GROUP_PORT4 */

/* Original proven group. Needs eleven fly-wires. */
#define CAMERA_FLEXIO_DATA_PORT           PORT4
#define CAMERA_FLEXIO_DATA_PORT_CLOCK     kCLOCK_Port4
#define CAMERA_FLEXIO_DATA_GPIO_START_PIN (12U)  /* P4_12 */
#define CAMERA_FLEXIO_DATA_PIN_START      (20U)  /* FLEXIO0_D20 */

#define CAMERA_FLEXIO_PCLK_PORT           PORT4
#define CAMERA_FLEXIO_PCLK_PORT_CLOCK     kCLOCK_Port4
#define CAMERA_FLEXIO_PCLK_GPIO_PIN       (20U)  /* P4_20 */
#define CAMERA_FLEXIO_PCLK_PIN            (28U)  /* FLEXIO0_D28 */

#define CAMERA_FLEXIO_HREF_PORT           PORT4
#define CAMERA_FLEXIO_HREF_PORT_CLOCK     kCLOCK_Port4
#define CAMERA_FLEXIO_HREF_GPIO           GPIO4
#define CAMERA_FLEXIO_HREF_GPIO_PIN       (21U)  /* P4_21 */
#define CAMERA_FLEXIO_HREF_PIN            (29U)  /* FLEXIO0_D29 */

#define CAMERA_FLEXIO_VSYNC_PORT          PORT4
#define CAMERA_FLEXIO_VSYNC_PORT_CLOCK    kCLOCK_Port4
#define CAMERA_FLEXIO_VSYNC_GPIO          GPIO4
#define CAMERA_FLEXIO_VSYNC_GPIO_CLOCK    kCLOCK_Gpio4
#define CAMERA_FLEXIO_VSYNC_GPIO_PIN      (22U)  /* P4_22 */
#define CAMERA_FLEXIO_VSYNC_IRQN          GPIO40_IRQn
#define CAMERA_FLEXIO_PIN_GROUP_NAME \
    "D0-D7=P4_12..P4_19/FLEXIO0_D20..D27 PCLK=P4_20/FLEXIO0_D28 HREF=P4_21/FLEXIO0_D29 VSYNC=P4_22/GPIO IRQ"

#endif

#define CAMERA_FLEXIO_DATA_WIDTH (8U)
#define CAMERA_FLEXIO_VSYNC_MASK (1UL << CAMERA_FLEXIO_VSYNC_GPIO_PIN)
#define CAMERA_FLEXIO_HREF_MASK (1UL << CAMERA_FLEXIO_HREF_GPIO_PIN)
/*
 * HREF and VSYNC land on the same GPIO instance in both pin groups (GPIO4 for
 * PORT4, GPIO0 for PORT1), so stale flags for both are cleared together. This
 * matches what the original PORT4-only code did.
 */
#define CAMERA_FLEXIO_SYNC_CLEAR_MASK (CAMERA_FLEXIO_VSYNC_MASK | CAMERA_FLEXIO_HREF_MASK)

#define CAMERA_FLEXIO_SHIFTER_START (0U)
#define CAMERA_FLEXIO_SHIFTER_COUNT (8U)
#define CAMERA_FLEXIO_TIMER (0U)
#define CAMERA_FLEXIO_EDMA_CHANNEL (0U)
#define CAMERA_FLEXIO_EDMA_REQUEST (kDma1RequestMuxFlexIO0ShiftRegister0Request)
#define CAMERA_FLEXIO_DMA_MINOR_BYTES (CAMERA_FLEXIO_SHIFTER_COUNT * sizeof(uint32_t))
#define CAMERA_FLEXIO_SAMPLE_WORDS (64U)
#define CAMERA_FLEXIO_BUSY_VSYNC_TIMEOUT (3U)
#define CAMERA_FLEXIO_SHIFT_MASK (((1UL << CAMERA_FLEXIO_SHIFTER_COUNT) - 1UL) << CAMERA_FLEXIO_SHIFTER_START)
#define CAMERA_FLEXIO_TIMER_MASK (1UL << CAMERA_FLEXIO_TIMER)
#define CAMERA_TIMING_SOURCE_NAME "OSTIMER0"
#define CAMERA_TIMING_CLK_ATTACH kCLK_1M_to_OSTIMER
#define CAMERA_TIMING_CLK_GATE kCLOCK_OsTimer
#define CAMERA_TIMING_TICKS_PER_SECOND (1000000U)
#define CAMERA_TIMING_MIN_INIT (0xFFFFFFFFU)
#define CAMERA_TIMING_OSTIMER_HIGH_MASK OSTIMER_EVTIMERH_EVTIMER_COUNT_VALUE_MASK


#if CONFIG__CAMERA_CAPTURE_BACKEND == CAMERA_CAPTURE_BACKEND_SMARTDMA_EZH
/*
 * EZH program image, assembled at runtime by bunny_build(). Lives in SRAM_H so
 * it is clear of the camera frame buffers.
 */
__BSS(SRAM_H) volatile uint32_t ezh_binary[512];
#endif

SDK_ALIGN(__BSS(FRAME_BUFFERS) uint16_t g_camera_buffer[CAMERA_FRAME_PIXELS * CAMERA_FRAME_BUFFER_COUNT],
          CAMERA_FLEXIO_DMA_MINOR_BYTES);



#if CONFIG__CAMERA_CAPTURE_BACKEND == CAMERA_CAPTURE_BACKEND_SMARTDMA_EZH
/*
 * SmartDMA/EZH capture state. Guarded so a FlexIO build cannot compile any of
 * it in - that matters because the EZH camera data pins are P1_4..P1_11, the
 * same pins the FlexIO PORT1 group uses. An EZH init reaching those pins would
 * mux them to alt7 and silently break FlexIO capture.
 *
 * These were previously unguarded and excluded only by --gc-sections, which
 * works until any incidental reference pulls them back in.
 */
volatile uint32_t next_buffer = 0;

volatile uint32_t line;

volatile uint8_t img_ready = 0;   /* non-zero when new data signaled by SmartDMA IRQ */

static volatile uint8_t g_samrtdma_stack[EZH_STACK_SIZE];
#endif

static volatile uint32_t g_camera_diag_hsync_count;
static volatile uint32_t g_camera_diag_vsync_count;
static volatile uint32_t g_camera_diag_lines_last_frame;
static volatile uint32_t g_camera_diag_hsync_at_last_vsync;
static volatile uint32_t g_camera_diag_vsync_irq_disabled;
static volatile uint32_t g_camera_diag_irq_unexpected_count;
static volatile uint32_t g_camera_ref_hsync_count;
static volatile uint32_t g_camera_ref_vsync_count;
static volatile uint32_t g_camera_ref_lines_last_frame;
static volatile uint32_t g_camera_ref_hsync_at_last_vsync;
static volatile uint32_t g_camera_ref_irq_unexpected_count;
static volatile uint32_t g_camera_pipeline_diag_frame_count;
static volatile uint32_t g_camera_pipeline_diag_rejected_vsync_count;
static volatile uint32_t g_camera_pipeline_diag_next_buffer;
static edma_handle_t g_camera_flexio_edma_handle;
static volatile uint32_t g_camera_flexio_edma_started_count;
static volatile uint32_t g_camera_flexio_edma_done_count;
static volatile uint32_t g_camera_flexio_edma_busy_vsync_count;
static volatile uint32_t g_camera_flexio_edma_timeout_count;
static volatile uint32_t g_camera_flexio_edma_submit_error_count;
static volatile uint32_t g_camera_flexio_edma_callback_error_count;
static volatile uint32_t g_camera_flexio_edma_active_buffer;
static volatile uint32_t g_camera_flexio_edma_next_buffer;
static volatile uint32_t g_camera_flexio_edma_busy;
static volatile uint32_t g_camera_flexio_edma_busy_vsyncs;
static volatile uint32_t g_camera_flexio_edma_last_sample_nonzero;
static volatile uint16_t g_camera_flexio_edma_last_first_words[4];
static volatile uint32_t g_camera_timing_clock_hz;
static volatile uint32_t g_camera_timing_last_vsync_tick;
static volatile uint32_t g_camera_timing_have_vsync_tick;
static volatile uint32_t g_camera_timing_last_vsync_period_ticks;
static volatile uint32_t g_camera_timing_min_vsync_period_ticks;
static volatile uint32_t g_camera_timing_max_vsync_period_ticks;
static volatile uint32_t g_camera_timing_vsync_period_sample_count;
static volatile uint32_t g_camera_timing_active_frame_vsync_tick;
static volatile uint32_t g_camera_timing_last_dma_arm_ticks;
static volatile uint32_t g_camera_timing_min_dma_arm_ticks;
static volatile uint32_t g_camera_timing_max_dma_arm_ticks;
static volatile uint32_t g_camera_timing_last_dma_frame_ticks;
static volatile uint32_t g_camera_timing_min_dma_frame_ticks;
static volatile uint32_t g_camera_timing_max_dma_frame_ticks;

//This needs to be global
#if CONFIG__CAMERA_CAPTURE_BACKEND == CAMERA_CAPTURE_BACKEND_SMARTDMA_EZH
smartdma_camera_param_t smartdmaParam;
#endif

static uint16_t *camera__frame_buffer(uint32_t buffer_index)
{
    return &g_camera_buffer[(buffer_index & 0x01U) * CAMERA_FRAME_PIXELS];
}


void  EZH_Camera_320240_Whole_Buf(void)
{
	E_NOP;/*pattern*/
	E_NOP;/*pattern*/
	E_NOP;/*pattern*/
	E_NOP;/*pattern*/


	E_PER_READ(R6, ARM2EZH);
	//Align the value of R6 to word
	E_LSR(R6, R6, 2);
	E_LSL(R6, R6, 2);
	E_LDR(SP, R6, 0);//EZH stack initial
	E_LDR(R3, R6, 1);//R3 point to the store buffer in the RAM


	E_LABEL("init_0");
	E_LDR(CFS, PC, 1); // Load CFS
	E_LDR(CFM, PC, 1); // Load CFM
	E_ADD_IMM(PC, PC, 1 * 4); //E_goto

	// INPUTMUX_AttachSignal(INPUTMUX0, 0, kINPUTMUX_GpioPort0Pin4ToSmartDma); //vsync
	//    INPUTMUX_AttachSignal(INPUTMUX0, 1, kINPUTMUX_GpioPort0Pin11ToSmartDma);//hsync
	//    INPUTMUX_AttachSignal(INPUTMUX0, 2, kINPUTMUX_GpioPort0Pin5ToSmartDma); //pclk

	E_DCD_VAL(BS7(0) | BS6(0) | BS5(0) | BS4(0) | BS3(0) | BS2(1) | BS1(0) | BS0(2));   // Config source (C^D + C^D-)  where C^ is Clock rise, D- is Data inverted
	E_DCD_VAL(BS7(6) | BS6(6) | BS5(6) | BS4(6) | BS3(6) | BS2(BS_FALL) | BS1(BS_RISE) | BS0(BS_RISE) | (1 << 2)); // Config MUX    (C^D + C^D-)  where C^ is Clock rise, D- is Data inverted, enable OR



	E_BCLR_IMM(GPD, GPD, 0);
	E_BCLR_IMM(GPD, GPD, 1);
	E_BCLR_IMM(GPD, GPD, 2);
	E_BCLR_IMM(GPD, GPD, 3);
	E_BCLR_IMM(GPD, GPD, 4);
	E_BCLR_IMM(GPD, GPD, 5);
	E_BCLR_IMM(GPD, GPD, 6);
	E_BCLR_IMM(GPD, GPD, 7);
	E_BSET_IMM(GPD, GPD, 13);


	E_LOAD_IMM(R1, 0xFF);
	E_LABEL("PCLK_0");
	E_ACC_VECTORED_HOLD_LV(PC, (1 << 0));

	E_STRB_POST(R3, GPI, 1);  				// data will be stored from GPI bottom byte to RAM
	E_BSET_IMM(CFM, CFM, 0); 				//clear the vector flag

	E_SUB_IMMS(PC, PC, 4 * 5);
#if DEBUG__PCLK == (1)
	E_BSET_IMM(GPO, GPO, 13);				//toggle the P0_18, used to measure the timming in logic device
	E_BCLR_IMM(GPO, GPO, 13);
#else
	E_NOP;
	E_NOP;
#endif
	E_NOP;

	E_GOSUB("VSYNC_0");//BS1
	E_NOP;
	E_NOP;
	E_NOP;

	E_GOSUB("HSYNC_0");//BS2
	E_NOP;
	E_NOP;
	E_NOP;


	E_LABEL("VSYNC_0");
	E_BSET_IMM(CFM, CFM, 0); 			//clear the VSYNC vector flag
    E_BSET_IMM(CFM, CFM, 1); 			//enable HSYNC interrupt
	E_BSET_IMM(CFM, CFM, 2); 			//enable PCLK  interrupt
	E_LDR(R3, R6, 1);							//R3 point to the store buffer in the RAM
	E_INT_TRIGGER(0xFFFFFF); // interrupt and told ARM data is ready

	E_LOAD_IMM(R4,0);


#if DEBUG__VSYNC == (1)
	E_BSET_IMM(GPO, GPO, 13);				//toggle the P0_18, used to measure the timming in logic device
	E_BCLR_IMM(GPO, GPO, 13);
#endif

	//E_INT_TRIGGER(0x11); // interrupt and told ARM data is ready
	E_GOSUB("PCLK_0");


	E_LABEL("HSYNC_0");

	//E_PER_WRITE(R4, EZH2ARM);
//	E_STR(R6,R4, 2); //save the line
	E_ADD_IMM(R4,R4,1);
//	E_INT_TRIGGER(0xFFFFFF); // interrupt and told ARM data is ready

	E_BSET_IMM(CFM, CFM, 1); 			//clear the vector flag

	#if DEBUG__HSYNC == (1)
		E_BSET_IMM(GPO, GPO, 13);				//toggle the P0_18, used to measure the timming in logic device
		E_BCLR_IMM(GPO, GPO, 13);
		E_BSET_IMM(GPO, GPO, 13);				//toggle the P0_18, used to measure the timming in logic device
		E_BCLR_IMM(GPO, GPO, 13);
		#endif

	E_GOSUB("PCLK_0");
}


void camera__i2c_init();

status_t camera__i2c_receiveSCCB(uint8_t deviceAddress, uint32_t subAddress, uint8_t subAddressSize, uint8_t *rxBuff, uint8_t rxBuffSize);

status_t camera__i2c_sendSCCB(uint8_t deviceAddress, uint32_t subAddress, uint8_t subAddressSize, uint8_t *txBuff, uint8_t txBuffSize);


void camera__pull_reset_pin(bool pullUp)
{
    if (pullUp)
        GPIO_PortSet(BOARD_INITCAMERAPINS_CAM_RST_GPIO, BOARD_INITCAMERAPINS_CAM_RST_GPIO_PIN_MASK);
    else
        GPIO_PortClear(BOARD_INITCAMERAPINS_CAM_RST_GPIO, BOARD_INITCAMERAPINS_CAM_RST_GPIO_PIN_MASK);
}

void camera__pull_power_pin(bool pullUp)
{
    if (pullUp)
        GPIO_PortSet(BOARD_INITCAMERAPINS_CAM_PDWN_GPIO, BOARD_INITCAMERAPINS_CAM_PDWN_GPIO_PIN_MASK);
    else
        GPIO_PortClear(BOARD_INITCAMERAPINS_CAM_PDWN_GPIO, BOARD_INITCAMERAPINS_CAM_PDWN_GPIO_PIN_MASK);
}



#if CONFIG__CAMERA_SELECT == CAMERA__OV7670
/* Camera resource and handle */

	const ov7670_resource_t resource = {
		.i2cSendFunc    = BOARD_Camera_I2C_SendSCCB,
		.i2cReceiveFunc = BOARD_Camera_I2C_ReceiveSCCB,
		.xclock         = kOV7670_InputClock12MHZ,
	};

#endif



#if CONFIG__CAMERA_SELECT == CAMERA__OV5640

	const ov5640_resource_t resource = {
		 .i2cSendFunc    = BOARD_Camera_I2C_SendSCCB,
		 .i2cReceiveFunc = BOARD_Camera_I2C_ReceiveSCCB,
		.pullResetPin = camera__pull_reset_pin,      /*!< Function to pull reset pin high or low. */
		.pullPowerDownPin = camera__pull_power_pin,  /*!< Function to pull the power down pin high or low. */
	};

#endif



#if CONFIG__CAMERA_CAPTURE_BACKEND == CAMERA_CAPTURE_BACKEND_SMARTDMA_EZH
static void ezh_camera_callback(void *param)
{
	//EZH_SetExternalFlag(1) ;

	line = LPC_EZH_ARCH_B0->EZHB_EZH2ARM;

	/*
	 * NOTE: this is an assignment, not a comparison - "=" where "==" was almost
	 * certainly meant. It therefore always evaluates true and overwrites the
	 * value just read from EZHB_EZH2ARM.
	 *
	 * Left as-is deliberately. The current behaviour (advance the buffer on
	 * every EZH callback) is what the working competition image does, and
	 * "fixing" the typo would gate frame advance on EZHB_EZH2ARM actually
	 * reading 0xFFFFFF - a behavioural change with unknown EZH-side semantics
	 * that must be verified on hardware before it is made.
	 */
	if(line = 0xFFFFFF)
	{
			next_buffer++;

			next_buffer&=0x01;

			uint16_t * buf;

			if(next_buffer==0)
				buf = camera__frame_buffer(0U);
			else
				buf = camera__frame_buffer(1U);

			smartdmaParam.p_buffer = (uint32_t *)buf;

			nxpc__next_frame(buf);
	}

}
#endif



camera_device_handle_t handle =
{
    .resource = (void *)&resource,

	#if CONFIG__CAMERA_SELECT == CAMERA__OV7670
		.ops      = &ov7670_ops,
	#endif

	#if CONFIG__CAMERA_SELECT == CAMERA__OV5640
		.ops      = &ov5640_ops
	#endif

};

static void camera__configure_xclk(void)
{
#if CONFIG__CAMERA_SELECT == CAMERA__OV7670
    CLOCK_AttachClk(kFRO12M_to_CLKOUT);
    CLOCK_SetClkDiv(kCLOCK_DivClkOut, 1U);
#else

    /* 6MHz for the current 320x200 OV5640 configuration. */
    CLOCK_AttachClk(kMAIN_CLK_to_CLKOUT);

    uint32_t res = CONFIG__CAMERA_RESOLUTION;

    switch(res)
    {
    default:
    case FSL_VIDEO_RESOLUTION(320,200):
		CLOCK_SetClkDiv(kCLOCK_DivClkOut,12U); //320x200
	break;
    case FSL_VIDEO_RESOLUTION(320,102):
		CLOCK_SetClkDiv(kCLOCK_DivClkOut, 11U); //320x102
	break;
    case FSL_VIDEO_RESOLUTION(320,120):
		CLOCK_SetClkDiv(kCLOCK_DivClkOut, 11U); //320x102
	break;
    case FSL_VIDEO_RESOLUTION(160,120):
		CLOCK_SetClkDiv(kCLOCK_DivClkOut, 6U); //320x102
	break;
    case FSL_VIDEO_RESOLUTION(320,240):
		CLOCK_SetClkDiv(kCLOCK_DivClkOut, 23U); //320x102
	break;
    }

#endif
}

static void camera__configure_i2c(void)
{
    CLOCK_AttachClk(kFRO12M_to_FLEXCOMM7);
    CLOCK_EnableClock(kCLOCK_LPFlexComm7);
    CLOCK_EnableClock(kCLOCK_LPI2c7);
    CLOCK_SetClkDiv(kCLOCK_DivFlexcom7Clk, 1u);

    camera__i2c_init();
}

static void camera__init_sensor(void)
{
#if (CONFIG__CAMERA_SELECT == CAMERA__OV7670)


        /* Init ov7670 module with default setting. */
        camera_config_t camconfig = {
            .pixelFormat                = kVIDEO_PixelFormatRGB565,
            .resolution                 = kVIDEO_ResolutionQVGA,
            .framePerSec                = 30,
            .interface                  = kCAMERA_InterfaceGatedClock,
            .frameBufferLinePitch_Bytes = 0, /* Not used. */
            .controlFlags               = 0, /* Not used. */
            .bytesPerPixel              = 0, /* Not used. */
            .mipiChannel                = 0, /* Not used. */
            .csiLanes                   = 0, /* Not used. */
        };

    	camera__pull_reset_pin(true);

    	camera__pull_power_pin(true);
    	SDK_DelayAtLeastUs(10000,SystemCoreClock);
       	camera__pull_power_pin(false);
        SDK_DelayAtLeastUs(20000,SystemCoreClock);

    	camera__pull_reset_pin(false);
    	SDK_DelayAtLeastUs(20000,SystemCoreClock);

    	camera__pull_reset_pin(true);
    	SDK_DelayAtLeastUs(20000,SystemCoreClock);


#else

        camera_config_t camconfig = {
            .pixelFormat                = kVIDEO_PixelFormatRGB565,
            .resolution                 = CONFIG__CAMERA_RESOLUTION,
            .framePerSec                = 30,//
            .interface                  = kCAMERA_InterfaceNonGatedClock ,
            .frameBufferLinePitch_Bytes = 0, /* Not used. */
            .controlFlags               = kCAMERA_DataLatchOnRisingEdge | kCAMERA_HrefActiveHigh | kCAMERA_VsyncActiveHigh, /* Not used. */
            .bytesPerPixel              = 0, /* Not used. */
            .mipiChannel                = 0, /* Not used. */
            .csiLanes                   = 0, /* Not used. */
        };

#endif

    CAMERA_DEVICE_Init(&handle, &camconfig);
}

static void camera__configure_flexio_diag_inputs(void)
{
    CLOCK_EnableClock(kCLOCK_Gpio4);
    CLOCK_EnableClock(kCLOCK_Port4);

    const gpio_pin_config_t input_config = {
        .pinDirection = kGPIO_DigitalInput,
        .outputLogic = 0U
    };

    GPIO_PinInit(GPIO4, 20U, &input_config);
    GPIO_PinInit(GPIO4, 21U, &input_config);
    GPIO_PinInit(GPIO4, 22U, &input_config);

    const port_pin_config_t port4_pclk_input = {
        .pullSelect = kPORT_PullDisable,
        .pullValueSelect = kPORT_LowPullResistor,
        .slewRate = kPORT_FastSlewRate,
        .passiveFilterEnable = kPORT_PassiveFilterDisable,
        .openDrainEnable = kPORT_OpenDrainDisable,
        .driveStrength = kPORT_LowDriveStrength,
        .mux = kPORT_MuxAlt0,
        .inputBuffer = kPORT_InputBufferEnable,
        .invertInput = kPORT_InputNormal,
        .lockRegister = kPORT_UnlockRegister
    };

    const port_pin_config_t port4_sync_input = {
        .pullSelect = kPORT_PullDown,
        .pullValueSelect = kPORT_LowPullResistor,
        .slewRate = kPORT_FastSlewRate,
        .passiveFilterEnable = kPORT_PassiveFilterEnable,
        .openDrainEnable = kPORT_OpenDrainDisable,
        .driveStrength = kPORT_LowDriveStrength,
        .mux = kPORT_MuxAlt0,
        .inputBuffer = kPORT_InputBufferEnable,
        .invertInput = kPORT_InputNormal,
        .lockRegister = kPORT_UnlockRegister
    };

    PORT_SetPinConfig(PORT4, CAMERA_DIAG_PCLK_PIN, &port4_pclk_input);
    PORT_SetPinConfig(PORT4, CAMERA_DIAG_HSYNC_PIN, &port4_sync_input);
    PORT_SetPinConfig(PORT4, CAMERA_DIAG_VSYNC_PIN, &port4_sync_input);

    GPIO_SetPinInterruptConfig(GPIO4, CAMERA_DIAG_PCLK_PIN, kGPIO_InterruptStatusFlagDisabled);
    GPIO_SetPinInterruptConfig(GPIO4, CAMERA_DIAG_HSYNC_PIN, kGPIO_InterruptStatusFlagDisabled);
    GPIO_SetPinInterruptConfig(GPIO4, CAMERA_DIAG_VSYNC_PIN, kGPIO_InterruptStatusFlagDisabled);

#if (defined(FSL_FEATURE_GPIO_HAS_INTERRUPT_CHANNEL_SELECT) && FSL_FEATURE_GPIO_HAS_INTERRUPT_CHANNEL_SELECT)
    GPIO_SetPinInterruptChannel(GPIO4, CAMERA_DIAG_PCLK_PIN, kGPIO_InterruptOutput0);
    GPIO_SetPinInterruptChannel(GPIO4, CAMERA_DIAG_HSYNC_PIN, kGPIO_InterruptOutput0);
    GPIO_SetPinInterruptChannel(GPIO4, CAMERA_DIAG_VSYNC_PIN, kGPIO_InterruptOutput0);
    GPIO_GpioClearInterruptChannelFlags(GPIO4, CAMERA_DIAG_PCLK_MASK | CAMERA_DIAG_IRQ_MASK, 0U);
#else
    GPIO_GpioClearInterruptFlags(GPIO4, CAMERA_DIAG_PCLK_MASK | CAMERA_DIAG_IRQ_MASK);
#endif

    GPIO_SetPinInterruptConfig(GPIO4, CAMERA_DIAG_PCLK_PIN, kGPIO_FlagRisingEdge);
    GPIO_SetPinInterruptConfig(GPIO4, CAMERA_DIAG_HSYNC_PIN, kGPIO_InterruptRisingEdge);
    GPIO_SetPinInterruptConfig(GPIO4, CAMERA_DIAG_VSYNC_PIN, kGPIO_InterruptRisingEdge);

    NVIC_ClearPendingIRQ(GPIO40_IRQn);
    NVIC_SetPriority(GPIO40_IRQn, 4);
    EnableIRQ(GPIO40_IRQn);

    DEBUG("FlexIO camera diag inputs configured: PCLK=P4_20 HSYNC/HREF=P4_21 VSYNC=P4_22\r\n");
    DEBUG("FlexIO camera diag interrupts: P4_21 HSYNC/HREF and P4_22 VSYNC rising edges on GPIO40_IRQn; P4_20 PCLK flag polled\r\n");
    DEBUG("FlexIO camera diag VSYNC IRQ guard: disable P4_22 IRQ above %u edges/sec and fall back to flag polling\r\n",
          CAMERA_DIAG_VSYNC_IRQ_MAX_PER_SECOND);
    DEBUG("FlexIO camera diag sync inputs use weak pulldown and passive filter\r\n");
    DEBUG("FlexIO camera diag levels: pclk=%u hsync=%u vsync=%u\r\n",
          GPIO_PinRead(GPIO4, CAMERA_DIAG_PCLK_PIN),
          GPIO_PinRead(GPIO4, CAMERA_DIAG_HSYNC_PIN),
          GPIO_PinRead(GPIO4, CAMERA_DIAG_VSYNC_PIN));
}

static void camera__configure_reference_diag_inputs(void)
{
    CLOCK_EnableClock(kCLOCK_Gpio0);

    GPIO_SetPinInterruptConfig(GPIO0, CAMERA_REF_PCLK_PIN, kGPIO_InterruptStatusFlagDisabled);
    GPIO_SetPinInterruptConfig(GPIO0, CAMERA_REF_HSYNC_PIN, kGPIO_InterruptStatusFlagDisabled);
    GPIO_SetPinInterruptConfig(GPIO0, CAMERA_REF_VSYNC_PIN, kGPIO_InterruptStatusFlagDisabled);

#if (defined(FSL_FEATURE_GPIO_HAS_INTERRUPT_CHANNEL_SELECT) && FSL_FEATURE_GPIO_HAS_INTERRUPT_CHANNEL_SELECT)
    GPIO_SetPinInterruptChannel(GPIO0, CAMERA_REF_PCLK_PIN, kGPIO_InterruptOutput0);
    GPIO_SetPinInterruptChannel(GPIO0, CAMERA_REF_HSYNC_PIN, kGPIO_InterruptOutput0);
    GPIO_SetPinInterruptChannel(GPIO0, CAMERA_REF_VSYNC_PIN, kGPIO_InterruptOutput0);
    GPIO_GpioClearInterruptChannelFlags(GPIO0, CAMERA_REF_PCLK_MASK | CAMERA_REF_IRQ_MASK, 0U);
#else
    GPIO_GpioClearInterruptFlags(GPIO0, CAMERA_REF_PCLK_MASK | CAMERA_REF_IRQ_MASK);
#endif

    GPIO_SetPinInterruptConfig(GPIO0, CAMERA_REF_PCLK_PIN, kGPIO_FlagRisingEdge);
    GPIO_SetPinInterruptConfig(GPIO0, CAMERA_REF_HSYNC_PIN, kGPIO_InterruptRisingEdge);
    GPIO_SetPinInterruptConfig(GPIO0, CAMERA_REF_VSYNC_PIN, kGPIO_InterruptRisingEdge);

    NVIC_ClearPendingIRQ(GPIO00_IRQn);
    NVIC_SetPriority(GPIO00_IRQn, 4);
    EnableIRQ(GPIO00_IRQn);

    DEBUG("Reference camera diag inputs: PCLK=P0_5 HSYNC/HREF=P0_11 VSYNC=P0_4\r\n");
}

static uint32_t camera__flexio_timer_trigger_sel_pininput(uint32_t pin)
{
    return pin << 1U;
}

static void camera__configure_flexio_edma_pins(void)
{
    CLOCK_EnableClock(CAMERA_FLEXIO_DATA_PORT_CLOCK);
    CLOCK_EnableClock(CAMERA_FLEXIO_PCLK_PORT_CLOCK);
    CLOCK_EnableClock(CAMERA_FLEXIO_HREF_PORT_CLOCK);
    CLOCK_EnableClock(CAMERA_FLEXIO_VSYNC_PORT_CLOCK);
    CLOCK_EnableClock(CAMERA_FLEXIO_VSYNC_GPIO_CLOCK);

    const port_pin_config_t flexio_input_config = {
        .pullSelect = kPORT_PullDisable,
        .pullValueSelect = kPORT_LowPullResistor,
        .slewRate = kPORT_FastSlewRate,
        .passiveFilterEnable = kPORT_PassiveFilterDisable,
        .openDrainEnable = kPORT_OpenDrainDisable,
        .driveStrength = kPORT_LowDriveStrength,
        .mux = kPORT_MuxAlt6,
        .inputBuffer = kPORT_InputBufferEnable,
        .invertInput = kPORT_InputNormal,
        .lockRegister = kPORT_UnlockRegister
    };

    const port_pin_config_t vsync_gpio_config = {
        .pullSelect = kPORT_PullDown,
        .pullValueSelect = kPORT_LowPullResistor,
        .slewRate = kPORT_FastSlewRate,
        .passiveFilterEnable = kPORT_PassiveFilterEnable,
        .openDrainEnable = kPORT_OpenDrainDisable,
        .driveStrength = kPORT_LowDriveStrength,
        .mux = kPORT_MuxAlt0,
        .inputBuffer = kPORT_InputBufferEnable,
        .invertInput = kPORT_InputNormal,
        .lockRegister = kPORT_UnlockRegister
    };

    /*
     * Data, PCLK, and HREF are configured separately rather than as one
     * contiguous sweep. The PORT4 group happens to have all three adjacent on
     * one port; the PORT1 group does not (data ends at P1_11, PCLK is P1_14,
     * and HREF is on Port 0), so the sweep cannot be assumed.
     */
    for (uint32_t i = 0U; i < CAMERA_FLEXIO_DATA_WIDTH; i++)
    {
        PORT_SetPinConfig(CAMERA_FLEXIO_DATA_PORT,
                          CAMERA_FLEXIO_DATA_GPIO_START_PIN + i,
                          &flexio_input_config);
    }

    PORT_SetPinConfig(CAMERA_FLEXIO_PCLK_PORT, CAMERA_FLEXIO_PCLK_GPIO_PIN, &flexio_input_config);
    PORT_SetPinConfig(CAMERA_FLEXIO_HREF_PORT, CAMERA_FLEXIO_HREF_GPIO_PIN, &flexio_input_config);

    const gpio_pin_config_t input_config = {
        .pinDirection = kGPIO_DigitalInput,
        .outputLogic = 0U
    };

    GPIO_PinInit(CAMERA_FLEXIO_VSYNC_GPIO, CAMERA_FLEXIO_VSYNC_GPIO_PIN, &input_config);
    PORT_SetPinConfig(CAMERA_FLEXIO_VSYNC_PORT, CAMERA_FLEXIO_VSYNC_GPIO_PIN, &vsync_gpio_config);

    /* HREF belongs to FlexIO now, so make sure no GPIO interrupt is left armed on it. */
    GPIO_SetPinInterruptConfig(CAMERA_FLEXIO_HREF_GPIO, CAMERA_FLEXIO_HREF_GPIO_PIN,
                               kGPIO_InterruptStatusFlagDisabled);
    GPIO_SetPinInterruptConfig(CAMERA_FLEXIO_VSYNC_GPIO, CAMERA_FLEXIO_VSYNC_GPIO_PIN,
                               kGPIO_InterruptStatusFlagDisabled);

#if (defined(FSL_FEATURE_GPIO_HAS_INTERRUPT_CHANNEL_SELECT) && FSL_FEATURE_GPIO_HAS_INTERRUPT_CHANNEL_SELECT)
    GPIO_SetPinInterruptChannel(CAMERA_FLEXIO_VSYNC_GPIO, CAMERA_FLEXIO_VSYNC_GPIO_PIN,
                                kGPIO_InterruptOutput0);
    GPIO_GpioClearInterruptChannelFlags(CAMERA_FLEXIO_VSYNC_GPIO, CAMERA_FLEXIO_SYNC_CLEAR_MASK, 0U);
#else
    GPIO_GpioClearInterruptFlags(CAMERA_FLEXIO_VSYNC_GPIO, CAMERA_FLEXIO_SYNC_CLEAR_MASK);
#endif

    GPIO_SetPinInterruptConfig(CAMERA_FLEXIO_VSYNC_GPIO, CAMERA_FLEXIO_VSYNC_GPIO_PIN,
                               kGPIO_InterruptRisingEdge);

    NVIC_ClearPendingIRQ(CAMERA_FLEXIO_VSYNC_IRQN);
    NVIC_SetPriority(CAMERA_FLEXIO_VSYNC_IRQN, 4);
    EnableIRQ(CAMERA_FLEXIO_VSYNC_IRQN);

    DEBUG("FlexIO camera eDMA pins: " CAMERA_FLEXIO_PIN_GROUP_NAME "\r\n");

    /*
     * Read the mux fields back and report them.
     *
     * This exists because the pin group is now selectable, so a capture failure
     * has two very different causes: the pins were not muxed the way this build
     * intended, or they were and the problem is wiring/FlexIO. Printing the
     * actual PCR MUX values separates those two without a debugger.
     *
     * Expect alt6 (FlexIO) on every data pin plus PCLK and HREF, and alt0
     * (GPIO) on VSYNC.
     */
    {
        uint32_t data_mux = 0U;
        bool data_ok = true;

        for (uint32_t i = 0U; i < CAMERA_FLEXIO_DATA_WIDTH; i++)
        {
            uint32_t mux = (CAMERA_FLEXIO_DATA_PORT->PCR[CAMERA_FLEXIO_DATA_GPIO_START_PIN + i] &
                            PORT_PCR_MUX_MASK) >> PORT_PCR_MUX_SHIFT;

            data_mux |= (mux & 0xFU) << (i * 4U);

            if (mux != 6U)
            {
                data_ok = false;
            }
        }

        uint32_t pclk_mux = (CAMERA_FLEXIO_PCLK_PORT->PCR[CAMERA_FLEXIO_PCLK_GPIO_PIN] &
                             PORT_PCR_MUX_MASK) >> PORT_PCR_MUX_SHIFT;
        uint32_t href_mux = (CAMERA_FLEXIO_HREF_PORT->PCR[CAMERA_FLEXIO_HREF_GPIO_PIN] &
                             PORT_PCR_MUX_MASK) >> PORT_PCR_MUX_SHIFT;
        uint32_t vsync_mux = (CAMERA_FLEXIO_VSYNC_PORT->PCR[CAMERA_FLEXIO_VSYNC_GPIO_PIN] &
                              PORT_PCR_MUX_MASK) >> PORT_PCR_MUX_SHIFT;

        DEBUG("FlexIO camera pin mux readback: data(D7..D0)=0x%08X pclk=%u href=%u vsync=%u %s\r\n",
              (unsigned int)data_mux,
              (unsigned int)pclk_mux,
              (unsigned int)href_mux,
              (unsigned int)vsync_mux,
              (data_ok && pclk_mux == 6U && href_mux == 6U && vsync_mux == 0U)
                  ? "OK" : "UNEXPECTED");
    }
}

static void camera__configure_flexio_camera(void)
{
    CLOCK_SetClkDiv(kCLOCK_DivFlexioClk, 1U);
    CLOCK_AttachClk(kPLL0_to_FLEXIO);
    CLOCK_EnableClock(kCLOCK_Flexio);

    FLEXIO0->CTRL |= FLEXIO_CTRL_SWRST_MASK;
    FLEXIO0->CTRL = 0U;

    FLEXIO0->SHIFTSIEN = 0U;
    FLEXIO0->SHIFTSDEN = 0U;
    FLEXIO0->SHIFTSTAT = CAMERA_FLEXIO_SHIFT_MASK;
    FLEXIO0->SHIFTERR = CAMERA_FLEXIO_SHIFT_MASK;
    FLEXIO0->TIMSTAT = CAMERA_FLEXIO_TIMER_MASK;

    for (uint32_t shifter = CAMERA_FLEXIO_SHIFTER_START;
         shifter < (CAMERA_FLEXIO_SHIFTER_START + CAMERA_FLEXIO_SHIFTER_COUNT);
         shifter++)
    {
        uint32_t input_source = (shifter == (CAMERA_FLEXIO_SHIFTER_START + CAMERA_FLEXIO_SHIFTER_COUNT - 1U)) ? 0U : 1U;

        FLEXIO0->SHIFTCFG[shifter] =
            FLEXIO_SHIFTCFG_INSRC(input_source) |
            FLEXIO_SHIFTCFG_PWIDTH(7U) |
            FLEXIO_SHIFTCFG_SSTOP(0U) |
            FLEXIO_SHIFTCFG_SSTART(0U);

        FLEXIO0->SHIFTCTL[shifter] =
            FLEXIO_SHIFTCTL_TIMSEL(CAMERA_FLEXIO_TIMER) |
            FLEXIO_SHIFTCTL_TIMPOL(0U) |
            FLEXIO_SHIFTCTL_PINCFG(0U) |
            FLEXIO_SHIFTCTL_PINSEL(CAMERA_FLEXIO_DATA_PIN_START) |
            FLEXIO_SHIFTCTL_PINPOL(0U) |
            FLEXIO_SHIFTCTL_SMOD(1U);
    }

    FLEXIO0->TIMCFG[CAMERA_FLEXIO_TIMER] =
        FLEXIO_TIMCFG_TIMOUT(1U) |
        FLEXIO_TIMCFG_TIMDEC(2U) |
        FLEXIO_TIMCFG_TIMRST(6U) |
        FLEXIO_TIMCFG_TIMDIS(6U) |
        FLEXIO_TIMCFG_TIMENA(6U) |
        FLEXIO_TIMCFG_TSTOP(0U) |
        FLEXIO_TIMCFG_TSTART(0U);

    FLEXIO0->TIMCMP[CAMERA_FLEXIO_TIMER] = FLEXIO_TIMCMP_CMP((8U * CAMERA_FLEXIO_SHIFTER_COUNT) - 1U);

    FLEXIO0->TIMCTL[CAMERA_FLEXIO_TIMER] =
        FLEXIO_TIMCTL_TRGSEL(camera__flexio_timer_trigger_sel_pininput(CAMERA_FLEXIO_HREF_PIN)) |
        FLEXIO_TIMCTL_TRGPOL(0U) |
        FLEXIO_TIMCTL_TRGSRC(1U) |
        FLEXIO_TIMCTL_PINCFG(0U) |
        FLEXIO_TIMCTL_PINSEL(CAMERA_FLEXIO_PCLK_PIN) |
        FLEXIO_TIMCTL_PINPOL(0U) |
        FLEXIO_TIMCTL_TIMOD(3U);

    FLEXIO0->CTRL = FLEXIO_CTRL_DBGE(1U) | FLEXIO_CTRL_FLEXEN(1U);

    DEBUG("FlexIO camera engine: flexio_clk=%uHz shifter=%u count=%u timer=%u dma_minor=%u frame_bytes=%u\r\n",
          CLOCK_GetFlexioClkFreq(),
          CAMERA_FLEXIO_SHIFTER_START,
          CAMERA_FLEXIO_SHIFTER_COUNT,
          CAMERA_FLEXIO_TIMER,
          CAMERA_FLEXIO_DMA_MINOR_BYTES,
          CAMERA_FRAME_BYTES);
}

static uint64_t camera__timing_gray_to_binary(uint64_t gray)
{
    gray ^= gray >> 1U;
    gray ^= gray >> 2U;
    gray ^= gray >> 4U;
    gray ^= gray >> 8U;
    gray ^= gray >> 16U;
    gray ^= gray >> 32U;

    return gray;
}

static uint32_t camera__timing_read_ticks(void)
{
    uint32_t high_before;
    uint32_t high_after;
    uint32_t low;
    uint64_t gray;
    uint64_t binary;

    do
    {
        high_before = OSTIMER0->EVTIMERH & CAMERA_TIMING_OSTIMER_HIGH_MASK;
        low = OSTIMER0->EVTIMERL;
        high_after = OSTIMER0->EVTIMERH & CAMERA_TIMING_OSTIMER_HIGH_MASK;
    } while (high_before != high_after);

    gray = ((uint64_t)high_after << 32U) | low;
    binary = camera__timing_gray_to_binary(gray);

    return (uint32_t)binary;
}

static void camera__timing_update_min_max(volatile uint32_t *min_ticks,
                                          volatile uint32_t *max_ticks,
                                          uint32_t ticks)
{
    if (ticks < *min_ticks)
    {
        *min_ticks = ticks;
    }

    if (ticks > *max_ticks)
    {
        *max_ticks = ticks;
    }
}

static uint32_t camera__timing_ticks_to_us(uint32_t ticks)
{
    uint32_t hz = g_camera_timing_clock_hz;

    if (hz == 0U)
    {
        return 0U;
    }

    if (hz == CAMERA_TIMING_TICKS_PER_SECOND)
    {
        return ticks;
    }

    return (uint32_t)((((uint64_t)ticks * 1000000ULL) + (hz / 2U)) / hz);
}

static uint32_t camera__timing_fps_x100(uint32_t frames, uint32_t elapsed_ticks)
{
    uint32_t hz = g_camera_timing_clock_hz;

    if ((hz == 0U) || (elapsed_ticks == 0U))
    {
        return 0U;
    }

    return (uint32_t)((((uint64_t)frames * 100ULL * hz) + (elapsed_ticks / 2U)) / elapsed_ticks);
}

static void camera__timing_on_vsync(uint32_t vsync_tick)
{
    if (g_camera_timing_have_vsync_tick != 0U)
    {
        uint32_t period_ticks = vsync_tick - g_camera_timing_last_vsync_tick;

        g_camera_timing_last_vsync_period_ticks = period_ticks;
        camera__timing_update_min_max(&g_camera_timing_min_vsync_period_ticks,
                                      &g_camera_timing_max_vsync_period_ticks,
                                      period_ticks);
        g_camera_timing_vsync_period_sample_count++;
    }

    g_camera_timing_last_vsync_tick = vsync_tick;
    g_camera_timing_have_vsync_tick = 1U;
}

static void camera__timing_on_dma_armed(uint32_t vsync_tick)
{
    uint32_t arm_ticks = camera__timing_read_ticks() - vsync_tick;

    g_camera_timing_last_dma_arm_ticks = arm_ticks;
    camera__timing_update_min_max(&g_camera_timing_min_dma_arm_ticks,
                                  &g_camera_timing_max_dma_arm_ticks,
                                  arm_ticks);
}

static void camera__timing_on_dma_done(void)
{
    uint32_t frame_ticks = camera__timing_read_ticks() - g_camera_timing_active_frame_vsync_tick;

    g_camera_timing_last_dma_frame_ticks = frame_ticks;
    camera__timing_update_min_max(&g_camera_timing_min_dma_frame_ticks,
                                  &g_camera_timing_max_dma_frame_ticks,
                                  frame_ticks);
}

static void camera__configure_frame_timing(void)
{
    CLOCK_AttachClk(CAMERA_TIMING_CLK_ATTACH);
    CLOCK_EnableClock(CAMERA_TIMING_CLK_GATE);

    g_camera_timing_clock_hz = CLOCK_GetOstimerClkFreq();
    g_camera_timing_last_vsync_tick = 0U;
    g_camera_timing_have_vsync_tick = 0U;
    g_camera_timing_last_vsync_period_ticks = 0U;
    g_camera_timing_min_vsync_period_ticks = CAMERA_TIMING_MIN_INIT;
    g_camera_timing_max_vsync_period_ticks = 0U;
    g_camera_timing_vsync_period_sample_count = 0U;
    g_camera_timing_active_frame_vsync_tick = 0U;
    g_camera_timing_last_dma_arm_ticks = 0U;
    g_camera_timing_min_dma_arm_ticks = CAMERA_TIMING_MIN_INIT;
    g_camera_timing_max_dma_arm_ticks = 0U;
    g_camera_timing_last_dma_frame_ticks = 0U;
    g_camera_timing_min_dma_frame_ticks = CAMERA_TIMING_MIN_INIT;
    g_camera_timing_max_dma_frame_ticks = 0U;

    DEBUG("FlexIO camera timing: %s free-running at %uHz for VSYNC/DMA timestamps\r\n",
          CAMERA_TIMING_SOURCE_NAME,
          g_camera_timing_clock_hz);
}

static void camera__flexio_edma_callback(edma_handle_t *handle, void *userData, bool transferDone, uint32_t tcds)
{
    (void)handle;
    (void)userData;
    (void)tcds;

    FLEXIO0->SHIFTSDEN &= ~CAMERA_FLEXIO_SHIFT_MASK;
    EDMA_StopTransfer(&g_camera_flexio_edma_handle);

    if (transferDone)
    {
        uint32_t buffer_index = g_camera_flexio_edma_active_buffer;
        uint16_t *buffer = camera__frame_buffer(buffer_index);
        uint32_t nonzero = 0U;

        for (uint32_t i = 0U; (i < CAMERA_FLEXIO_SAMPLE_WORDS) && (i < CAMERA_FRAME_PIXELS); i++)
        {
            if (buffer[i] != 0U)
            {
                nonzero++;
            }
        }

        for (uint32_t i = 0U; i < 4U; i++)
        {
            g_camera_flexio_edma_last_first_words[i] = buffer[i];
        }

        g_camera_flexio_edma_last_sample_nonzero = nonzero;
        g_camera_flexio_edma_done_count++;
        g_camera_flexio_edma_busy = 0U;
        g_camera_flexio_edma_busy_vsyncs = 0U;
        camera__timing_on_dma_done();

        nxpc__next_frame(buffer);
    }
    else
    {
        g_camera_flexio_edma_callback_error_count++;
        g_camera_flexio_edma_busy = 0U;
    }
}

static void camera__flexio_edma_abort_active(void)
{
    FLEXIO0->SHIFTSDEN &= ~CAMERA_FLEXIO_SHIFT_MASK;
    EDMA_AbortTransfer(&g_camera_flexio_edma_handle);
    g_camera_flexio_edma_busy = 0U;
    g_camera_flexio_edma_busy_vsyncs = 0U;
}

static void camera__flexio_edma_start_frame(uint32_t vsync_tick)
{
    edma_transfer_config_t transfer_config;
    uint32_t buffer_index = g_camera_flexio_edma_next_buffer;
    uint16_t *buffer = camera__frame_buffer(buffer_index);

    g_camera_flexio_edma_next_buffer = (buffer_index + 1U) & 0x01U;
    g_camera_flexio_edma_active_buffer = buffer_index;

    FLEXIO0->SHIFTSDEN &= ~CAMERA_FLEXIO_SHIFT_MASK;
    FLEXIO0->SHIFTSTAT = CAMERA_FLEXIO_SHIFT_MASK;
    FLEXIO0->SHIFTERR = CAMERA_FLEXIO_SHIFT_MASK;
    FLEXIO0->TIMSTAT = CAMERA_FLEXIO_TIMER_MASK;

    EDMA_PrepareTransfer(&transfer_config,
                         (void *)&FLEXIO0->SHIFTBUF[CAMERA_FLEXIO_SHIFTER_START],
                         CAMERA_FLEXIO_DMA_MINOR_BYTES,
                         (void *)buffer,
                         CAMERA_FLEXIO_DMA_MINOR_BYTES,
                         CAMERA_FLEXIO_DMA_MINOR_BYTES,
                         CAMERA_FRAME_BYTES,
                         kEDMA_PeripheralToMemory);

    status_t status = EDMA_SubmitTransfer(&g_camera_flexio_edma_handle, &transfer_config);

    if (status == kStatus_Success)
    {
        g_camera_flexio_edma_started_count++;
        g_camera_flexio_edma_busy = 1U;
        g_camera_flexio_edma_busy_vsyncs = 0U;
        g_camera_timing_active_frame_vsync_tick = vsync_tick;

        EDMA_StartTransfer(&g_camera_flexio_edma_handle);
        FLEXIO0->SHIFTSDEN |= (1UL << CAMERA_FLEXIO_SHIFTER_START);
        camera__timing_on_dma_armed(vsync_tick);
    }
    else
    {
        g_camera_flexio_edma_submit_error_count++;
        g_camera_flexio_edma_busy = 0U;
    }
}

static void camera__flexio_edma_on_vsync(uint32_t vsync_tick)
{
    if (g_camera_flexio_edma_busy != 0U)
    {
        g_camera_flexio_edma_busy_vsync_count++;
        g_camera_flexio_edma_busy_vsyncs++;

        if (g_camera_flexio_edma_busy_vsyncs >= CAMERA_FLEXIO_BUSY_VSYNC_TIMEOUT)
        {
            g_camera_flexio_edma_timeout_count++;
            camera__flexio_edma_abort_active();
        }

        return;
    }

    camera__flexio_edma_start_frame(vsync_tick);
}

static void camera__configure_flexio_edma(void)
{
    edma_config_t edma_config;

    EDMA_GetDefaultConfig(&edma_config);
    edma_config.enableDebugMode = true;
    EDMA_Init(DMA1, &edma_config);
    EDMA_CreateHandle(&g_camera_flexio_edma_handle, DMA1, CAMERA_FLEXIO_EDMA_CHANNEL);
    EDMA_SetChannelMux(DMA1, CAMERA_FLEXIO_EDMA_CHANNEL, CAMERA_FLEXIO_EDMA_REQUEST);
    EDMA_SetCallback(&g_camera_flexio_edma_handle, camera__flexio_edma_callback, NULL);

    memset((void *)g_camera_buffer, 0xA5, sizeof(g_camera_buffer));

    g_camera_flexio_edma_started_count = 0U;
    g_camera_flexio_edma_done_count = 0U;
    g_camera_flexio_edma_busy_vsync_count = 0U;
    g_camera_flexio_edma_timeout_count = 0U;
    g_camera_flexio_edma_submit_error_count = 0U;
    g_camera_flexio_edma_callback_error_count = 0U;
    g_camera_flexio_edma_active_buffer = 0U;
    g_camera_flexio_edma_next_buffer = 0U;
    g_camera_flexio_edma_busy = 0U;
    g_camera_flexio_edma_busy_vsyncs = 0U;
    g_camera_flexio_edma_last_sample_nonzero = CAMERA_FLEXIO_SAMPLE_WORDS;

    DEBUG("FlexIO camera eDMA: DMA1 channel %u request %u, buffers prefilled with 0xA5 before first real capture\r\n",
          CAMERA_FLEXIO_EDMA_CHANNEL,
          CAMERA_FLEXIO_EDMA_REQUEST);
}

static uint16_t camera__rgb565(uint8_t red, uint8_t green, uint8_t blue)
{
    return (uint16_t)(((uint16_t)(red & 0xF8U) << 8U) |
                      ((uint16_t)(green & 0xFCU) << 3U) |
                      ((uint16_t)(blue & 0xF8U) >> 3U));
}

static uint16_t camera__pipeline_diag_color_bar(uint32_t bar)
{
    switch (bar & 0x07U)
    {
    default:
    case 0U:
        return camera__rgb565(255U, 255U, 255U);
    case 1U:
        return camera__rgb565(255U, 255U, 0U);
    case 2U:
        return camera__rgb565(0U, 255U, 255U);
    case 3U:
        return camera__rgb565(0U, 255U, 0U);
    case 4U:
        return camera__rgb565(255U, 0U, 255U);
    case 5U:
        return camera__rgb565(255U, 0U, 0U);
    case 6U:
        return camera__rgb565(0U, 0U, 255U);
    case 7U:
        return camera__rgb565(24U, 24U, 24U);
    }
}

static void camera__fill_pipeline_diag_buffer(uint32_t buffer_index)
{
    uint16_t *frame = camera__frame_buffer(buffer_index);

    for (uint32_t y = 0; y < CONFIG__CAMERA_RESOLUTION_Y; y++)
    {
        for (uint32_t x = 0; x < CONFIG__CAMERA_RESOLUTION_X; x++)
        {
            uint32_t pixel = (y * CONFIG__CAMERA_RESOLUTION_X) + x;
            uint32_t bar = (x * 8U) / CONFIG__CAMERA_RESOLUTION_X;
            uint32_t checker = ((x / 16U) ^ (y / 16U)) & 0x01U;
            uint8_t red = (uint8_t)((x * 255U) / (CONFIG__CAMERA_RESOLUTION_X - 1U));
            uint8_t green = (uint8_t)((y * 255U) / (CONFIG__CAMERA_RESOLUTION_Y - 1U));
            uint8_t blue = (uint8_t)(checker != 0U ? 255U : 40U);

            if ((buffer_index & 0x01U) == 0U)
            {
                frame[pixel] = camera__pipeline_diag_color_bar(bar);
            }
            else
            {
                frame[pixel] = camera__rgb565(red, green, blue);
            }
        }
    }
}

static void camera__fill_pipeline_diag_buffers(void)
{
    camera__fill_pipeline_diag_buffer(0U);
    camera__fill_pipeline_diag_buffer(1U);
}

static uint32_t camera__pipeline_diag_buffer_index(uint16_t *buffer, uint32_t *buffer_index)
{
    if (buffer == camera__frame_buffer(0U))
    {
        *buffer_index = 0U;
        return 1U;
    }

    if (buffer == camera__frame_buffer(1U))
    {
        *buffer_index = 1U;
        return 1U;
    }

    return 0U;
}

void nxpc_camera__prepare_frame(uint16_t *buffer)
{
#if CONFIG__CAMERA_CAPTURE_BACKEND == CAMERA_CAPTURE_BACKEND_FLEXIO_PIPELINE_DIAG
    uint32_t buffer_index;

    if (camera__pipeline_diag_buffer_index(buffer, &buffer_index) != 0U)
    {
        camera__fill_pipeline_diag_buffer(buffer_index);
    }
#else
    (void)buffer;
#endif
}

static void camera__pipeline_diag_on_vsync(uint32_t lines)
{
#if CONFIG__CAMERA_CAPTURE_BACKEND == CAMERA_CAPTURE_BACKEND_FLEXIO_PIPELINE_DIAG
    if ((lines >= CAMERA_PIPELINE_DIAG_LINES_MIN) && (lines <= CAMERA_PIPELINE_DIAG_LINES_MAX))
    {
        uint32_t buffer_index = g_camera_pipeline_diag_next_buffer;

        g_camera_pipeline_diag_next_buffer = (buffer_index + 1U) & 0x01U;
        g_camera_pipeline_diag_frame_count++;

        nxpc__next_frame(camera__frame_buffer(buffer_index));
    }
    else
    {
        g_camera_pipeline_diag_rejected_vsync_count++;
    }
#else
    (void)lines;
#endif
}

void GPIO40_IRQHandler(void)
{
#if (defined(FSL_FEATURE_GPIO_HAS_INTERRUPT_CHANNEL_SELECT) && FSL_FEATURE_GPIO_HAS_INTERRUPT_CHANNEL_SELECT)
    uint32_t status = GPIO_GpioGetInterruptChannelFlags(GPIO4, 0U);
    GPIO_GpioClearInterruptChannelFlags(GPIO4, status & CAMERA_DIAG_IRQ_MASK, 0U);
#else
    uint32_t status = GPIO_GpioGetInterruptFlags(GPIO4);
    GPIO_GpioClearInterruptFlags(GPIO4, status & CAMERA_DIAG_IRQ_MASK);
#endif

#if CONFIG__CAMERA_CAPTURE_BACKEND == CAMERA_CAPTURE_BACKEND_FLEXIO_EDMA &&     CONFIG__CAMERA_FLEXIO_PIN_GROUP == CAMERA_FLEXIO_PIN_GROUP_PORT4
    if ((status & CAMERA_DIAG_VSYNC_MASK) != 0UL)
    {
        uint32_t irq_tick = camera__timing_read_ticks();

        g_camera_diag_vsync_count++;
        camera__timing_on_vsync(irq_tick);
        camera__flexio_edma_on_vsync(irq_tick);
    }
#else
    if ((status & CAMERA_DIAG_HSYNC_MASK) != 0UL)
    {
        g_camera_diag_hsync_count++;
    }

    if ((status & CAMERA_DIAG_VSYNC_MASK) != 0UL)
    {
        uint32_t hsync_count = g_camera_diag_hsync_count;
        uint32_t lines = hsync_count - g_camera_diag_hsync_at_last_vsync;

        g_camera_diag_vsync_count++;
        g_camera_diag_lines_last_frame = lines;
        g_camera_diag_hsync_at_last_vsync = hsync_count;
        camera__pipeline_diag_on_vsync(lines);
    }
#endif

    if ((status & CAMERA_DIAG_IRQ_MASK) == 0UL)
    {
        g_camera_diag_irq_unexpected_count++;
    }

    SDK_ISR_EXIT_BARRIER;
}

void GPIO00_IRQHandler(void)
{
#if (defined(FSL_FEATURE_GPIO_HAS_INTERRUPT_CHANNEL_SELECT) && FSL_FEATURE_GPIO_HAS_INTERRUPT_CHANNEL_SELECT)
    uint32_t status = GPIO_GpioGetInterruptChannelFlags(GPIO0, 0U);
    GPIO_GpioClearInterruptChannelFlags(GPIO0, status & CAMERA_REF_IRQ_MASK, 0U);
#else
    uint32_t status = GPIO_GpioGetInterruptFlags(GPIO0);
    GPIO_GpioClearInterruptFlags(GPIO0, status & CAMERA_REF_IRQ_MASK);
#endif

    if ((status & CAMERA_REF_HSYNC_MASK) != 0UL)
    {
        g_camera_ref_hsync_count++;
    }

    if ((status & CAMERA_REF_VSYNC_MASK) != 0UL)
    {
        uint32_t hsync_count = g_camera_ref_hsync_count;

        g_camera_ref_vsync_count++;
        g_camera_ref_lines_last_frame = hsync_count - g_camera_ref_hsync_at_last_vsync;
        g_camera_ref_hsync_at_last_vsync = hsync_count;

#if CONFIG__CAMERA_CAPTURE_BACKEND == CAMERA_CAPTURE_BACKEND_FLEXIO_EDMA &&     CONFIG__CAMERA_FLEXIO_PIN_GROUP == CAMERA_FLEXIO_PIN_GROUP_PORT1
        /*
         * The PORT1 group puts frame start on P0_4, which this handler already
         * serviced as a reference counter. Promote it to driving capture.
         * P0_11 is FlexIO HREF in this group, so g_camera_ref_hsync_count stops
         * advancing and the lines-per-frame figure above reads zero.
         */
        {
            uint32_t irq_tick = camera__timing_read_ticks();

            camera__timing_on_vsync(irq_tick);
            camera__flexio_edma_on_vsync(irq_tick);
        }
#endif
    }

    if ((status & CAMERA_REF_IRQ_MASK) == 0UL)
    {
        g_camera_ref_irq_unexpected_count++;
    }

    SDK_ISR_EXIT_BARRIER;
}

static uint32_t camera__diag_take_gpio_flag(GPIO_Type *base, uint32_t pin)
{
    uint32_t was_set = GPIO_PinGetInterruptFlag(base, pin);

    if (was_set != 0UL)
    {
        GPIO_PinClearInterruptFlag(base, pin);
    }

    return was_set;
}

void nxpc_camera__service(void)
{
#if CONFIG__CAMERA_CAPTURE_BACKEND == CAMERA_CAPTURE_BACKEND_FLEXIO_PIPELINE_DIAG
    static uint32_t last_frame_report;
    static uint32_t last_vsync_report;
    uint32_t frame_count = g_camera_pipeline_diag_frame_count;
    uint32_t vsync_count = g_camera_diag_vsync_count;
    uint32_t frame_delta = frame_count - last_frame_report;
    uint32_t vsync_delta = vsync_count - last_vsync_report;
    uint32_t should_report = 0U;

    if (frame_delta >= 30U)
    {
        should_report = 1U;
    }
    else if ((frame_delta == 0U) && (vsync_delta >= 300U))
    {
        should_report = 1U;
    }

    if (should_report != 0U)
    {
        uint32_t p4_pclk_seen = camera__diag_take_gpio_flag(GPIO4, CAMERA_DIAG_PCLK_PIN);

        DEBUG("cam_pipe frames=%u(+%u) reject=%u p4_vs=%u(+%u) p4_hs=%u p4_lines=%u p4_pclk=%u next_buf=%u line_win=%u-%u\r\n",
              frame_count,
              frame_delta,
              g_camera_pipeline_diag_rejected_vsync_count,
              vsync_count,
              vsync_delta,
              g_camera_diag_hsync_count,
              g_camera_diag_lines_last_frame,
              p4_pclk_seen,
              g_camera_pipeline_diag_next_buffer,
              CAMERA_PIPELINE_DIAG_LINES_MIN,
              CAMERA_PIPELINE_DIAG_LINES_MAX);

        last_frame_report = frame_count;
        last_vsync_report = vsync_count;
    }
#elif CONFIG__CAMERA_CAPTURE_BACKEND == CAMERA_CAPTURE_BACKEND_FLEXIO_EDMA
    static uint32_t last_done_report;
    static uint32_t last_started_report;
    static uint32_t last_vsync_report;
    static uint32_t last_busy_vsync_report;
    static uint32_t last_timeout_report;
    static uint32_t last_submit_error_report;
    static uint32_t last_callback_error_report;
    static uint32_t last_report_tick;
    static uint32_t last_report_ms;
    uint32_t done_count = g_camera_flexio_edma_done_count;
    uint32_t started_count = g_camera_flexio_edma_started_count;
    uint32_t vsync_count = g_camera_diag_vsync_count;
    uint32_t busy_vsync_count = g_camera_flexio_edma_busy_vsync_count;
    uint32_t timeout_count = g_camera_flexio_edma_timeout_count;
    uint32_t submit_error_count = g_camera_flexio_edma_submit_error_count;
    uint32_t callback_error_count = g_camera_flexio_edma_callback_error_count;
    uint32_t done_delta = done_count - last_done_report;
    uint32_t started_delta = started_count - last_started_report;
    uint32_t vsync_delta = vsync_count - last_vsync_report;
    uint32_t busy_vsync_delta = busy_vsync_count - last_busy_vsync_report;
    uint32_t timeout_delta = timeout_count - last_timeout_report;
    uint32_t submit_error_delta = submit_error_count - last_submit_error_report;
    uint32_t callback_error_delta = callback_error_count - last_callback_error_report;
    uint32_t now_tick = camera__timing_read_ticks();
    uint32_t now_ms = e_tick__get_ms();
    uint32_t elapsed_ticks;
    uint32_t elapsed_ms;
    uint32_t should_report = 0U;

    if (last_report_tick == 0U)
    {
        last_report_tick = now_tick;
    }

    if (last_report_ms == 0U)
    {
        last_report_ms = now_ms;
    }

    elapsed_ticks = now_tick - last_report_tick;
    elapsed_ms = now_ms - last_report_ms;

    if (elapsed_ms >= 1000U)
    {
        should_report = 1U;
    }
    else if (done_delta >= 30U)
    {
        should_report = 1U;
    }
    else if ((done_delta == 0U) && (vsync_delta >= 30U))
    {
        should_report = 1U;
    }
    else if ((g_camera_flexio_edma_timeout_count != 0U) && (vsync_delta >= 30U))
    {
        should_report = 1U;
    }

    if (should_report != 0U)
    {
        uint32_t remaining = 0U;
        uint32_t fps_x100 = camera__timing_fps_x100(done_delta, elapsed_ticks);
        uint32_t elapsed_us = camera__timing_ticks_to_us(elapsed_ticks);
        uint32_t vsync_last_us = camera__timing_ticks_to_us(g_camera_timing_last_vsync_period_ticks);
        uint32_t vsync_min_us = (g_camera_timing_vsync_period_sample_count == 0U) ? 0U :
            camera__timing_ticks_to_us(g_camera_timing_min_vsync_period_ticks);
        uint32_t vsync_max_us = camera__timing_ticks_to_us(g_camera_timing_max_vsync_period_ticks);
        uint32_t dma_arm_us = camera__timing_ticks_to_us(g_camera_timing_last_dma_arm_ticks);
        uint32_t dma_arm_min_us = (g_camera_timing_min_dma_arm_ticks == CAMERA_TIMING_MIN_INIT) ? 0U :
            camera__timing_ticks_to_us(g_camera_timing_min_dma_arm_ticks);
        uint32_t dma_arm_max_us = camera__timing_ticks_to_us(g_camera_timing_max_dma_arm_ticks);
        uint32_t dma_frame_us = camera__timing_ticks_to_us(g_camera_timing_last_dma_frame_ticks);
        uint32_t dma_frame_min_us = (g_camera_timing_min_dma_frame_ticks == CAMERA_TIMING_MIN_INIT) ? 0U :
            camera__timing_ticks_to_us(g_camera_timing_min_dma_frame_ticks);
        uint32_t dma_frame_max_us = camera__timing_ticks_to_us(g_camera_timing_max_dma_frame_ticks);

        if (g_camera_flexio_edma_busy != 0U)
        {
            remaining = EDMA_GetRemainingMajorLoopCount(DMA1, CAMERA_FLEXIO_EDMA_CHANNEL);
        }

        DEBUG("cam_dma t_ms=%u t_us=%u fps=%u.%02u done=%u(+%u) start=%u(+%u) p4_vs=%u(+%u) vs_us=%u/%u/%u arm_us=%u/%u/%u frame_us=%u/%u/%u busy=%u busy_vs=%u(+%u) timeout=%u(+%u) submit_err=%u(+%u) cb_err=%u(+%u) sample_nz=%u first=%04x,%04x,%04x,%04x rem=%u shiftstat=%02x shifterr=%02x timstat=%02x\r\n",
              elapsed_ms,
              elapsed_us,
              fps_x100 / 100U,
              fps_x100 % 100U,
              done_count,
              done_delta,
              started_count,
              started_delta,
              vsync_count,
              vsync_delta,
              vsync_last_us,
              vsync_min_us,
              vsync_max_us,
              dma_arm_us,
              dma_arm_min_us,
              dma_arm_max_us,
              dma_frame_us,
              dma_frame_min_us,
              dma_frame_max_us,
              g_camera_flexio_edma_busy,
              busy_vsync_count,
              busy_vsync_delta,
              timeout_count,
              timeout_delta,
              submit_error_count,
              submit_error_delta,
              callback_error_count,
              callback_error_delta,
              g_camera_flexio_edma_last_sample_nonzero,
              g_camera_flexio_edma_last_first_words[0],
              g_camera_flexio_edma_last_first_words[1],
              g_camera_flexio_edma_last_first_words[2],
              g_camera_flexio_edma_last_first_words[3],
              remaining,
              FLEXIO0->SHIFTSTAT,
              FLEXIO0->SHIFTERR,
              FLEXIO0->TIMSTAT);

        last_done_report = done_count;
        last_started_report = started_count;
        last_vsync_report = vsync_count;
        last_busy_vsync_report = busy_vsync_count;
        last_timeout_report = timeout_count;
        last_submit_error_report = submit_error_count;
        last_callback_error_report = callback_error_count;
        last_report_tick = now_tick;
        last_report_ms = now_ms;
    }
#endif
}

static void camera__run_flexio_diag_loop(void)
{
    uint32_t last_hsync_count = 0;
    uint32_t last_vsync_count = 0;
    uint32_t last_ref_hsync_count = 0;
    uint32_t last_ref_vsync_count = 0;
    uint32_t elapsed_ms = 0;

    while (1)
    {
        SDK_DelayAtLeastUs(1000000U, SystemCoreClock);
        elapsed_ms += 1000U;

        uint32_t hsync_count = g_camera_diag_hsync_count;
        uint32_t vsync_count = g_camera_diag_vsync_count;
        uint32_t ref_hsync_count = g_camera_ref_hsync_count;
        uint32_t ref_vsync_count = g_camera_ref_vsync_count;
        uint32_t hsync_delta = hsync_count - last_hsync_count;
        uint32_t vsync_delta = vsync_count - last_vsync_count;
        uint32_t ref_hsync_delta = ref_hsync_count - last_ref_hsync_count;
        uint32_t ref_vsync_delta = ref_vsync_count - last_ref_vsync_count;
        uint32_t p4_pclk_seen = camera__diag_take_gpio_flag(GPIO4, CAMERA_DIAG_PCLK_PIN);
        uint32_t p4_vsync_irq_disabled = g_camera_diag_vsync_irq_disabled;
        uint32_t p4_vsync_flag_seen = 0U;
        uint32_t ref_pclk_seen = camera__diag_take_gpio_flag(GPIO0, CAMERA_REF_PCLK_PIN);

        if ((vsync_delta > CAMERA_DIAG_VSYNC_IRQ_MAX_PER_SECOND) && (p4_vsync_irq_disabled == 0U))
        {
            GPIO_SetPinInterruptConfig(GPIO4, CAMERA_DIAG_VSYNC_PIN, kGPIO_FlagRisingEdge);
            g_camera_diag_vsync_irq_disabled = 1U;
            p4_vsync_irq_disabled = 1U;
        }

        if (p4_vsync_irq_disabled != 0U)
        {
            p4_vsync_flag_seen = camera__diag_take_gpio_flag(GPIO4, CAMERA_DIAG_VSYNC_PIN);
        }

        last_hsync_count = hsync_count;
        last_vsync_count = vsync_count;
        last_ref_hsync_count = ref_hsync_count;
        last_ref_vsync_count = ref_vsync_count;

        DEBUG("cam_diag t_ms=%u p4_hs=%u(+%u) p4_vs_irq=%u(+%u) p4_lines=%u p4_pclk=%u p4_vs_flag=%u p4_vs_off=%u ref_hs=%u(+%u) ref_vs=%u(+%u) ref_lines=%u ref_pclk=%u lvl_p4=%u%u%u lvl_ref=%u%u%u unexp=%u/%u\r\n",
              elapsed_ms,
              hsync_count,
              hsync_delta,
              vsync_count,
              vsync_delta,
              g_camera_diag_lines_last_frame,
              p4_pclk_seen,
              p4_vsync_flag_seen,
              p4_vsync_irq_disabled,
              ref_hsync_count,
              ref_hsync_delta,
              ref_vsync_count,
              ref_vsync_delta,
              g_camera_ref_lines_last_frame,
              ref_pclk_seen,
              GPIO_PinRead(GPIO4, CAMERA_DIAG_PCLK_PIN),
              GPIO_PinRead(GPIO4, CAMERA_DIAG_HSYNC_PIN),
              GPIO_PinRead(GPIO4, CAMERA_DIAG_VSYNC_PIN),
              GPIO_PinRead(GPIO0, CAMERA_REF_PCLK_PIN),
              GPIO_PinRead(GPIO0, CAMERA_REF_HSYNC_PIN),
              GPIO_PinRead(GPIO0, CAMERA_REF_VSYNC_PIN),
              g_camera_diag_irq_unexpected_count,
              g_camera_ref_irq_unexpected_count);
    }
}

#if CONFIG__CAMERA_CAPTURE_BACKEND == CAMERA_CAPTURE_BACKEND_SMARTDMA_EZH
static void nxpc_camera__init_smartdma_ezh(void)
{

	DEBUG("Camera capture backend: SMARTDMA_EZH\r\n");
	DEBUG("Build EZH Camera application\r\n");

	uint32_t words_assembled = bunny_build((uint32_t* )&ezh_binary[0],
							   sizeof(ezh_binary),
							   EZH_Camera_320240_Whole_Buf);

	DEBUG(VT100_YELLOW"%d "VT100_DEFAULT" 32bit words assembled (%d bytes) \r\n"VT100_DEFAULT,words_assembled, words_assembled<<2);


    INPUTMUX_Init(INPUTMUX0);


    /*
     * AHB matrix priority - UNDOCUMENTED REGISTER.
     *
     * Raises the SmartDMA/EZH master's arbitration priority for AHB bus
     * transactions. SYSCON->AHBMATPRIO does not appear in the reference manual;
     * this is tribal knowledge carried from an early NXP sample. Do not expect
     * to find bit definitions for it.
     *
     * It matters because the EZH samples the camera's pixel clock in software
     * and cannot tolerate being held off the bus - losing arbitration shows up
     * as corrupted pixels, not as a stall you can detect.
     *
     * This is genuinely SmartDMA-specific and stays here. Note the FlexIO
     * capture path has no equivalent and does not appear to need one: it uses
     * eDMA, a different bus master, and captures cleanly at the same frame rate
     * with zero shifter errors. If FlexIO capture is ever pushed to a higher
     * pixel clock, revisit whether eDMA wants similar treatment.
     *
     * Cache enables used to live here too. They are not EZH-specific and now
     * run for every build in nxpc__flash_cache_init() from nxpc__init(); leaving
     * them here meant a FlexIO build ran with the flash data cache disabled.
     */
    SYSCON->AHBMATPRIO |= (0x3<<4)|(0x3<<6);
    

    INPUTMUX_AttachSignal(INPUTMUX0, 0, kINPUTMUX_GpioPort0Pin4ToSmartDma); //vsync
    INPUTMUX_AttachSignal(INPUTMUX0, 1, kINPUTMUX_GpioPort0Pin11ToSmartDma);//hsync
    INPUTMUX_AttachSignal(INPUTMUX0, 2, kINPUTMUX_GpioPort0Pin5ToSmartDma); //pclk
    /* Turn off clock to inputmux to save power. Clock is only needed to make changes */
    INPUTMUX_Deinit(INPUTMUX0);

    PORT1->PCR[4] = (7 << 8) | (1 << 12); // EZH_PIO0, PIO1_4,P1_4/EZH_LCD_D0_CAMERA_D0/SAI0_TXD1
    PORT1->PCR[5] = (7 << 8) | (1 << 12); // EZH_PIO1, PIO1_5,P1_5/EZH_LCD_D1_CAMERA_D1
    PORT1->PCR[6] = (7 << 8) | (1 << 12); // EZH_PIO2, PIO1_6,P1_6/EZH_LCD_D2_CAMERA_D2
    PORT1->PCR[7] = (7 << 8) | (1 << 12); // EZH_PIO3, PIO1_7,P1_7/EZH_LCD_D3_CAMERA_D3
    PORT3->PCR[4] = (7 << 8) | (1 << 12); // EZH_PIO4, PIO1_8,P1_8/EZH_LCD_D4_CAMERA_D4
    PORT3->PCR[5] = (7 << 8) | (1 << 12); // EZH_PIO5, PIO1_9,P1_9/EZH_LCD_D5_CAMERA_D5
    PORT1->PCR[10] = (7 << 8) | (1 << 12); // EZH_PIO6, PIO1_10,P1_10/EZH_LCD_D6_CAMERA_D6
    PORT1->PCR[11] = (7 << 8) | (1 << 12); // EZH_PIO7, PIO1_11,P1_11/EZH_LCD_D7_CAMERA_D7

    PORT1->PCR[17] = (7 << 8) | (1 << 12); // EZH_PIO13, PIO1_17

	smartdmaParam.smartdma_stack = (uint32_t*)g_samrtdma_stack;
	smartdmaParam.p_buffer  	 = (uint32_t*)g_camera_buffer;


    SMARTDMA_InstallCallback(ezh_camera_callback, NULL);
    NVIC_EnableIRQ(SMARTDMA_IRQn);
    NVIC_SetPriority(SMARTDMA_IRQn, 3);

    memset((void *)g_camera_buffer, 2, sizeof(g_camera_buffer));

    SMARTDMA_InitWithoutFirmware();

    #define SMARTDMA_HANDSHAKE_EVENT  0U
	#define SMARTDMA_HANDSHAKE_ENABLE 1U
	#define SMARTDMA_MASK_RESP        2U
	#define SMARTDMA_ENABLE_AHBBUF    3U
	#define SMARTDMA_ENABLE_GPISYNCH  4U

    LPC_EZH_ARCH_B0->EZHB_CTRL    = (0xC0DE0000U | (1U << SMARTDMA_ENABLE_GPISYNCH));
    LPC_EZH_ARCH_B0->EZHB_ARM2EZH = ((uint32_t)(uint8_t *)(&smartdmaParam)) | 0x02;
    LPC_EZH_ARCH_B0->EZHB_BOOT = (uint32_t)(&ezh_binary[0]);
    LPC_EZH_ARCH_B0->EZHB_CTRL    = 0xC0DE0011U | (0U << SMARTDMA_MASK_RESP) | (0U << SMARTDMA_ENABLE_AHBBUF); /* BOOT */


    DEBUG("EZH interface configured\r\n");

}
#endif

static void nxpc_camera__init_flexio_diag(void)
{
    DEBUG("Camera capture backend: FLEXIO_DIAG\r\n");

    camera__configure_flexio_diag_inputs();
    camera__configure_reference_diag_inputs();

    DEBUG("SmartDMA/EZH camera capture disabled for FlexIO diagnostic wiring test\r\n");
    camera__run_flexio_diag_loop();
}

static void nxpc_camera__init_flexio_pipeline_diag(void)
{
    DEBUG("Camera capture backend: FLEXIO_PIPELINE_DIAG\r\n");

    g_camera_pipeline_diag_frame_count = 0U;
    g_camera_pipeline_diag_rejected_vsync_count = 0U;
    g_camera_pipeline_diag_next_buffer = 0U;

    camera__fill_pipeline_diag_buffers();
    camera__configure_flexio_diag_inputs();
    camera__configure_reference_diag_inputs();

    DEBUG("SmartDMA/EZH camera capture disabled for FlexIO pipeline diagnostic\r\n");
    DEBUG("FlexIO pipeline diagnostic emits synthetic RGB565 frames on plausible VSYNC/HSYNC timing\r\n");
    DEBUG("FlexIO pipeline diagnostic line window: %u-%u HSYNC edges per VSYNC for active %ux%u mode\r\n",
          CAMERA_PIPELINE_DIAG_LINES_MIN,
          CAMERA_PIPELINE_DIAG_LINES_MAX,
          CONFIG__CAMERA_RESOLUTION_X,
          CONFIG__CAMERA_RESOLUTION_Y);
}

static void nxpc_camera__init_flexio_edma(void)
{
    DEBUG("Camera capture backend: FLEXIO_EDMA\r\n");

    camera__configure_flexio_camera();
    camera__configure_flexio_edma();
    camera__configure_frame_timing();
    camera__configure_flexio_edma_pins();

    DEBUG("SmartDMA/EZH camera capture disabled for FlexIO eDMA test\r\n");
    DEBUG("FlexIO eDMA captures one %ux%u RGB565 frame per VSYNC into ping-pong frame buffers\r\n",
          CONFIG__CAMERA_RESOLUTION_X,
          CONFIG__CAMERA_RESOLUTION_Y);
    DEBUG("With D0-D7 held low, expected captured pixels are 0x0000 black; first RTT sample_nz should trend to 0\r\n");
}

void nxpc_camera__init(void)
{
    /*
     * Sensor bring-up is common to every capture backend - the OV5640 needs its
     * clock, its SCCB bus, and its register configuration regardless of how the
     * pixels are subsequently captured. This sequence used to be duplicated in
     * all four backend init functions.
     *
     * Ordering note: this now runs before the backend-specific setup in every
     * case. Previously the EZH path ran bunny_build() first and the pipeline
     * diagnostic filled its buffers first; both are RAM-only operations that do
     * not touch the camera, so moving them after this is safe.
     */
    camera__configure_xclk();
    camera__configure_i2c();
    camera__init_sensor();

#if CONFIG__CAMERA_CAPTURE_BACKEND == CAMERA_CAPTURE_BACKEND_SMARTDMA_EZH
    nxpc_camera__init_smartdma_ezh();
#elif CONFIG__CAMERA_CAPTURE_BACKEND == CAMERA_CAPTURE_BACKEND_FLEXIO_DIAG
    nxpc_camera__init_flexio_diag();
#elif CONFIG__CAMERA_CAPTURE_BACKEND == CAMERA_CAPTURE_BACKEND_FLEXIO_PIPELINE_DIAG
    nxpc_camera__init_flexio_pipeline_diag();
#elif CONFIG__CAMERA_CAPTURE_BACKEND == CAMERA_CAPTURE_BACKEND_FLEXIO_EDMA
    nxpc_camera__init_flexio_edma();
#else
#error Invalid CONFIG__CAMERA_CAPTURE_BACKEND.
#endif
}


#define BOARD_CAMERA_I2C_CLOCK_FREQ       CLOCK_GetLPFlexCommClkFreq(BOARD_CAMERA_I2C_INSTANCE)

void camera__i2c_init()
{
    CLOCK_EnableClock(kCLOCK_LPI2c7);

    LP_FLEXCOMM_Init(BOARD_CAMERA_I2C_INSTANCE, LP_FLEXCOMM_PERIPH_LPI2C);

    lpi2c_master_config_t lpi2cConfig = {0};
    LPI2C_MasterGetDefaultConfig(&lpi2cConfig);
    LPI2C_MasterInit(LPI2C7, &lpi2cConfig, BOARD_CAMERA_I2C_CLOCK_FREQ);
}
