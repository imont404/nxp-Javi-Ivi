#include "nxpc_framework.h"

#include <stdarg.h>
#include <stdio.h>

#include "nxpc__io.h"
#include "nxpc__line_processor.h"
#include "nxp_cup.h"

#include "nxpc__motor_encoder_qdc.h"

#if CONFIG__USB_DEBUG_STREAM_ENABLE
#include "nxpc_usb_debug_stream.h"
#endif

#define NXPC_FRAME_BUDGET_US (41000U)
#define NXPC_CALLBACK_OVERRUN_LIMIT (3U)
#define NXPC_CAMERA_TIMEOUT_MS (250U)
#define NXPC_MOTOR_LEASE_MS (100U)
#define NXPC_MOTOR_LEASE_TICK_MS (10U)
#define NXPC_ENCODER_SAMPLE_MS (100U)
#define NXPC_ACTUATOR_TELEMETRY_MS (100U)

#if CONFIG__DISPLAY_ENABLE && CONFIG__DISPLAY_SCOPE_MARKER_ENABLE
#define NXPC_SCOPE_DUMP_BEGIN() GPIO_PinWrite(GPIO4, 1U, 1U)
#define NXPC_SCOPE_DUMP_END() GPIO_PinWrite(GPIO4, 1U, 0U)
#else
#define NXPC_SCOPE_DUMP_BEGIN() do { } while (0)
#define NXPC_SCOPE_DUMP_END() do { } while (0)
#endif

static volatile uint16_t *g_latest_frame;
static volatile bool g_frame_ready;
static volatile uint32_t g_frame_generation;
static volatile uint32_t g_frame_drop_count;
static volatile uint32_t g_last_frame_ms;

static uint32_t g_callback_start_cycles;
static uint32_t g_callback_us;
static uint32_t g_consecutive_overruns;

static volatile uint32_t g_motor_lease_remaining_ms;
static volatile bool g_motor_lease_expired;
static e_tick__trigger_handler_t g_motor_lease_trigger;
static nxpc_system_mode_t g_last_actuator_mode;

static eGFX_ImagePlane g_status_plane;
static eGFX_ImagePlane g_camera_plane;
static uint8_t g_status_buffer[eGFX_CALCULATE_16BPP_IMAGE_STORAGE_SPACE_SIZE(320, 40)];
static nxpc_system_mode_t g_last_rendered_mode;
static nxpc_system_state_t g_last_rendered_state;
static bool g_last_camera_seen;
static bool g_banner_initialized;

static nxpc_motor_encoder_sample_t g_encoder_samples[NXPC_MOTOR_ENCODER_COUNT];
static uint32_t g_encoder_sample_tick;
static uint32_t g_encoder_sample_age_tick;
static bool g_encoder_sample_valid;
static uint32_t g_actuator_telemetry_tick;

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

