#include "lpspi1.h"
#include "fsl_debug_console.h"
#include "fsl_gpio.h"
#include "e_debug.h"


#define MASTER_DMA_RX_CHANNEL   0U
#define MASTER_DMA_TX_CHANNEL   1U
#define DEMO_LPSPI_TRANSMIT_EDMA_CHANNEL      kDma0RequestMuxLpFlexcomm1Tx
#define DEMO_LPSPI_RECEIVE_EDMA_CHANNEL       kDma0RequestMuxLpFlexcomm1Rx

#define LPSPI_MASTER_CLK_FREQ CLOCK_GetLPFlexCommClkFreq(1u)
#include "nxpc__master_config.h"

/* Requested SCK; see CONFIG__DISPLAY_SPI_BAUD_HZ. */
#define CONFIG__TRANSFER_BAUDRATE CONFIG__DISPLAY_SPI_BAUD_HZ
#define CONFIG__BITS_PER_FRAME  8


AT_NONCACHEABLE_SECTION_INIT(lpspi_master_edma_handle_t g_m_edma_handle) = {0};
edma_handle_t lpspiEdmaMasterRxRegToRxDataHandle;
edma_handle_t lpspiEdmaMasterTxDataToTxRegHandle;
edma_config_t dma_config = {0};
volatile bool isTransferCompleted  = true;
volatile bool mem_transfer_done = false;

edma_handle_t g_DMA_Handle;

volatile bool request_frame_for_display = true;


#define LPSPI1_TRANSFER_CONFIG_FLAGS \
    (kLPSPI_Pcs0 | kLPSPI_MasterByteSwap | kLPSPI_MasterPcsContinuous)

static bool lpspi1_hardware_ready = false;

/*
 * Change the SPI frame size without re-initialising anything.
 *
 * The display path alternates between 8-bit control bytes and 32-bit pixel
 * words. Frame size lives in TCR[FRAMESZ], and both LPSPI_MasterTransferEDMALite
 * and LPSPI_MasterTransferPrepareEDMALite read it back out of TCR at call time,
 * so switching is a register write plus a re-prepare to refresh the handle's
 * bytesEachWrite/bytesEachRead fields. PrepareEDMALite preserves FRAMESZ - it
 * only rewrites CONT, CONTC, BYSW and PCS.
 *
 * The module is disabled around the TCR write. With LPSPI enabled a TCR write
 * enqueues a command rather than taking effect directly, which is the wrong
 * behaviour for a mode change between transfers.
 */
void lpspi1_set_frame_size(uint8_t transaction_bits)
{
    lpspi1_wait_idle();

    LPSPI_Enable(LPSPI1, false);
    LPSPI1->TCR = (LPSPI1->TCR & ~(uint32_t)LPSPI_TCR_FRAMESZ_MASK) |
                  LPSPI_TCR_FRAMESZ((uint32_t)transaction_bits - 1U);
    LPSPI_Enable(LPSPI1, true);

    (void)LPSPI_MasterTransferPrepareEDMALite(LPSPI1, &g_m_edma_handle,
                                              LPSPI1_TRANSFER_CONFIG_FLAGS);
}

/*
 * Recover the SCK the hardware actually settled on.
 *
 * LPSPI_MasterSetBaudRate searches for the closest achievable divider and does
 * not report when it cannot reach the request, so the configured
 * CONFIG__TRANSFER_BAUDRATE is a wish rather than a fact. A register-derived
 * diagnostic estimate is:
 *
 *     SCK = srcClock / (2^PRESCALE * (SCKDIV + 2))
 *
 * Do not treat this estimate as authoritative: the final divider experiment
 * contradicted earlier register-derived clock conclusions. Use the P4_1 scope
 * marker or measured transfer duration for timing claims. See
 * docs/research/AVC_LCD_SPI_Design.md.
 */
uint32_t lpspi1_get_actual_sck_hz(uint32_t *src_hz_out,
                                  uint8_t *sckdiv_out,
                                  uint8_t *prescale_out)
{
    uint32_t src      = LPSPI_MASTER_CLK_FREQ;
    uint32_t sckdiv   = (LPSPI1->CCR & LPSPI_CCR_SCKDIV_MASK) >> LPSPI_CCR_SCKDIV_SHIFT;
    uint32_t prescale = (LPSPI1->TCR & LPSPI_TCR_PRESCALE_MASK) >> LPSPI_TCR_PRESCALE_SHIFT;

    if (src_hz_out != NULL)   { *src_hz_out = src; }
    if (sckdiv_out != NULL)   { *sckdiv_out = (uint8_t)sckdiv; }
    if (prescale_out != NULL) { *prescale_out = (uint8_t)prescale; }

    return src / ((1U << prescale) * (sckdiv + 2U));
}

