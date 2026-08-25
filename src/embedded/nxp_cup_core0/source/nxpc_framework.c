#include "nxpc_framework.h"

#include <stdarg.h>
#include <stdio.h>

#include "nxpc__io.h"
#include "nxpc_color.h"
#include "nxpc_graphics.h"
#include "nxp_cup.h"

#include "nxpc__motor_encoder_qdc.h"

#if CONFIG__USB_DEBUG_STREAM_ENABLE
#include "nxpc_usb_debug_stream.h"
#endif

#define NXPC_FRAME_BUDGET_US FRAME_BUDGET_MICROSECONDS
#define NXPC_CALLBACK_OVERRUN_LIMIT (3U)
#define NXPC_MOTOR_LEASE_MS (100U)
#define NXPC_MOTOR_LEASE_TICK_MS (10U)
#define NXPC_ENCODER_SAMPLE_MS (100U)
#define NXPC_ACTUATOR_TELEMETRY_MS (100U)
#define NXPC_TEST_MOTOR_DUTY_LIMIT (0.25f)
#define NXPC_TEST_IO_SAMPLE_MS (100U)
#define NXPC_TEST_ACTUATOR_CONTROL_MS (20U)

#if CONFIG__DISPLAY_ENABLE && CONFIG__DISPLAY_SCOPE_MARKER_ENABLE
#define NXPC_SCOPE_DUMP_BEGIN() GPIO_PinWrite(GPIO4, 1U, 1U)
#define NXPC_SCOPE_DUMP_END() GPIO_PinWrite(GPIO4, 1U, 0U)
#else
#define NXPC_SCOPE_DUMP_BEGIN() do { } while (0)
#define NXPC_SCOPE_DUMP_END() do { } while (0)
#endif

static uint16_t * volatile g_latest_frame;
static volatile bool g_frame_ready;
static volatile uint32_t g_frame_generation;
static volatile uint32_t g_capture_drop_count;
static uint32_t g_callback_overrun_count;
static volatile uint32_t g_last_frame_ms;
static volatile uint32_t g_last_frame_period_ms;
static uint32_t g_active_frame_capture_ms;
static bool g_active_frame_timestamp_valid;

static uint32_t g_callback_start_cycles;
static uint32_t g_callback_us;
static uint32_t g_consecutive_overruns;
static button_snapshot_t g_button_snapshot;

typedef struct
{
    uint32_t release_sequence;
    bool suppress_release;
} nxpc_button_release_consumer_t;

typedef enum
{
    NXPC_RELEASE_NONE = 0,
    NXPC_RELEASE_SINGLE,
    NXPC_RELEASE_AMBIGUOUS
} nxpc_release_result_t;

static nxpc_button_release_consumer_t g_left_navigation;
static nxpc_button_release_consumer_t g_right_navigation;

static void nxpc__baseline_test_navigation(void);
static nxpc_release_result_t nxpc__take_release(
    nxpc_button_release_consumer_t *consumer,
    const button_state_snapshot_t *button);

static volatile uint32_t g_motor_lease_remaining_ms;
static volatile bool g_motor_lease_expired;
static e_tick__trigger_handler_t g_motor_lease_trigger;
static nxpc_system_mode_t g_last_actuator_mode;

static uint16_t g_status_buffer[320U * 40U];
static const nxpc_rgb565_surface_t g_status_surface = {
    .pixels = g_status_buffer,
    .width = 320,
    .height = 40,
    .stride_pixels = 320,
};
static nxpc_system_mode_t g_last_rendered_mode;
static nxpc_system_state_t g_last_rendered_state;
static nxpc_test_page_t g_last_rendered_test_page;
static bool g_last_camera_seen;
static bool g_banner_initialized;

static nxpc_motor_encoder_sample_t g_encoder_samples[NXPC_MOTOR_ENCODER_COUNT];
static uint32_t g_encoder_sample_tick;
static uint32_t g_encoder_sample_age_tick;
static bool g_encoder_sample_valid;
static uint32_t g_actuator_telemetry_tick;

typedef struct
{
    uint8_t alpha_percent;
    uint8_t beta_percent;
    uint8_t gamma_percent;
    bool left_held;
    bool right_held;
    bool center_held;
    uint16_t battery_centivolts;
    uint32_t frame_period_ms;
    uint32_t callback_us;
    uint32_t callback_percent;
    uint32_t frame_drops;
} nxpc_test_io_snapshot_t;

static nxpc_test_io_snapshot_t g_test_io_snapshot;
static uint32_t g_test_io_sample_tick;

typedef struct
{
    uint8_t left_pot_percent;
    uint8_t steering_pot_percent;
    uint8_t right_pot_percent;
    int32_t left_command_percent;
    int32_t steering_command_percent;
    int32_t right_command_percent;
    int32_t left_rpm;
    int32_t right_rpm;
    bool wheel_speed_valid;
} nxpc_test_actuator_snapshot_t;

static nxpc_test_actuator_snapshot_t g_test_actuator_snapshot;
static uint32_t g_test_actuator_control_tick;

static float nxpc__clamp_unit(float value)
{
    if (value != value)
    {
        return 0.0f;
    }
    if (value > 1.0f)
    {
        return 1.0f;
    }
    if (value < -1.0f)
    {
        return -1.0f;
    }
    return value;
}

