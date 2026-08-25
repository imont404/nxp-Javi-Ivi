#include "nxpc__master_config.h"
#include "eGFX.h"
#include "eGFX_Driver_ER-TFT020-3.h"
#include "st7789.h"
#include "fsl_gpio.h"
#include "lpspi1.h"
#include "st7789.h"

#ifdef eGFX_DRIVER_ER_TFT020_3

/*
 * Frame-dump timing instrumentation; its configuration is documented in
 * nxpc__master_config.h.
 * Measurement only; compiles to nothing when the knob is off.
 */
#if CONFIG__DISPLAY_TIMING_DIAG_ENABLE

#include "nxpc__io.h"
#include "e_debug.h"

/* Cycles, accumulated across CONFIG__DISPLAY_TIMING_DIAG_REPORT_CALLS calls. */
static uint32_t diag_init8, diag_setpos, diag_init32, diag_blocks;
static uint32_t diag_calls, diag_blocks_seen;

/*
 * TCR frame size read back after each mode switch. Proves the 8/32-bit change
 * took effect, which otherwise could only be confirmed by looking at the panel.
 */
static uint8_t diag_fs8, diag_fs32;
static bool diag_fs_reported;

#define eGFX_TIMING_FS(var)     do { (var) = lpspi1_get_frame_size(); } while (0)

#define eGFX_TIMING_DECL()      uint32_t diag_t0, diag_t1
#define eGFX_TIMING_MARK()      do { diag_t0 = CYCLE_COUNTER; } while (0)
#define eGFX_TIMING_ACC(field)  do {                                          \
        diag_t1 = CYCLE_COUNTER;                                              \
        (field) += (diag_t1 - diag_t0);                                       \
        diag_t0 = diag_t1;                                                    \
    } while (0)

/* Core is 150 MHz, so cycles/150 is microseconds. */
#define eGFX_DIAG_US(cycles) ((cycles) / 150u)

#define eGFX_TIMING_REPORT(blocks)                                            \
    do {                                                                      \
        diag_blocks_seen += (blocks);                                         \
        if (++diag_calls >= CONFIG__DISPLAY_TIMING_DIAG_REPORT_CALLS) {       \
            uint32_t n = diag_calls;                                          \
            uint32_t total = diag_init8 + diag_setpos                         \
                           + diag_init32 + diag_blocks;                       \
            DEBUG(                                                            \
                "lcd_dump n=%lu avg_us total=%lu init8=%lu setpos=%lu "       \
                "init32=%lu blocks=%lu blks=%lu overhead_us=%lu\r\n",         \
                (unsigned long)n,                                             \
                (unsigned long)eGFX_DIAG_US(total / n),                       \
                (unsigned long)eGFX_DIAG_US(diag_init8 / n),                  \
                (unsigned long)eGFX_DIAG_US(diag_setpos / n),                 \
                (unsigned long)eGFX_DIAG_US(diag_init32 / n),                 \
                (unsigned long)eGFX_DIAG_US(diag_blocks / n),                 \
                (unsigned long)(diag_blocks_seen / n),                        \
                (unsigned long)eGFX_DIAG_US(                                  \
                    (diag_init8 + diag_setpos + diag_init32) / n));           \
            diag_init8 = diag_setpos = diag_init32 = diag_blocks = 0;         \
            diag_calls = diag_blocks_seen = 0;                                \
            if (!diag_fs_reported) {                                          \
                diag_fs_reported = true;                                      \
                DEBUG("lcd_dump framesz control=%u pixel=%u bits\r\n",        \
                      (unsigned)diag_fs8, (unsigned)diag_fs32);               \
                lpspi1_report_clock();                                        \
            }                                                                 \
        }                                                                     \
    } while (0)

#else

