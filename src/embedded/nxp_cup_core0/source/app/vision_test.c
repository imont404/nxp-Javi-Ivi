#include "vision_test.h"

#include "nxp_cup.h"
#include "utils.h"

static color_features_t scanline[CAMERA_WIDTH];

static bool scan_row_center(uint16_t *frame, uint32_t row, int32_t *center,
                             int32_t *left, int32_t *right)
{
    uint8_t luma_min = 255U;
    uint8_t luma_max = 0U;
    uint8_t threshold;
    int32_t track_width;

    color_convert_rgb565_to_yhsv(camera_row(frame, row), scanline, CAMERA_WIDTH);

    for (uint32_t x = 0U; x < CAMERA_WIDTH; x++)
    {
        if (scanline[x].y < luma_min) { luma_min = scanline[x].y; }
        if (scanline[x].y > luma_max) { luma_max = scanline[x].y; }
    }

    threshold = (uint8_t)(luma_min + (((uint32_t)luma_max - luma_min) * 3U) / 4U);

    return white_center(scanline, CAMERA_WIDTH, threshold,
                         center, left, right, &track_width);
}

static void draw_scanline_marker(uint16_t *frame, uint32_t row, uint16_t color)
{
    int32_t top = (row < (CAMERA_HEIGHT - 1U)) ? (int32_t)row : (int32_t)row - 1;
    frame_draw_horizontal_line(frame, 0, CAMERA_WIDTH - 1U, top, color);
}

void vision_test_on_frame(uint16_t *frame)
{
    /* Las 3 filas ahora agrupadas cerca del centro, separadas 15px entre si */
    uint32_t row_2 = CAMERA_HEIGHT / 2U;       /* la del medio: sigue siendo el "centro" */
    uint32_t row_1 = row_2 - 15U;
    uint32_t row_3 = row_2 + 15U;

    int32_t center1, left1, right1;
    int32_t center2, left2, right2;
    int32_t center3, left3, right3;
    bool found1, found2, found3;

    uint16_t scan_color = color_rgb565(255U, 255U, 0U);
    
    found1 = scan_row_center(frame, row_1, &center1, &left1, &right1);
    found2 = scan_row_center(frame, row_2, &center2, &left2, &right2);
    found3 = scan_row_center(frame, row_3, &center3, &left3, &right3);
    
    draw_scanline_marker(frame, row_1, scan_color);
    draw_scanline_marker(frame, row_2, scan_color);
    draw_scanline_marker(frame, row_3, scan_color);

    if (found1)
    {
        draw_filled_circle(frame, center1, (int32_t)row_1, 4, color_rgb565(255U, 0U, 0U));
    }

    if (found2)
    {
        draw_filled_circle(frame, center2, (int32_t)row_2, 4, color_rgb565(255U, 0U, 0U));
    }

    if (found3)
    {
        draw_filled_circle(frame, center3, (int32_t)row_3, 4, color_rgb565(255U, 0U, 0U));
    }

    (void)telemetry_i32("vision.center1", found1 ? center1 : -1, "pixel");
    (void)telemetry_i32("vision.center2", found2 ? center2 : -1, "pixel");
    (void)telemetry_i32("vision.center3", found3 ? center3 : -1, "pixel");
    (void)telemetry_u32("vision.found1", found1 ? 1U : 0U, "bool");
    (void)telemetry_u32("vision.found2", found2 ? 1U : 0U, "bool");
    (void)telemetry_u32("vision.found3", found3 ? 1U : 0U, "bool");
}