/*
 * Print the register-derived diagnostic estimate and the request it came from.
 * Lives here because CONFIG__TRANSFER_BAUDRATE is local to this file; this is
 * not a substitute for a measured transfer duration or scope observation.
 */
void lpspi1_report_clock(void)
{
    uint32_t src;
    uint8_t sckdiv, prescale;
    uint32_t sck = lpspi1_get_actual_sck_hz(&src, &sckdiv, &prescale);

    DEBUG("lpspi1 clock src=%luHz sckdiv=%u prescale=%u actual=%luHz requested=%luHz\r\n",
          (unsigned long)src, (unsigned)sckdiv, (unsigned)prescale,
          (unsigned long)sck, (unsigned long)CONFIG__TRANSFER_BAUDRATE);
    DEBUG("lpspi1 raw CCR=%08lX TCR=%08lX CFGR1=%08lX\r\n",
          (unsigned long)LPSPI1->CCR, (unsigned long)LPSPI1->TCR,
          (unsigned long)LPSPI1->CFGR1);
}

/* Frame size currently programmed in TCR, in bits. Used to prove the mode
 * switch took effect without needing to look at the panel. */
uint8_t lpspi1_get_frame_size(void)
{
    return (uint8_t)(((LPSPI1->TCR & LPSPI_TCR_FRAMESZ_MASK) >>
                      LPSPI_TCR_FRAMESZ_SHIFT) + 1U);
}

/*
 * Bring up LPSPI1 and its DMA channels. Safe to call repeatedly: the expensive
 * work happens once and later calls only change frame size.
 *
 * This used to re-run in full on every call, which meant EDMA_Init() reset the
 * whole DMA0 controller twice per display update - four times per frame, since
 * the frame is drawn in two regions. DMA0 is shared with the camera, so that
 * was a hazard as well as costing ~770 us a time.
 */
void lpspi1_init(uint8_t transaction_bits)
{
    uint32_t srcClock_Hz;
    lpspi_master_config_t masterConfig;

    if (lpspi1_hardware_ready)
    {
        lpspi1_set_frame_size(transaction_bits);
        return;
    }

    while (!isTransferCompleted);

    /*Master config*/
    LPSPI_MasterGetDefaultConfig(&masterConfig);
    masterConfig.bitsPerFrame = transaction_bits;
    masterConfig.baudRate = CONFIG__TRANSFER_BAUDRATE;
    masterConfig.whichPcs = kLPSPI_Pcs0;
    masterConfig.direction = kLPSPI_MsbFirst;
    masterConfig.pcsToSckDelayInNanoSec        = 0;
    masterConfig.lastSckToPcsDelayInNanoSec    = 0;
    masterConfig.betweenTransferDelayInNanoSec = 0;

    srcClock_Hz = LPSPI_MASTER_CLK_FREQ;
    LPSPI_MasterInit(LPSPI1, &masterConfig, srcClock_Hz);

#if CONFIG__DISPLAY_SPI_SCKDIV >= 0
    /*
     * Set the divider directly rather than trusting the baud-rate search.
     *
     * LPSPI_MasterSetBaudRate reads the source clock through
     * CLOCK_GetLPFlexCommClkFreq at init time and picks the closest divider it
     * can, silently. When PLLCLKDIV is changed in the same function that then
     * initialises LPSPI, the value it sees has been observed not to match the
     * clock the peripheral actually ends up running from - which produced a
     * 75 MHz SCK from a 50 MHz request.
     *
     * SCK = src / (2^PRESCALE * (SCKDIV + 2)), so from a 150 MHz source:
     *   SCKDIV 0 -> 75 MHz, SCKDIV 1 -> 50 MHz, SCKDIV 2 -> 37.5 MHz
     */
    LPSPI_Enable(LPSPI1, false);
    LPSPI1->CCR = (LPSPI1->CCR & ~(uint32_t)LPSPI_CCR_SCKDIV_MASK) |
                  LPSPI_CCR_SCKDIV(CONFIG__DISPLAY_SPI_SCKDIV);
    LPSPI_Enable(LPSPI1, true);
#endif


    EDMA_GetDefaultConfig(&dma_config);
    EDMA_Init(DMA0, &dma_config);

    /*Set up lpspi master*/
    memset(&(lpspiEdmaMasterRxRegToRxDataHandle), 0, sizeof(lpspiEdmaMasterRxRegToRxDataHandle));
    memset(&(lpspiEdmaMasterTxDataToTxRegHandle), 0, sizeof(lpspiEdmaMasterTxDataToTxRegHandle));

    EDMA_CreateHandle(&(lpspiEdmaMasterRxRegToRxDataHandle), DMA0,
                      MASTER_DMA_RX_CHANNEL);
    EDMA_CreateHandle(&(lpspiEdmaMasterTxDataToTxRegHandle), DMA0,
                      MASTER_DMA_TX_CHANNEL);
    EDMA_CreateHandle(&g_DMA_Handle, DMA0, 2);

    EDMA_SetChannelMux(DMA0, MASTER_DMA_TX_CHANNEL,
                       DEMO_LPSPI_TRANSMIT_EDMA_CHANNEL);
    EDMA_SetChannelMux(DMA0, MASTER_DMA_RX_CHANNEL,
                       DEMO_LPSPI_RECEIVE_EDMA_CHANNEL);

    EDMA_SetCallback(&g_DMA_Handle, DMA_Callback, NULL);

    LPSPI_MasterTransferCreateHandleEDMA(LPSPI1, &g_m_edma_handle, LPSPI_MasterUserCallback,
                                         NULL, &lpspiEdmaMasterRxRegToRxDataHandle,
                                         &lpspiEdmaMasterTxDataToTxRegHandle);
    
    LPSPI_MasterTransferPrepareEDMALite(LPSPI1, &g_m_edma_handle, LPSPI1_TRANSFER_CONFIG_FLAGS);

    lpspi1_hardware_ready = true;
}


