

#include "fsl_ov7670.h"
#include "fsl_ov5640.h"
#include "fsl_lpi2c.h"
#include "fsl_lpflexcomm.h"
#include "fsl_smartdma.h"
#include "fsl_inputmux.h"
#include "fsl_gpio.h"
#include "fsl_port.h"
#include "board.h"
#include "bunny_build.h"
#include "cr_section_macros.h"
#include "pin_mux.h"
#include "bv_camera__interface.h"
#include "avc__master_config.h"

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
#define CAMERA_DIAG_IRQ_MASK (CAMERA_DIAG_HSYNC_MASK)

#define CAMERA_REF_PCLK_PIN (5U)
#define CAMERA_REF_HSYNC_PIN (11U)
#define CAMERA_REF_VSYNC_PIN (4U)
#define CAMERA_REF_PCLK_MASK (1UL << CAMERA_REF_PCLK_PIN)
#define CAMERA_REF_HSYNC_MASK (1UL << CAMERA_REF_HSYNC_PIN)
#define CAMERA_REF_VSYNC_MASK (1UL << CAMERA_REF_VSYNC_PIN)
#define CAMERA_REF_IRQ_MASK (CAMERA_REF_HSYNC_MASK | CAMERA_REF_VSYNC_MASK)


__BSS(SRAM_H) volatile uint32_t ezh_binary[512];

__BSS(FRAME_BUFFERS) uint16_t g_camera_buffer[FSL_VIDEO_EXTRACT_WIDTH(CONFIG__CAMERA_RESOLUTION) *
											  FSL_VIDEO_EXTRACT_HEIGHT(CONFIG__CAMERA_RESOLUTION) *
											  2];



volatile uint32_t next_buffer=0;

volatile uint32_t line;


volatile uint8_t img_ready = 0;   /* non-zero when new data signaled by SmartDMA IRQ */

static volatile uint8_t g_samrtdma_stack[EZH_STACK_SIZE];

volatile uint32_t ezh_binary[512];

static volatile uint32_t g_camera_diag_hsync_count;
static volatile uint32_t g_camera_diag_irq_unexpected_count;
static volatile uint32_t g_camera_ref_hsync_count;
static volatile uint32_t g_camera_ref_vsync_count;
static volatile uint32_t g_camera_ref_lines_last_frame;
static volatile uint32_t g_camera_ref_hsync_at_last_vsync;
static volatile uint32_t g_camera_ref_irq_unexpected_count;

//This needs to be global
smartdma_camera_param_t smartdmaParam;


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



static void ezh_camera_callback(void *param)
{
	//EZH_SetExternalFlag(1) ;

	line = LPC_EZH_ARCH_B0->EZHB_EZH2ARM;
	if(line = 0xFFFFFF)
	{
			next_buffer++;

			next_buffer&=0x01;

			uint16_t * buf;

			if(next_buffer==0)
				buf = &g_camera_buffer[0];
			else
				buf = &g_camera_buffer[sizeof(g_camera_buffer)/4];

			smartdmaParam.p_buffer = (uint32_t *)buf;

			avc__next_frame(buf);
	}

}



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
    GPIO_SetPinInterruptConfig(GPIO4, CAMERA_DIAG_VSYNC_PIN, kGPIO_FlagRisingEdge);

    NVIC_ClearPendingIRQ(GPIO40_IRQn);
    NVIC_SetPriority(GPIO40_IRQn, 4);
    EnableIRQ(GPIO40_IRQn);

    DEBUG("FlexIO camera diag inputs configured: PCLK=P4_20 HSYNC/HREF=P4_21 VSYNC=P4_22\r\n");
    DEBUG("FlexIO camera diag interrupts: P4_21 HSYNC/HREF rising edges on GPIO40_IRQn; P4_20 PCLK and P4_22 VSYNC flags polled\r\n");
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