static void nxpc__motor_lease_tick(void *argument)
{
    (void)argument;

    if (g_motor_lease_remaining_ms == 0U)
    {
        return;
    }

    if (g_motor_lease_remaining_ms > NXPC_MOTOR_LEASE_TICK_MS)
    {
        g_motor_lease_remaining_ms -= NXPC_MOTOR_LEASE_TICK_MS;
        return;
    }

    g_motor_lease_remaining_ms = 0U;
    g_motor_lease_expired = true;
    nxpc__set_motor_pwm(0.0f, 0.0f);
}

static void nxpc__encoder_service(void)
{
    if (e_tick__timeout(&g_encoder_sample_tick, NXPC_ENCODER_SAMPLE_MS) != 0U)
    {
        nxpc__motor_encoder_qdc_sample(NXPC_ENCODER_SAMPLE_MS, g_encoder_samples);
        g_encoder_sample_age_tick = e_tick__get_ms();
        g_encoder_sample_valid = true;
    }
}

static void nxpc__clear_camera_display(void)
{
#if CONFIG__DISPLAY_ENABLE
    const uint32_t strip_height = 40U;

    /* Reuse the status buffer as a black strip; no second camera buffer is
     * needed merely to prevent an old frame from remaining on the LCD. */
    nxpc_graphics__fill_rectangle(&g_status_surface, 0, 0, 319, 39, 0U);
    for (uint32_t y = 40U; y < 240U; y += strip_height)
    {
        eGFX_DumpRaw((uint8_t *)g_status_buffer,
                     sizeof(g_status_buffer),
                     0U,
                     319U,
                     y,
                     y + strip_height - 1U);
    }
#endif
}

static void nxpc__apply_actuator_mode_transition(void)
{
    nxpc_system_mode_t mode = nxpc_system__mode();
    uint32_t interrupt_state;

    if (mode == g_last_actuator_mode)
    {
        return;
    }

    interrupt_state = DisableGlobalIRQ();
    g_motor_lease_remaining_ms = 0U;
    g_motor_lease_expired = false;
    EnableGlobalIRQ(interrupt_state);

    if (mode == NXPC_SYSTEM_MODE_RACE_WAITING)
    {
        nxpc__clear_camera_display();
    }

    if (mode == NXPC_SYSTEM_MODE_RACE_RUNNING)
    {
        /* Audible armed cue at zero duty; no command lease starts here. */
        nxpc__enable_motor_control();
    }
    else if (mode == NXPC_SYSTEM_MODE_TEST)
    {
        nxpc__baseline_test_navigation();
    }

    g_last_actuator_mode = mode;
}

static void nxpc__actuator_telemetry_service(void)
{
#if CONFIG__USB_DEBUG_STREAM_ENABLE
    if (e_tick__timeout(&g_actuator_telemetry_tick, NXPC_ACTUATOR_TELEMETRY_MS) == 0U)
    {
        return;
    }

    (void)nxpc_usb_debug_stream__framework_telemetry_bool(
        "motor.enabled",
        nxpc__motor_control_enabled());
    (void)nxpc_usb_debug_stream__framework_telemetry_f32(
        "motor.left.command",
        nxpc__motor_left_command(),
        "ratio");
    (void)nxpc_usb_debug_stream__framework_telemetry_f32(
        "motor.right.command",
        nxpc__motor_right_command(),
        "ratio");
    (void)nxpc_usb_debug_stream__framework_telemetry_f32(
        "steering.command",
        nxpc__servo_command(),
        "ratio");
    (void)nxpc_usb_debug_stream__framework_telemetry_text(
        "system.mode",
        nxpc_system__mode_label(nxpc_system__mode()));

    (void)nxpc_usb_debug_stream__framework_telemetry_text(
        "system.state",
        nxpc_system__state_label(nxpc_system__state()));
    (void)nxpc_usb_debug_stream__framework_telemetry_text(
        "system.test_page",
        (nxpc_system__mode() == NXPC_SYSTEM_MODE_TEST) ?
            nxpc_system__test_page_label(nxpc_system__test_page()) :
            "NOT ACTIVE");
#endif
}

static uint8_t nxpc__ratio_percent(float value)
{
    if (value != value || value <= 0.0f)
    {
        return 0U;
    }
    if (value >= 1.0f)
    {
        return 100U;
    }
    return (uint8_t)((value * 100.0f) + 0.5f);
}

static int32_t nxpc__signed_ratio_percent(float value)
{
    value = nxpc__clamp_unit(value);
    return (int32_t)(value * 100.0f);
}

static uint32_t nxpc__frame_budget_percent(uint32_t callback_us)
{
    if (callback_us >= (NXPC_FRAME_BUDGET_US * 10U))
    {
        return 999U;
    }
    return (callback_us + (NXPC_FRAME_BUDGET_US / 200U)) /
           (NXPC_FRAME_BUDGET_US / 100U);
}

static bool nxpc__camera_live(void)
{
    return nxpc_system__camera_live();
}

static uint32_t nxpc__frame_drop_count_snapshot(void)
{
    uint32_t capture_drops;
    uint32_t interrupt_state = DisableGlobalIRQ();

    capture_drops = g_capture_drop_count;
    EnableGlobalIRQ(interrupt_state);
    return capture_drops + g_callback_overrun_count;
}

