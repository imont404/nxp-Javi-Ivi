/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "board.h"
#include "pin_mux.h"
#include "fsl_flexio_camera.h"
#include "fsl_iomuxc.h"

#include "ov7670_driver.h"
#include "fsl_sccb_master_driver.h"
#include "flexio_ov7670.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
#pragma data_alignment = 32
static FrameBuffer_t g_FlexioCameraFrameBuffer[OV7670_FRAME_BUFFER_CNT] @"NonCacheable";
static pFrameBuffer_t pFlexioCameraFrameBuffer = g_FlexioCameraFrameBuffer;
pFrameBuffer_t pLCDFrameBuffer;

/* OV7670 configuration structures. */
static const ov7670_handler_t s_Ov7670CameraHandler = {
    .i2cBase = BOARD_CAMERA_I2C_INST,
    .i2cDeviceAddr = OV7670_I2C_ADDR
};

static const ov7670_advanced_config_t s_Ov7670CameraAdvancedConfig =
{
    .filter = (ov7670_filter_config_t *)&OV7670_FILTER_DISABLED,
    .nightMode = (ov7670_night_mode_config_t *)&OV7670_NIGHT_MODE_AUTO_FR_DIVBY2,
    .whiteBalance = (ov7670_white_balance_config_t *)&OV7670_WHITE_BALANCE_DEFAULT,
    .lightMode = (ov7670_light_mode_config_t *)&OV7670_LIGHT_MODE_AUTO,
    .colorSaturation = (ov7670_color_saturation_config_t *)&OV7670_COLOR_SATURATION_DEFAULT,
    .specialEffect = (ov7670_special_effect_config_t *)&OV7670_SPECIAL_EFFECT_DISABLED,
    .gammaCurveSlope = (ov7670_gamma_curve_slope_config_t *)&OV7670_GAMMA_CURVE_SLOPE_DEFAULT,
};

static const ov7670_config_t s_Ov7670CameraConfig =
{
    .outputFormat = (ov7670_output_format_config_t *)&OV7670_FORMAT_RGB565,
    .resolution =
    {
        .width = OV7670_FRAME_WIDTH,
        .height = OV7670_FRAME_HEIGHT,
    },
    .frameRate = (ov7670_frame_rate_config_t *)&OV7670_30FPS_24MHZ_XCLK,
    .contrast = 0x30,
    .brightness = 0x80,
    .advancedConfig = (ov7670_advanced_config_t *)&s_Ov7670CameraAdvancedConfig,
};

static const FLEXIO_CAMERA_Type s_FlexioCameraDevice = {
    .flexioBase = BOARD_CAMERA_FLEXIO_INST,
    .datPinStartIdx = BOARD_CAMERA_FLEXIO_DATA_PIN_START_INDEX,
    .pclkPinIdx = BOARD_CAMERA_FLEXIO_PCLK_PIN_INDEX,
    .hrefPinIdx = BOARD_CAMERA_FLEXIO_HREF_PIN_INDEX,
    .shifterStartIdx = 0U,
    .shifterCount = 8,
    .timerIdx = 0U,
};

static flexio_camera_config_t s_FlexioCameraConfig;

/*******************************************************************************
 * Code
 ******************************************************************************/
/* Use FlexIO Timer 1 to generate 24MHz clock. */
static void FLEXIO_CameraXclkConfig(void)
{
    flexio_timer_config_t timerConfig;

    timerConfig.triggerSelect = 0u;
    timerConfig.triggerPolarity = kFLEXIO_TimerTriggerPolarityActiveHigh;
    timerConfig.triggerSource = kFLEXIO_TimerTriggerSourceInternal;
    timerConfig.pinConfig = kFLEXIO_PinConfigOutput;
    timerConfig.pinSelect = BOARD_CAMERA_FLEXIO_XCLK_PIN_INDEX;
    timerConfig.pinPolarity = kFLEXIO_PinActiveHigh;
    timerConfig.timerMode = kFLEXIO_TimerModeDual8BitPWM;
    timerConfig.timerOutput = kFLEXIO_TimerOutputZeroNotAffectedByReset;
    timerConfig.timerDecrement = kFLEXIO_TimerDecSrcOnFlexIOClockShiftTimerOutput;
    timerConfig.timerReset = kFLEXIO_TimerResetNever;
    timerConfig.timerDisable = kFLEXIO_TimerDisableNever;
    timerConfig.timerEnable = kFLEXIO_TimerEnabledAlways;
    timerConfig.timerStop = kFLEXIO_TimerStopBitDisabled;
    timerConfig.timerStart = kFLEXIO_TimerStartBitDisabled;
    timerConfig.timerCompare = 0x0201; /* 120MHz clock source generates 24MHz clock.*/

    FLEXIO_SetTimerConfig(BOARD_CAMERA_FLEXIO_INST, 1u, &timerConfig);
}

static void configFlexIO(void)
{
    /* Configure FlexIO. */
    FLEXIO_Reset(BOARD_CAMERA_FLEXIO_INST);
    /*Init the flexio to the camera mode */
    FLEXIO_CAMERA_GetDefaultConfig(&s_FlexioCameraConfig);
    FLEXIO_CAMERA_Init(&s_FlexioCameraDevice, &s_FlexioCameraConfig);
    /* Clear all the flag. */
    FLEXIO_CAMERA_ClearStatusFlags(&s_FlexioCameraDevice, kFLEXIO_CAMERA_RxDataRegFullFlag | kFLEXIO_CAMERA_RxErrorFlag);

    FLEXIO_CameraXclkConfig();

    /* Enable FlexIO. */
    FLEXIO_CAMERA_Enable(&s_FlexioCameraDevice, true);
}

