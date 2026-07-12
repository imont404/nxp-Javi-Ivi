#include "st7789_parallel_bitbang.h"

#include <stdint.h>

#include "avc__master_config.h"
#include "e.h"
#include "fsl_clock.h"
#include "fsl_common.h"
#include "fsl_gpio.h"
#include "fsl_port.h"
#include "st7789.h"

#define LCDP_PIN_MASK(pin) (1UL << (pin))

/* ER-TFT020-7 8080 proof map, matching docs/research/ER-TFT020-7/parallel_ezh_io_map.md. */
#define LCDP_D0_GPIO GPIO1
#define LCDP_D0_PORT PORT1
#define LCDP_D0_PIN  4U
#define LCDP_D1_GPIO GPIO1
#define LCDP_D1_PORT PORT1
#define LCDP_D1_PIN  5U
#define LCDP_D2_GPIO GPIO1
#define LCDP_D2_PORT PORT1
#define LCDP_D2_PIN  6U
#define LCDP_D3_GPIO GPIO1
#define LCDP_D3_PORT PORT1
#define LCDP_D3_PIN  7U
#define LCDP_D4_GPIO GPIO3
#define LCDP_D4_PORT PORT3
#define LCDP_D4_PIN  4U
#define LCDP_D5_GPIO GPIO3
#define LCDP_D5_PORT PORT3
#define LCDP_D5_PIN  5U
#define LCDP_D6_GPIO GPIO1
#define LCDP_D6_PORT PORT1
#define LCDP_D6_PIN  10U
#define LCDP_D7_GPIO GPIO1
#define LCDP_D7_PORT PORT1
#define LCDP_D7_PIN  11U

#define LCDP_WR_GPIO GPIO0
#define LCDP_WR_PORT PORT0
#define LCDP_WR_PIN  4U
#define LCDP_RS_GPIO GPIO0
#define LCDP_RS_PORT PORT0
#define LCDP_RS_PIN  11U
#define LCDP_CS_GPIO GPIO4
#define LCDP_CS_PORT PORT4
#define LCDP_CS_PIN  4U
#define LCDP_RST_GPIO GPIO1
#define LCDP_RST_PORT PORT1
#define LCDP_RST_PIN  19U

#define LCDP_DATA_P1_MASK (LCDP_PIN_MASK(LCDP_D0_PIN) | LCDP_PIN_MASK(LCDP_D1_PIN) | \
                           LCDP_PIN_MASK(LCDP_D2_PIN) | LCDP_PIN_MASK(LCDP_D3_PIN) | \
                           LCDP_PIN_MASK(LCDP_D6_PIN) | LCDP_PIN_MASK(LCDP_D7_PIN))
#define LCDP_DATA_P3_MASK (LCDP_PIN_MASK(LCDP_D4_PIN) | LCDP_PIN_MASK(LCDP_D5_PIN))
#define LCDP_WR_MASK      LCDP_PIN_MASK(LCDP_WR_PIN)
#define LCDP_RS_MASK      LCDP_PIN_MASK(LCDP_RS_PIN)
#define LCDP_CS_MASK      LCDP_PIN_MASK(LCDP_CS_PIN)
#define LCDP_RST_MASK     LCDP_PIN_MASK(LCDP_RST_PIN)

#if defined(CONFIG_DISPLAY_ORIENTATION) && (CONFIG_DISPLAY_ORIENTATION == PORTRAIT)
#define LCDP_WIDTH  240U
#define LCDP_HEIGHT 320U
#define LCDP_MADCTL 0x00U
#elif defined(CONFIG_DISPLAY_ORIENTATION) && (CONFIG_DISPLAY_ORIENTATION == LANDSCAPE)
#define LCDP_WIDTH  320U
#define LCDP_HEIGHT 240U
#define LCDP_MADCTL 0x60U
#else
#error Invalid CONFIG_DISPLAY_ORIENTATION.
#endif

static void st7789_parallel__delay_cycles(uint32_t cycles)
{
    while (cycles-- != 0U)
    {
        __NOP();
    }
}

static void st7789_parallel__delay_ms(uint32_t ms)
{
    e_tick__delay_ms(ms);
}