static void nxpc__test_io_service(void)
{
    uint32_t now;

    if ((nxpc_system__mode() != NXPC_SYSTEM_MODE_TEST) ||
        (nxpc_system__test_page() != NXPC_TEST_PAGE_CAMERA_IO))
    {
        return;
    }

    now = e_tick__get_ms();
    if ((uint32_t)(now - g_test_io_sample_tick) < NXPC_TEST_IO_SAMPLE_MS)
    {
        return;
    }
    g_test_io_sample_tick = now;
    g_test_io_snapshot.alpha_percent = nxpc__ratio_percent(nxpc__read_alpha());
    g_test_io_snapshot.beta_percent = nxpc__ratio_percent(nxpc__read_beta());
    g_test_io_snapshot.gamma_percent = nxpc__ratio_percent(nxpc__read_gamma());
    g_test_io_snapshot.left_held = g_button_snapshot.button[BUTTON_ID_LEFT].held;
    g_test_io_snapshot.right_held = g_button_snapshot.button[BUTTON_ID_RIGHT].held;
    g_test_io_snapshot.center_held = g_button_snapshot.button[BUTTON_ID_CENTER].held;
    g_test_io_snapshot.battery_centivolts = nxpc__read_battery_voltage();
    g_test_io_snapshot.frame_period_ms = g_last_frame_period_ms;
    g_test_io_snapshot.callback_us = g_callback_us;
    g_test_io_snapshot.callback_percent = nxpc__frame_budget_percent(g_callback_us);
    g_test_io_snapshot.frame_drops = nxpc__frame_drop_count_snapshot();
}

static void nxpc__test_actuator_service(void)
{
    uint32_t now;
    float alpha;
    float beta;
    float gamma;

    if ((nxpc_system__mode() != NXPC_SYSTEM_MODE_TEST) ||
        (nxpc_system__test_page() != NXPC_TEST_PAGE_ACTUATORS))
    {
        return;
    }

    now = e_tick__get_ms();
    if ((uint32_t)(now - g_test_actuator_control_tick) <
        NXPC_TEST_ACTUATOR_CONTROL_MS)
    {
        return;
    }
    g_test_actuator_control_tick = now;

    alpha = nxpc__clamp_unit(nxpc__read_alpha());
    beta = nxpc__clamp_unit(nxpc__read_beta());
    gamma = nxpc__clamp_unit(nxpc__read_gamma());
    if (alpha < 0.0f)
    {
        alpha = 0.0f;
    }
    if (beta < 0.0f)
    {
        beta = 0.0f;
    }
    if (gamma < 0.0f)
    {
        gamma = 0.0f;
    }

    g_test_actuator_snapshot.left_pot_percent = nxpc__ratio_percent(alpha);
    g_test_actuator_snapshot.steering_pot_percent = nxpc__ratio_percent(beta);
    g_test_actuator_snapshot.right_pot_percent = nxpc__ratio_percent(gamma);

    if (nxpc_system__outputs_allowed())
    {
        motors_set_duty((alpha * 2.0f) - 1.0f,
                        (gamma * 2.0f) - 1.0f);
        steering_set((beta * 2.0f) - 1.0f);
    }

    g_test_actuator_snapshot.left_command_percent =
        nxpc__signed_ratio_percent(nxpc__motor_left_command());
    g_test_actuator_snapshot.steering_command_percent =
        nxpc__signed_ratio_percent(nxpc__servo_command());
    g_test_actuator_snapshot.right_command_percent =
        nxpc__signed_ratio_percent(nxpc__motor_right_command());
    g_test_actuator_snapshot.wheel_speed_valid = wheel_speed_available();
    if (g_test_actuator_snapshot.wheel_speed_valid)
    {
        g_test_actuator_snapshot.left_rpm =
            g_encoder_samples[NXPC_MOTOR_ENCODER_M0].rpm_milli / 1000;
        g_test_actuator_snapshot.right_rpm =
            g_encoder_samples[NXPC_MOTOR_ENCODER_M1].rpm_milli / 1000;
    }
    else
    {
        g_test_actuator_snapshot.left_rpm = 0;
        g_test_actuator_snapshot.right_rpm = 0;
    }
}

static nxpc_test_page_t nxpc__previous_test_page(nxpc_test_page_t page)
{
    switch (page)
    {
        case NXPC_TEST_PAGE_CAMERA_IO:
            return NXPC_TEST_PAGE_ACTUATORS;
        case NXPC_TEST_PAGE_VISION:
            return NXPC_TEST_PAGE_CAMERA_IO;
        case NXPC_TEST_PAGE_ACTUATORS:
        default:
            return NXPC_TEST_PAGE_VISION;
    }
}

static nxpc_test_page_t nxpc__next_test_page(nxpc_test_page_t page)
{
    switch (page)
    {
        case NXPC_TEST_PAGE_CAMERA_IO:
            return NXPC_TEST_PAGE_VISION;
        case NXPC_TEST_PAGE_VISION:
            return NXPC_TEST_PAGE_ACTUATORS;
        case NXPC_TEST_PAGE_ACTUATORS:
        default:
            return NXPC_TEST_PAGE_CAMERA_IO;
    }
}

static void nxpc__test_navigation_service(void)
{
    nxpc_release_result_t left_release;
    nxpc_release_result_t right_release;

    if (nxpc_system__mode() != NXPC_SYSTEM_MODE_TEST)
    {
        return;
    }

    left_release = nxpc__take_release(
        &g_left_navigation,
        &g_button_snapshot.button[BUTTON_ID_LEFT]);
    right_release = nxpc__take_release(
        &g_right_navigation,
        &g_button_snapshot.button[BUTTON_ID_RIGHT]);

    /* Consume but ignore simultaneous or ambiguous navigation input. */
    if ((left_release != NXPC_RELEASE_NONE) &&
        (right_release != NXPC_RELEASE_NONE))
    {
        return;
    }
    if (left_release == NXPC_RELEASE_SINGLE)
    {
        (void)nxpc_framework__select_test_page(
            nxpc__previous_test_page(nxpc_system__test_page()));
    }
    else if (right_release == NXPC_RELEASE_SINGLE)
    {
        (void)nxpc_framework__select_test_page(
            nxpc__next_test_page(nxpc_system__test_page()));
    }
}