void GPIO40_IRQHandler(void)
{
#if (defined(FSL_FEATURE_GPIO_HAS_INTERRUPT_CHANNEL_SELECT) && FSL_FEATURE_GPIO_HAS_INTERRUPT_CHANNEL_SELECT)
    uint32_t status = GPIO_GpioGetInterruptChannelFlags(GPIO4, 0U);
    GPIO_GpioClearInterruptChannelFlags(GPIO4, status & CAMERA_DIAG_IRQ_MASK, 0U);
#else
    uint32_t status = GPIO_GpioGetInterruptFlags(GPIO4);
    GPIO_GpioClearInterruptFlags(GPIO4, status & CAMERA_DIAG_IRQ_MASK);
#endif

    if ((status & CAMERA_DIAG_HSYNC_MASK) != 0UL)
    {
        g_camera_diag_hsync_count++;
    }

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

static void camera__run_flexio_diag_loop(void)
{
    uint32_t last_hsync_count = 0;
    uint32_t last_ref_hsync_count = 0;
    uint32_t last_ref_vsync_count = 0;
    uint32_t elapsed_ms = 0;

    while (1)
    {
        SDK_DelayAtLeastUs(1000000U, SystemCoreClock);
        elapsed_ms += 1000U;

        uint32_t hsync_count = g_camera_diag_hsync_count;
        uint32_t ref_hsync_count = g_camera_ref_hsync_count;
        uint32_t ref_vsync_count = g_camera_ref_vsync_count;
        uint32_t hsync_delta = hsync_count - last_hsync_count;
        uint32_t ref_hsync_delta = ref_hsync_count - last_ref_hsync_count;
        uint32_t ref_vsync_delta = ref_vsync_count - last_ref_vsync_count;
        uint32_t p4_pclk_seen = camera__diag_take_gpio_flag(GPIO4, CAMERA_DIAG_PCLK_PIN);
        uint32_t p4_vsync_seen = camera__diag_take_gpio_flag(GPIO4, CAMERA_DIAG_VSYNC_PIN);
        uint32_t ref_pclk_seen = camera__diag_take_gpio_flag(GPIO0, CAMERA_REF_PCLK_PIN);

        last_hsync_count = hsync_count;
        last_ref_hsync_count = ref_hsync_count;
        last_ref_vsync_count = ref_vsync_count;

        DEBUG("cam_diag t_ms=%u p4_hs=%u(+%u) p4_pclk=%u p4_vs=%u ref_hs=%u(+%u) ref_vs=%u(+%u) ref_lines=%u ref_pclk=%u lvl_p4=%u%u%u lvl_ref=%u%u%u unexp=%u/%u\r\n",
              elapsed_ms,
              hsync_count,
              hsync_delta,
              p4_pclk_seen,
              p4_vsync_seen,
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

static void avc_camera__init_smartdma_ezh(void)
{

	DEBUG("Camera capture backend: SMARTDMA_EZH\r\n");
	DEBUG("Build EZH Camera application\r\n");

	uint32_t words_assembled = bunny_build((uint32_t* )&ezh_binary[0],
							   sizeof(ezh_binary),
							   EZH_Camera_320240_Whole_Buf);

	DEBUG(VT100_YELLOW"%d "VT100_DEFAULT" 32bit words assembled (%d bytes) \r\n"VT100_DEFAULT,words_assembled, words_assembled<<2);

    camera__configure_xclk();
    camera__configure_i2c();
    camera__init_sensor();

    INPUTMUX_Init(INPUTMUX0);


    SYSCON->LPCAC_CTRL &= ~1;                                  // rocky: enable LPCAC ICache
    SYSCON->NVM_CTRL &= SYSCON->NVM_CTRL & ~(1 << 2 | 1 << 4); // enable flash Data cache     /* init I3C0*/
    SYSCON->AHBMATPRIO |= (0x3<<4)|(0x3<<6); // Give priority to SmartDMA
    

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

static void avc_camera__init_flexio_diag(void)
{
    DEBUG("Camera capture backend: FLEXIO_DIAG\r\n");

    camera__configure_xclk();
    camera__configure_i2c();
    camera__init_sensor();
    camera__configure_flexio_diag_inputs();
    camera__configure_reference_diag_inputs();

    DEBUG("SmartDMA/EZH camera capture disabled for FlexIO diagnostic wiring test\r\n");
    camera__run_flexio_diag_loop();
}

void avc_camera__init(void)
{
#if CONFIG__CAMERA_CAPTURE_BACKEND == CAMERA_CAPTURE_BACKEND_SMARTDMA_EZH
    avc_camera__init_smartdma_ezh();
#elif CONFIG__CAMERA_CAPTURE_BACKEND == CAMERA_CAPTURE_BACKEND_FLEXIO_DIAG
    avc_camera__init_flexio_diag();
#elif CONFIG__CAMERA_CAPTURE_BACKEND == CAMERA_CAPTURE_BACKEND_FLEXIO_EDMA
#error CAMERA_CAPTURE_BACKEND_FLEXIO_EDMA is not implemented yet.
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
