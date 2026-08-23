#include "nxpc_framework.h"

#include <stdarg.h>
#include <stdio.h>

#include "avc__io.h"
#include "avc__line_processor.h"
#include "nxp_cup.h"

#if CONFIG__MOTOR_ENCODER_BACKEND == MOTOR_ENCODER_BACKEND_QDC
#include "avc__motor_encoder_qdc.h"
#endif

#if CONFIG__USB_DEBUG_STREAM_ENABLE
#include "avc_usb_debug_stream.h"
#endif

#define NXPC_FRAME_BUDGET_US (41000U)
#define NXPC_CALLBACK_OVERRUN_LIMIT (3U)
#define NXPC_CAMERA_TIMEOUT_MS (250U)
#define NXPC_MOTOR_LEASE_MS (100U)
#define NXPC_MOTOR_LEASE_TICK_MS (10U)
#define NXPC_ENCODER_SAMPLE_MS (100U)

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

static eGFX_ImagePlane g_status_plane;
static eGFX_ImagePlane g_camera_plane;
static uint8_t g_status_buffer[eGFX_CALCULATE_16BPP_IMAGE_STORAGE_SPACE_SIZE(320, 40)];
static avc_system_mode_t g_last_rendered_mode;
static bool g_last_camera_seen;
static bool g_last_test_armed;
static bool g_last_test_pending;
static bool g_banner_initialized;

#if CONFIG__MOTOR_ENCODER_BACKEND == MOTOR_ENCODER_BACKEND_QDC
static avc_motor_encoder_sample_t g_encoder_samples[AVC_MOTOR_ENCODER_COUNT];
static uint32_t g_encoder_sample_tick;
static uint32_t g_encoder_sample_age_tick;
static bool g_encoder_sample_valid;
#endif

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
    avc__set_motor_pwm(0.0f, 0.0f);
}

static void nxpc__encoder_service(void)
{
#if CONFIG__MOTOR_ENCODER_BACKEND == MOTOR_ENCODER_BACKEND_QDC
    if (e_tick__timeout(&g_encoder_sample_tick, NXPC_ENCODER_SAMPLE_MS) != 0U)
    {
        avc__motor_encoder_qdc_sample(NXPC_ENCODER_SAMPLE_MS, g_encoder_samples);
        g_encoder_sample_age_tick = e_tick__get_ms();
        g_encoder_sample_valid = true;
    }
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
    avc_system_mode_t mode = avc_system__mode();
    bool camera_seen = avc_system__camera_frame_seen();
    bool test_armed = avc_system__test_outputs_armed();
    bool test_pending = avc_system__test_arm_pending();
    uint32_t green = eGFX_COLOR_RGB888_TO_RGB565(0U, 255U, 0U);
    uint32_t yellow = eGFX_COLOR_RGB888_TO_RGB565(255U, 255U, 0U);
    uint32_t red = eGFX_COLOR_RGB888_TO_RGB565(255U, 0U, 0U);
    char text[64];

    if (g_banner_initialized && (mode == g_last_rendered_mode) &&
        (camera_seen == g_last_camera_seen) && (test_armed == g_last_test_armed) &&
        (test_pending == g_last_test_pending))
    {
        return;
    }

    eGFX_ImagePlane_Clear(&g_status_plane);

    switch (mode)
    {
        case AVC_SYSTEM_MODE_TEST:
            nxpc__draw_status_text("TEST MODE", 5, 1, green, &FONT_10_14_1BPP);
            nxpc__draw_status_text(test_armed ? "MOTORS ARMED" :
                                   (test_pending ? "CENTER POTS" : "EXE: ARM MOTORS"),
                                   145,
                                   4,
                                   test_armed ? red : yellow,
                                   &FONT_5_7_1BPP);
            break;

        case AVC_SYSTEM_MODE_RACE_WAITING:
            nxpc__draw_status_text("RACE MODE", 5, 1, yellow, &FONT_10_14_1BPP);
            nxpc__draw_status_text(camera_seen ? "PRESS EXE TO START" : "WAITING FOR CAMERA",
                                   5,
                                   22,
                                   camera_seen ? green : yellow,
                                   &FONT_5_7_1BPP);
            break;

        case AVC_SYSTEM_MODE_RACE_RUNNING:
            nxpc__draw_status_text("RACE RUNNING", 5, 1, green, &FONT_10_14_1BPP);
            break;

        case AVC_SYSTEM_MODE_SAFE_FAULT:
            nxpc__draw_status_text("SAFE FAULT", 5, 1, red, &FONT_10_14_1BPP);
            nxpc__draw_status_text("MOTORS DISABLED", 5, 22, red, &FONT_5_7_1BPP);
            break;

        case AVC_SYSTEM_MODE_ENTERING_ISP:
            nxpc__draw_status_text("ENTERING USB ISP", 5, 1, yellow, &FONT_10_14_1BPP);
            break;

        case AVC_SYSTEM_MODE_STARTUP:
        default:
            nxpc__draw_status_text("STARTUP", 5, 1, yellow, &FONT_10_14_1BPP);
            break;
    }

    if (mode != AVC_SYSTEM_MODE_RACE_WAITING)
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
    g_last_camera_seen = camera_seen;
    g_last_test_armed = test_armed;
    g_last_test_pending = test_pending;
    g_banner_initialized = true;
}

