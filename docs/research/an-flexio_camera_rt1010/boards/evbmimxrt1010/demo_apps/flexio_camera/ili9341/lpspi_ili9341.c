/*
 * Copyright (c) 2015-2016, Freescale Semiconductor, Inc.
 * All rights reserved.
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
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
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


/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "board.h"
#include "lpspi_ili9341.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/
static uint32_t LPSPI_Cmd = 0;

/*******************************************************************************
 * Functions
 ******************************************************************************/

static void ILI9341_LPSPI_Init(void)
{
    LPSPI_Cmd = LPSPI_Cmd & (~LPSPI_TCR_CPHA_MASK) | LPSPI_TCR_CPHA(0U)
                          & (~LPSPI_TCR_CPOL_MASK) | LPSPI_TCR_CPOL(0U)
                          & (~LPSPI_TCR_PRESCALE_MASK) | LPSPI_TCR_PRESCALE(0U)
                          & (~LPSPI_TCR_PCS_MASK) | LPSPI_TCR_PCS(1U)
                          & (~LPSPI_TCR_LSBF_MASK) | LPSPI_TCR_LSBF(0U)
                          & (~LPSPI_TCR_BYSW_MASK) | LPSPI_TCR_BYSW(0U)
                          & (~LPSPI_TCR_CONT_MASK) | LPSPI_TCR_CONT(0U)
                          & (~LPSPI_TCR_CONTC_MASK) | LPSPI_TCR_CONTC(0U)
                          & (~LPSPI_TCR_RXMSK_MASK) | LPSPI_TCR_RXMSK(1U)
                          & (~LPSPI_TCR_TXMSK_MASK) | LPSPI_TCR_TXMSK(0U)
                          & (~LPSPI_TCR_WIDTH_MASK) | LPSPI_TCR_WIDTH(0U)
                          & (~LPSPI_TCR_FRAMESZ_MASK) | LPSPI_TCR_FRAMESZ(7U);

    BOARD_SPILCD_SPI_INST->CFGR1 |= LPSPI_CFGR1_MASTER_MASK;
    BOARD_SPILCD_SPI_INST->CCR = LPSPI_CCR_SCKDIV(2U) | LPSPI_CCR_PCSSCK(0U) | LPSPI_CCR_SCKPCS(0U);
    BOARD_SPILCD_SPI_INST->FCR = LPSPI_FCR_TXWATER(0x0);
    BOARD_SPILCD_SPI_INST->DER = LPSPI_DER_TDDE_MASK;
    BOARD_SPILCD_SPI_INST->CR |= LPSPI_CR_DBGEN_MASK | LPSPI_CR_MEN_MASK;
}

static void Delay(uint32_t t)
{
    for(; t>0; t--)
    {
        for(volatile uint32_t i=10000; i>0; i--)
        {
        }
    }
}

static void ILI9341_LPSPI_ConfigFrameSize(uint8_t size)
{
    LPSPI_Cmd = LPSPI_Cmd & (~LPSPI_TCR_FRAMESZ_MASK) | LPSPI_TCR_FRAMESZ(size-1U);
    BOARD_SPILCD_SPI_INST->TCR = LPSPI_Cmd;
}

static void ILI9341_LPSPI_Write(uint32_t bytes)
{
    BOARD_SPILCD_pullCsPin(false);
    BOARD_SPILCD_SPI_INST->TDR = bytes;
    while(!(BOARD_SPILCD_SPI_INST->SR & LPSPI_SR_TCF_MASK))
    {
    }
    BOARD_SPILCD_SPI_INST->SR |= LPSPI_SR_TCF_MASK;
    BOARD_SPILCD_pullCsPin(true);
}

