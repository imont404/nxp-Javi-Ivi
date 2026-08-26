#include "nxpc__io.h"

#if CONFIG__DISPLAY_TEST_MODE && CONFIG__DISPLAY_PARALLEL_BITBANG_TEST_MODE
#include "st7789_parallel_bitbang.h"
#endif

#if CONFIG__USB_DEBUG_STREAM_ENABLE
#include "nxpc_usb_debug_stream.h"
#endif

#include "nxpc__motor_encoder_qdc.h"

#ifndef CONFIG__NXPC_UART_TX_Q_SIZE_BYTES
#define CONFIG__NXPC_UART_TX_Q_SIZE_BYTES 2048
#endif

BYTE_QUEUE__MAKE(UART4_TX_Q, CONFIG__NXPC_UART_TX_Q_SIZE_BYTES);

static void nxpc_io__uart_init(void);

#if CONFIG__DISPLAY_TEST_MODE && !CONFIG__DISPLAY_PARALLEL_BITBANG_TEST_MODE
static uint16_t display_test_rows[NXPC_DISPLAY_WIDTH * 16U];

static uint16_t nxpc__display_test_color(uint32_t x)
{
    uint32_t band = (x * 3U) / NXPC_DISPLAY_WIDTH;

    switch (band)
    {
    case 0:
        return 0xF800; /* red */
    case 1:
        return 0x07E0; /* green */
    default:
        return 0x001F; /* blue */
    }
}

static void nxpc__display_test_draw(void)
{
    const uint32_t rows_per_chunk = 16U;

    for (uint32_t y0 = 0U; y0 < NXPC_DISPLAY_HEIGHT; y0 += rows_per_chunk)
    {
        uint32_t rows = NXPC_DISPLAY_HEIGHT - y0;
        if (rows > rows_per_chunk)
        {
            rows = rows_per_chunk;
        }

        for (uint32_t y = 0U; y < rows; y++)
        {
            for (uint32_t x = 0U; x < NXPC_DISPLAY_WIDTH; x++)
            {
                display_test_rows[(y * NXPC_DISPLAY_WIDTH) + x] = nxpc__display_test_color(x);
            }
        }

        nxpc_display__write((uint8_t *)display_test_rows,
                            rows * NXPC_DISPLAY_WIDTH * sizeof(display_test_rows[0]), 0U,
                            NXPC_DISPLAY_WIDTH - 1U, y0, y0 + rows - 1U);
    }
}
#endif

#if CONFIG__DISPLAY_TEST_MODE
static void nxpc__display_test_mode_run(void)
{
#if CONFIG__DISPLAY_PARALLEL_BITBANG_TEST_MODE
    (void)DEBUG(
        "Display parallel bitbang test mode active: panel=%u wr_delay=%u frame_delay_ms=%u\r\n",
        CONFIG__DISPLAY_PANEL, CONFIG__DISPLAY_PARALLEL_BITBANG_WR_DELAY_CYCLES,
        CONFIG__DISPLAY_PARALLEL_BITBANG_FRAME_DELAY_MS);

    st7789_parallel_bitbang__run_test();
#else
    uint32_t phase = 0U;

    (void)DEBUG("Display test mode active: panel=%u test=%u te=%u\r\n", CONFIG__DISPLAY_PANEL,
                CONFIG__DISPLAY_TEST_MODE, CONFIG__DISPLAY_TE_ENABLE);

    nxpc_display__init();

    while (1)
    {
        nxpc__display_test_draw();
        phase++;
        (void)DEBUG("display_test frame=%u\r\n", phase);
        e_tick__delay_ms(1000);
    }
#endif
}
#endif

void LP_FLEXCOMM4_IRQHandler(void)
{
    if ((kLPUART_TxDataRegEmptyFlag)&LPUART_GetStatusFlags(LPUART4))
    {
        if (bq__bytes_available_to_write(&UART4_TX_Q))
        {
            LPUART4->DATA = bq__dequeue_next(&UART4_TX_Q);
        }
        else
        {
            LPUART_DisableInterrupts(LPUART4, kLPUART_TxDataRegEmptyInterruptEnable);
        }
    }

    SDK_ISR_EXIT_BARRIER;
}

