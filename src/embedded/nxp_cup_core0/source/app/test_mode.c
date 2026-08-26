#include "test_mode.h"

#include "nxp_cup.h"

typedef struct
{
    uint16_t x;
    int16_t gradient;
} vision_edge_t;

typedef enum
{
    VISION_CLASS_UNKNOWN = 0,
    VISION_CLASS_BLACK,
    VISION_CLASS_WHITE
} vision_class_t;

typedef enum
{
    VISION_BOUNDARY_NONE = 0,
    VISION_BOUNDARY_WHITE_TO_BLACK,
    VISION_BOUNDARY_BLACK_TO_WHITE
} vision_boundary_t;

#define VISION_LUMA_EDGE_THRESHOLD (24U)
#define VISION_GRADIENT_RADIUS (2U)
#define VISION_REGION_SAMPLES (4U)
#define VISION_REGION_REQUIRED (3U)

static color_features_t line_color[CAMERA_WIDTH];
static vision_edge_t line_edges[CAMERA_WIDTH];

static uint32_t gradient_magnitude(int32_t gradient)
{
    return (gradient < 0) ? (uint32_t)(-gradient) : (uint32_t)gradient;
}

static uint32_t find_luma_edges(const color_features_t *pixels, uint32_t length, uint32_t radius,
                                uint32_t threshold, vision_edge_t *edges, uint32_t edge_capacity)
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
        int32_t gradient = (int32_t)pixels[x + radius].y - (int32_t)pixels[x - radius].y;
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
            gradient = (int32_t)pixels[x + radius].y - (int32_t)pixels[x - radius].y;
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
            edges[edge_count].x =
                (uint16_t)((weighted_x_sum + (magnitude_sum / 2U)) / magnitude_sum);
            edges[edge_count].gradient = (int16_t)strongest_gradient;
            edge_count++;
        }
    }

    return edge_count;
}

static void low_pass_yhsv(color_features_t *pixels, uint32_t length)
{
    color_features_t previous;
    color_features_t current;

    if (!pixels || (length < 3U))
    {
        return;
    }

    previous = pixels[0];
    current = pixels[1];

    /*
     * Symmetric 1-2-1 filtering suppresses isolated pixel noise without
     * shifting an edge. The rolling originals make this safe in place. Hue is
     * circular and is not used here, so copying it avoids an invalid average.
     */
    for (uint32_t x = 1U; (x + 1U) < length; x++)
    {
        color_features_t next = pixels[x + 1U];

        pixels[x].y = (uint8_t)(((uint32_t)previous.y + (2U * current.y) + next.y + 2U) / 4U);
        pixels[x].h = current.h;
        pixels[x].s = (uint8_t)(((uint32_t)previous.s + (2U * current.s) + next.s + 2U) / 4U);
        pixels[x].v = (uint8_t)(((uint32_t)previous.v + (2U * current.v) + next.v + 2U) / 4U);

        previous = current;
        current = next;
    }
}

static vision_class_t classify_black_white(const color_features_t *pixel, uint32_t black_v_max,
                                           uint32_t white_y_min, uint32_t white_s_max)
{
    /* Saturation is unreliable near black, so black uses value alone. */
    if (pixel->v <= black_v_max)
    {
        return VISION_CLASS_BLACK;
    }

    /* High luma plus low saturation rejects bright, strongly colored pixels. */
    if ((pixel->y >= white_y_min) && (pixel->s <= white_s_max))
    {
        return VISION_CLASS_WHITE;
    }

    return VISION_CLASS_UNKNOWN;
}

static vision_boundary_t classify_black_white_boundary(const color_features_t *pixels,
                                                       uint32_t length, uint32_t x, uint32_t radius,
                                                       uint32_t black_v_max, uint32_t white_y_min,
                                                       uint32_t white_s_max)
{
    uint32_t left_black = 0U;
    uint32_t left_white = 0U;
    uint32_t right_black = 0U;
    uint32_t right_white = 0U;

    if ((x < (radius + VISION_REGION_SAMPLES)) || ((x + radius + VISION_REGION_SAMPLES) > length))
    {
        return VISION_BOUNDARY_NONE;
    }

    for (uint32_t i = 0U; i < VISION_REGION_SAMPLES; i++)
    {
        vision_class_t left =
            classify_black_white(&pixels[x - radius - i], black_v_max, white_y_min, white_s_max);
        vision_class_t right =
            classify_black_white(&pixels[x + radius + i], black_v_max, white_y_min, white_s_max);

        left_black += (left == VISION_CLASS_BLACK) ? 1U : 0U;
        left_white += (left == VISION_CLASS_WHITE) ? 1U : 0U;
        right_black += (right == VISION_CLASS_BLACK) ? 1U : 0U;
        right_white += (right == VISION_CLASS_WHITE) ? 1U : 0U;
    }

    if ((left_white >= VISION_REGION_REQUIRED) && (right_black >= VISION_REGION_REQUIRED))
    {
        return VISION_BOUNDARY_WHITE_TO_BLACK;
    }
    if ((left_black >= VISION_REGION_REQUIRED) && (right_white >= VISION_REGION_REQUIRED))
    {
        return VISION_BOUNDARY_BLACK_TO_WHITE;
    }
    return VISION_BOUNDARY_NONE;
}