static void configLCD(void)
{
    /* Reset LCD module via the reset pin */
    BOARD_SPILCD_pullRstPin(false);
    Delay(50);
    BOARD_SPILCD_pullRstPin(true);
    Delay(50);

    ILI9341_LPSPI_ConfigFrameSize(8U);

    BOARD_SPILCD_pullDcPin(false);
    ILI9341_LPSPI_Write(0xCB);
    BOARD_SPILCD_pullDcPin(true);
    ILI9341_LPSPI_Write(0x39);
    ILI9341_LPSPI_Write(0x2C);
    ILI9341_LPSPI_Write(0x00);
    ILI9341_LPSPI_Write(0x34);
    ILI9341_LPSPI_Write(0x02);

    BOARD_SPILCD_pullDcPin(false);
    ILI9341_LPSPI_Write(0xCF);
    BOARD_SPILCD_pullDcPin(true);
    ILI9341_LPSPI_Write(0x00);
    ILI9341_LPSPI_Write(0XC1);
    ILI9341_LPSPI_Write(0X30);

    BOARD_SPILCD_pullDcPin(false);
    ILI9341_LPSPI_Write(0xE8);
    BOARD_SPILCD_pullDcPin(true);
    ILI9341_LPSPI_Write(0x85);
    ILI9341_LPSPI_Write(0x00);
    ILI9341_LPSPI_Write(0x78);

    BOARD_SPILCD_pullDcPin(false);
    ILI9341_LPSPI_Write(0xEA);
    BOARD_SPILCD_pullDcPin(true);
    ILI9341_LPSPI_Write(0x00);
    ILI9341_LPSPI_Write(0x00);

    BOARD_SPILCD_pullDcPin(false);
    ILI9341_LPSPI_Write(0xED);
    ILI9341_LPSPI_Write(0x64);
    ILI9341_LPSPI_Write(0x03);
    ILI9341_LPSPI_Write(0X12);
    ILI9341_LPSPI_Write(0X81);

    BOARD_SPILCD_pullDcPin(false);
    ILI9341_LPSPI_Write(0xF7);
    BOARD_SPILCD_pullDcPin(true);
    ILI9341_LPSPI_Write(0x20);

    BOARD_SPILCD_pullDcPin(false);
    ILI9341_LPSPI_Write(0xC0);
    BOARD_SPILCD_pullDcPin(true);
    ILI9341_LPSPI_Write(0x23);

    BOARD_SPILCD_pullDcPin(false);
    ILI9341_LPSPI_Write(0xC1);
    BOARD_SPILCD_pullDcPin(true);
    ILI9341_LPSPI_Write(0x10);

    BOARD_SPILCD_pullDcPin(false);
    ILI9341_LPSPI_Write(0xC5);
    BOARD_SPILCD_pullDcPin(true);
    ILI9341_LPSPI_Write(0x3e);
    ILI9341_LPSPI_Write(0x28);

    BOARD_SPILCD_pullDcPin(false);
    ILI9341_LPSPI_Write(0xC7);
    BOARD_SPILCD_pullDcPin(true);
    ILI9341_LPSPI_Write(0x86);

    BOARD_SPILCD_pullDcPin(false);
    ILI9341_LPSPI_Write(0x36);
    BOARD_SPILCD_pullDcPin(true);
    ILI9341_LPSPI_Write(0x28);

    BOARD_SPILCD_pullDcPin(false);
    ILI9341_LPSPI_Write(0x3A);
    BOARD_SPILCD_pullDcPin(true);
    ILI9341_LPSPI_Write(0x55);

    BOARD_SPILCD_pullDcPin(false);
    ILI9341_LPSPI_Write(0xB1);
    BOARD_SPILCD_pullDcPin(true);
    ILI9341_LPSPI_Write(0x00);
    ILI9341_LPSPI_Write(0x18);

    BOARD_SPILCD_pullDcPin(false);
    ILI9341_LPSPI_Write(0xB6);
    BOARD_SPILCD_pullDcPin(true);
    ILI9341_LPSPI_Write(0x08);
    ILI9341_LPSPI_Write(0x82);
    ILI9341_LPSPI_Write(0x27);

    BOARD_SPILCD_pullDcPin(false);
    ILI9341_LPSPI_Write(0xF2);
    BOARD_SPILCD_pullDcPin(true);
    ILI9341_LPSPI_Write(0x00);

    BOARD_SPILCD_pullDcPin(false);
    ILI9341_LPSPI_Write(0x26);
    BOARD_SPILCD_pullDcPin(true);
    ILI9341_LPSPI_Write(0x01);

    BOARD_SPILCD_pullDcPin(false);
    ILI9341_LPSPI_Write(0xE0);
    BOARD_SPILCD_pullDcPin(true);
    ILI9341_LPSPI_Write(0x0F);
    ILI9341_LPSPI_Write(0x31);
    ILI9341_LPSPI_Write(0x2B);
    ILI9341_LPSPI_Write(0x0C);
    ILI9341_LPSPI_Write(0x0E);
    ILI9341_LPSPI_Write(0x08);
    ILI9341_LPSPI_Write(0x4E);
    ILI9341_LPSPI_Write(0xF1);
    ILI9341_LPSPI_Write(0x37);
    ILI9341_LPSPI_Write(0x07);
    ILI9341_LPSPI_Write(0x10);
    ILI9341_LPSPI_Write(0x03);
    ILI9341_LPSPI_Write(0x0E);
    ILI9341_LPSPI_Write(0x09);
    ILI9341_LPSPI_Write(0x00);

    BOARD_SPILCD_pullDcPin(false);
    ILI9341_LPSPI_Write(0XE1);
    BOARD_SPILCD_pullDcPin(true);
    ILI9341_LPSPI_Write(0x00);
    ILI9341_LPSPI_Write(0x0E);
    ILI9341_LPSPI_Write(0x14);
    ILI9341_LPSPI_Write(0x03);
    ILI9341_LPSPI_Write(0x11);
    ILI9341_LPSPI_Write(0x07);
    ILI9341_LPSPI_Write(0x31);
    ILI9341_LPSPI_Write(0xC1);
    ILI9341_LPSPI_Write(0x48);
    ILI9341_LPSPI_Write(0x08);
    ILI9341_LPSPI_Write(0x0F);
    ILI9341_LPSPI_Write(0x0C);
    ILI9341_LPSPI_Write(0x31);
    ILI9341_LPSPI_Write(0x36);
    ILI9341_LPSPI_Write(0x0F);

    BOARD_SPILCD_pullDcPin(false);
    ILI9341_LPSPI_Write(0x11);
    Delay(120);

    BOARD_SPILCD_pullDcPin(false);
    ILI9341_LPSPI_Write(0x29);
    ILI9341_LPSPI_Write(0x2c);
}