void lpspi1_transfer_block(void *block, uint32_t block_size)
{
    lpspi_transfer_t masterXfer;

    while (!isTransferCompleted);

    masterXfer.txData   = block;
    masterXfer.rxData   = NULL;
    masterXfer.dataSize = block_size;
    masterXfer.configFlags = kLPSPI_MasterPcs0 | kLPSPI_MasterPcsContinuous | kLPSPI_MasterByteSwap;
    isTransferCompleted = false;

    //This is needed to avoid chip select pulses between each transfer...
    LPSPI1->TCR |=  LPSPI_TCR_CONT(1);
    //LPSPI_MasterTransferBlocking(LPSPI1, &masterXfer);
    LPSPI_MasterTransferEDMALite(LPSPI1, &g_m_edma_handle,&masterXfer);

}

void lpspi1_transfer_byte(uint8_t next_byte)
{
    lpspi1_transfer_block(&next_byte, 1);
}

/*
 * Block until the last submitted transfer has completed.
 *
 * lpspi1_transfer_block returns once a transfer is *submitted*; it waits for
 * the previous one on the way in. Callers that need the data to have actually
 * reached the panel - or that are timing the transfer - must wait explicitly.
 */
void lpspi1_wait_idle(void)
{
    while (!isTransferCompleted)
    {
    }
}


void LPSPI_MasterUserCallback(LPSPI_Type *base, lpspi_master_edma_handle_t *handle, status_t status, void *userData)
{
    isTransferCompleted = true;
}





void dma_copy_buffer(void * src_buff, void * dest_buff, uint8_t data_width, uint32_t transfer_size)
{
    GPIO_PinWrite(GPIO4, 0, 1);
    edma_transfer_config_t transferConfig;

    EDMA_PrepareTransfer(&transferConfig,   //GOOD
                            src_buff,       //GOOD
                            data_width,     //GOOD
                            dest_buff,      //GOOD
                            data_width,     //GOOD
                            data_width * transfer_size, 
                            data_width * transfer_size, 
                            kEDMA_MemoryToMemory);
    mem_transfer_done =false;

    EDMA_SubmitTransfer(&g_DMA_Handle, &transferConfig);
    EDMA_StartTransfer(&g_DMA_Handle);


}

/* User callback function for EDMA transfer. */
void DMA_Callback(edma_handle_t *handle, void *userData, bool transferDone, uint32_t tcds)
{
    if (transferDone)
    {
        mem_transfer_done = true;
        GPIO_PinWrite(GPIO4, 0, 0);
    }
}