static void draw_two_pixel_horizontal(uint16_t *frame, uint32_t y, uint16_t color)
{
    int32_t top = (y < (CAMERA_HEIGHT - 1U)) ? (int32_t)y : (int32_t)y - 1;

    frame_draw_horizontal_line(frame, 0, CAMERA_WIDTH - 1U, top, color);
    frame_draw_horizontal_line(frame, 0, CAMERA_WIDTH - 1U, top + 1, color);
}

static void draw_two_pixel_vertical(uint16_t *frame, uint32_t x, int32_t y0, int32_t y1,
                                    uint16_t color)
{
    int32_t left = (x < (CAMERA_WIDTH - 1U)) ? (int32_t)x : (int32_t)x - 1;

    frame_draw_vertical_line(frame, left, y0, y1, color);
    frame_draw_vertical_line(frame, left + 1, y0, y1, color);
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
    uint32_t black_v_max = 40U + (uint32_t)(beta * 80.0f);
    uint32_t white_y_min = 220U - (uint32_t)(beta * 80.0f);
    uint32_t white_s_max = 16U + (uint32_t)(gamma * 112.0f);
    uint32_t edge_count;
    uint32_t boundary_count = 0U;
    uint32_t black_count = 0U;
    uint32_t white_count = 0U;
    uint16_t scan_color = color_rgb565(255U, 255U, 0U);
    uint16_t falling_color = color_rgb565(255U, 64U, 64U);
    uint16_t rising_color = color_rgb565(64U, 255U, 64U);

    /*
     * This is a color-validated edge example, not a lane detector or race
     * solution. The luma gradient finds precise candidate locations. Nearby
     * pixels must then form stable black and white regions: black has low HSV
     * value, while white has high luma and low saturation. A symmetric spatial
     * low-pass first suppresses isolated pixel noise without moving edges.
     * Alpha selects the row, Beta changes brightness strictness, and Gamma
     * changes how much saturation white may contain. Only validated
     * transitions are marked: red is white-to-black and green is
     * black-to-white.
     */
    color_convert_rgb565_to_yhsv(camera_row(frame, line), line_color, CAMERA_WIDTH);
    low_pass_yhsv(line_color, CAMERA_WIDTH);
    edge_count = find_luma_edges(line_color, CAMERA_WIDTH, VISION_GRADIENT_RADIUS,
                                 VISION_LUMA_EDGE_THRESHOLD, line_edges, CAMERA_WIDTH);

    for (uint32_t x = 0U; x < CAMERA_WIDTH; x++)
    {
        vision_class_t pixel_class =
            classify_black_white(&line_color[x], black_v_max, white_y_min, white_s_max);
        black_count += (pixel_class == VISION_CLASS_BLACK) ? 1U : 0U;
        white_count += (pixel_class == VISION_CLASS_WHITE) ? 1U : 0U;
    }

    draw_two_pixel_horizontal(frame, line, scan_color);

    for (uint32_t i = 0U; i < edge_count; i++)
    {
        vision_boundary_t boundary = classify_black_white_boundary(
            line_color, CAMERA_WIDTH, line_edges[i].x, VISION_GRADIENT_RADIUS, black_v_max,
            white_y_min, white_s_max);

        if (boundary == VISION_BOUNDARY_WHITE_TO_BLACK)
        {
            draw_two_pixel_vertical(frame, line_edges[i].x, (int32_t)line - 10, (int32_t)line + 10,
                                    falling_color);
            boundary_count++;
        }
        else if (boundary == VISION_BOUNDARY_BLACK_TO_WHITE)
        {
            draw_two_pixel_vertical(frame, line_edges[i].x, (int32_t)line - 10, (int32_t)line + 10,
                                    rising_color);
            boundary_count++;
        }
    }

    (void)telemetry_f32("input.alpha", alpha, "ratio");
    (void)telemetry_f32("input.beta", beta, "ratio");
    (void)telemetry_f32("input.gamma", gamma, "ratio");
    (void)telemetry_u32("vision.scan_row", line, "pixel");
    (void)telemetry_u32("vision.black_v_max", black_v_max, "V");
    (void)telemetry_u32("vision.white_y_min", white_y_min, "Y");
    (void)telemetry_u32("vision.white_s_max", white_s_max, "S");
    (void)telemetry_u32("vision.black_count", black_count, "pixel");
    (void)telemetry_u32("vision.white_count", white_count, "pixel");
    (void)telemetry_u32("vision.edge_count", edge_count, "edge");
    (void)telemetry_u32("vision.boundary_count", boundary_count, "edge");
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