static void nxpc__draw_status_text(const char *text,
                                   int32_t x,
                                   int32_t y,
                                   uint16_t color)
{
    nxpc_graphics__text(&g_status_surface, x, y, text, color);
}

static void nxpc__overlay_camera_io(uint16_t *frame)
{
    const uint16_t white = 0xFFFFU;
    const uint16_t cyan = 0x07FFU;
    const uint16_t yellow = 0xFFE0U;
    char text[64];

    frame_fill_rectangle(frame, 0, 0, CAMERA_WIDTH - 1, 27, 0U);
    (void)snprintf(text,
                   sizeof(text),
                   "POTS A:%03u B:%03u G:%03u",
                   (unsigned)g_test_io_snapshot.alpha_percent,
                   (unsigned)g_test_io_snapshot.beta_percent,
                   (unsigned)g_test_io_snapshot.gamma_percent);
    frame_draw_text(frame, 3, 2, text, cyan);

    (void)snprintf(text,
                   sizeof(text),
                   "BTN L:%u R:%u EXE:%u  BAT:%u.%02uV",
                   g_test_io_snapshot.left_held ? 1U : 0U,
                   g_test_io_snapshot.right_held ? 1U : 0U,
                   g_test_io_snapshot.center_held ? 1U : 0U,
                   (unsigned)g_test_io_snapshot.battery_centivolts / 100U,
                   (unsigned)g_test_io_snapshot.battery_centivolts % 100U);
    frame_draw_text(frame, 3, 11, text, white);

    (void)snprintf(text,
                   sizeof(text),
                   "FRAME:%lums WORK:%luus %lu%% DROP:%lu",
                   (unsigned long)g_test_io_snapshot.frame_period_ms,
                   (unsigned long)g_test_io_snapshot.callback_us,
                   (unsigned long)g_test_io_snapshot.callback_percent,
                   (unsigned long)g_test_io_snapshot.frame_drops);
    frame_draw_text(frame, 3, 20, text, yellow);
}

static void nxpc__overlay_test_actuators(uint16_t *frame)
{
    const uint16_t white = 0xFFFFU;
    const uint16_t green = 0x07E0U;
    const uint16_t cyan = 0x07FFU;
    const uint16_t yellow = 0xFFE0U;
    char text[64];

    frame_fill_rectangle(frame, 0, 0, CAMERA_WIDTH - 1, 36, 0U);
    (void)snprintf(text,
                   sizeof(text),
                   "POTS LEFT:%03u STEER:%03u RIGHT:%03u",
                   (unsigned)g_test_actuator_snapshot.left_pot_percent,
                   (unsigned)g_test_actuator_snapshot.steering_pot_percent,
                   (unsigned)g_test_actuator_snapshot.right_pot_percent);
    frame_draw_text(frame, 3, 2, text, cyan);

    (void)snprintf(text,
                   sizeof(text),
                   "CMD LEFT:%+ld%% STEER:%+ld%% RIGHT:%+ld%%",
                   (long)g_test_actuator_snapshot.left_command_percent,
                   (long)g_test_actuator_snapshot.steering_command_percent,
                   (long)g_test_actuator_snapshot.right_command_percent);
    frame_draw_text(frame, 3, 11, text, white);

    if (g_test_actuator_snapshot.wheel_speed_valid)
    {
        (void)snprintf(text,
                       sizeof(text),
                       "WHEEL RPM LEFT:%ld RIGHT:%ld",
                       (long)g_test_actuator_snapshot.left_rpm,
                       (long)g_test_actuator_snapshot.right_rpm);
    }
    else
    {
        (void)snprintf(text, sizeof(text), "WHEEL RPM: WAITING FOR SAMPLE");
    }
    frame_draw_text(frame, 3, 20, text, yellow);

    frame_draw_text(frame,
                    3,
                    29,
                    nxpc_system__outputs_allowed() ?
                        "OUTPUTS ARMED - EXE DISARMS" :
                        "OUTPUTS SAFE - EXE REQUESTS ARM",
                    nxpc_system__outputs_allowed() ? green : yellow);
}

static void nxpc__refresh_vision_timing_status(void)
{
    const uint16_t cyan = 0x07FFU;
    char text[48];

    nxpc_graphics__fill_rectangle(&g_status_surface, 0, 29, 319, 39, 0U);
    (void)snprintf(text,
                   sizeof(text),
                   "ALGO: %lu.%03lums %lu%% OF FRAME",
                   (unsigned long)(g_callback_us / 1000U),
                   (unsigned long)(g_callback_us % 1000U),
                   (unsigned long)nxpc__frame_budget_percent(g_callback_us));
    nxpc__draw_status_text(text, 5, 31, cyan);
}

static void nxpc__baseline_release_consumer(
    nxpc_button_release_consumer_t *consumer,
    const button_state_snapshot_t *button)
{
    consumer->release_sequence = button->release_sequence;
    consumer->suppress_release = button->release_pending;
}

