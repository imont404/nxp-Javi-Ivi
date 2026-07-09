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

#include "fsl_sccb_master_driver.h"
#include "board.h"
#include "fsl_gpio.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#if BOARD_I2C_EMU_WITH_GPIO
#define WRITE_PIN   GPIO_WritePinOutput
#define READ_PIN    GPIO_ReadPinInput
#define SDA         BOARD_I2C_SDA_GPIO_BASE, BOARD_I2C_SDA_PIN_INDEX
#define SCL         BOARD_I2C_SCL_GPIO_BASE, BOARD_I2C_SCL_PIN_INDEX
#define SDA_IN()    (BOARD_I2C_SDA_GPIO_BASE->GDIR &= ~(1U << BOARD_I2C_SDA_PIN_INDEX))
#define SDA_OUT()   (BOARD_I2C_SDA_GPIO_BASE->GDIR |= 1U << BOARD_I2C_SDA_PIN_INDEX)

#endif

/*******************************************************************************
 * Code
 ******************************************************************************/
void FLEXIO_Ov7670SccbInit(LPI2C_Type *base)
{
    base->MCR |= LPI2C_MCR_RST_MASK;
    base->MCR &= ~LPI2C_MCR_RST_MASK;
    base->MCR |= LPI2C_MCR_DBGEN_MASK;

    base->MCFGR1 = LPI2C_MCFGR1_PRESCALE(1);                                    /* 12MHz/2 = 6MHz */
    base->MCFGR2 = LPI2C_MCFGR2_BUSIDLE(5);
    base->MCCR0 = LPI2C_MCCR0_DATAVD(4) | LPI2C_MCCR0_SETHOLD(20) |
                  LPI2C_MCCR0_CLKHI(29) |LPI2C_MCCR0_CLKLO(29);                 /* 6MHz/60 = 100KHz */

    base->MCR |=  LPI2C_MCR_MEN_MASK;
}

#if BOARD_I2C_EMU_WITH_GPIO
static void Delay(void)
{
    for (uint32_t i = 0; i < 500u; i++)
    {
        __NOP();
    }
}

static void gpioI2cStart(void)
{
    Delay();
    WRITE_PIN(SDA, 0u);
    SDA_OUT();
    Delay();
    Delay();
    WRITE_PIN(SCL, 0u);
    Delay();
}

static void gpioI2cStop(void)
{
    WRITE_PIN(SDA, 0u);
    SDA_OUT();
    Delay();
    WRITE_PIN(SCL, 1u);
    Delay();
    Delay();
    WRITE_PIN(SDA, 1u);
    Delay();
}

static void gpioI2cSendData(uint8_t value)
{
    SDA_OUT();
    for (uint32_t i = 0; i < 8; i++)
    {
        WRITE_PIN(SDA, (value & 0x80) ? 1u: 0u);
        value <<= 1u;
        Delay();
        WRITE_PIN(SCL, 1u);
        Delay();
        Delay();
        WRITE_PIN(SCL, 0u);
        Delay();
    }
}

static void gpioI2cSendAck(bool ack)
{
    WRITE_PIN(SDA, (ack & 0x80) ? 0u: 1u);
    SDA_OUT();
    Delay();
    WRITE_PIN(SCL, 1u);
    Delay();
    Delay();
    WRITE_PIN(SCL, 0u);
    Delay();
}

static uint8_t gpioI2cReceiveData(void)
{
    uint8_t value = 0u;

    SDA_IN();
    for (uint32_t i = 0; i < 8; i++)
    {
        Delay();
        WRITE_PIN(SCL, 1u);
        Delay();
        value <<= 1u;
        value |= READ_PIN(SDA);
        Delay();
        WRITE_PIN(SCL, 0u);
        Delay();
    }
    
    return value;
}

static bool gpioI2cReceiveAck(void)
{
    bool ack;

    SDA_IN();
    Delay();
    WRITE_PIN(SCL, 1u);
    Delay();
    ack = READ_PIN(SDA);
    Delay();
    WRITE_PIN(SCL, 0u);
    Delay();
    
    return ack ? false : true;    // ACK-TRUE, NACK-FALSE
}
#endif