static void ILI9341_SetWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    ILI9341_LPSPI_ConfigFrameSize(8U);

    BOARD_SPILCD_pullDcPin(false);
    ILI9341_LPSPI_Write(0x2A);
    BOARD_SPILCD_pullDcPin(true);
    ILI9341_LPSPI_Write(x1 >> 8U);
    ILI9341_LPSPI_Write(x1 & 0xFF);
    ILI9341_LPSPI_Write(x2 >> 8U);
    ILI9341_LPSPI_Write(x2 & 0xFF);

    BOARD_SPILCD_pullDcPin(false);
    ILI9341_LPSPI_Write(0x2B);
    BOARD_SPILCD_pullDcPin(true);
    ILI9341_LPSPI_Write(y1 >> 8U);
    ILI9341_LPSPI_Write(y1 & 0xFF);
    ILI9341_LPSPI_Write(y2 >> 8U);
    ILI9341_LPSPI_Write(y2 & 0xFF);
}

void ILI9341_Init(void)
{
    ILI9341_LPSPI_Init();
    BOARD_SPILCD_RstPinInit();
    BOARD_SPILCD_DcPinInit();
    BOARD_SPILCD_CsPinInit();
    BOARD_SPILCD_LedPinInit();
    
    BOARD_SPILCD_pullLedPin(true);

    configLCD();
}

void ILI9341_FillPic(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t * Pic)
{
    uint32_t points = (uint32_t)(x2 - x1 + 1) * (uint32_t)(y2 - y1 + 1);

    ILI9341_SetWindow(x1, y1, x2, y2);

    BOARD_SPILCD_pullDcPin(false);
    ILI9341_LPSPI_Write(0x2C);

    BOARD_SPILCD_pullDcPin(true);
    ILI9341_LPSPI_ConfigFrameSize(16U);
    for(uint32_t i=0; i<points; i++)
    {
        ILI9341_LPSPI_Write(Pic[i]);
    }
}

void ILI9341_FillColor(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t Color)
{
    uint32_t points = (uint32_t)(x2 - x1 + 1) * (uint32_t)(y2 - y1 + 1);

    ILI9341_SetWindow(x1, y1, x2, y2);

    BOARD_SPILCD_pullDcPin(false);
    ILI9341_LPSPI_Write(0x2C);

    BOARD_SPILCD_pullDcPin(true);
    ILI9341_LPSPI_ConfigFrameSize(16U);
    for(uint32_t i=0; i<points; i++)
    {
        ILI9341_LPSPI_Write(Color);
    }
}

/*******************************************************************************
 * Transfer with DMA 
 ******************************************************************************/
#define DMA_TRSF_SIZE               2u              /* 2 bytes */
#define DMA_MINOR_LOOP_SIZE         DMA_TRSF_SIZE
#define DMA_MAX_MAJOR_LOOP_SIZE     32000u
static uint32_t saddr;
static uint32_t totalMajorLoops;

