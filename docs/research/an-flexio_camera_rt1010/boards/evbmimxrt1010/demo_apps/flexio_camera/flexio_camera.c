/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "pin_mux.h"
#include "fsl_edma.h"
#include "fsl_dmamux.h"
#include "fsl_gpio.h"
#include "flexio_ov7670.h"
#include "lpspi_ili9341.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
extern pFrameBuffer_t pLCDFrameBuffer;
volatile bool newFrame = false;

/*******************************************************************************
 * Code
 ******************************************************************************/
void BOARD_CAMERA_VSYNC_IRQHandler(void)
{
    GPIO_ClearPinsInterruptFlags(BOARD_CAMERA_VSYNC_GPIO_BASE, 1U << BOARD_CAMERA_VSYNC_PIN_INDEX);

    FLEXIO_Ov7670CaptureDone();

    ILI9341_startDMATrsf(0, 0, OV7670_FRAME_WIDTH-1u, OV7670_FRAME_HEIGHT-1u, (uint16_t *)(*pLCDFrameBuffer));

    FLEXIO_Ov7670CaptureStart();

    __DSB();
}

/*!
 * @brief Main function
 */
int main(void)
{
    edma_config_t edmaConfig;

    /* Init board hardware. */
    BOARD_ConfigMPU();
    BOARD_InitPins();
    BOARD_BootClockRUN();

    /* Configure DMA an DMAMUX */
    DMAMUX_Init(DMAMUX);
    EDMA_GetDefaultConfig(&edmaConfig);
    edmaConfig.enableDebugMode = true;
    EDMA_Init(DMA0, &edmaConfig);

    /* LCD initialization. */
    ILI9341_Init();
    ILI9341_setDMATrsf();

    /* Camera initialization. */
    FLEXIO_Ov7670Init();

    while (1)
    {
    }
}
