/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _BOARD_H_
#define _BOARD_H_

#include "clock_config.h"
#include "fsl_common.h"
#include "fsl_gpio.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief The board name */
#define BOARD_NAME "MIMXRT1010-EVB"

/* The UART to use for debug messages. */
#define BOARD_DEBUG_UART_TYPE kSerialPort_Uart
#define BOARD_DEBUG_UART_BASEADDR (uint32_t) LPUART1
#define BOARD_DEBUG_UART_INSTANCE 1U

#define BOARD_DEBUG_UART_CLK_FREQ BOARD_DebugConsoleSrcFreq()

#define BOARD_UART_IRQ LPUART1_IRQn
#define BOARD_UART_IRQ_HANDLER LPUART1_IRQHandler

#ifndef BOARD_DEBUG_UART_BAUDRATE
#define BOARD_DEBUG_UART_BAUDRATE (115200U)
#endif /* BOARD_DEBUG_UART_BAUDRATE */

/* @Brief Board accelerator sensor configuration */
#define BOARD_ACCEL_I2C_BASEADDR LPI2C1
#define BOARD_ACCEL_I2C_CLOCK_SOURCE_SELECT (0U)
#define BOARD_ACCEL_I2C_CLOCK_SOURCE_DIVIDER (5U)
#define BOARD_ACCEL_I2C_CLOCK_FREQ (CLOCK_GetFreq(kCLOCK_Usb1PllClk) / 8 / (BOARD_ACCEL_I2C_CLOCK_SOURCE_DIVIDER + 1U))

#define BOARD_CODEC_I2C_BASEADDR LPI2C1
#define BOARD_CODEC_I2C_INSTANCE 1U
#define BOARD_CODEC_I2C_CLOCK_SOURCE_SELECT (0U)
#define BOARD_CODEC_I2C_CLOCK_SOURCE_DIVIDER (5U)
#define BOARD_CODEC_I2C_CLOCK_FREQ (10000000U)

/*! @brief The USER_LED used for board */
#define LOGIC_LED_ON (0U)
#define LOGIC_LED_OFF (1U)
#ifndef BOARD_USER_LED_GPIO
#define BOARD_USER_LED_GPIO GPIO1
#endif
#ifndef BOARD_USER_LED_GPIO_PIN
#define BOARD_USER_LED_GPIO_PIN (11U)
#endif

#define USER_LED_INIT(output)                                            \
    GPIO_PinWrite(BOARD_USER_LED_GPIO, BOARD_USER_LED_GPIO_PIN, output); \
    BOARD_USER_LED_GPIO->GDIR |= (1U << BOARD_USER_LED_GPIO_PIN) /*!< Enable target USER_LED */
#define USER_LED_OFF() \
    GPIO_PortClear(BOARD_USER_LED_GPIO, 1U << BOARD_USER_LED_GPIO_PIN)                 /*!< Turn off target USER_LED */
#define USER_LED_ON() GPIO_PortSet(BOARD_USER_LED_GPIO, 1U << BOARD_USER_LED_GPIO_PIN) /*!<Turn on target USER_LED*/
#define USER_LED_TOGGLE()                                       \
    GPIO_PinWrite(BOARD_USER_LED_GPIO, BOARD_USER_LED_GPIO_PIN, \
                  0x1 ^ GPIO_PinRead(BOARD_USER_LED_GPIO, BOARD_USER_LED_GPIO_PIN)) /*!< Toggle target USER_LED */

/*! @brief Define the port interrupt number for the board switches */
#ifndef BOARD_USER_BUTTON_GPIO
#define BOARD_USER_BUTTON_GPIO GPIO2
#endif
#ifndef BOARD_USER_BUTTON_GPIO_PIN
#define BOARD_USER_BUTTON_GPIO_PIN (5U)
#endif
#define BOARD_USER_BUTTON_IRQ GPIO2_Combined_0_15_IRQn
#define BOARD_USER_BUTTON_IRQ_HANDLER GPIO2_Combined_0_15_IRQHandler
#define BOARD_USER_BUTTON_NAME "SW4"