static void st7789_parallel__gpio_output(PORT_Type *port, GPIO_Type *gpio, uint32_t pin, uint8_t initial)
{
    gpio_pin_config_t config = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic = initial,
    };

    GPIO_PinInit(gpio, pin, &config);
    PORT_SetPinMux(port, pin, kPORT_MuxAlt0);
}

static void st7789_parallel__configure_pins(void)
{
    CLOCK_EnableClock(kCLOCK_Gpio0);
    CLOCK_EnableClock(kCLOCK_Gpio1);
    CLOCK_EnableClock(kCLOCK_Gpio3);
    CLOCK_EnableClock(kCLOCK_Gpio4);
    CLOCK_EnableClock(kCLOCK_Port0);
    CLOCK_EnableClock(kCLOCK_Port1);
    CLOCK_EnableClock(kCLOCK_Port3);
    CLOCK_EnableClock(kCLOCK_Port4);

    st7789_parallel__gpio_output(LCDP_D0_PORT, LCDP_D0_GPIO, LCDP_D0_PIN, 0U);
    st7789_parallel__gpio_output(LCDP_D1_PORT, LCDP_D1_GPIO, LCDP_D1_PIN, 0U);
    st7789_parallel__gpio_output(LCDP_D2_PORT, LCDP_D2_GPIO, LCDP_D2_PIN, 0U);
    st7789_parallel__gpio_output(LCDP_D3_PORT, LCDP_D3_GPIO, LCDP_D3_PIN, 0U);
    st7789_parallel__gpio_output(LCDP_D4_PORT, LCDP_D4_GPIO, LCDP_D4_PIN, 0U);
    st7789_parallel__gpio_output(LCDP_D5_PORT, LCDP_D5_GPIO, LCDP_D5_PIN, 0U);
    st7789_parallel__gpio_output(LCDP_D6_PORT, LCDP_D6_GPIO, LCDP_D6_PIN, 0U);
    st7789_parallel__gpio_output(LCDP_D7_PORT, LCDP_D7_GPIO, LCDP_D7_PIN, 0U);

    st7789_parallel__gpio_output(LCDP_WR_PORT, LCDP_WR_GPIO, LCDP_WR_PIN, 1U);
    st7789_parallel__gpio_output(LCDP_RS_PORT, LCDP_RS_GPIO, LCDP_RS_PIN, 1U);
    st7789_parallel__gpio_output(LCDP_CS_PORT, LCDP_CS_GPIO, LCDP_CS_PIN, 1U);
    st7789_parallel__gpio_output(LCDP_RST_PORT, LCDP_RST_GPIO, LCDP_RST_PIN, 1U);
}

static void st7789_parallel__data_bus_write(uint8_t value)
{
    uint32_t p1_set = 0U;
    uint32_t p3_set = 0U;

    if ((value & 0x01U) != 0U)
    {
        p1_set |= LCDP_PIN_MASK(LCDP_D0_PIN);
    }
    if ((value & 0x02U) != 0U)
    {
        p1_set |= LCDP_PIN_MASK(LCDP_D1_PIN);
    }
    if ((value & 0x04U) != 0U)
    {
        p1_set |= LCDP_PIN_MASK(LCDP_D2_PIN);
    }
    if ((value & 0x08U) != 0U)
    {
        p1_set |= LCDP_PIN_MASK(LCDP_D3_PIN);
    }
    if ((value & 0x10U) != 0U)
    {
        p3_set |= LCDP_PIN_MASK(LCDP_D4_PIN);
    }
    if ((value & 0x20U) != 0U)
    {
        p3_set |= LCDP_PIN_MASK(LCDP_D5_PIN);
    }
    if ((value & 0x40U) != 0U)
    {
        p1_set |= LCDP_PIN_MASK(LCDP_D6_PIN);
    }
    if ((value & 0x80U) != 0U)
    {
        p1_set |= LCDP_PIN_MASK(LCDP_D7_PIN);
    }

    GPIO1->PCOR = LCDP_DATA_P1_MASK;
    GPIO3->PCOR = LCDP_DATA_P3_MASK;
    GPIO1->PSOR = p1_set;
    GPIO3->PSOR = p3_set;
}