static void nxpc__clear_race_display(void)
{
#if CONFIG__DISPLAY_ENABLE
    const uint32_t strip_height = 40U;

    /* Reuse the status buffer as a black strip so race entry does not need
     * another full camera-sized buffer. The banner is redrawn afterward. */
    eGFX_ImagePlane_Clear(&g_status_plane);
    for (uint32_t y = 40U; y < 240U; y += strip_height)
    {
        eGFX_DumpRaw((uint8_t *)g_status_plane.Data,
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
        nxpc__clear_race_display();
    }

    if (mode == NXPC_SYSTEM_MODE_RACE_RUNNING)
    {
        /* Audible armed cue at zero duty; no command lease starts here. */
        nxpc__enable_motor_control();
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
#endif
}

static void nxpc__draw_status_text(const char *text,
                                   int32_t x,
                                   int32_t y,
                                   uint32_t color,
                                   const eGFX_Font *font)
{
    eGFX_DrawStringColored(&g_status_plane, (char *)text, x, y, font, color);
}

static void nxpc__render_banner_if_changed(void)
{
    nxpc_system_mode_t mode = nxpc_system__mode();
    nxpc_system_state_t state = nxpc_system__state();
    bool camera_seen = nxpc_system__camera_frame_seen();
    uint32_t green = eGFX_COLOR_RGB888_TO_RGB565(0U, 255U, 0U);
    uint32_t yellow = eGFX_COLOR_RGB888_TO_RGB565(255U, 255U, 0U);
    uint32_t red = eGFX_COLOR_RGB888_TO_RGB565(255U, 0U, 0U);
    char text[64];

    if (g_banner_initialized && (mode == g_last_rendered_mode) &&
        (state == g_last_rendered_state) && (camera_seen == g_last_camera_seen))
    {
        return;
    }

    eGFX_ImagePlane_Clear(&g_status_plane);

    switch (mode)
    {
        case NXPC_SYSTEM_MODE_TEST:
            nxpc__draw_status_text("TEST MODE", 5, 1, green, &FONT_10_14_1BPP);
            nxpc__draw_status_text(nxpc_system__state_label(state),
                                   145,
                                   4,
                                   (state == NXPC_SYSTEM_STATE_TEST_ARMED) ? red : yellow,
                                   &FONT_5_7_1BPP);
            break;

        case NXPC_SYSTEM_MODE_RACE_WAITING:
            nxpc__draw_status_text("RACE MODE", 5, 1, yellow, &FONT_10_14_1BPP);
            nxpc__draw_status_text(nxpc_system__state_label(state),
                                   5,
                                   22,
                                   (state == NXPC_SYSTEM_STATE_RACE_READY) ? green : yellow,
                                   &FONT_5_7_1BPP);
            break;

        case NXPC_SYSTEM_MODE_RACE_RUNNING:
            nxpc__draw_status_text("RACE RUNNING", 5, 1, green, &FONT_10_14_1BPP);
            break;

        case NXPC_SYSTEM_MODE_SAFE_FAULT:
            nxpc__draw_status_text("SAFE FAULT", 5, 1, red, &FONT_10_14_1BPP);
            nxpc__draw_status_text(nxpc_system__state_label(state), 5, 22, red, &FONT_5_7_1BPP);
            break;

        case NXPC_SYSTEM_MODE_ENTERING_ISP:
            nxpc__draw_status_text("ENTERING USB ISP", 5, 1, yellow, &FONT_10_14_1BPP);
            break;

        case NXPC_SYSTEM_MODE_STARTUP:
        default:
            nxpc__draw_status_text("STARTUP", 5, 1, yellow, &FONT_10_14_1BPP);
            break;
    }

    if ((mode == NXPC_SYSTEM_MODE_TEST) || (mode == NXPC_SYSTEM_MODE_RACE_RUNNING))
    {
        (void)snprintf(text, sizeof(text), "CAMERA: %s", camera_seen ? "FRAME OK" : "WAITING");
        nxpc__draw_status_text(text,
                               5,
                               22,
                               camera_seen ? green : yellow,
                               &FONT_5_7_1BPP);
    }

#if CONFIG__DISPLAY_ENABLE
    eGFX_DumpRaw((uint8_t *)g_status_plane.Data, 320U * 40U * 2U, 0U, 319U, 0U, 39U);
#endif

    g_last_rendered_mode = mode;
    g_last_rendered_state = state;
    g_last_camera_seen = camera_seen;
    g_banner_initialized = true;
}

void nxpc__next_frame(uint16_t *buffer)
{
    if (g_frame_ready)
    {
        g_frame_drop_count++;
    }

    g_latest_frame = buffer;
    g_frame_ready = true;
    g_frame_generation++;
    g_last_frame_ms = e_tick__get_ms();
    nxpc_system__notify_camera_frame();
#if CONFIG__USB_DEBUG_STREAM_ENABLE
    nxpc_usb_debug_stream__notify_camera_frame();
#endif
}

void nxpc_framework__init(void)
{
    nxpc_system__init();
    g_last_actuator_mode = nxpc_system__mode();

    g_status_plane.Data = g_status_buffer;
    g_status_plane.SizeX = 320U;
    g_status_plane.SizeY = 40U;
    g_status_plane.Type = eGFX_IMAGE_PLANE_16BPP_RGB565;

    g_camera_plane.SizeX = CAMERA_WIDTH;
    g_camera_plane.SizeY = CAMERA_HEIGHT;
    g_camera_plane.Type = eGFX_IMAGE_PLANE_16BPP_RGB565;

    if (nxpc_system__mode() == NXPC_SYSTEM_MODE_RACE_WAITING)
    {
        nxpc__clear_race_display();
    }

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

    nxpc__render_banner_if_changed();
}

void nxpc_framework__service(void)
{
    nxpc_camera__service();
    nxpc__encoder_service();

#if CONFIG__USB_DEBUG_STREAM_ENABLE
    nxpc_usb_debug_stream__service();
#endif

    nxpc_system__service();
    nxpc__apply_actuator_mode_transition();

    if ((nxpc_system__mode() == NXPC_SYSTEM_MODE_RACE_RUNNING) &&
        ((e_tick__get_ms() - g_last_frame_ms) > NXPC_CAMERA_TIMEOUT_MS))
    {
        nxpc_system__enter_fault(NXPC_SYSTEM_FAULT_CAMERA_LOST);
    }

    if (g_motor_lease_expired)
    {
        g_motor_lease_expired = false;
        nxpc__disable_motor_control();
        DEBUG("Motor command lease expired; outputs disabled.\r\n");
    }

    if (!nxpc_system__outputs_allowed())
    {
        g_motor_lease_remaining_ms = 0U;
    }


    nxpc__actuator_telemetry_service();

    nxpc__render_banner_if_changed();
}

uint16_t *nxpc_framework__take_latest_frame(void)
{
    uint16_t *frame = NULL;
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
        frame = (uint16_t *)g_latest_frame;
        g_frame_ready = false;
    }
    EnableGlobalIRQ(interrupt_state);

    if (frame != NULL)
    {
        nxpc_camera__prepare_frame(frame);
    }
    return frame;
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
        g_frame_drop_count++;
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

void nxpc_framework__finish_frame(uint16_t *frame)
{
    if (frame == NULL)
    {
        return;
    }

#if CONFIG__USB_DEBUG_STREAM_ENABLE
    (void)nxpc_usb_debug_stream__publish_frame(frame);
#endif

#if CONFIG__DISPLAY_ENABLE
    if (nxpc_system__mode() == NXPC_SYSTEM_MODE_TEST)
    {
        g_camera_plane.Data = (uint8_t *)frame;
        NXPC_SCOPE_DUMP_BEGIN();
        eGFX_DumpRaw((uint8_t *)g_status_plane.Data, 320U * 40U * 2U, 0U, 319U, 0U, 39U);
        eGFX_DumpRaw((uint8_t *)g_camera_plane.Data,
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
    nxpc_color_features_t raw = nxpc__rgb565_to_yhsv(pixel);
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

void frame_draw_vertical_line(uint16_t *frame,
                              uint32_t x,
                              uint32_t y0,
                              uint32_t y1,
                              uint16_t color)
{
    if ((frame == NULL) || (x >= CAMERA_WIDTH))
    {
        return;
    }
    if (y0 >= CAMERA_HEIGHT)
    {
        y0 = CAMERA_HEIGHT - 1U;
    }
    if (y1 >= CAMERA_HEIGHT)
    {
        y1 = CAMERA_HEIGHT - 1U;
    }
    if (y1 < y0)
    {
        uint32_t swap = y0;
        y0 = y1;
        y1 = swap;
    }
    for (uint32_t y = y0; y <= y1; y++)
    {
        frame[(y * CAMERA_STRIDE_PIXELS) + x] = color;
    }
}

float input_alpha(void) { return nxpc__read_alpha(); }
float input_beta(void) { return nxpc__read_beta(); }
float input_gamma(void) { return nxpc__read_gamma(); }
bool input_left_button(void) { return button__is_active(&left_btn) != 0U; }
bool input_right_button(void) { return button__is_active(&right_btn) != 0U; }
float battery_voltage(void) { return (float)nxpc__read_battery_voltage() * 0.01f; }

void motors_set_duty(float left, float right)
{
    if (!nxpc_system__outputs_allowed())
    {
        nxpc__disable_motor_control();
        return;
    }

    nxpc__enable_motor_control();
    nxpc__set_motor_pwm(nxpc__clamp_unit(left), nxpc__clamp_unit(right));
    g_motor_lease_remaining_ms = NXPC_MOTOR_LEASE_MS;
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
uint32_t frame_drop_count(void) { return g_frame_drop_count; }

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
