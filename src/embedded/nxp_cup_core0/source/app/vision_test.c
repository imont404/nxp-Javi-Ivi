#include "vision_test.h"

#include "nxp_cup.h"

static color_features_t scanline[CAMERA_WIDTH];

static float clamp_input(float value)
{
    if ((value != value) || (value < 0.0f))
    {
        return 0.0f;
    }
    if (value > 1.0f)
    {
        return 1.0f;
    }
    return value;
}

static int32_t luma_plot_y(uint8_t luma)
{
    /* Conventional graph orientation: bright is high and dark is low. */
    return (int32_t)(CAMERA_HEIGHT - 1U) -
           (int32_t)(((uint32_t)luma * (CAMERA_HEIGHT - 1U) + 127U) / 255U);
}

static void draw_scanline_marker(uint16_t *frame, uint32_t row, uint16_t color)
{
    int32_t top = (row < (CAMERA_HEIGHT - 1U)) ? (int32_t)row : (int32_t)row - 1;

    frame_draw_horizontal_line(frame, 0, CAMERA_WIDTH - 1U, top, color);
    frame_draw_horizontal_line(frame, 0, CAMERA_WIDTH - 1U, top + 1, color);
}

void vision_test_on_frame(uint16_t *frame)
{
    float alpha = clamp_input(input_alpha());
    uint32_t row = (uint32_t)(alpha * (float)(CAMERA_HEIGHT - 1U));
    uint8_t luma_min = 255U;
    uint8_t luma_max = 0U;
    uint16_t scan_color = color_rgb565(255U, 255U, 0U);
    uint16_t plot_color = color_rgb565(0U, 255U, 255U);
    int32_t previous_y;

    /*
     * Alpha selects one camera row. The yellow marker shows the sampled row;
     * the cyan graph plots that row's luma across the full image. This is the
     * simplest useful bridge from camera pixels to a later gradient or edge
     * experiment, without deciding which edges belong to a track.
     */
    color_convert_rgb565_to_yhsv(camera_row(frame, row), scanline, CAMERA_WIDTH);
    draw_scanline_marker(frame, row, scan_color);
    previous_y = luma_plot_y(scanline[0].y);

    for (uint32_t x = 0U; x < CAMERA_WIDTH; x++)
    {
        int32_t y = luma_plot_y(scanline[x].y);

        if (scanline[x].y < luma_min)
        {
            luma_min = scanline[x].y;
        }
        if (scanline[x].y > luma_max)
        {
            luma_max = scanline[x].y;
        }
        if (x != 0U)
        {
            frame_draw_line(frame, (int32_t)x - 1, previous_y, (int32_t)x, y, plot_color);
        }
        previous_y = y;
    }

    (void)telemetry_f32("input.alpha", alpha, "ratio");
    (void)telemetry_u32("vision.scan_row", row, "pixel");
    (void)telemetry_u32("vision.luma_min", luma_min, "Y");
    (void)telemetry_u32("vision.luma_max", luma_max, "Y");
}
