#include "nxpc_display.h"

#include "fsl_clock.h"
#include "fsl_gpio.h"
#include "lpspi1.h"
#include "nxpc__master_config.h"
#include "st7789.h"

/* Optional bench instrumentation; see nxpc__master_config.h. */
#if CONFIG__DISPLAY_TIMING_DIAG_ENABLE

#include "e_debug.h"
#include "nxpc__io.h"

static uint32_t diag_init8;
static uint32_t diag_setpos;
static uint32_t diag_init32;
static uint32_t diag_blocks;
static uint32_t diag_calls;
static uint32_t diag_blocks_seen;
static uint8_t diag_fs8;
static uint8_t diag_fs32;
static bool diag_fs_reported;

#define NXPC_DISPLAY_TIMING_DECL() uint32_t diag_t0, diag_t1
#define NXPC_DISPLAY_TIMING_MARK() diag_t0 = CYCLE_COUNTER
#define NXPC_DISPLAY_TIMING_ACC(field)                                                             \
    do                                                                                             \
    {                                                                                              \
        diag_t1 = CYCLE_COUNTER;                                                                   \
        (field) += diag_t1 - diag_t0;                                                              \
        diag_t0 = diag_t1;                                                                         \
    } while (0)
#define NXPC_DISPLAY_TIMING_FS(var) (var) = lpspi1_get_frame_size()

static void nxpc_display__timing_report(uint32_t blocks)
{
    diag_blocks_seen += blocks;
    if (++diag_calls < CONFIG__DISPLAY_TIMING_DIAG_REPORT_CALLS)
    {
        return;
    }

    uint32_t calls = diag_calls;
    uint32_t total = diag_init8 + diag_setpos + diag_init32 + diag_blocks;

    /* The core runs at 150 MHz, so cycles / 150 is microseconds. */
    DEBUG("lcd_dump n=%lu avg_us total=%lu init8=%lu setpos=%lu "
          "init32=%lu blocks=%lu blks=%lu overhead_us=%lu\r\n",
          (unsigned long)calls, (unsigned long)((total / calls) / 150U),
          (unsigned long)((diag_init8 / calls) / 150U),
          (unsigned long)((diag_setpos / calls) / 150U),
          (unsigned long)((diag_init32 / calls) / 150U),
          (unsigned long)((diag_blocks / calls) / 150U), (unsigned long)(diag_blocks_seen / calls),
          (unsigned long)(((diag_init8 + diag_setpos + diag_init32) / calls) / 150U));

    diag_init8 = 0U;
    diag_setpos = 0U;
    diag_init32 = 0U;
    diag_blocks = 0U;
    diag_calls = 0U;
    diag_blocks_seen = 0U;

    if (!diag_fs_reported)
    {
        diag_fs_reported = true;
        DEBUG("lcd_dump framesz control=%u pixel=%u bits\r\n", (unsigned)diag_fs8,
              (unsigned)diag_fs32);
        lpspi1_report_clock();
    }
}

#define NXPC_DISPLAY_TIMING_REPORT(blocks) nxpc_display__timing_report(blocks)

#else

#define NXPC_DISPLAY_TIMING_DECL()                                                                 \
    do                                                                                             \
    {                                                                                              \
    } while (0)
#define NXPC_DISPLAY_TIMING_MARK()                                                                 \
    do                                                                                             \
    {                                                                                              \
    } while (0)
#define NXPC_DISPLAY_TIMING_ACC(field)                                                             \
    do                                                                                             \
    {                                                                                              \
    } while (0)
#define NXPC_DISPLAY_TIMING_FS(var)                                                                \
    do                                                                                             \
    {                                                                                              \
    } while (0)
#define NXPC_DISPLAY_TIMING_REPORT(blocks)                                                         \
    do                                                                                             \
    {                                                                                              \
        (void)(blocks);                                                                            \
    } while (0)

#endif

void nxpc_display__init(void)
{
    /* This 37.5 MHz path is the competition-verified panel configuration. */
    CLOCK_SetClkDiv(kCLOCK_DivPllClk, CONFIG__DISPLAY_SPI_PLLCLKDIV);
    CLOCK_SetClkDiv(kCLOCK_DivFlexcom1Clk, 1U);
    CLOCK_AttachClk(kPLL_DIV_to_FLEXCOMM1);

    lpspi1_init(8U);
    st7789__init();
#if CONFIG_DISPLAY_ORIENTATION == PORTRAIT
    st7789__set_window(0U, NXPC_DISPLAY_HEIGHT - 1U, 0U, NXPC_DISPLAY_WIDTH - 1U);
#elif CONFIG_DISPLAY_ORIENTATION == LANDSCAPE
    st7789__set_window(0U, NXPC_DISPLAY_WIDTH - 1U, 0U, NXPC_DISPLAY_HEIGHT - 1U);
#else
#error Invalid CONFIG_DISPLAY_ORIENTATION.
#endif
}

void nxpc_display__write(const uint8_t *buffer, uint32_t length, uint32_t x0, uint32_t x1,
                         uint32_t y0, uint32_t y1)
{
    NXPC_DISPLAY_TIMING_DECL();

    NXPC_DISPLAY_TIMING_MARK();
    lpspi1_init(8U);
    NXPC_DISPLAY_TIMING_ACC(diag_init8);
    NXPC_DISPLAY_TIMING_FS(diag_fs8);

    st7789__set_window(x0, x1, y0, y1);
    NXPC_DISPLAY_TIMING_ACC(diag_setpos);

    lpspi1_init(32U);
    NXPC_DISPLAY_TIMING_ACC(diag_init32);
    NXPC_DISPLAY_TIMING_FS(diag_fs32);

    st7789__begin_pixels();
    uint32_t blocks = 0U;
    while (length != 0U)
    {
        uint32_t block_size = length < 8192U ? length : 8192U;
        lpspi1_transfer_block(buffer, block_size);
        buffer += block_size;
        length -= block_size;
        blocks++;
    }

    /* Include completion of the final DMA submission in timing and lifetime. */
    lpspi1_wait_idle();
    NXPC_DISPLAY_TIMING_ACC(diag_blocks);
    NXPC_DISPLAY_TIMING_REPORT(blocks);
}