static void nxpc_io__uart_enqueue_hook(void *arg)
{
    LPUART_EnableInterrupts((LPUART_Type *)arg, kLPUART_TxDataRegEmptyInterruptEnable);
}

static void nxpc_io__uart_init(void)
{
    CLOCK_SetClkDiv(kCLOCK_DivFlexcom4Clk, 1u);
    CLOCK_AttachClk(kFRO12M_to_FLEXCOMM4);

    UART4_TX_Q.enqueue_hook = nxpc_io__uart_enqueue_hook;
    UART4_TX_Q.hook_arg = (void *)LPUART4;

    lpuart_config_t config;

    /*
     *
     * UART 4
     *
     */

    RESET_ClearPeripheralReset(kFC4_RST_SHIFT_RSTn);
    LPUART_GetDefaultConfig(&config);
    config.baudRate_Bps = 115200;
    config.enableTx = true;
    config.enableRx = false;

    LPUART_Init(LPUART4, &config, 12000000);

    EnableIRQ(LP_FLEXCOMM4_IRQn);
}

/*
 * Enable the instruction and flash data caches.
 *
 * These were previously set inside nxpc_camera__init_smartdma_ezh(), so any
 * build not using the EZH capture backend ran with the flash data cache
 * disabled. Nothing about them is camera-specific, and flash-resident lookup
 * tables in particular depend on the data cache, so they belong in common
 * startup.
 *
 * SystemInit() already enables LPCAC; repeating it is harmless and keeps the
 * two related settings together and greppable.
 */
static void nxpc__flash_cache_init(void)
{
    SYSCON->LPCAC_CTRL &= ~SYSCON_LPCAC_CTRL_DIS_LPCAC_MASK;

    SYSCON->NVM_CTRL &=
        ~(SYSCON_NVM_CTRL_DIS_FLASH_CACHE_MASK | SYSCON_NVM_CTRL_DIS_FLASH_DATA_MASK);
}

void nxpc__init()
{
    BOARD_InitBootPins();
    BOARD_InitBootClocks();

    nxpc__flash_cache_init();

    CLOCK_EnableClock(kCLOCK_Gpio0);
    CLOCK_EnableClock(kCLOCK_Dma0);

    INIT_CYCLE_COUNTER;

    nxpc_io__uart_init();

    button__configure(BUTTON_ID_LEFT, IN_PORT, LEFT_BTN_PIN, BUTTON_POLARITY_LOW_ACTIVE, 50U);
    button__configure(BUTTON_ID_RIGHT, IN_PORT, RIGHT_BTN_PIN, BUTTON_POLARITY_LOW_ACTIVE, 50U);
    button__configure(BUTTON_ID_CENTER, IN_PORT, CENTER_BTN_PIN, BUTTON_POLARITY_LOW_ACTIVE, 50U);

    e__init();

    MAILBOX_Init(MAILBOX);

    /* Print the initial banner from Primary core */
    (void)DEBUG("\r\nHello World from core 0!\r\n");

    /* Boot Secondary core application */
    (void)DEBUG("Starting Secondary core.\r\n");

#if CONFIG__USB_DEBUG_STREAM_ENABLE
    nxpc_usb_debug_stream__init();
#endif

#if defined(FSL_FEATURE_MAILBOX_SIDE_A)
    NVIC_SetPriority(MAILBOX_IRQn, 5);
#else
    NVIC_SetPriority(MAILBOX_IRQn, 2);
#endif

    NVIC_EnableIRQ(MAILBOX_IRQn);

#if CONFIG__DISPLAY_TEST_MODE
    nxpc__display_test_mode_run();
#endif

    nxpc_camera__init();
#if CONFIG__DISPLAY_ENABLE
    nxpc_display__init();
#endif

    nxpc__adc_init();
    nxpc__motor_control_init();
    nxpc__servo_control_init();

    /* QDC wheel feedback is standard and always available to the framework. */
    nxpc__motor_encoder_qdc_init();
}