/*! @brief The flash size */
#define BOARD_FLASH_SIZE (0x1000000U)

/* USB PHY condfiguration */
#define BOARD_USB_PHY_D_CAL (0x0CU)
#define BOARD_USB_PHY_TXCAL45DP (0x06U)
#define BOARD_USB_PHY_TXCAL45DM (0x06U)

#define BOARD_ARDUINO_INT_IRQ (GPIO1_Combined_16_31_IRQn)
#define BOARD_ARDUINO_I2C_IRQ (LPI2C1_IRQn)
#define BOARD_ARDUINO_I2C_INDEX (1)

/*! @brief The WIFI-QCA shield pin. */
#define BOARD_INITSILEX2401SHIELD_PWRON_GPIO GPIO1                    /*!< GPIO device name: GPIO */
#define BOARD_INITSILEX2401SHIELD_PWRON_PORT 1U                       /*!< PORT device index: 1 */
#define BOARD_INITSILEX2401SHIELD_PWRON_GPIO_PIN 8U                   /*!< PIO1 pin index: 8 */
#define BOARD_INITSILEX2401SHIELD_PWRON_PIN_NAME GPIO1_08             /*!< Pin name */
#define BOARD_INITSILEX2401SHIELD_PWRON_LABEL "PWRON"                 /*!< Label */
#define BOARD_INITSILEX2401SHIELD_PWRON_NAME "PWRON"                  /*!< Identifier name */
#define BOARD_INITSILEX2401SHIELD_PWRON_DIRECTION kGPIO_DigitalOutput /*!< Direction */

#define BOARD_INITSILEX2401SHIELD_IRQ_GPIO GPIO1                   /*!< GPIO device name: GPIO */
#define BOARD_INITSILEX2401SHIELD_IRQ_PORT 1U                      /*!< PORT device index: 1 */
#define BOARD_INITSILEX2401SHIELD_IRQ_GPIO_PIN 4U                  /*!< PIO1 pin index: 4 */
#define BOARD_INITSILEX2401SHIELD_IRQ_PIN_NAME GPIO1_04            /*!< Pin name */
#define BOARD_INITSILEX2401SHIELD_IRQ_LABEL "IRQ"                  /*!< Label */
#define BOARD_INITSILEX2401SHIELD_IRQ_NAME "IRQ"                   /*!< Identifier name */
#define BOARD_INITSILEX2401SHIELD_IRQ_DIRECTION kGPIO_DigitalInput /*!< Direction */

/* FlexIO Camera configurations */
#define BOARD_CAMERA_FLEXIO_INST                    FLEXIO1
#define BOARD_CAMERA_FLEXIO_DATA_PIN_START_INDEX    1
#define BOARD_CAMERA_FLEXIO_DATA_PIN_END_INDEX      8
#define BOARD_CAMERA_FLEXIO_XCLK_PIN_INDEX          9
#define BOARD_CAMERA_FLEXIO_PCLK_PIN_INDEX          10
#define BOARD_CAMERA_FLEXIO_HREF_PIN_INDEX          11
#define BOARD_CAMERA_VSYNC_GPIO_BASE                GPIO1
#define BOARD_CAMERA_VSYNC_PIN_INDEX                6u
#define BOARD_CAMERA_VSYNC_IRQn                     GPIO1_Combined_0_15_IRQn
#define BOARD_CAMERA_VSYNC_IRQHandler               GPIO1_Combined_0_15_IRQHandler

/* I2C for camera SCCB. */
#define BOARD_CAMERA_I2C_INST                       LPI2C2

/* I2C for camera SCCB. */
#define BOARD_I2C_SCL_GPIO_BASE         GPIO1
#define BOARD_I2C_SDA_GPIO_BASE         GPIO1
#define BOARD_I2C_SCL_PIN_INDEX         22u
#define BOARD_I2C_SDA_PIN_INDEX         21u