void avc__next_frame(uint16_t *buffer)
{
    if (g_frame_ready)
    {
        g_frame_drop_count++;
    }

    g_latest_frame = buffer;
    g_frame_ready = true;
    g_frame_generation++;
    g_last_frame_ms = e_tick__get_ms();
    avc_system__notify_camera_frame();
#if CONFIG__USB_DEBUG_STREAM_ENABLE
    avc_usb_debug_stream__notify_camera_frame();
#endif
}

void nxpc_framework__init(void)
{
    avc_system__init();

    g_status_plane.Data = g_status_buffer;
    g_status_plane.SizeX = 320U;
    g_status_plane.SizeY = 40U;
    g_status_plane.Type = eGFX_IMAGE_PLANE_16BPP_RGB565;

    g_camera_plane.SizeX = CAMERA_WIDTH;
    g_camera_plane.SizeY = CAMERA_HEIGHT;
    g_camera_plane.Type = eGFX_IMAGE_PLANE_16BPP_RGB565;

    g_motor_lease_trigger.interval__mS = NXPC_MOTOR_LEASE_TICK_MS;
    g_motor_lease_trigger.duration__mS = NXPC_MOTOR_LEASE_TICK_MS;
    g_motor_lease_trigger.count = E_TICK__TRIGGER_FOREVER;
    g_motor_lease_trigger.hook = nxpc__motor_lease_tick;
    g_motor_lease_trigger.arg = NULL;
    g_motor_lease_trigger.marked_for_exe = false;
    g_motor_lease_trigger.next_item = NULL;
    e_tick__register_trigger(&g_motor_lease_trigger, true);

#if CONFIG__MOTOR_ENCODER_BACKEND == MOTOR_ENCODER_BACKEND_QDC
    g_encoder_sample_tick = e_tick__get_ms();
    g_encoder_sample_age_tick = g_encoder_sample_tick;
#endif

    nxpc__render_banner_if_changed();
}

void nxpc_framework__service(void)
{
    avc_camera__service();
    nxpc__encoder_service();

#if CONFIG__USB_DEBUG_STREAM_ENABLE
    avc_usb_debug_stream__service();
#endif

    avc_system__service();

    if ((avc_system__mode() == AVC_SYSTEM_MODE_RACE_RUNNING) &&
        ((e_tick__get_ms() - g_last_frame_ms) > NXPC_CAMERA_TIMEOUT_MS))
    {
        avc_system__enter_fault(AVC_SYSTEM_FAULT_CAMERA_LOST);
    }

    if (g_motor_lease_expired)
    {
        g_motor_lease_expired = false;
        avc__disable_motor_control();
        DEBUG("Motor command lease expired; outputs disabled.\r\n");
    }

    if (!avc_system__outputs_allowed())
    {
        g_motor_lease_remaining_ms = 0U;
    }

    nxpc__render_banner_if_changed();
}

