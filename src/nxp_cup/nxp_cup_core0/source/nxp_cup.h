#ifndef NXP_CUP_H_
#define NXP_CUP_H_

#include <stdbool.h>
#include <stdint.h>

#define CAMERA_WIDTH (320U)
#define CAMERA_HEIGHT (200U)
#define CAMERA_STRIDE_PIXELS CAMERA_WIDTH

typedef enum
{
    WHEEL_LEFT = 0,
    WHEEL_RIGHT = 1
} wheel_t;

typedef struct
{
    uint8_t y;
    uint8_t h;
    uint8_t s;
    uint8_t v;
} color_features_t;

/* The frame pointer is writable RGB565 and valid only during the mode callback. */
static inline uint16_t *camera_row(uint16_t *frame, uint32_t y)
{
    return &frame[y * CAMERA_STRIDE_PIXELS];
}

color_features_t color_rgb565_to_yhsv(uint16_t pixel);
void color_convert_rgb565_to_yhsv(const uint16_t *pixels,
                                  color_features_t *features,
                                  uint32_t count);
uint16_t color_rgb565(uint8_t red, uint8_t green, uint8_t blue);
void frame_draw_vertical_line(uint16_t *frame,
                              uint32_t x,
                              uint32_t y0,
                              uint32_t y1,
                              uint16_t color);

/* Potentiometers are normalized to 0.0 through 1.0. */
float input_alpha(void);
float input_beta(void);
float input_gamma(void);
bool input_left_button(void);
bool input_right_button(void);
/* Battery voltage uses the board's filtered ADC input and divider scaling. */
float battery_voltage(void);

/* Motor duty and steering position are clamped to -1.0 through 1.0.
 * Each motor command renews a 100 ms dead-man lease. */
void motors_set_duty(float left, float right);
void motors_stop(void);
void steering_set(float position);

/* Positive wheel speed is forward. A stale/unavailable measurement returns 0. */
float wheel_speed_rpm(wheel_t wheel);
float wheel_speed_mps(wheel_t wheel);
uint32_t wheel_speed_age_ms(void);
bool wheel_speed_available(void);

uint32_t time_milliseconds(void);
uint32_t frame_processing_microseconds(void);
uint32_t frame_drop_count(void);

bool telemetry_i32(const char *name, int32_t value, const char *units);
bool telemetry_u32(const char *name, uint32_t value, const char *units);
bool telemetry_f32(const char *name, float value, const char *units);
bool telemetry_bool(const char *name, bool value);
/* Text values are non-empty UTF-8 strings of at most 48 bytes. */
bool telemetry_text(const char *name, const char *value);
bool telemetry_log(const char *category, const char *format, ...);

#endif /* NXP_CUP_H_ */
