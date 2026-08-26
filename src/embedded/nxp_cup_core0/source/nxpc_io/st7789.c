#include "st7789.h"

#include "fsl_common.h"
#include "fsl_gpio.h"
#include "lpspi1.h"
#include "nxpc__master_config.h"

#define ST7789_RS_SET() GPIO_PinWrite(GPIO0, 26U, 1U)
#define ST7789_RS_CLEAR() GPIO_PinWrite(GPIO0, 26U, 0U)
#define ST7789_RESET_SET() GPIO_PinWrite(GPIO0, 28U, 1U)
#define ST7789_RESET_CLEAR() GPIO_PinWrite(GPIO0, 28U, 0U)

static void st7789__write_command(uint8_t command)
{
    ST7789_RS_CLEAR();
    lpspi1_transfer_byte(command);
}

static void st7789__write_parameter(uint8_t parameter)
{
    ST7789_RS_SET();
    lpspi1_transfer_byte(parameter);
}

static void st7789__delay_ms(uint32_t count)
{
    while (count != 0U)
    {
        SDK_DelayAtLeastUs(1000U, SystemCoreClock);
        count--;
    }
}

static void st7789__reset_panel(void)
{
    ST7789_RESET_SET();
    st7789__delay_ms(1U);
    ST7789_RESET_CLEAR();
    st7789__delay_ms(10U);
    ST7789_RESET_SET();
    st7789__delay_ms(120U);
}

static uint8_t st7789__madctl(void)
{
#if CONFIG_DISPLAY_ORIENTATION == PORTRAIT
    return 0x00U;
#elif CONFIG_DISPLAY_ORIENTATION == LANDSCAPE
    return 0x60U;
#else
#error Invalid CONFIG_DISPLAY_ORIENTATION.
#endif
}

static void st7789__write_common_format(void)
{
    st7789__write_command(0x36U);
    st7789__write_parameter(st7789__madctl());

#if CONFIG_DISPLAY_ENDIANESS == LITTLE_ENDIAN
    st7789__write_command(0xB0U);
    st7789__write_parameter(0x00U);
    st7789__write_parameter(0x08U);
#elif CONFIG_DISPLAY_ENDIANESS == BIG_ENDIAN
    /* Use the controller's default byte order. */
#else
#error Invalid CONFIG_DISPLAY_ENDIANESS.
#endif

    st7789__write_command(0x3AU);
    st7789__write_parameter(0x05U);
}

static void st7789__write_te_config(void)
{
#if CONFIG__DISPLAY_TE_ENABLE
    st7789__write_command(0x35U);
    st7789__write_parameter(0x00U);
#else
    st7789__write_command(0x34U);
#endif
}

static void st7789__initial_er_tft020_3(void)
{
    st7789__reset_panel();
    st7789__write_common_format();

    st7789__write_command(0xB2U);
    st7789__write_parameter(0x0CU);
    st7789__write_parameter(0x0CU);
    st7789__write_parameter(0x00U);
    st7789__write_parameter(0x33U);
    st7789__write_parameter(0x33U);

    st7789__write_command(0xB7U);
    st7789__write_parameter(0x44U);
    st7789__write_command(0xBBU);
    st7789__write_parameter(0x24U);
    st7789__write_command(0xC0U);
    st7789__write_parameter(0x2CU);
    st7789__write_command(0xC2U);
    st7789__write_parameter(0x01U);
    st7789__write_command(0xC3U);
    st7789__write_parameter(0x13U);
    st7789__write_command(0xC4U);
    st7789__write_parameter(0x20U);
    st7789__write_command(0xC6U);
    st7789__write_parameter(0x0FU);
    st7789__write_command(0xD0U);
    st7789__write_parameter(0xA4U);
    st7789__write_parameter(0xA1U);
    st7789__write_command(0xD6U);
    st7789__write_parameter(0xA1U);

    static const uint8_t positive_gamma[] = {0xF0U, 0x00U, 0x04U, 0x04U, 0x04U, 0x05U, 0x29U,
                                             0x33U, 0x3EU, 0x38U, 0x12U, 0x12U, 0x28U, 0x30U};
    static const uint8_t negative_gamma[] = {0xF0U, 0x07U, 0x0AU, 0x0DU, 0x0BU, 0x07U, 0x28U,
                                             0x33U, 0x3EU, 0x36U, 0x14U, 0x14U, 0x29U, 0x32U};

    st7789__write_command(0xE0U);
    for (uint32_t index = 0U; index < sizeof(positive_gamma); index++)
    {
        st7789__write_parameter(positive_gamma[index]);
    }
    st7789__write_command(0xE1U);
    for (uint32_t index = 0U; index < sizeof(negative_gamma); index++)
    {
        st7789__write_parameter(negative_gamma[index]);
    }

    st7789__write_command(0x21U);
    st7789__write_command(0x11U);
    st7789__delay_ms(120U);
    st7789__write_command(0x29U);
}