static void st7789_parallel__write_active_byte(uint8_t value)
{
    st7789_parallel__data_bus_write(value);
    st7789_parallel__delay_cycles(CONFIG__DISPLAY_PARALLEL_BITBANG_WR_DELAY_CYCLES);

    GPIO0->PCOR = LCDP_WR_MASK;
    st7789_parallel__delay_cycles(CONFIG__DISPLAY_PARALLEL_BITBANG_WR_DELAY_CYCLES);

    GPIO0->PSOR = LCDP_WR_MASK;
    st7789_parallel__delay_cycles(CONFIG__DISPLAY_PARALLEL_BITBANG_WR_DELAY_CYCLES);
}

static void st7789_parallel__write_command(uint8_t command)
{
    GPIO0->PCOR = LCDP_RS_MASK;
    GPIO4->PCOR = LCDP_CS_MASK;
    st7789_parallel__write_active_byte(command);
    GPIO4->PSOR = LCDP_CS_MASK;
}

static void st7789_parallel__write_data8(uint8_t data)
{
    GPIO0->PSOR = LCDP_RS_MASK;
    GPIO4->PCOR = LCDP_CS_MASK;
    st7789_parallel__write_active_byte(data);
    GPIO4->PSOR = LCDP_CS_MASK;
}

static void st7789_parallel__write_data16_active(uint16_t data)
{
    st7789_parallel__write_active_byte((uint8_t)(data >> 8));
    st7789_parallel__write_active_byte((uint8_t)data);
}

static void st7789_parallel__reset_panel(void)
{
    GPIO1->PSOR = LCDP_RST_MASK;
    st7789_parallel__delay_ms(1U);
    GPIO1->PCOR = LCDP_RST_MASK;
    st7789_parallel__delay_ms(10U);
    GPIO1->PSOR = LCDP_RST_MASK;
    st7789_parallel__delay_ms(120U);
}

static void st7789_parallel__write_te_config(void)
{
#if CONFIG__DISPLAY_TE_ENABLE
    st7789_parallel__write_command(0x35U);
    st7789_parallel__write_data8(0x00U);
#else
    st7789_parallel__write_command(0x34U);
#endif
}

static void st7789_parallel__set_window(uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1)
{
    st7789_parallel__write_command(0x2AU);
    st7789_parallel__write_data8((uint8_t)(x0 >> 8));
    st7789_parallel__write_data8((uint8_t)x0);
    st7789_parallel__write_data8((uint8_t)(x1 >> 8));
    st7789_parallel__write_data8((uint8_t)x1);

    st7789_parallel__write_command(0x2BU);
    st7789_parallel__write_data8((uint8_t)(y0 >> 8));
    st7789_parallel__write_data8((uint8_t)y0);
    st7789_parallel__write_data8((uint8_t)(y1 >> 8));
    st7789_parallel__write_data8((uint8_t)y1);

    st7789_parallel__write_command(0x2CU);
}

static uint16_t st7789_parallel__test_color(uint32_t x)
{
    uint32_t band = (x * 3U) / LCDP_WIDTH;

    switch (band)
    {
        case 0:
            return 0xF800U;
        case 1:
            return 0x07E0U;
        default:
            return 0x001FU;
    }
}