#define eGFX_TIMING_DECL()      do { } while (0)
#define eGFX_TIMING_MARK()      do { } while (0)
#define eGFX_TIMING_ACC(field)  do { } while (0)
#define eGFX_TIMING_FS(var)     do { } while (0)
#define eGFX_TIMING_REPORT(b)   do { (void)(b); } while (0)

#endif

eGFX_VSyncCallback_t *VSyncCallback;

extern void ST7789_Initial(void);


void eGFX_InitDriver(eGFX_VSyncCallback_t VS)
{
    /*
     * PLLCLKDIV feeds this LPSPI path. Divider 2 at 37.5 MHz is the only
     * competition-verified configuration; changing the divider at runtime
     * produced a black panel even when nominal SCK was restored. See
     * CONFIG__DISPLAY_SPI_PLLCLKDIV and its adjacent bench notes.
     */
    CLOCK_SetClkDiv(kCLOCK_DivPllClk, CONFIG__DISPLAY_SPI_PLLCLKDIV);
    CLOCK_SetClkDiv(kCLOCK_DivFlexcom1Clk, 1u);
    CLOCK_AttachClk(kPLL_DIV_to_FLEXCOMM1);

	lpspi1_init(8); // Initialize with 8-bit SPI transactions

    VSyncCallback = VS;

    ST7789_Initial();
 
#if (defined(CONFIG_DISPLAY_ORIENTATION) && (CONFIG_DISPLAY_ORIENTATION == PORTRAIT))
    LCD_SetPos(0, 239, 0, 319); // 320x240
#elif (defined(CONFIG_DISPLAY_ORIENTATION) && (CONFIG_DISPLAY_ORIENTATION == LANDSCAPE))
    LCD_SetPos(0, 319, 0, 239); // 320x240
#endif

}


void eGFX_Dump(eGFX_ImagePlane *Image)
{
    uint8_t *buffer = (uint8_t *)Image->Data;
    uint32_t packets;

    lpspi1_init(8);
  //  GPIO_PinWrite(GPIO4, 1, 1);

#if (defined(CONFIG_DISPLAY_ORIENTATION) && (CONFIG_DISPLAY_ORIENTATION == LANDSCAPE))
    LCD_SetPos(0, 319, 0, 239); // 320x240
#endif
    LCD_RS__SET;
    lpspi1_init(32);
    ST7789__display_img(buffer);
   // GPIO_PinWrite(GPIO4, 1, 0);
}

void eGFX_DumpRaw(uint8_t *buffer,
				  uint32_t length,
				  uint32_t x0,
				  uint32_t x1,
				  uint32_t y0,
				  uint32_t y1)
{
    eGFX_TIMING_DECL();

    eGFX_TIMING_MARK();
    lpspi1_init(8);
    eGFX_TIMING_ACC(diag_init8);
    eGFX_TIMING_FS(diag_fs8);

    LCD_SetPos(x0, x1, y0, y1); // 320x240
    eGFX_TIMING_ACC(diag_setpos);

    lpspi1_init(32);
    eGFX_TIMING_ACC(diag_init32);
    eGFX_TIMING_FS(diag_fs32);

	LCD_RS__SET;
    uint32_t blk_size;
    uint32_t blocks = 0;
    while(length)
    {
    	if(length >= 8192)
    	{
    		blk_size = 8192;
    	}
    	else
    	{
    		blk_size = length;
    	}


        lpspi1_transfer_block(buffer, blk_size);

        buffer += blk_size;
        length-=blk_size;
        blocks++;

    }

    /*
     * The block loop returns as soon as the last transfer is submitted, not
     * when it completes - lpspi1_transfer_block waits for the *previous* one.
     * Wait here so the measurement covers the pixels actually reaching the
     * panel rather than stopping at the final DMA submit.
     */
    lpspi1_wait_idle();
    eGFX_TIMING_ACC(diag_blocks);

    eGFX_TIMING_REPORT(blocks);
}