static void nxpc__baseline_test_navigation(void)
{
    nxpc__baseline_release_consumer(
        &g_left_navigation,
        &g_button_snapshot.button[BUTTON_ID_LEFT]);
    nxpc__baseline_release_consumer(
        &g_right_navigation,
        &g_button_snapshot.button[BUTTON_ID_RIGHT]);
}

static nxpc_release_result_t nxpc__take_release(
    nxpc_button_release_consumer_t *consumer,
    const button_state_snapshot_t *button)
{
    uint32_t release_delta;

    if (consumer->suppress_release)
    {
        if (button->release_pending)
        {
            return NXPC_RELEASE_NONE;
        }

        consumer->release_sequence = button->release_sequence;
        consumer->suppress_release = false;
        return NXPC_RELEASE_NONE;
    }

    release_delta = button->release_sequence - consumer->release_sequence;
    if (release_delta == 0U)
    {
        return NXPC_RELEASE_NONE;
    }

    consumer->release_sequence = button->release_sequence;
    return (release_delta == 1U) ? NXPC_RELEASE_SINGLE : NXPC_RELEASE_AMBIGUOUS;
}

static void nxpc__render_banner_if_changed(void)
{
    nxpc_system_mode_t mode = nxpc_system__mode();
    nxpc_system_state_t state = nxpc_system__state();
    nxpc_test_page_t test_page = nxpc_system__test_page();
    bool camera_seen = nxpc__camera_live();
    const uint16_t green = 0x07E0U;
    const uint16_t yellow = 0xFFE0U;
    const uint16_t red = 0xF800U;
    char text[64];

    if (g_banner_initialized && (mode == g_last_rendered_mode) &&
        (state == g_last_rendered_state) && (camera_seen == g_last_camera_seen))
    {
        if ((mode != NXPC_SYSTEM_MODE_TEST) ||
            (test_page == g_last_rendered_test_page))
        {
            return;
        }
    }

    if (g_banner_initialized && g_last_camera_seen && !camera_seen)
    {
        nxpc__clear_camera_display();
    }

    nxpc_graphics__fill_rectangle(&g_status_surface, 0, 0, 319, 39, 0U);

    switch (mode)
    {
        case NXPC_SYSTEM_MODE_TEST:
            (void)snprintf(text,
                           sizeof(text),
                           "TEST < %s > %u/3",
                           nxpc_system__test_page_label(test_page),
                           (unsigned)test_page + 1U);
            nxpc__draw_status_text(text, 5, 1, green);
            if (test_page == NXPC_TEST_PAGE_ACTUATORS)
            {
                const char *action = "EXE: ARM";

                if (state == NXPC_SYSTEM_STATE_TEST_ARMED)
                {
                    action = "EXE: DISARM";
                }
                else if (state == NXPC_SYSTEM_STATE_TEST_CENTER_POTS)
                {
                    action = "CENTER POTS";
                }
                (void)snprintf(text, sizeof(text), "LEFT/RIGHT: PAGE  %s", action);
                nxpc__draw_status_text(
                    text,
                    5,
                    22,
                    (state == NXPC_SYSTEM_STATE_TEST_ARMED) ? red : yellow);
            }
            else if (test_page == NXPC_TEST_PAGE_CAMERA_IO)
            {
                (void)snprintf(text,
                               sizeof(text),
                               "LEFT/RIGHT: PAGE  CAM: %s",
                               camera_seen ? "OK" : "WAIT");
                nxpc__draw_status_text(text, 5, 22, camera_seen ? green : yellow);
            }
            else
            {
                (void)snprintf(text,
                               sizeof(text),
                               "LEFT/RIGHT: PAGE  CAM: %s",
                               camera_seen ? "OK" : "WAIT");
                nxpc__draw_status_text(text, 5, 22, camera_seen ? green : yellow);
            }
            break;

        case NXPC_SYSTEM_MODE_RACE_WAITING:
            nxpc__draw_status_text("RACE MODE", 5, 1, yellow);
            nxpc__draw_status_text(nxpc_system__state_label(state),
                                   5,
                                   22,
                                   (state == NXPC_SYSTEM_STATE_RACE_READY) ? green : yellow);
            break;

        case NXPC_SYSTEM_MODE_RACE_RUNNING:
            nxpc__draw_status_text("RACE RUNNING", 5, 1, green);
            break;

        case NXPC_SYSTEM_MODE_SAFE_FAULT:
            nxpc__draw_status_text("SAFE FAULT", 5, 1, red);
            nxpc__draw_status_text(nxpc_system__state_label(state), 5, 22, red);
            break;

        case NXPC_SYSTEM_MODE_ENTERING_ISP:
            nxpc__draw_status_text("ENTERING USB ISP", 5, 1, yellow);
            break;

        case NXPC_SYSTEM_MODE_STARTUP:
        default:
            nxpc__draw_status_text("STARTUP", 5, 1, yellow);
            break;
    }

    if (mode == NXPC_SYSTEM_MODE_RACE_RUNNING)
    {
        (void)snprintf(text, sizeof(text), "CAMERA: %s", camera_seen ? "FRAME OK" : "WAITING");
        nxpc__draw_status_text(text,
                               5,
                               22,
                               camera_seen ? green : yellow);
    }

#if CONFIG__DISPLAY_ENABLE
    eGFX_DumpRaw((uint8_t *)g_status_buffer, sizeof(g_status_buffer), 0U, 319U, 0U, 39U);
#endif

    g_last_rendered_mode = mode;
    g_last_rendered_state = state;
    g_last_rendered_test_page = test_page;
    g_last_camera_seen = camera_seen;
    g_banner_initialized = true;
}