void st7789_parallel_bitbang__init(void)
{
    st7789_parallel__configure_pins();

    GPIO4->PSOR = LCDP_CS_MASK;
    GPIO0->PSOR = LCDP_WR_MASK;
    GPIO0->PSOR = LCDP_RS_MASK;

    st7789_parallel__reset_panel();

    st7789_parallel__write_command(0x11U);
    st7789_parallel__delay_ms(120U);

    st7789_parallel__write_command(0xB2U);
    st7789_parallel__write_data8(0x0CU);
    st7789_parallel__write_data8(0x0CU);
    st7789_parallel__write_data8(0x00U);
    st7789_parallel__write_data8(0x33U);
    st7789_parallel__write_data8(0x33U);

    st7789_parallel__write_te_config();

    st7789_parallel__write_command(0x36U);
    st7789_parallel__write_data8(LCDP_MADCTL);

    st7789_parallel__write_command(0x3AU);
    st7789_parallel__write_data8(0x05U);

    st7789_parallel__write_command(0xB7U);
    st7789_parallel__write_data8(0x06U);

    st7789_parallel__write_command(0xBBU);
    st7789_parallel__write_data8(0x1AU);

    st7789_parallel__write_command(0xC0U);
    st7789_parallel__write_data8(0x2CU);

    st7789_parallel__write_command(0xC2U);
    st7789_parallel__write_data8(0x01U);

    st7789_parallel__write_command(0xC3U);
    st7789_parallel__write_data8(0x05U);

    st7789_parallel__write_command(0xC6U);
    st7789_parallel__write_data8(0x0FU);

    st7789_parallel__write_command(0xD0U);
    st7789_parallel__write_data8(0xA7U);

    st7789_parallel__write_command(0xD0U);
    st7789_parallel__write_data8(0xA4U);
    st7789_parallel__write_data8(0xA1U);

    st7789_parallel__write_command(0xD6U);
    st7789_parallel__write_data8(0xA1U);

    st7789_parallel__write_command(0xE0U);
    st7789_parallel__write_data8(0x0FU);
    st7789_parallel__write_data8(0x12U);
    st7789_parallel__write_data8(0x14U);
    st7789_parallel__write_data8(0x08U);
    st7789_parallel__write_data8(0x06U);
    st7789_parallel__write_data8(0x23U);
    st7789_parallel__write_data8(0x2FU);
    st7789_parallel__write_data8(0x44U);
    st7789_parallel__write_data8(0x45U);
    st7789_parallel__write_data8(0x2AU);
    st7789_parallel__write_data8(0x16U);
    st7789_parallel__write_data8(0x16U);
    st7789_parallel__write_data8(0x25U);
    st7789_parallel__write_data8(0x28U);

    st7789_parallel__write_command(0xE1U);
    st7789_parallel__write_data8(0x0FU);
    st7789_parallel__write_data8(0x0FU);
    st7789_parallel__write_data8(0x12U);
    st7789_parallel__write_data8(0x06U);
    st7789_parallel__write_data8(0x04U);
    st7789_parallel__write_data8(0x00U);
    st7789_parallel__write_data8(0x2EU);
    st7789_parallel__write_data8(0x43U);
    st7789_parallel__write_data8(0x44U);
    st7789_parallel__write_data8(0x3BU);
    st7789_parallel__write_data8(0x18U);
    st7789_parallel__write_data8(0x19U);
    st7789_parallel__write_data8(0x26U);
    st7789_parallel__write_data8(0x29U);

    st7789_parallel__write_command(0x21U);
    st7789_parallel__write_command(0x29U);
}

void st7789_parallel_bitbang__draw_test_pattern(void)
{
    st7789_parallel__set_window(0U, LCDP_WIDTH - 1U, 0U, LCDP_HEIGHT - 1U);

    GPIO0->PSOR = LCDP_RS_MASK;
    GPIO4->PCOR = LCDP_CS_MASK;

    for (uint32_t y = 0U; y < LCDP_HEIGHT; y++)
    {
        for (uint32_t x = 0U; x < LCDP_WIDTH; x++)
        {
            st7789_parallel__write_data16_active(st7789_parallel__test_color(x));
        }
    }

    GPIO4->PSOR = LCDP_CS_MASK;
}

void st7789_parallel_bitbang__run_test(void)
{
    uint32_t frame = 0U;

    (void)DEBUG("parallel_lcd pins: D0-D7=P1_4,P1_5,P1_6,P1_7,P3_4,P3_5,P1_10,P1_11 WR=P0_4 RS=P0_11 CS=P4_4 RST=P1_19 RD=external-inactive-high\r\n");

    st7789_parallel_bitbang__init();

    while (1)
    {
        st7789_parallel_bitbang__draw_test_pattern();
        frame++;
        (void)DEBUG("parallel_lcd frame=%u width=%u height=%u wr_delay=%u\r\n",
                    frame,
                    LCDP_WIDTH,
                    LCDP_HEIGHT,
                    CONFIG__DISPLAY_PARALLEL_BITBANG_WR_DELAY_CYCLES);
        st7789_parallel__delay_ms(CONFIG__DISPLAY_PARALLEL_BITBANG_FRAME_DELAY_MS);
    }
}
