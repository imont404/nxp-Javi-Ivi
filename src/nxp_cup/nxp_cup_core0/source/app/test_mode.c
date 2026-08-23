#include "test_mode.h"

#include "nxp_cup.h"

static color_features_t line_color[CAMERA_WIDTH];
static uint8_t black_filter[CAMERA_WIDTH];

void test_mode_on_frame(uint16_t *frame)
{
    uint32_t line = 50U + (uint32_t)(input_alpha() * 149.0f);
    uint8_t black_y_threshold = (uint8_t)(input_beta() * 255.0f);

    color_convert_rgb565_to_yhsv(camera_row(frame, line), line_color, CAMERA_WIDTH);

    for (uint32_t x = 0U; x < CAMERA_WIDTH; x++)
    {
        black_filter[x] = (line_color[x].y <= black_y_threshold) ? 1U : 0U;
        frame_draw_vertical_line(frame,
                                 x,
                                 line - (black_filter[x] ? 2U : 1U),
                                 line + (black_filter[x] ? 2U : 1U),
                                 black_filter[x] ? color_rgb565(0U, 255U, 0U)
                                                 : color_rgb565(255U, 255U, 255U));
    }

    /* In TEST mode the framework owns arming and the midpoint interlock. */
    motors_set_duty((input_alpha() * 2.0f) - 1.0f,
                    (input_gamma() * 2.0f) - 1.0f);
    steering_set((input_beta() * 2.0f) - 1.0f);

    (void)telemetry_f32("input.alpha", input_alpha(), "ratio");
    (void)telemetry_f32("input.beta", input_beta(), "ratio");
    (void)telemetry_f32("input.gamma", input_gamma(), "ratio");
    if (wheel_speed_available())
    {
        (void)telemetry_f32("wheel.left.rpm", wheel_speed_rpm(WHEEL_LEFT), "rpm");
        (void)telemetry_f32("wheel.right.rpm", wheel_speed_rpm(WHEEL_RIGHT), "rpm");
    }
}
