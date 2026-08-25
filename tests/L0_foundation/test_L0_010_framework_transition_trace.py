"""L0_010 - execute framework TEST/RACE transitions against fake hardware."""

import shutil
import subprocess
from pathlib import Path

import pytest


REPO = Path(__file__).resolve().parents[2]
SOURCE = REPO / "src/embedded/nxp_cup_core0/source"
BUTTON_INCLUDE = SOURCE / "nxpc_io"


STUB_HEADER = r"""
#ifndef NXPC_FRAMEWORK_TRACE_STUBS_H_
#define NXPC_FRAMEWORK_TRACE_STUBS_H_

#include <stdbool.h>
#include <stdint.h>

#define NXPC__IO_H_
#define CONFIG__USB_DEBUG_STREAM_ENABLE 0
#define CONFIG__DISPLAY_ENABLE 1
#define CONFIG__DISPLAY_SCOPE_MARKER_ENABLE 0
#define CONFIG__FORCE_TEST_MODE 0
#define CONFIG__FORCE_RACE_WAITING_MODE 0
#define CONFIG__MOTOR_ENCODER_WHEEL_DIAMETER_MM 60
#define TEST_SW_PIN 18U
#define GPIO3 ((void *)0)
#define DEBUG(...) do { } while (0)
#define CYCLE_COUNTER fake_cycle_counter
#define E_TICK__TRIGGER_FOREVER (-1)

typedef void (*e_tick__hook_t)(void *argument);
typedef struct e_tick__trigger_handler
{
    void *next_item;
    int32_t interval__mS;
    int32_t duration__mS;
    int32_t count;
    e_tick__hook_t hook;
    bool marked_for_exe;
    void *arg;
} e_tick__trigger_handler_t;

extern uint32_t fake_cycle_counter;
extern uint32_t SystemCoreClock;

uint32_t DisableGlobalIRQ(void);
void EnableGlobalIRQ(uint32_t state);
uint32_t CLOCK_GetCoreSysClkFreq(void);
uint32_t e_tick__get_ms(void);
uint32_t e_tick__delta(uint32_t *time_in);
uint32_t e_tick__timeout(uint32_t *time_in, uint32_t delta);
void e_tick__register_trigger(e_tick__trigger_handler_t *trigger, bool irq_exe);

void nxpc__init(void);
void nxpc_camera__service(void);
void nxpc_camera__prepare_frame(uint16_t *frame);
void nxpc__enable_motor_control(void);
void nxpc__disable_motor_control(void);
void nxpc__set_motor_pwm(float left, float right);
bool nxpc__motor_control_enabled(void);
float nxpc__motor_left_command(void);
float nxpc__motor_right_command(void);
void nxpc__set_servo(float position);
float nxpc__servo_command(void);
float nxpc__read_alpha(void);
float nxpc__read_beta(void);
float nxpc__read_gamma(void);
uint16_t nxpc__read_battery_voltage(void);
uint32_t GPIO_PinRead(void *base, uint32_t pin);
void eGFX_DumpRaw(uint8_t *buffer,
                  uint32_t length,
                  uint32_t x0,
                  uint32_t x1,
                  uint32_t y0,
                  uint32_t y1);

#endif
"""