void BOARD_Init_I2C_GPIO_Pins(void) {
    gpio_pin_config_t gpio_pin_config ={
        .direction = kGPIO_DigitalOutput,
        .outputLogic = 1U,
        .interruptMode = kGPIO_NoIntmode
    };

    GPIO_PinInit(BOARD_I2C_SCL_GPIO_BASE, BOARD_I2C_SCL_PIN_INDEX, &gpio_pin_config);
    GPIO_PinInit(BOARD_I2C_SDA_GPIO_BASE, BOARD_I2C_SDA_PIN_INDEX, &gpio_pin_config);
    
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_07_GPIOMUX_IO21, 0U);
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_08_GPIOMUX_IO22, 0U);
    
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_07_GPIOMUX_IO21, 0xD8A0U);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_08_GPIOMUX_IO22, 0xD8A0U);
}

static void configCamera(void)
{
    ov7670_status_t ov7670Status;

    /* Init the I2C to communicate with the SCCB in OV7670 */
#if BOARD_I2C_EMU_WITH_GPIO
    BOARD_Init_I2C_GPIO_Pins();
#else
    BOARD_Init_I2C_Pins();
    FLEXIO_Ov7670SccbInit(BOARD_CAMERA_I2C_INST);
#endif
    /* Configure the OV7670 camera */
    do
    {
        ov7670Status = OV7670_Init(&s_Ov7670CameraHandler, (ov7670_config_t *)&s_Ov7670CameraConfig);
    } while (ov7670Status != kStatus_OV7670_Success);
}

static void configDMA(void)
{
    /* Configure DMA TCD */
    DMA0->TCD[FLEXIO_CAMERA_DMA_CHN].SADDR = FLEXIO_CAMERA_GetRxBufferAddress(&s_FlexioCameraDevice);
    DMA0->TCD[FLEXIO_CAMERA_DMA_CHN].SOFF = 0u;
    DMA0->TCD[FLEXIO_CAMERA_DMA_CHN].ATTR = DMA_ATTR_SMOD(0u) |
                                            DMA_ATTR_SSIZE(5u) |
                                            DMA_ATTR_DMOD(0u) |
                                            DMA_ATTR_DSIZE(5u);
    DMA0->TCD[FLEXIO_CAMERA_DMA_CHN].NBYTES_MLNO = 32u;
    DMA0->TCD[FLEXIO_CAMERA_DMA_CHN].SLAST = 0u;
    DMA0->TCD[FLEXIO_CAMERA_DMA_CHN].DADDR = (uint32_t)(*pFlexioCameraFrameBuffer);
    DMA0->TCD[FLEXIO_CAMERA_DMA_CHN].DOFF = 32u;
    DMA0->TCD[FLEXIO_CAMERA_DMA_CHN].CITER_ELINKNO = (OV7670_FRAME_BYTES / 32u);
    DMA0->TCD[FLEXIO_CAMERA_DMA_CHN].DLAST_SGA = 0;
    DMA0->TCD[FLEXIO_CAMERA_DMA_CHN].CSR = 0u;
    DMA0->TCD[FLEXIO_CAMERA_DMA_CHN].CSR |= DMA_CSR_DREQ_MASK;
    DMA0->TCD[FLEXIO_CAMERA_DMA_CHN].BITER_ELINKNO = (OV7670_FRAME_BYTES / 32u);

    /* Configure DMA MUX Source */
    DMAMUX->CHCFG[FLEXIO_CAMERA_DMA_CHN] = DMAMUX->CHCFG[FLEXIO_CAMERA_DMA_CHN] &
                                            (~DMAMUX_CHCFG_SOURCE_MASK) | 
                                            DMAMUX_CHCFG_SOURCE(FLEXIO_CAMERA_DMA_MUX_SRC);
    /* Enable DMA channel. */
    DMAMUX->CHCFG[FLEXIO_CAMERA_DMA_CHN] |= DMAMUX_CHCFG_ENBL_MASK;
}

void FLEXIO_Ov7670CaptureStart(void)
{
    if(pFlexioCameraFrameBuffer == &g_FlexioCameraFrameBuffer[OV7670_FRAME_BUFFER_CNT-1])
    {
        pFlexioCameraFrameBuffer = &g_FlexioCameraFrameBuffer[0];
    }
    else
    {
        pFlexioCameraFrameBuffer++;
    }

    DMA0->TCD[FLEXIO_CAMERA_DMA_CHN].DADDR = (uint32_t)pFlexioCameraFrameBuffer;

    /* Enable DMA channel request. */
    DMA0->SERQ = DMA_SERQ_SERQ(FLEXIO_CAMERA_DMA_CHN);
}

void FLEXIO_Ov7670CaptureDone(void)
{
    FLEXIO_CAMERA_ClearStatusFlags(&s_FlexioCameraDevice,
                                   kFLEXIO_CAMERA_RxDataRegFullFlag | kFLEXIO_CAMERA_RxErrorFlag);

    pLCDFrameBuffer = pFlexioCameraFrameBuffer;
}

void FLEXIO_Ov7670Init(void)
{
    configFlexIO();

    configCamera();

    configDMA();

    /* Enable FlexIO DMA request. */
    FLEXIO_CAMERA_EnableRxDMA(&s_FlexioCameraDevice, true);
    /* Init VSYNC pin, and enable the GPIO interrupt. */
    BOARD_Camera_VsynPinInit();
}