uint16_t *nxpc_framework__take_latest_frame(void)
{
    uint16_t *frame = NULL;
    uint32_t interrupt_state;
    avc_system_mode_t mode = avc_system__mode();

    if ((mode != AVC_SYSTEM_MODE_TEST) && (mode != AVC_SYSTEM_MODE_RACE_RUNNING) &&
        (mode != AVC_SYSTEM_MODE_RACE_WAITING))
    {
        return NULL;
    }

#if CONFIG__USB_DEBUG_STREAM_ENABLE
    if ((mode == AVC_SYSTEM_MODE_RACE_WAITING) &&
        !avc_usb_debug_stream__camera_frames_active())
    {
        return NULL;
    }
#else
    if (mode == AVC_SYSTEM_MODE_RACE_WAITING)
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
        avc_camera__prepare_frame(frame);
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
            avc_system__enter_fault(AVC_SYSTEM_FAULT_CALLBACK_OVERRUN);
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
    (void)avc_usb_debug_stream__publish_frame(frame);
#endif

#if CONFIG__DISPLAY_ENABLE
    if (avc_system__mode() == AVC_SYSTEM_MODE_TEST)
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
    avc_color_features_t raw = avc__rgb565_to_yhsv(pixel);
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

float input_alpha(void) { return avc__read_alpha(); }
float input_beta(void) { return avc__read_beta(); }
float input_gamma(void) { return avc__read_gamma(); }
bool input_left_button(void) { return button__is_active(&left_btn) != 0U; }
bool input_right_button(void) { return button__is_active(&right_btn) != 0U; }

void motors_set_duty(float left, float right)
{
    if (!avc_system__outputs_allowed())
    {
        avc__disable_motor_control();
        return;
    }

    avc__enable_motor_control();
    avc__set_motor_pwm(nxpc__clamp_unit(left), nxpc__clamp_unit(right));
    g_motor_lease_remaining_ms = NXPC_MOTOR_LEASE_MS;
}

void motors_stop(void)
{
    g_motor_lease_remaining_ms = 0U;
    avc__disable_motor_control();
}

void steering_set(float position)
{
    avc__set_servo(avc_system__outputs_allowed() ? nxpc__clamp_unit(position) : 0.0f);
}

float wheel_speed_rpm(wheel_t wheel)
{
#if CONFIG__MOTOR_ENCODER_BACKEND == MOTOR_ENCODER_BACKEND_QDC
    uint32_t index = (wheel == WHEEL_RIGHT) ? AVC_MOTOR_ENCODER_M1 : AVC_MOTOR_ENCODER_M0;
    if (!wheel_speed_available())
    {
        return 0.0f;
    }
    return (float)g_encoder_samples[index].rpm_milli / 1000.0f;
#else
    (void)wheel;
    return 0.0f;
#endif
}

float wheel_speed_mps(wheel_t wheel)
{
    const float circumference_m =
        3.14159265f * ((float)CONFIG__MOTOR_ENCODER_WHEEL_DIAMETER_MM / 1000.0f);
    return (wheel_speed_rpm(wheel) / 60.0f) * circumference_m;
}

uint32_t wheel_speed_age_ms(void)
{
#if CONFIG__MOTOR_ENCODER_BACKEND == MOTOR_ENCODER_BACKEND_QDC
    return e_tick__delta(&g_encoder_sample_age_tick);
#else
    return UINT32_MAX;
#endif
}

bool wheel_speed_available(void)
{
#if CONFIG__MOTOR_ENCODER_BACKEND == MOTOR_ENCODER_BACKEND_QDC
    return g_encoder_sample_valid &&
           (wheel_speed_age_ms() <= (NXPC_ENCODER_SAMPLE_MS * 3U));
#else
    return false;
#endif
}

uint32_t time_milliseconds(void) { return e_tick__get_ms(); }
uint32_t frame_processing_microseconds(void) { return g_callback_us; }
uint32_t frame_drop_count(void) { return g_frame_drop_count; }

bool telemetry_i32(const char *name, int32_t value, const char *units)
{
#if CONFIG__USB_DEBUG_STREAM_ENABLE
    return avc_usb_debug_stream__telemetry_i32(name, value, units);
#else
    (void)name; (void)value; (void)units; return false;
#endif
}

bool telemetry_u32(const char *name, uint32_t value, const char *units)
{
#if CONFIG__USB_DEBUG_STREAM_ENABLE
    return avc_usb_debug_stream__telemetry_u32(name, value, units);
#else
    (void)name; (void)value; (void)units; return false;
#endif
}

bool telemetry_f32(const char *name, float value, const char *units)
{
#if CONFIG__USB_DEBUG_STREAM_ENABLE
    return avc_usb_debug_stream__telemetry_f32(name, value, units);
#else
    (void)name; (void)value; (void)units; return false;
#endif
}

bool telemetry_bool(const char *name, bool value)
{
#if CONFIG__USB_DEBUG_STREAM_ENABLE
    return avc_usb_debug_stream__telemetry_bool(name, value);
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
    return avc_usb_debug_stream__log_text(AVC_DBG_LOG_LEVEL_INFO, category, text);
#else
    (void)category; (void)format; return false;
#endif
}