HARNESS = r"""
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "nxpc_framework.h"
#include "nxpc_graphics.h"
#include "nxpc_color.h"
#include "nxpc__motor_encoder_qdc.h"
#include "nxp_cup.h"

void nxpc__next_frame(uint16_t *buffer);

uint32_t fake_cycle_counter;
uint32_t SystemCoreClock = 150000000U;
const uint32_t nxpc_rgb565_yhsv_lut[NXPC_RGB565_COLOR_LUT_ENTRY_COUNT] = {0};

static uint32_t now_ms;
static uint32_t irq_disabled;
static bool test_jumper = true;
static float alpha = 0.5f;
static float beta = 0.5f;
static float gamma = 0.5f;
static bool motor_enabled;
static float motor_left;
static float motor_right;
static float servo_position;
static bool inject_expiry_on_irq_disable;
static bool record_transition_order;
static nxpc_test_page_t page_seen_at_disable = NXPC_TEST_PAGE_COUNT;
static nxpc_test_page_t page_seen_at_center = NXPC_TEST_PAGE_COUNT;
static button_snapshot_t buttons;
static e_tick__trigger_handler_t *lease_trigger;
static unsigned camera_frame_dump_count;

static int fail(const char *message)
{
    (void)fprintf(stderr, "%s\n", message);
    return 1;
}

static bool near(float left, float right)
{
    return fabsf(left - right) < 0.001f;
}

static void expire_lease_now(void)
{
    unsigned index;

    if (lease_trigger == NULL) return;
    for (index = 0U; index < 16U; index++)
        lease_trigger->hook(lease_trigger->arg);
}

uint32_t DisableGlobalIRQ(void)
{
    uint32_t previous = irq_disabled;

    if (inject_expiry_on_irq_disable && (irq_disabled == 0U))
    {
        inject_expiry_on_irq_disable = false;
        expire_lease_now();
    }
    irq_disabled = 1U;
    return previous;
}

void EnableGlobalIRQ(uint32_t state) { irq_disabled = state; }
uint32_t CLOCK_GetCoreSysClkFreq(void) { return 150000000U; }
uint32_t e_tick__get_ms(void) { return now_ms; }
uint32_t e_tick__delta(uint32_t *time_in) { return now_ms - *time_in; }
uint32_t e_tick__timeout(uint32_t *time_in, uint32_t delta)
{
    uint32_t elapsed = now_ms - *time_in;
    if (elapsed < delta) return 0U;
    *time_in = now_ms;
    return 1U + (elapsed - delta);
}
void e_tick__register_trigger(e_tick__trigger_handler_t *trigger, bool irq_exe)
{
    (void)irq_exe;
    lease_trigger = trigger;
}

void nxpc__init(void) { }
void nxpc_camera__service(void) { }
void nxpc_camera__prepare_frame(uint16_t *frame) { (void)frame; }
void nxpc__enable_motor_control(void)
{
    if (!motor_enabled)
    {
        motor_left = 0.0f;
        motor_right = 0.0f;
        motor_enabled = true;
    }
}
void nxpc__disable_motor_control(void)
{
    if (record_transition_order &&
        (page_seen_at_disable == NXPC_TEST_PAGE_COUNT))
        page_seen_at_disable = nxpc_system__test_page();
    motor_left = 0.0f;
    motor_right = 0.0f;
    motor_enabled = false;
}
void nxpc__set_motor_pwm(float left, float right)
{
    if (motor_enabled)
    {
        motor_left = left;
        motor_right = right;
    }
}
bool nxpc__motor_control_enabled(void) { return motor_enabled; }
float nxpc__motor_left_command(void) { return motor_left; }
float nxpc__motor_right_command(void) { return motor_right; }
void nxpc__set_servo(float position)
{
    if (record_transition_order && (position == 0.0f) &&
        (page_seen_at_center == NXPC_TEST_PAGE_COUNT))
        page_seen_at_center = nxpc_system__test_page();
    servo_position = position;
}
float nxpc__servo_command(void) { return servo_position; }
float nxpc__read_alpha(void) { return alpha; }
float nxpc__read_beta(void) { return beta; }
float nxpc__read_gamma(void) { return gamma; }
uint16_t nxpc__read_battery_voltage(void) { return 740U; }
uint32_t GPIO_PinRead(void *base, uint32_t pin)
{
    (void)base;
    (void)pin;
    return test_jumper ? 0U : 1U;
}
void button__snapshot(button_snapshot_t *snapshot) { *snapshot = buttons; }

void eGFX_DumpRaw(uint8_t *buffer,
                  uint32_t length,
                  uint32_t x0,
                  uint32_t x1,
                  uint32_t y0,
                  uint32_t y1)
{
    (void)buffer;
    (void)x0;
    (void)x1;
    if ((length == (CAMERA_WIDTH * CAMERA_HEIGHT * 2U)) &&
        (y0 == 40U) && (y1 == 239U))
    {
        camera_frame_dump_count++;
    }
}

void nxpc__motor_encoder_qdc_sample(
    uint32_t elapsed_ms,
    nxpc_motor_encoder_sample_t samples[NXPC_MOTOR_ENCODER_COUNT])
{
    (void)elapsed_ms;
    samples[NXPC_MOTOR_ENCODER_M0].rpm_milli = 12000;
    samples[NXPC_MOTOR_ENCODER_M1].rpm_milli = -8000;
}

void nxpc_graphics__pixel(const nxpc_rgb565_surface_t *surface, int32_t x, int32_t y,
                          uint16_t color)
{ (void)surface; (void)x; (void)y; (void)color; }
void nxpc_graphics__horizontal_line(const nxpc_rgb565_surface_t *surface, int32_t x0, int32_t x1,
                                    int32_t y, uint16_t color)
{ (void)surface; (void)x0; (void)x1; (void)y; (void)color; }
void nxpc_graphics__vertical_line(const nxpc_rgb565_surface_t *surface, int32_t x, int32_t y0,
                                  int32_t y1, uint16_t color)
{ (void)surface; (void)x; (void)y0; (void)y1; (void)color; }
void nxpc_graphics__line(const nxpc_rgb565_surface_t *surface, int32_t x0, int32_t y0, int32_t x1,
                         int32_t y1, uint16_t color)
{ (void)surface; (void)x0; (void)y0; (void)x1; (void)y1; (void)color; }
void nxpc_graphics__fill_rectangle(const nxpc_rgb565_surface_t *surface, int32_t x0, int32_t y0,
                                   int32_t x1, int32_t y1, uint16_t color)
{ (void)surface; (void)x0; (void)y0; (void)x1; (void)y1; (void)color; }
void nxpc_graphics__text(const nxpc_rgb565_surface_t *surface, int32_t x, int32_t y,
                         const char *text, uint16_t color)
{ (void)surface; (void)x; (void)y; (void)text; (void)color; }
void nxpc_graphics__text_large(const nxpc_rgb565_surface_t *surface, int32_t x, int32_t y,
                               const char *text, uint16_t color)
{ (void)surface; (void)x; (void)y; (void)text; (void)color; }
int32_t nxpc_graphics__text_large_width(const char *text)
{ (void)text; return 0; }

void frame_draw_pixel(uint16_t *frame, int32_t x, int32_t y, uint16_t color)
{ (void)frame; (void)x; (void)y; (void)color; }
void frame_draw_horizontal_line(uint16_t *frame, int32_t x0, int32_t x1, int32_t y,
                                uint16_t color)
{ (void)frame; (void)x0; (void)x1; (void)y; (void)color; }
void frame_draw_vertical_line(uint16_t *frame, int32_t x, int32_t y0, int32_t y1, uint16_t color)
{ (void)frame; (void)x; (void)y0; (void)y1; (void)color; }
void frame_draw_line(uint16_t *frame, int32_t x0, int32_t y0, int32_t x1, int32_t y1,
                     uint16_t color)
{ (void)frame; (void)x0; (void)y0; (void)x1; (void)y1; (void)color; }
void frame_fill_rectangle(uint16_t *frame, int32_t x0, int32_t y0, int32_t x1, int32_t y1,
                          uint16_t color)
{ (void)frame; (void)x0; (void)y0; (void)x1; (void)y1; (void)color; }
void frame_draw_text(uint16_t *frame, int32_t x, int32_t y, const char *text, uint16_t color)
{ (void)frame; (void)x; (void)y; (void)text; (void)color; }

static void advance_ms(uint32_t elapsed)
{
    uint32_t step;

    for (step = 0U; step < elapsed; step++)
    {
        now_ms++;
        if (lease_trigger != NULL)
        {
            lease_trigger->duration__mS--;
            if (lease_trigger->duration__mS <= 0)
            {
                lease_trigger->duration__mS += lease_trigger->interval__mS;
                lease_trigger->hook(lease_trigger->arg);
            }
        }
    }
}

static void release_button(button_id_t id)
{
    buttons.button[id].release_sequence++;
    nxpc_framework__service();
}

static void complete_center_dwell(void)
{
    unsigned sample;

    for (sample = 0U; sample < 5U; sample++)
    {
        advance_ms(50U);
        nxpc_framework__service();
    }
}

static void arm_actuators(void)
{
    alpha = 0.5f;
    beta = 0.5f;
    gamma = 0.5f;
    release_button(BUTTON_ID_CENTER);
    complete_center_dwell();
}

static int enter_race(uint16_t *frame)
{
    test_jumper = false;
    nxpc_framework__init();
    nxpc_framework__service();
    nxpc__next_frame(frame);
    nxpc_framework__service();
    release_button(BUTTON_ID_CENTER);
    if (nxpc_system__mode() != NXPC_SYSTEM_MODE_RACE_RUNNING)
        return fail("RACE scenario did not start after camera and EXE");
    if (!motor_enabled || !near(motor_left, 0.0f) || !near(motor_right, 0.0f))
        return fail("RACE entry did not enable zero-duty audible arming");
    return 0;
}

static int run_camera_loss_scenario(void)
{
    uint16_t frame = 0U;

    if (enter_race(&frame) != 0) return 1;
    motors_set_duty(0.5f, -0.5f);
    steering_set(0.5f);
    advance_ms(NXPC_SYSTEM_CAMERA_FRESH_MS + 1U);
    /* Renew after the independent motor lease expires so the following safe
     * output transition is attributable to camera loss itself. */
    motors_set_duty(-0.5f, 0.5f);
    steering_set(-0.5f);
    if (!motor_enabled || !near(motor_left, -0.5f) || !near(motor_right, 0.5f))
        return fail("RACE command did not recover immediately before camera fault");
    nxpc_framework__service();
    if ((nxpc_system__mode() != NXPC_SYSTEM_MODE_SAFE_FAULT) ||
        (nxpc_system__fault() != NXPC_SYSTEM_FAULT_CAMERA_LOST))
        return fail("camera loss did not latch the typed safe fault");
    if (motor_enabled || !near(motor_left, 0.0f) || !near(motor_right, 0.0f) ||
        !near(servo_position, 0.0f))
        return fail("camera-loss fault did not safe all outputs");
    return 0;
}

static int run_ambiguous_waiting_scenario(void)
{
    test_jumper = false;
    nxpc_framework__init();
    nxpc_framework__service();

    /* Seed unsafe fake hardware to prove the ambiguous WAITING branch actively
     * restores the safe state rather than merely relying on prior state. */
    motor_enabled = true;
    motor_left = 0.5f;
    motor_right = -0.5f;
    servo_position = 0.5f;
    buttons.button[BUTTON_ID_CENTER].release_sequence += 2U;
    nxpc_framework__service();
    if ((nxpc_system__mode() != NXPC_SYSTEM_MODE_RACE_WAITING) || motor_enabled ||
        !near(motor_left, 0.0f) || !near(motor_right, 0.0f) ||
        !near(servo_position, 0.0f))
        return fail("ambiguous WAITING EXE epoch did not fail safe");
    return 0;
}

static int run_ambiguous_running_scenario(void)
{
    uint16_t frame = 0U;

    if (enter_race(&frame) != 0) return 1;
    motors_set_duty(0.5f, -0.5f);
    steering_set(0.5f);
    buttons.button[BUTTON_ID_CENTER].release_sequence += 2U;
    nxpc_framework__service();
    if ((nxpc_system__mode() != NXPC_SYSTEM_MODE_RACE_WAITING) || motor_enabled ||
        !near(motor_left, 0.0f) || !near(motor_right, 0.0f) ||
        !near(servo_position, 0.0f))
        return fail("ambiguous RUNNING EXE epoch did not stop safely");
    return 0;
}

static int run_typed_stop_scenario(void)
{
    uint16_t frame = 0U;

    if (enter_race(&frame) != 0) return 1;
    motors_set_duty(0.5f, -0.5f);
    steering_set(-0.5f);
    if (nxpc_system__request_action(NXPC_SYSTEM_ACTION_STOP) !=
        NXPC_SYSTEM_ACTION_ACCEPTED)
        return fail("typed STOP action was rejected");
    nxpc_framework__service();
    if ((nxpc_system__mode() != NXPC_SYSTEM_MODE_RACE_WAITING) || motor_enabled ||
        !near(motor_left, 0.0f) || !near(motor_right, 0.0f) ||
        !near(servo_position, 0.0f))
        return fail("typed STOP did not safe live outputs through the framework");
    return 0;
}

static int run_callback_overrun_scenario(void)
{
    uint16_t frame = 0U;
    unsigned index;

    if (enter_race(&frame) != 0) return 1;
    motors_set_duty(0.5f, -0.5f);
    steering_set(-0.5f);
    for (index = 0U; index < 3U; index++)
    {
        nxpc_framework__begin_callback();
        fake_cycle_counter +=
            (FRAME_BUDGET_MICROSECONDS + 1U) * (SystemCoreClock / 1000000U);
        nxpc_framework__end_callback();
    }
    if ((nxpc_system__mode() != NXPC_SYSTEM_MODE_SAFE_FAULT) ||
        (nxpc_system__fault() != NXPC_SYSTEM_FAULT_CALLBACK_OVERRUN))
        return fail("three callback overruns did not latch the typed safe fault");
    if (motor_enabled || !near(motor_left, 0.0f) || !near(motor_right, 0.0f) ||
        !near(servo_position, 0.0f))
        return fail("callback-overrun fault did not safe all outputs");
    return 0;
}

int main(int argc, char **argv)
{
    uint16_t dummy_frame = 0U;
    uint16_t newer_frame = 0U;
    uint16_t *taken_frame;
    unsigned frame_dumps_before;

    if ((argc == 2) && (strcmp(argv[1], "camera-loss") == 0))
        return run_camera_loss_scenario();
    if ((argc == 2) && (strcmp(argv[1], "callback-overrun") == 0))
        return run_callback_overrun_scenario();
    if ((argc == 2) && (strcmp(argv[1], "ambiguous-waiting") == 0))
        return run_ambiguous_waiting_scenario();
    if ((argc == 2) && (strcmp(argv[1], "ambiguous-running") == 0))
        return run_ambiguous_running_scenario();
    if ((argc == 2) && (strcmp(argv[1], "typed-stop") == 0))
        return run_typed_stop_scenario();

    nxpc_framework__init();
    if (nxpc_system__test_page() != NXPC_TEST_PAGE_CAMERA_IO)
        return fail("TEST did not start on CAMERA / IO");

    /* A frame that ages out after take must not be redrawn after the stale
     * transition clears the LCD. */
    nxpc__next_frame(&dummy_frame);
    nxpc_framework__service();
    taken_frame = nxpc_framework__take_latest_frame();
    if (taken_frame != &dummy_frame)
        return fail("fresh frame was not handed to the foreground");
    advance_ms(NXPC_SYSTEM_CAMERA_FRESH_MS + 1U);
    nxpc_framework__service();
    frame_dumps_before = camera_frame_dump_count;
    nxpc_framework__finish_frame(taken_frame);
    if (camera_frame_dump_count != frame_dumps_before)
        return fail("frame that aged out during processing was drawn");

    /* Likewise, consume an already-stale pending frame without returning or
     * publishing it. */
    nxpc__next_frame(&dummy_frame);
    nxpc_framework__service();
    advance_ms(NXPC_SYSTEM_CAMERA_FRESH_MS + 1U);
    nxpc_framework__service();
    if (nxpc_framework__take_latest_frame() != NULL)
        return fail("already-stale pending frame reached the foreground");

    /* A newer live capture must not make the older foreground-owned frame
     * publishable after that older frame ages out. */
    nxpc__next_frame(&dummy_frame);
    nxpc_framework__service();
    taken_frame = nxpc_framework__take_latest_frame();
    if (taken_frame != &dummy_frame)
        return fail("old frame was not taken before newer-capture interleaving");
    advance_ms(NXPC_SYSTEM_CAMERA_FRESH_MS + 1U);
    nxpc__next_frame(&newer_frame);
    nxpc_framework__service();
    frame_dumps_before = camera_frame_dump_count;
    nxpc_framework__finish_frame(taken_frame);
    if (camera_frame_dump_count != frame_dumps_before)
        return fail("newer capture made an aged foreground frame publishable");
    taken_frame = nxpc_framework__take_latest_frame();
    if (taken_frame != &newer_frame)
        return fail("newer capture was not retained after stale foreground rejection");
    nxpc_framework__finish_frame(taken_frame);
    if (camera_frame_dump_count != (frame_dumps_before + 1U))
        return fail("fresh newer capture was not drawn");

    buttons.button[BUTTON_ID_RIGHT].release_sequence = UINT32_MAX;
    nxpc_framework__service();
    if (nxpc_system__test_page() != NXPC_TEST_PAGE_CAMERA_IO)
        return fail("ambiguous pre-wrap navigation changed page");
    buttons.button[BUTTON_ID_RIGHT].release_sequence++;
    nxpc_framework__service();
    if (nxpc_system__test_page() != NXPC_TEST_PAGE_VISION)
        return fail("wrapped release epoch did not navigate exactly once");
    nxpc_framework__select_test_page(NXPC_TEST_PAGE_CAMERA_IO);

    release_button(BUTTON_ID_LEFT);
    if (nxpc_system__test_page() != NXPC_TEST_PAGE_ACTUATORS)
        return fail("left did not wrap CAMERA / IO to ACTUATORS");
    release_button(BUTTON_ID_RIGHT);
    if (nxpc_system__test_page() != NXPC_TEST_PAGE_CAMERA_IO)
        return fail("right did not wrap ACTUATORS to CAMERA / IO");

    release_button(BUTTON_ID_RIGHT);
    if (nxpc_system__test_page() != NXPC_TEST_PAGE_VISION)
        return fail("right did not navigate to VISION");

    buttons.button[BUTTON_ID_LEFT].release_sequence += 2U;
    nxpc_framework__service();
    if (nxpc_system__test_page() != NXPC_TEST_PAGE_VISION)
        return fail("ambiguous navigation epoch changed page");

    buttons.button[BUTTON_ID_LEFT].release_sequence++;
    buttons.button[BUTTON_ID_RIGHT].release_sequence++;
    nxpc_framework__service();
    if (nxpc_system__test_page() != NXPC_TEST_PAGE_VISION)
        return fail("simultaneous navigation was not ignored");

    buttons.button[BUTTON_ID_RIGHT].release_pending = true;
    nxpc_framework__service();
    nxpc_framework__select_test_page(NXPC_TEST_PAGE_CAMERA_IO);
    buttons.button[BUTTON_ID_RIGHT].release_sequence++;
    buttons.button[BUTTON_ID_RIGHT].release_pending = false;
    nxpc_framework__service();
    if (nxpc_system__test_page() != NXPC_TEST_PAGE_CAMERA_IO)
        return fail("inherited held release changed page");

    release_button(BUTTON_ID_RIGHT);
    release_button(BUTTON_ID_RIGHT);
    if (nxpc_system__test_page() != NXPC_TEST_PAGE_ACTUATORS)
        return fail("right did not wrap through VISION to ACTUATORS");

    alpha = 0.5f;
    beta = 0.5f;
    gamma = 0.5f;
    buttons.button[BUTTON_ID_CENTER].release_sequence = UINT32_MAX;
    nxpc_framework__service();
    if (nxpc_system__test_arm_pending())
        return fail("ambiguous pre-wrap EXE release opened arming");
    buttons.button[BUTTON_ID_CENTER].release_sequence++;
    nxpc_framework__service();
    if (!nxpc_system__test_arm_pending())
        return fail("wrapped EXE release did not open arming");
    complete_center_dwell();
    if (!nxpc_system__outputs_allowed() || !motor_enabled)
        return fail("ACTUATORS did not arm at centered controls");
    if (!near(motor_left, 0.0f) || !near(motor_right, 0.0f) || !near(servo_position, 0.0f))
        return fail("first armed command was not neutral");

    alpha = 1.0f;
    beta = 1.0f;
    gamma = 0.0f;
    advance_ms(20U);
    nxpc_framework__service();
    if (!near(motor_left, 0.25f) || !near(motor_right, -0.25f))
        return fail("TEST motor cap or pot mapping failed");
    if (!near(servo_position, 1.0f))
        return fail("TEST steering did not retain full range");

    advance_ms(100U);
    if (!near(motor_left, 0.0f) || !near(motor_right, 0.0f))
        return fail("lease ISR did not neutralize motors");
    nxpc_framework__service();
    if (nxpc_system__outputs_allowed() || motor_enabled || !near(servo_position, 0.0f))
        return fail("lease expiry did not disarm and center TEST");

    nxpc_framework__service();
    arm_actuators();
    if (!nxpc_system__outputs_allowed())
        return fail("fresh EXE did not re-arm after expiry");
    inject_expiry_on_irq_disable = true;
    advance_ms(20U);
    nxpc_framework__service();
    if (nxpc_system__outputs_allowed() || motor_enabled)
        return fail("transaction-boundary expiry renewed TEST without EXE");

    nxpc_framework__service();
    arm_actuators();
    alpha = 1.0f;
    advance_ms(20U);
    nxpc_framework__service();
    record_transition_order = true;
    buttons.button[BUTTON_ID_CENTER].release_sequence++;
    buttons.button[BUTTON_ID_RIGHT].release_sequence++;
    nxpc_framework__service();
    record_transition_order = false;
    if ((nxpc_system__test_page() != NXPC_TEST_PAGE_CAMERA_IO) || motor_enabled ||
        !near(servo_position, 0.0f))
        return fail("page exit did not safe-stop before wrapping");
    if ((page_seen_at_disable != NXPC_TEST_PAGE_ACTUATORS) ||
        (page_seen_at_center != NXPC_TEST_PAGE_ACTUATORS))
        return fail("safe outputs did not observe old page before mutation");

    release_button(BUTTON_ID_RIGHT);
    motors_set_duty(1.0f, 1.0f);
    steering_set(1.0f);
    if (motor_enabled || !near(servo_position, 0.0f))
        return fail("VISION participant commands escaped output gating");

    test_jumper = false;
    nxpc_framework__service();
    nxpc__next_frame(&dummy_frame);
    nxpc_framework__service();
    release_button(BUTTON_ID_CENTER);
    if (nxpc_system__mode() != NXPC_SYSTEM_MODE_RACE_RUNNING)
        return fail("RACE did not start after camera and EXE");
    if (!motor_enabled || !near(motor_left, 0.0f) || !near(motor_right, 0.0f))
        return fail("RACE entry did not enable zero-duty audible arming");
    motors_set_duty(1.0f, -1.0f);
    if (!near(motor_left, 1.0f) || !near(motor_right, -1.0f))
        return fail("TEST cap leaked into RACE");
    advance_ms(100U);
    if (!near(motor_left, 0.0f) || !near(motor_right, 0.0f))
        return fail("RACE lease ISR did not neutralize both motors");
    nxpc_framework__service();
    if (motor_enabled)
        return fail("RACE lease expiry did not disable motor control");
    motors_set_duty(-1.0f, 1.0f);
    if (!near(motor_left, -1.0f) || !near(motor_right, 1.0f))
        return fail("valid RACE command did not recover after lease expiry");

    steering_set(0.75f);
    release_button(BUTTON_ID_CENTER);
    if ((nxpc_system__mode() != NXPC_SYSTEM_MODE_RACE_WAITING) || motor_enabled ||
        !near(motor_left, 0.0f) || !near(motor_right, 0.0f) ||
        !near(servo_position, 0.0f))
        return fail("EXE stop did not return to waiting with safe outputs");

    buttons.button[BUTTON_ID_LEFT].release_pending = true;
    buttons.button[BUTTON_ID_CENTER].release_pending = true;
    test_jumper = true;
    nxpc_framework__service();
    nxpc_framework__service();
    buttons.button[BUTTON_ID_LEFT].release_sequence++;
    buttons.button[BUTTON_ID_CENTER].release_sequence++;
    buttons.button[BUTTON_ID_LEFT].release_pending = false;
    buttons.button[BUTTON_ID_CENTER].release_pending = false;
    nxpc_framework__service();
    if ((nxpc_system__mode() != NXPC_SYSTEM_MODE_TEST) ||
        (nxpc_system__test_page() != NXPC_TEST_PAGE_CAMERA_IO) ||
        nxpc_system__test_arm_pending() || nxpc_system__outputs_allowed())
        return fail("TEST entry inherited a held navigation or EXE release");

    return 0;
}
"""