void nxpc__next_frame(uint16_t *buffer)
{
    uint32_t now = e_tick__get_ms();

    if (g_frame_ready)
    {
        g_capture_drop_count++;
    }

    if (g_frame_generation != 0U)
    {
        g_last_frame_period_ms = now - g_last_frame_ms;
    }

    g_latest_frame = buffer;
    g_frame_ready = true;
    g_frame_generation++;
    g_last_frame_ms = now;
    nxpc_system__notify_camera_frame();
#if CONFIG__USB_DEBUG_STREAM_ENABLE
    nxpc_usb_debug_stream__notify_camera_frame();
#endif
}

void nxpc_framework__init(void)
{
    nxpc_system__init();
    button__snapshot(&g_button_snapshot);
    nxpc__baseline_test_navigation();
    g_last_actuator_mode = nxpc_system__mode();

    /* Start with a known lower display in both TEST and RACE modes. */
    nxpc__clear_camera_display();

    g_motor_lease_trigger.interval__mS = NXPC_MOTOR_LEASE_TICK_MS;
    g_motor_lease_trigger.duration__mS = NXPC_MOTOR_LEASE_TICK_MS;
    g_motor_lease_trigger.count = E_TICK__TRIGGER_FOREVER;
    g_motor_lease_trigger.hook = nxpc__motor_lease_tick;
    g_motor_lease_trigger.arg = NULL;
    g_motor_lease_trigger.marked_for_exe = false;
    g_motor_lease_trigger.next_item = NULL;
    e_tick__register_trigger(&g_motor_lease_trigger, true);

    g_encoder_sample_tick = e_tick__get_ms();
    g_encoder_sample_age_tick = g_encoder_sample_tick;
    g_actuator_telemetry_tick = g_encoder_sample_tick;
    g_test_io_sample_tick = g_encoder_sample_tick;
    g_test_actuator_control_tick = g_encoder_sample_tick;

    nxpc__render_banner_if_changed();
}

void nxpc_framework__service(void)
{
    button__snapshot(&g_button_snapshot);
    nxpc_camera__service();
    nxpc__encoder_service();

#if CONFIG__USB_DEBUG_STREAM_ENABLE
    nxpc_usb_debug_stream__service();
#endif

    nxpc_system__service(&g_button_snapshot);
    nxpc__apply_actuator_mode_transition();
    nxpc__test_navigation_service();
    nxpc__test_io_service();

    if ((nxpc_system__mode() == NXPC_SYSTEM_MODE_RACE_RUNNING) &&
        !nxpc_system__camera_live())
    {
        nxpc_system__enter_fault(NXPC_SYSTEM_FAULT_CAMERA_LOST);
    }

    if (g_motor_lease_expired)
    {
        g_motor_lease_expired = false;
        nxpc__disable_motor_control();
        nxpc_system__notify_test_motor_lease_expired();
        DEBUG("Motor command lease expired; outputs disabled.\r\n");
    }

    if (!nxpc_system__outputs_allowed())
    {
        g_motor_lease_remaining_ms = 0U;
    }

    nxpc__test_actuator_service();
    nxpc__actuator_telemetry_service();

    nxpc__render_banner_if_changed();
}

bool nxpc_framework__select_test_page(nxpc_test_page_t page)
{
    uint32_t interrupt_state;
    bool selected;

    if ((nxpc_system__mode() != NXPC_SYSTEM_MODE_TEST) ||
        ((uint32_t)page >= (uint32_t)NXPC_TEST_PAGE_COUNT))
    {
        return false;
    }
    if (page == nxpc_system__test_page())
    {
        return true;
    }

    /* Complete the safe stop before the typed page becomes observable. */
    interrupt_state = DisableGlobalIRQ();
    g_motor_lease_remaining_ms = 0U;
    g_motor_lease_expired = false;
    EnableGlobalIRQ(interrupt_state);
    nxpc__disable_motor_control();
    nxpc__set_servo(0.0f);

    selected = nxpc_system__select_test_page(page, &g_button_snapshot);
    if (selected)
    {
        nxpc__baseline_test_navigation();
    }
    return selected;
}

uint16_t *nxpc_framework__take_latest_frame(void)
{
    uint16_t *frame = NULL;
    uint32_t capture_ms = 0U;
    uint32_t now;
    uint32_t interrupt_state;
    nxpc_system_mode_t mode = nxpc_system__mode();

    if ((mode != NXPC_SYSTEM_MODE_TEST) && (mode != NXPC_SYSTEM_MODE_RACE_RUNNING) &&
        (mode != NXPC_SYSTEM_MODE_RACE_WAITING))
    {
        return NULL;
    }

#if CONFIG__USB_DEBUG_STREAM_ENABLE
    if ((mode == NXPC_SYSTEM_MODE_RACE_WAITING) &&
        !nxpc_usb_debug_stream__camera_frames_active())
    {
        return NULL;
    }
#else
    if (mode == NXPC_SYSTEM_MODE_RACE_WAITING)
    {
        return NULL;
    }
#endif

    interrupt_state = DisableGlobalIRQ();
    if (g_frame_ready)
    {
        frame = g_latest_frame;
        capture_ms = g_last_frame_ms;
        g_frame_ready = false;
    }
    EnableGlobalIRQ(interrupt_state);
    now = e_tick__get_ms();

    if ((frame != NULL) &&
        ((uint32_t)(now - capture_ms) <= NXPC_SYSTEM_CAMERA_FRESH_MS))
    {
        g_active_frame_capture_ms = capture_ms;
        g_active_frame_timestamp_valid = true;
        nxpc_camera__prepare_frame(frame);
        return frame;
    }

    g_active_frame_timestamp_valid = false;
    return NULL;
}