void ILI9341_setDMATrsf(void)
{
    uint32_t size=0u;
    while(1u << size < DMA_TRSF_SIZE) /* size = log2(DMA_TRSF_SIZE) */
    {
        size++;
    }

    /* Configure DMA TCD */
    DMA0->TCD[SPI_LCD_DMA_CHN].SOFF = DMA_TRSF_SIZE;
    DMA0->TCD[SPI_LCD_DMA_CHN].ATTR = DMA_ATTR_SMOD(0u) |
                                      DMA_ATTR_SSIZE(size) |
                                      DMA_ATTR_DMOD(0u) |
                                      DMA_ATTR_DSIZE(size);
    DMA0->TCD[SPI_LCD_DMA_CHN].NBYTES_MLNO = DMA_MINOR_LOOP_SIZE;
    DMA0->TCD[SPI_LCD_DMA_CHN].SLAST = 0;
    DMA0->TCD[SPI_LCD_DMA_CHN].DADDR = (uint32_t)(&(BOARD_SPILCD_SPI_INST->TDR));
    DMA0->TCD[SPI_LCD_DMA_CHN].DOFF = 0u;
    DMA0->TCD[SPI_LCD_DMA_CHN].DLAST_SGA = 0u;
    DMA0->TCD[SPI_LCD_DMA_CHN].CSR = 0u;
    DMA0->TCD[SPI_LCD_DMA_CHN].CSR = DMA_CSR_DREQ_MASK | DMA_CSR_INTMAJOR_MASK;
    
    /* Configure DMA MUX Source */
    DMAMUX->CHCFG[SPI_LCD_DMA_CHN] = DMAMUX->CHCFG[SPI_LCD_DMA_CHN] &
                                      (~DMAMUX_CHCFG_SOURCE_MASK) | 
                                      DMAMUX_CHCFG_SOURCE(SPI_LCD_DMA_MUX_SRC);
    /* Enable DMA channel. */
    DMAMUX->CHCFG[SPI_LCD_DMA_CHN] |= DMAMUX_CHCFG_ENBL_MASK;
    
    /* Enable interrupt. */
    EnableIRQ(SPI_LCD_DMA_IRQn);
}

static void startDMATrsf(uint32_t *pSaddr, uint32_t *pTotalMajorLoops)
{
    uint32_t currentMajorLoops = *pTotalMajorLoops <= DMA_MAX_MAJOR_LOOP_SIZE ? *pTotalMajorLoops : DMA_MAX_MAJOR_LOOP_SIZE;

    DMA0->TCD[SPI_LCD_DMA_CHN].SADDR = *pSaddr;
    DMA0->TCD[SPI_LCD_DMA_CHN].CITER_ELINKNO = currentMajorLoops;
    DMA0->TCD[SPI_LCD_DMA_CHN].BITER_ELINKNO = currentMajorLoops;

    *pSaddr += currentMajorLoops * DMA_MINOR_LOOP_SIZE;
    *pTotalMajorLoops -= currentMajorLoops;

    /* Enable DMA channel request. */
    DMA0->SERQ = DMA_SERQ_SERQ(SPI_LCD_DMA_CHN);
}

void ILI9341_startDMATrsf(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t * Pic)
{
    ILI9341_SetWindow(x1, y1, x2, y2);

    ILI9341_LPSPI_ConfigFrameSize(8U);
    BOARD_SPILCD_pullDcPin(false);
    ILI9341_LPSPI_Write(0x2C);
    
    BOARD_SPILCD_pullDcPin(true);
    ILI9341_LPSPI_ConfigFrameSize(16U);

    totalMajorLoops = (uint32_t)(x2 - x1 + 1) * (uint32_t)(y2 - y1 + 1) * sizeof(Pic[0]) / DMA_MINOR_LOOP_SIZE;
    saddr = (uint32_t)Pic;

    /* Pull CS line low. */
    BOARD_SPILCD_pullCsPin(false);

    startDMATrsf(&saddr, &totalMajorLoops);
}

void SPI_LCD_DMA_IRQHandler(void)
{
    DMA0->CDNE = DMA_CDNE_CDNE(SPI_LCD_DMA_CHN);
    DMA0->CINT = DMA_CINT_CINT(SPI_LCD_DMA_CHN);
    
    if(totalMajorLoops)
    {
        startDMATrsf(&saddr, &totalMajorLoops);
    }
    else
    {
        /* Pull CS line high. */
        BOARD_SPILCD_pullCsPin(true);
    }

    __DSB();
}

/* EOF */