/* GPIO for SPI LCD pins. */
#define BOARD_SPILCD_SPI_INST           LPSPI1
#define BOARD_SPILCD_RST_GPIO_BASE      GPIO1
#define BOARD_SPILCD_RST_PIN_INDEX      7u
#define BOARD_SPILCD_DC_GPIO_BASE       GPIO1
#define BOARD_SPILCD_DC_PIN_INDEX       8u
#define BOARD_SPILCD_CS_GPIO_BASE       GPIO1
#define BOARD_SPILCD_CS_PIN_INDEX       19u
#define BOARD_SPILCD_LED_GPIO_BASE      GPIO2
#define BOARD_SPILCD_LED_PIN_INDEX      13u

/* DMA channel assigements. */
#define FLEXIO_CAMERA_DMA_CHN           0u
#define FLEXIO_CAMERA_DMA_MUX_SRC       (kDmaRequestMuxFlexIO1Request0Request1 & 0xFF)

#define SPI_LCD_DMA_CHN                 1u
#define SPI_LCD_DMA_MUX_SRC             (kDmaRequestMuxLPSPI1Tx & 0xFF)
#define SPI_LCD_DMA_IRQn                DMA1_IRQn
#define SPI_LCD_DMA_IRQHandler          DMA1_IRQHandler


#define BOARD_I2C_EMU_WITH_GPIO         true

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
 * API
 ******************************************************************************/
uint32_t BOARD_DebugConsoleSrcFreq(void);

void BOARD_InitDebugConsole(void);
void BOARD_ConfigMPU(void);
#if defined(SDK_I2C_BASED_COMPONENT_USED) && SDK_I2C_BASED_COMPONENT_USED
void BOARD_InitDebugConsole(void);
void BOARD_LPI2C_Init(LPI2C_Type *base, uint32_t clkSrc_Hz);
status_t BOARD_LPI2C_Send(LPI2C_Type *base,
                          uint8_t deviceAddress,
                          uint32_t subAddress,
                          uint8_t subaddressSize,
                          uint8_t *txBuff,
                          uint8_t txBuffSize);
status_t BOARD_LPI2C_Receive(LPI2C_Type *base,
                             uint8_t deviceAddress,
                             uint32_t subAddress,
                             uint8_t subaddressSize,
                             uint8_t *rxBuff,
                             uint8_t rxBuffSize);
void BOARD_Accel_I2C_Init(void);
status_t BOARD_Accel_I2C_Send(uint8_t deviceAddress, uint32_t subAddress, uint8_t subaddressSize, uint32_t txBuff);
status_t BOARD_Accel_I2C_Receive(
    uint8_t deviceAddress, uint32_t subAddress, uint8_t subaddressSize, uint8_t *rxBuff, uint8_t rxBuffSize);
void BOARD_Codec_I2C_Init(void);
status_t BOARD_Codec_I2C_Send(
    uint8_t deviceAddress, uint32_t subAddress, uint8_t subAddressSize, const uint8_t *txBuff, uint8_t txBuffSize);
status_t BOARD_Codec_I2C_Receive(
    uint8_t deviceAddress, uint32_t subAddress, uint8_t subAddressSize, uint8_t *rxBuff, uint8_t rxBuffSize);
#endif /* SDK_I2C_BASED_COMPONENT_USED */

void BOARD_SPILCD_RstPinInit(void);
void BOARD_SPILCD_DcPinInit(void);
void BOARD_SPILCD_CsPinInit(void);
void BOARD_SPILCD_LedPinInit(void);
void BOARD_SPILCD_pullRstPin(bool value);
void BOARD_SPILCD_pullDcPin(bool value);
void BOARD_SPILCD_pullCsPin(bool value);
void BOARD_SPILCD_pullLedPin(bool value);
void BOARD_Camera_RstPinInit(void);
void BOARD_Camera_PdwnPinInit(void);
void BOARD_Camera_VsynPinInit(void);
void BOARD_Camera_pullRstPin(bool value);
void BOARD_Camera_pullPdwnPin(bool value);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _BOARD_H_ */