static void st7789__initial_er_tft020_7(void)
{
    st7789__reset_panel();
    st7789__write_command(0x11U);
    st7789__delay_ms(120U);

    st7789__write_command(0xB2U);
    st7789__write_parameter(0x0CU);
    st7789__write_parameter(0x0CU);
    st7789__write_parameter(0x00U);
    st7789__write_parameter(0x33U);
    st7789__write_parameter(0x33U);
    st7789__write_te_config();
    st7789__write_common_format();

    st7789__write_command(0xB7U);
    st7789__write_parameter(0x06U);
    st7789__write_command(0xBBU);
    st7789__write_parameter(0x1AU);
    st7789__write_command(0xC0U);
    st7789__write_parameter(0x2CU);
    st7789__write_command(0xC2U);
    st7789__write_parameter(0x01U);
    st7789__write_command(0xC3U);
    st7789__write_parameter(0x05U);
    st7789__write_command(0xC6U);
    st7789__write_parameter(0x0FU);
    st7789__write_command(0xD0U);
    st7789__write_parameter(0xA7U);
    st7789__write_command(0xD0U);
    st7789__write_parameter(0xA4U);
    st7789__write_parameter(0xA1U);
    st7789__write_command(0xD6U);
    st7789__write_parameter(0xA1U);

    static const uint8_t positive_gamma[] = {0x0FU, 0x12U, 0x14U, 0x08U, 0x06U, 0x23U, 0x2FU,
                                             0x44U, 0x45U, 0x2AU, 0x16U, 0x16U, 0x25U, 0x28U};
    static const uint8_t negative_gamma[] = {0x0FU, 0x0FU, 0x12U, 0x06U, 0x04U, 0x00U, 0x2EU,
                                             0x43U, 0x44U, 0x3BU, 0x18U, 0x19U, 0x26U, 0x29U};

    st7789__write_command(0xE0U);
    for (uint32_t index = 0U; index < sizeof(positive_gamma); index++)
    {
        st7789__write_parameter(positive_gamma[index]);
    }
    st7789__write_command(0xE1U);
    for (uint32_t index = 0U; index < sizeof(negative_gamma); index++)
    {
        st7789__write_parameter(negative_gamma[index]);
    }

    st7789__write_command(0x21U);
    st7789__write_command(0x29U);
}

void st7789__init(void)
{
#if CONFIG__DISPLAY_PANEL == DISPLAY_PANEL_ER_TFT020_3
    st7789__initial_er_tft020_3();
#elif CONFIG__DISPLAY_PANEL == DISPLAY_PANEL_ER_TFT020_7
    st7789__initial_er_tft020_7();
#else
#error Invalid CONFIG__DISPLAY_PANEL.
#endif
}

void st7789__set_window(uint32_t x0, uint32_t x1, uint32_t y0, uint32_t y1)
{
    st7789__write_command(0x2AU);
    st7789__write_parameter((uint8_t)(x0 >> 8U));
    st7789__write_parameter((uint8_t)x0);
    st7789__write_parameter((uint8_t)(x1 >> 8U));
    st7789__write_parameter((uint8_t)x1);

    st7789__write_command(0x2BU);
    st7789__write_parameter((uint8_t)(y0 >> 8U));
    st7789__write_parameter((uint8_t)y0);
    st7789__write_parameter((uint8_t)(y1 >> 8U));
    st7789__write_parameter((uint8_t)y1);
    st7789__write_command(0x2CU);
}

void st7789__begin_pixels(void)
{
    ST7789_RS_SET();
}