void eGFX_line_double(uint8_t *buffer,uint16_t lines)
{


    lpspi1_init(8);
   // GPIO_PinWrite(GPIO4, 1, 1);

#if (defined(CONFIG_DISPLAY_ORIENTATION) && (CONFIG_DISPLAY_ORIENTATION == LANDSCAPE))
    LCD_SetPos(0, 319, 0, 239); // 320x240
#endif

    lpspi1_init(32);

    for(uint32_t i = 0; i < lines; i++)
    {

        ST7789__display_row(&buffer[i * 640], eGFX_PHYSICAL_SCREEN_SIZE_X);
        ST7789__display_row(&buffer[i * 640], eGFX_PHYSICAL_SCREEN_SIZE_X);
     }
   // GPIO_PinWrite(GPIO4, 1, 0);

}


void eGFX_duplicate_and_dump2(eGFX_ImagePlane *Image)
{
    uint16_t *original_row_buffer = (uint16_t *)Image->Data;
    uint16_t scaled_row_buff[eGFX_PHYSICAL_SCREEN_SIZE_X];
    uint32_t delta;

    lpspi1_init(8);
   // GPIO_PinWrite(GPIO4, 1, 1);

#if (defined(CONFIG_DISPLAY_ORIENTATION) && (CONFIG_DISPLAY_ORIENTATION == LANDSCAPE))
    LCD_SetPos(0, 319, 0, 239); // 320x240
#endif

    lpspi1_init(32);

    for(uint32_t j = 0; j < 120; j++) 
    {
        delta = 160*j;
        for(uint32_t i = 0; i < 160; i++)
        {   
            scaled_row_buff[2*i] = original_row_buffer[i + delta];
            scaled_row_buff[2*i + 1] = original_row_buffer[i + delta];
        }
        ST7789__display_row((uint8_t * )scaled_row_buff, eGFX_PHYSICAL_SCREEN_SIZE_X);
        ST7789__display_row((uint8_t * )scaled_row_buff, eGFX_PHYSICAL_SCREEN_SIZE_X);
    }
   // GPIO_PinWrite(GPIO4, 1, 0);

}



uint16_t scaled_row_buff_storage1[eGFX_PHYSICAL_SCREEN_SIZE_X*2];
uint16_t scaled_row_buff_storage2[eGFX_PHYSICAL_SCREEN_SIZE_X*2];

uint8_t buff_index = 0;

uint16_t *scaled_row_buff;

uint16_t y8;
uint16_t y8_max;


void eGFX_duplicate_and_dump(eGFX_ImagePlane *Image)
{
    uint16_t *original_row_buffer = (uint16_t *)Image->Data;

    uint32_t delta;


    lpspi1_init(8);

    uint16_t temp;

#if (defined(CONFIG_DISPLAY_ORIENTATION) && (CONFIG_DISPLAY_ORIENTATION == LANDSCAPE))
    LCD_SetPos(0, 319, 0, 239); // 320x240
#endif

    lpspi1_init(32);

    delta = 0;
    buff_index = 0;
    scaled_row_buff = &scaled_row_buff_storage1[0];


    for(uint32_t j = 0; j < 120; j++)
    {

        for(uint32_t i = 0; i < 160; i++)
        {

         	temp = original_row_buffer[i + delta];

            scaled_row_buff[2*i] = temp;
            scaled_row_buff[2*i + 1] = temp;

            scaled_row_buff[2*i + 320] = temp;
            scaled_row_buff[2*i + 1 + 320] = temp;

        }

        delta +=160;

        ST7789__display_row((uint8_t * )scaled_row_buff, eGFX_PHYSICAL_SCREEN_SIZE_X*2);

        if(scaled_row_buff == &scaled_row_buff_storage2[0])
        {
        	scaled_row_buff = &scaled_row_buff_storage1[0];
        }
        else
        {
        	scaled_row_buff = &scaled_row_buff_storage2[0];
        }

   }

}


void eGFX_SetBacklight(uint8_t BacklightValue)
{
}

#endif
