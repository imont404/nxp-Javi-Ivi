#include "button.h"

#include <stddef.h>

#include "fsl_common.h"
#include "fsl_gpio.h"

typedef enum
{
    BUTTON_DEBOUNCE_WAIT_FOR_PRESS = 0,
    BUTTON_DEBOUNCE_PRESS_STABLE,
    BUTTON_DEBOUNCE_WAIT_FOR_RELEASE,
    BUTTON_DEBOUNCE_RELEASE_STABLE
} button_debounce_state_t;

typedef struct
{
    uint8_t io_port;
    uint8_t io_bit;
    uint8_t polarity;
    uint32_t debounce_time_ms;
    uint32_t debounce_timer_ms;
    uint32_t hold_time_ms;
    button_debounce_state_t state;
    volatile bool held;
    volatile bool release_pending;
    volatile uint32_t press_sequence;
    volatile uint32_t release_sequence;
    volatile uint32_t release_hold_ms;
} button_t;

static button_t g_buttons[BUTTON_ID_COUNT];

static bool button__valid_id(button_id_t id)
{
    return ((uint32_t)id < (uint32_t)BUTTON_ID_COUNT);
}

static bool button__read_pin(const button_t *button)
{
    uint32_t pin_high = 0U;

    if (button->io_port == 3U)
    {
        pin_high = GPIO_PinRead(GPIO3, button->io_bit);
    }

    if (button->polarity == BUTTON_POLARITY_LOW_ACTIVE)
    {
        return (pin_high == 0U);
    }
    return (pin_high != 0U);
}

static void button__process_one(button_t *button, uint32_t process_time_ms)
{
    bool pressed = button__read_pin(button);

    switch (button->state)
    {
    case BUTTON_DEBOUNCE_WAIT_FOR_PRESS:
        if (pressed)
        {
            button->debounce_timer_ms = 0U;
            button->state = BUTTON_DEBOUNCE_PRESS_STABLE;
        }
        break;

    case BUTTON_DEBOUNCE_PRESS_STABLE:
        if (!pressed)
        {
            button->state = BUTTON_DEBOUNCE_WAIT_FOR_PRESS;
            break;
        }

        button->debounce_timer_ms += process_time_ms;
        if (button->debounce_timer_ms >= button->debounce_time_ms)
        {
            button->hold_time_ms = button->debounce_timer_ms;
            button->held = true;
            button->press_sequence++;
            button->state = BUTTON_DEBOUNCE_WAIT_FOR_RELEASE;
        }
        break;

    case BUTTON_DEBOUNCE_WAIT_FOR_RELEASE:
        if (pressed)
        {
            if (button->hold_time_ms <= (UINT32_MAX - process_time_ms))
            {
                button->hold_time_ms += process_time_ms;
            }
            else
            {
                button->hold_time_ms = UINT32_MAX;
            }
            break;
        }

        button->debounce_timer_ms = 0U;
        button->state = BUTTON_DEBOUNCE_RELEASE_STABLE;
        break;

    case BUTTON_DEBOUNCE_RELEASE_STABLE:
    default:
        if (pressed)
        {
            button->debounce_timer_ms = 0U;
            button->state = BUTTON_DEBOUNCE_WAIT_FOR_RELEASE;
            break;
        }

        button->debounce_timer_ms += process_time_ms;
        if (button->debounce_timer_ms >= button->debounce_time_ms)
        {
            button->held = false;
            button->release_hold_ms = button->hold_time_ms;
            button->release_sequence++;
            button->state = BUTTON_DEBOUNCE_WAIT_FOR_PRESS;
        }
        break;
    }

    /*
     * A transition must suppress the eventual release even when the press is
     * still inside its debounce window. Keep this separate from participant-
     * facing held, which remains true only after a qualified press.
     */
    button->release_pending = pressed || (button->state != BUTTON_DEBOUNCE_WAIT_FOR_PRESS);
}

void button__configure(button_id_t id, uint8_t io_port, uint8_t io_bit, uint8_t polarity,
                       uint32_t debounce_time_ms)
{
    button_t *button;

    if (!button__valid_id(id))
    {
        return;
    }

    button = &g_buttons[id];
    button->io_port = io_port;
    button->io_bit = io_bit;
    button->polarity = polarity;
    button->debounce_time_ms = debounce_time_ms;
    button->debounce_timer_ms = 0U;
    button->hold_time_ms = 0U;
    button->state = BUTTON_DEBOUNCE_WAIT_FOR_PRESS;
    button->held = false;
    button->release_pending = false;
    button->press_sequence = 0U;
    button->release_sequence = 0U;
    button->release_hold_ms = 0U;
}

void button__process_all(uint32_t process_time_ms)
{
    uint32_t index;

    for (index = 0U; index < (uint32_t)BUTTON_ID_COUNT; index++)
    {
        button__process_one(&g_buttons[index], process_time_ms);
    }
}

void button__snapshot(button_snapshot_t *snapshot)
{
    uint32_t index;
    uint32_t interrupt_state;

    if (snapshot == NULL)
    {
        return;
    }

    interrupt_state = DisableGlobalIRQ();
    for (index = 0U; index < (uint32_t)BUTTON_ID_COUNT; index++)
    {
        snapshot->button[index].held = g_buttons[index].held;
        snapshot->button[index].release_pending = g_buttons[index].release_pending;
        snapshot->button[index].press_sequence = g_buttons[index].press_sequence;
        snapshot->button[index].release_sequence = g_buttons[index].release_sequence;
        snapshot->button[index].release_hold_ms = g_buttons[index].release_hold_ms;
    }
    EnableGlobalIRQ(interrupt_state);
}
