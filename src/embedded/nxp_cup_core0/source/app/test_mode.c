#include "test_mode.h"

#include "nxp_cup.h"

static color_features_t line_color[CAMERA_WIDTH];

static float clamp_input(float value)
{
    if (value != value || value < 0.0f)
    {
        return 0.0f;
    }
    if (value > 1.0f)
    {
        return 1.0f;
    }
    return value;
}

static float signed_input(float value)
{
    return (clamp_input(value) * 2.0f) - 1.0f;
}

static void camera_io_on_frame(uint16_t *frame)
{
    (void)frame;

    /*
     * CAMERA / IO is a safe place to inspect the common input API. The
     * framework draws the camera and status strip; these values are also
     * useful to host tools while checking a car.
     */
    (void)telemetry_f32("input.alpha", input_alpha(), "ratio");
    (void)telemetry_f32("input.beta", input_beta(), "ratio");
    (void)telemetry_f32("input.gamma", input_gamma(), "ratio");
    (void)telemetry_f32("battery.voltage", battery_voltage(), "V");
}

static void vision_on_frame(uint16_t *frame)
{
    float alpha = clamp_input(input_alpha());
    float beta = clamp_input(input_beta());
    float gamma = clamp_input(input_gamma());
    uint32_t line = 50U + (uint32_t)(alpha * 149.0f);
    uint8_t black_y_threshold = (uint8_t)(beta * 255.0f);
    uint16_t dark_color = color_rgb565(0U, 255U, 0U);
    uint16_t light_color = color_rgb565(255U, 255U, 255U);

    /*
     * This is a deliberately small pixel-classification sandbox, not a lane
     * detector or race solution. Alpha selects a camera row and beta changes
     * the dark-pixel threshold. Replace or extend it while exploring vision.
     */
    color_convert_rgb565_to_yhsv(camera_row(frame, line), line_color, CAMERA_WIDTH);

    for (uint32_t x = 0U; x < CAMERA_WIDTH; x++)
    {
        bool dark = line_color[x].y <= black_y_threshold;

        frame_draw_vertical_line(frame,
                                 x,
                                 (int32_t)line - (dark ? 2 : 1),
                                 (int32_t)line + (dark ? 2 : 1),
                                 dark ? dark_color : light_color);
    }

    (void)telemetry_f32("input.alpha", alpha, "ratio");
    (void)telemetry_f32("input.beta", beta, "ratio");
    (void)telemetry_f32("input.gamma", gamma, "ratio");
    (void)telemetry_f32("battery.voltage", battery_voltage(), "V");
    (void)telemetry_u32("vision.scan_row", line, "pixel");
    (void)telemetry_u32("vision.dark_threshold", black_y_threshold, "Y");
}

static void motors_on_frame(uint16_t *frame)
{
    float left = signed_input(input_alpha());
    float steering = signed_input(input_beta());
    float right = signed_input(input_gamma());

    (void)frame;

    /*
     * This is the same public actuator API used in race_mode.c. These calls
     * cannot move the car until the framework has selected MOTORS, observed a
     * deliberate EXE release, and confirmed that all three pots were centered.
     * The framework also caps TEST motor duty and enforces the command lease.
     */
    motors_set_duty(left, right);
    steering_set(steering);
}

void test_mode_on_frame(uint16_t *frame)
{
    switch (test_mode_page())
    {
        case TEST_MODE_CAMERA_IO:
            camera_io_on_frame(frame);
            break;

        case TEST_MODE_VISION:
            vision_on_frame(frame);
            break;

        case TEST_MODE_MOTORS:
            motors_on_frame(frame);
            break;

        default:
            break;
    }
}