def test_framework_transition_trace(tmp_path: Path):
    compiler = shutil.which("clang") or shutil.which("gcc") or shutil.which("cc")
    if compiler is None:
        pytest.skip("a native C compiler is required for the framework transition trace")

    stub = tmp_path / "nxpc_framework_trace_stubs.h"
    harness = tmp_path / "nxpc_framework_trace_harness.c"
    executable = tmp_path / (
        "nxpc_framework_trace_harness.exe"
        if Path(compiler).suffix.lower() == ".exe"
        else "nxpc_framework_trace_harness"
    )
    stub.write_text(STUB_HEADER, encoding="utf-8")
    harness.write_text(HARNESS, encoding="utf-8")

    compile_result = subprocess.run(
        [
            compiler,
            "-std=c99",
            "-Wall",
            "-Wextra",
            "-Werror",
            "-include",
            str(stub),
            str(harness),
            str(SOURCE / "nxpc_framework.c"),
            str(SOURCE / "nxpc_system.c"),
            f"-I{SOURCE}",
            f"-I{BUTTON_INCLUDE}",
            "-o",
            str(executable),
        ],
        cwd=REPO,
        capture_output=True,
        text=True,
        check=False,
    )
    assert compile_result.returncode == 0, compile_result.stdout + compile_result.stderr

    for scenario in (
        None,
        "camera-loss",
        "callback-overrun",
        "ambiguous-waiting",
        "ambiguous-running",
        "typed-stop",
    ):
        command = [str(executable)]
        if scenario is not None:
            command.append(scenario)
        run_result = subprocess.run(
            command,
            cwd=REPO,
            capture_output=True,
            text=True,
            check=False,
        )
        assert run_result.returncode == 0, (
            f"scenario={scenario or 'transition'}\n"
            + run_result.stdout
            + run_result.stderr
        )