ov7670_status_t I2C_Read_OV7670_Reg(LPI2C_Type *base, uint8_t deviceAddr, uint8_t regAddr, uint8_t *rxBuff, uint32_t rxSize)
{
#if !(BOARD_I2C_EMU_WITH_GPIO)
    uint32_t value;

    base->MCR |=  LPI2C_MCR_RRF_MASK | LPI2C_MCR_RTF_MASK;

    base->MTDR = LPI2C_MTDR_CMD(0x04) | LPI2C_MTDR_DATA((deviceAddr<<1) | 0);   /* Start, send device address, writing sequence. */
    base->MTDR = LPI2C_MTDR_CMD(0x00) | LPI2C_MTDR_DATA(regAddr);               /* Send device register address. */
    base->MTDR = LPI2C_MTDR_CMD(0x02);                                          /* Send stop. */

    while((base->MFSR & LPI2C_MFSR_TXCOUNT_MASK) ||
          (base->MSR & LPI2C_MSR_BBF_MASK) || (base->MSR & LPI2C_MSR_MBF_MASK))
    {}

    base->MTDR = LPI2C_MTDR_CMD(0x04) | LPI2C_MTDR_DATA((deviceAddr<<1) | 1);   /* Restart, send device address, reading sequence. */
    base->MTDR = LPI2C_MTDR_CMD(0x01) | LPI2C_MTDR_DATA(rxSize - 1);            /* Receive one bytes. */

    while(rxSize--)
    {
        do
        {
            value = base->MRDR;
        }while(value & LPI2C_MRDR_RXEMPTY_MASK);
        *rxBuff++ = value & 0xFF;
    }

    base->MTDR = LPI2C_MTDR_CMD(0x02);                                          /* Send stop. */

    while((base->MFSR & LPI2C_MFSR_TXCOUNT_MASK) ||
          (base->MSR & LPI2C_MSR_BBF_MASK) || (base->MSR & LPI2C_MSR_MBF_MASK))
    {}

    if(base->MSR & LPI2C_MSR_NDF_MASK)
    {
        return kStatus_OV7670_I2CFail;
    }
    else
    {
        return kStatus_OV7670_Success;
    }
#else
    bool ack = true;

    gpioI2cStart();
    gpioI2cSendData((deviceAddr<<1) | 0);                                       /* Send device address, writing sequence. */
    ack &= gpioI2cReceiveAck();
    gpioI2cSendData(regAddr);
    ack &= gpioI2cReceiveAck();
    gpioI2cStop();
    gpioI2cStart();
    gpioI2cSendData((deviceAddr<<1) | 1);
    ack &= gpioI2cReceiveAck();
    while(rxSize--)
    {
        *rxBuff++ = gpioI2cReceiveData();
        if(rxSize)
        {
            gpioI2cSendAck(true);
        }
        else
        {
            gpioI2cSendAck(false);
        }
    }
    gpioI2cStop();
    return ack ? kStatus_OV7670_Success : kStatus_OV7670_I2CFail;
#endif
}

ov7670_status_t I2C_Write_OV7670_Reg(LPI2C_Type *base, uint8_t deviceAddr, uint8_t regAddr, uint8_t value)
{
#if !(BOARD_I2C_EMU_WITH_GPIO)
    base->MCR |= LPI2C_MCR_RTF_MASK;

    base->MTDR = LPI2C_MTDR_CMD(0x04) | LPI2C_MTDR_DATA((deviceAddr<<1) | 0);   /* Start, send device address, writing sequence. */
    base->MTDR = LPI2C_MTDR_CMD(0x00) | LPI2C_MTDR_DATA(regAddr);               /* Send device register address. */
    base->MTDR = LPI2C_MTDR_CMD(0x00) | LPI2C_MTDR_DATA(value);                 /* Send data. */
    base->MTDR = LPI2C_MTDR_CMD(0x02);                                          /* Send stop. */

    while((base->MFSR & LPI2C_MFSR_TXCOUNT_MASK) ||
          (base->MSR & LPI2C_MSR_BBF_MASK) || (base->MSR & LPI2C_MSR_MBF_MASK))
    {}

    if(base->MSR & LPI2C_MSR_NDF_MASK)
    {
        return kStatus_OV7670_I2CFail;
    }
    else
    {
        return kStatus_OV7670_Success;
    }
#else
    bool ack = true;

    gpioI2cStart();
    gpioI2cSendData((deviceAddr<<1) | 0);                                       /* Send device address, writing sequence. */
    ack &= gpioI2cReceiveAck();
    gpioI2cSendData(regAddr);
    ack &= gpioI2cReceiveAck();
    gpioI2cSendData(value);
    ack &= gpioI2cReceiveAck();
    gpioI2cStop();

    return ack ? kStatus_OV7670_Success : kStatus_OV7670_I2CFail;
#endif
}