void nxpc_framework__begin_callback(void)
{
    g_callback_start_cycles = CYCLE_COUNTER;
}

void nxpc_framework__end_callback(void)
{
    uint32_t elapsed_cycles = CYCLE_COUNTER - g_callback_start_cycles;
    uint32_t cycles_per_us = SystemCoreClock / 1000000U;

    g_callback_us = (cycles_per_us == 0U) ? 0U : (elapsed_cycles / cycles_per_us);
    if (g_callback_us > NXPC_FRAME_BUDGET_US)
    {
        g_consecutive_overruns++;
        g_callback_overrun_count++;
        if (g_consecutive_overruns >= NXPC_CALLBACK_OVERRUN_LIMIT)
        {
            motors_stop();
            steering_set(0.0f);
            nxpc_system__enter_fault(NXPC_SYSTEM_FAULT_CALLBACK_OVERRUN);
        }
    }
    else
    {
        g_consecutive_overruns = 0U;
    }
}

void nxpc_framework__no_callback(void)
{
    g_callback_us = 0U;
    g_consecutive_overruns = 0U;
}

void nxpc_framework__finish_frame(uint16_t *frame)
{
    bool frame_timestamp_valid = g_active_frame_timestamp_valid;
    uint32_t capture_ms = g_active_frame_capture_ms;

    g_active_frame_timestamp_valid = false;
    if (frame == NULL)
    {
        return;
    }

    /* Never publish a frame that aged out while participant code was running.
     * A newer capture may already be live; it will be handled next iteration. */
    if (!frame_timestamp_valid ||
        ((uint32_t)(e_tick__get_ms() - capture_ms) > NXPC_SYSTEM_CAMERA_FRESH_MS))
    {
        if (!nxpc_system__camera_live())
        {
            nxpc__render_banner_if_changed();
        }
        return;
    }

    if ((nxpc_system__mode() == NXPC_SYSTEM_MODE_TEST) &&
        (nxpc_system__test_page() == NXPC_TEST_PAGE_CAMERA_IO))
    {
        nxpc__overlay_camera_io(frame);
    }
    else if ((nxpc_system__mode() == NXPC_SYSTEM_MODE_TEST) &&
             (nxpc_system__test_page() == NXPC_TEST_PAGE_ACTUATORS))
    {
        nxpc__overlay_test_actuators(frame);
    }
    else if ((nxpc_system__mode() == NXPC_SYSTEM_MODE_TEST) &&
             (nxpc_system__test_page() == NXPC_TEST_PAGE_VISION))
    {
        nxpc__refresh_vision_timing_status();
    }

#if CONFIG__USB_DEBUG_STREAM_ENABLE
    (void)nxpc_usb_debug_stream__publish_frame(frame);
#endif

#if CONFIG__DISPLAY_ENABLE
    if (nxpc_system__mode() == NXPC_SYSTEM_MODE_TEST)
    {
        NXPC_SCOPE_DUMP_BEGIN();
        eGFX_DumpRaw((uint8_t *)g_status_buffer, sizeof(g_status_buffer), 0U, 319U, 0U, 39U);
        eGFX_DumpRaw((uint8_t *)frame,
                     CAMERA_WIDTH * CAMERA_HEIGHT * 2U,
                     0U,
                     CAMERA_WIDTH - 1U,
                     40U,
                     39U + CAMERA_HEIGHT);
        NXPC_SCOPE_DUMP_END();
    }
#endif
}

color_features_t color_rgb565_to_yhsv(uint16_t pixel)
{
    nxpc_color_features_t raw = nxpc_color__rgb565_to_yhsv(pixel);
    color_features_t result = {.y = raw.y, .h = raw.h, .s = raw.s, .v = raw.v};
    return result;
}

void color_convert_rgb565_to_yhsv(const uint16_t *pixels,
                                  color_features_t *features,
                                  uint32_t count)
{
    for (uint32_t i = 0U; i < count; i++)
    {
        features[i] = color_rgb565_to_yhsv(pixels[i]);
    }
}

uint16_t color_rgb565(uint8_t red, uint8_t green, uint8_t blue)
{
    return (uint16_t)(((uint16_t)(red & 0xF8U) << 8U) |
                      ((uint16_t)(green & 0xFCU) << 3U) |
                      ((uint16_t)(blue & 0xF8U) >> 3U));
}

float input_alpha(void) { return nxpc__read_alpha(); }
float input_beta(void) { return nxpc__read_beta(); }
float input_gamma(void) { return nxpc__read_gamma(); }
bool input_left_button(void) { return g_button_snapshot.button[BUTTON_ID_LEFT].held; }
bool input_right_button(void) { return g_button_snapshot.button[BUTTON_ID_RIGHT].held; }
float battery_voltage(void) { return (float)nxpc__read_battery_voltage() * 0.01f; }

