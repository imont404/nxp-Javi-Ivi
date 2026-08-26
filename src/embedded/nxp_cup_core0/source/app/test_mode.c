#include "test_mode.h"

#include "nxp_cup.h"

typedef struct
{
    uint16_t x;
    int16_t gradient;
} vision_edge_t;

static color_features_t line_color[CAMERA_WIDTH];
static vision_edge_t line_edges[CAMERA_WIDTH];

static uint32_t gradient_magnitude(int32_t gradient)
{
    return (gradient < 0) ? (uint32_t)(-gradient) : (uint32_t)gradient;
}

static uint32_t find_luma_edges(const color_features_t *pixels,
                                uint32_t length,
                                uint32_t radius,
                                uint32_t threshold,
                                vision_edge_t *edges,
                                uint32_t edge_capacity)
{
    uint32_t edge_count = 0U;
    uint32_t x = radius;

    /*
     * This is a one-dimensional Sobel-style x-gradient:
     * compare luma on either side of x. Radius changes the spatial scale, and
     * the threshold rejects small differences, without the cost or complexity
     * of a full two-dimensional Canny pass. Each response run becomes one edge
     * at its magnitude-weighted center.
     */
    while ((x + radius) < length)
    {
        int32_t gradient = (int32_t)pixels[x + radius].y -
                           (int32_t)pixels[x - radius].y;
        uint32_t magnitude = gradient_magnitude(gradient);

        if (magnitude < threshold)
        {
            x++;
            continue;
        }

        bool rising = gradient > 0;
        int32_t strongest_gradient = gradient;
        uint32_t strongest_magnitude = magnitude;
        uint32_t magnitude_sum = magnitude;
        uint32_t weighted_x_sum = x * magnitude;

        x++;
        while ((x + radius) < length)
        {
            gradient = (int32_t)pixels[x + radius].y -
                       (int32_t)pixels[x - radius].y;
            magnitude = gradient_magnitude(gradient);
            if ((magnitude < threshold) || ((gradient > 0) != rising))
            {
                break;
            }
            if (magnitude > strongest_magnitude)
            {
                strongest_gradient = gradient;
                strongest_magnitude = magnitude;
            }
            magnitude_sum += magnitude;
            weighted_x_sum += x * magnitude;
            x++;
        }

        if (edge_count < edge_capacity)
        {
            edges[edge_count].x = (uint16_t)((weighted_x_sum + (magnitude_sum / 2U)) /
                                             magnitude_sum);
            edges[edge_count].gradient = (int16_t)strongest_gradient;
            edge_count++;
        }
    }

    return edge_count;
}

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
    uint32_t line = (uint32_t)(alpha * (float)(CAMERA_HEIGHT - 1U));
    uint32_t edge_threshold = 8U + (uint32_t)(beta * 120.0f);
    uint32_t gradient_radius = 1U + (uint32_t)(gamma * 4.0f);
    uint32_t edge_count;
    uint16_t scan_color = color_rgb565(255U, 255U, 0U);
    uint16_t falling_color = color_rgb565(255U, 64U, 64U);
    uint16_t rising_color = color_rgb565(64U, 255U, 64U);

    if (gradient_radius > 4U)
    {
        gradient_radius = 4U;
    }

    /*
     * This is an edge-finding example, not a lane detector or race solution.
     * A black track stripe normally gives one falling edge and one rising
     * edge. We intentionally keep every strong edge because another lane or
     * object may also cross the row; deciding which edges matter is your code.
     * The h/s/v fields beside y are available for later color experiments.
     */
    color_convert_rgb565_to_yhsv(camera_row(frame, line), line_color, CAMERA_WIDTH);
    edge_count = find_luma_edges(line_color,
                                 CAMERA_WIDTH,
                                 gradient_radius,
                                 edge_threshold,
                                 line_edges,
                                 CAMERA_WIDTH);

    frame_draw_horizontal_line(frame,
                               0,
                               CAMERA_WIDTH - 1U,
                               (int32_t)line,
                               scan_color);

    for (uint32_t i = 0U; i < edge_count; i++)
    {
        frame_draw_vertical_line(frame,
                                 line_edges[i].x,
                                 (int32_t)line - 8,
                                 (int32_t)line + 8,
                                 (line_edges[i].gradient < 0) ? falling_color : rising_color);
    }

    (void)telemetry_f32("input.alpha", alpha, "ratio");
    (void)telemetry_f32("input.beta", beta, "ratio");
    (void)telemetry_f32("input.gamma", gamma, "ratio");
    (void)telemetry_u32("vision.scan_row", line, "pixel");
    (void)telemetry_u32("vision.edge_threshold", edge_threshold, "Y");
    (void)telemetry_u32("vision.gradient_radius", gradient_radius, "pixel");
    (void)telemetry_u32("vision.edge_count", edge_count, "edge");
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
