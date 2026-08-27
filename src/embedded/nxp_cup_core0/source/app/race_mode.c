#include "race_mode.h"

#include <stdio.h>

#include "nxp_cup.h"
#include "utils.h"


/* Un buffer por linea procesada, igual que line_hsl / line_hsl2 / line_hsl3 */
static color_features_t line_hsl[CAMERA_WIDTH];
static color_features_t line_hsl2[CAMERA_WIDTH];
static color_features_t line_hsl3[CAMERA_WIDTH];

static bool found1 = false;
static bool found2 = false;
static bool found3 = false;

static uint32_t line_to_process1, line_to_process2, line_to_process3;

static char pot_text[64];

static void race_mode__update_overlay(uint16_t *frame);


void race_mode_on_frame(uint16_t *frame)
{
    /* Seleccion de linea */
    line_to_process1 = 110U;
    line_to_process2 = 150U;
    line_to_process3 = 170U;

    /* beta es el umbral de luminancia: 0.0-1.0 del pot escalado al luma 0-255 */
    uint8_t threshold = (uint8_t)(input_beta() * 255.0f);

    int32_t center1, left, right, width_px;
    int32_t center2, left2, right2, width_px2;
    int32_t center3, left3, right3, width_px3;

    color_convert_rgb565_to_yhsv(camera_row(frame, line_to_process1),
                                 line_hsl,
                                 CAMERA_WIDTH);

    color_convert_rgb565_to_yhsv(camera_row(frame, line_to_process2),
                                 line_hsl2,
                                 CAMERA_WIDTH);

    color_convert_rgb565_to_yhsv(camera_row(frame, line_to_process3),
                                 line_hsl3,
                                 CAMERA_WIDTH);

    found1 = white_center(line_hsl,
                          CAMERA_WIDTH, threshold,
                          &center1, &left, &right, &width_px);
    found2 = white_center(line_hsl2,
                          CAMERA_WIDTH, threshold,
                          &center2, &left2, &right2, &width_px2);
    found3 = white_center(line_hsl3,
                          CAMERA_WIDTH, threshold,
                          &center3, &left3, &right3, &width_px3);

    if (found1)
    {
        draw_filled_circle(frame, center1, (int32_t)line_to_process1, 4, color_rgb565(255U, 0U, 0U));
    }

    if (found2)
    {
        draw_filled_circle(frame, center2, (int32_t)line_to_process2, 4, color_rgb565(255U, 0U, 0U));
    }

    if (found3)
    {
        draw_filled_circle(frame, center3, (int32_t)line_to_process3, 4, color_rgb565(255U, 0U, 0U));
    }

    motor_control(center1, center2, center3, width_px, found1, found2, found3, true);

    race_mode__update_overlay(frame);

    (void)telemetry_i32("vision.center1", found1 ? center1 : -1, "pixel");
    (void)telemetry_i32("vision.center2", found2 ? center2 : -1, "pixel");
    (void)telemetry_i32("vision.center3", found3 ? center3 : -1, "pixel");
    (void)telemetry_u32("vision.threshold", threshold, "luma");
}


static void race_mode__update_overlay(uint16_t *frame)
{
    uint16_t text_color = color_rgb565(0U, 0xFFU, 0U);
    uint32_t cpu;

    /* Los pots se muestran en centesimos (50 = 0.50) para no depender de printf-float */
    snprintf(pot_text, sizeof(pot_text), "a:%d b:%d g:%d",
             (int)(input_alpha() * 100.0f),
             (int)(input_beta() * 100.0f),
             (int)(input_gamma() * 100.0f));

    frame_draw_text(frame, 3, 3, pot_text, text_color);

    cpu = (frame_processing_microseconds() * 100U) / FRAME_BUDGET_MICROSECONDS;

    snprintf(pot_text, sizeof(pot_text), "CPU:%u%%", (unsigned)cpu);

    frame_draw_text(frame, 3, 13, pot_text, color_rgb565(0U, 192U, 0U));
}