void motors_set_duty(float left, float right)
{
    uint32_t interrupt_state;
    bool reject_expired_test_lease = false;
    nxpc_system_mode_t mode;

    if (!nxpc_system__outputs_allowed())
    {
        nxpc__disable_motor_control();
        return;
    }

    mode = nxpc_system__mode();
    left = nxpc__clamp_unit(left);
    right = nxpc__clamp_unit(right);
    if (mode == NXPC_SYSTEM_MODE_TEST)
    {
        if (left > NXPC_TEST_MOTOR_DUTY_LIMIT)
        {
            left = NXPC_TEST_MOTOR_DUTY_LIMIT;
        }
        else if (left < -NXPC_TEST_MOTOR_DUTY_LIMIT)
        {
            left = -NXPC_TEST_MOTOR_DUTY_LIMIT;
        }
        if (right > NXPC_TEST_MOTOR_DUTY_LIMIT)
        {
            right = NXPC_TEST_MOTOR_DUTY_LIMIT;
        }
        else if (right < -NXPC_TEST_MOTOR_DUTY_LIMIT)
        {
            right = -NXPC_TEST_MOTOR_DUTY_LIMIT;
        }
    }

    /*
     * Keep TEST expiry sticky across the complete PWM/lease transaction. The
     * 10 ms ISR cannot expire between the gate and renewal or split the two
     * motor updates. RACE deliberately retains command-based recovery.
     */
    interrupt_state = DisableGlobalIRQ();
    if ((mode == NXPC_SYSTEM_MODE_TEST) && g_motor_lease_expired)
    {
        reject_expired_test_lease = true;
        g_motor_lease_remaining_ms = 0U;
    }
    else
    {
        if (mode == NXPC_SYSTEM_MODE_RACE_RUNNING)
        {
            g_motor_lease_expired = false;
        }
        nxpc__enable_motor_control();
        nxpc__set_motor_pwm(left, right);
        g_motor_lease_remaining_ms = NXPC_MOTOR_LEASE_MS;
    }
    EnableGlobalIRQ(interrupt_state);

    if (reject_expired_test_lease)
    {
        nxpc_system__notify_test_motor_lease_expired();
    }
}

void motors_stop(void)
{
    g_motor_lease_remaining_ms = 0U;
    nxpc__disable_motor_control();
}

void steering_set(float position)
{
    nxpc__set_servo(nxpc_system__outputs_allowed() ? nxpc__clamp_unit(position) : 0.0f);
}

float wheel_speed_rpm(wheel_t wheel)
{
    uint32_t index = (wheel == WHEEL_RIGHT) ? NXPC_MOTOR_ENCODER_M1 : NXPC_MOTOR_ENCODER_M0;
    if (!wheel_speed_available())
    {
        return 0.0f;
    }
    return (float)g_encoder_samples[index].rpm_milli / 1000.0f;
}

float wheel_speed_mps(wheel_t wheel)
{
    const float circumference_m =
        3.14159265f * ((float)CONFIG__MOTOR_ENCODER_WHEEL_DIAMETER_MM / 1000.0f);
    return (wheel_speed_rpm(wheel) / 60.0f) * circumference_m;
}

uint32_t wheel_speed_age_ms(void)
{
    return e_tick__delta(&g_encoder_sample_age_tick);
}

bool wheel_speed_available(void)
{
    return g_encoder_sample_valid &&
           (wheel_speed_age_ms() <= (NXPC_ENCODER_SAMPLE_MS * 3U));
}

uint32_t time_milliseconds(void) { return e_tick__get_ms(); }
uint32_t frame_processing_microseconds(void) { return g_callback_us; }
uint32_t frame_drop_count(void) { return nxpc__frame_drop_count_snapshot(); }

bool telemetry_i32(const char *name, int32_t value, const char *units)
{
#if CONFIG__USB_DEBUG_STREAM_ENABLE
    return nxpc_usb_debug_stream__telemetry_i32(name, value, units);
#else
    (void)name; (void)value; (void)units; return false;
#endif
}

bool telemetry_u32(const char *name, uint32_t value, const char *units)
{
#if CONFIG__USB_DEBUG_STREAM_ENABLE
    return nxpc_usb_debug_stream__telemetry_u32(name, value, units);
#else
    (void)name; (void)value; (void)units; return false;
#endif
}

bool telemetry_f32(const char *name, float value, const char *units)
{
#if CONFIG__USB_DEBUG_STREAM_ENABLE
    return nxpc_usb_debug_stream__telemetry_f32(name, value, units);
#else
    (void)name; (void)value; (void)units; return false;
#endif
}

bool telemetry_bool(const char *name, bool value)
{
#if CONFIG__USB_DEBUG_STREAM_ENABLE
    return nxpc_usb_debug_stream__telemetry_bool(name, value);
#else
    (void)name; (void)value; return false;
#endif
}

bool telemetry_text(const char *name, const char *value)
{
#if CONFIG__USB_DEBUG_STREAM_ENABLE
    return nxpc_usb_debug_stream__telemetry_text(name, value);
#else
    (void)name; (void)value; return false;
#endif
}

bool telemetry_log(const char *category, const char *format, ...)
{
#if CONFIG__USB_DEBUG_STREAM_ENABLE
    char text[128];
    va_list arguments;
    va_start(arguments, format);
    (void)vsnprintf(text, sizeof(text), format, arguments);
    va_end(arguments);
    return nxpc_usb_debug_stream__log_text(NXPC_DBG_LOG_LEVEL_INFO, category, text);
#else
    (void)category; (void)format; return false;
#endif
}
