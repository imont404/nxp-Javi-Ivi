#include "vision_test.h"

#include <stdio.h>

#include "nxp_cup.h"
#include "utils.h"


/* Misma configuracion que race_mode.c: un buffer por linea procesada */
static color_features_t line_hsl[CAMERA_WIDTH];
static color_features_t line_hsl2[CAMERA_WIDTH];
static color_features_t line_hsl3[CAMERA_WIDTH];

static bool found1 = false;
static bool found2 = false;
static bool found3 = false;

static uint32_t line_to_process1, line_to_process2, line_to_process3;

static char pot_text[64];

static void vision_test__update_overlay(uint16_t *frame, uint8_t threshold);


void vision_test_on_frame(uint16_t *frame)
{
    /* Seleccion de linea: iguales a las de race_mode.c */
    line_to_process1 = 110U;
    line_to_process2 = 150U;
    line_to_process3 = 170U;

    /* beta es el umbral de luminancia: 0.0-1.0 del pot escalado al luma 0-255 */
    uint8_t threshold = (uint8_t)(input_beta() * 255.0f);

    int32_t center1, left, right, width_px;
    int32_t center2, left2, right2, width_px2;
    int32_t center3, left3, right3, width_px3;

    uint16_t center_color = color_rgb565(255U, 0U, 0U);   /* centro de masa */
    uint16_t left_color = color_rgb565(0U, 255U, 0U);     /* primer pixel blanco */
    uint16_t right_color = color_rgb565(0U, 128U, 255U);  /* ultimo pixel blanco */
    uint16_t scan_color = color_rgb565(255U, 255U, 0U);   /* marca de la fila escaneada */

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

    /* Las filas se dibujan despues de escanear para no contaminar el dato */
    frame_draw_horizontal_line(frame, 0, CAMERA_WIDTH - 1U, (int32_t)line_to_process1, scan_color);
    frame_draw_horizontal_line(frame, 0, CAMERA_WIDTH - 1U, (int32_t)line_to_process2, scan_color);
    frame_draw_horizontal_line(frame, 0, CAMERA_WIDTH - 1U, (int32_t)line_to_process3, scan_color);

    if (found1)
    {
        draw_filled_circle(frame, center1, (int32_t)line_to_process1, 4, center_color);
        draw_filled_circle(frame, left,    (int32_t)line_to_process1, 3, left_color);
        draw_filled_circle(frame, right,   (int32_t)line_to_process1, 3, right_color);
    }

    if (found2)
    {
        draw_filled_circle(frame, center2, (int32_t)line_to_process2, 4, center_color);
        draw_filled_circle(frame, left2,   (int32_t)line_to_process2, 3, left_color);
        draw_filled_circle(frame, right2,  (int32_t)line_to_process2, 3, right_color);
    }

    if (found3)
    {
        draw_filled_circle(frame, center3, (int32_t)line_to_process3, 4, center_color);
        draw_filled_circle(frame, left3,   (int32_t)line_to_process3, 3, left_color);
        draw_filled_circle(frame, right3,  (int32_t)line_to_process3, 3, right_color);
    }

    vision_test__update_overlay(frame, threshold);

    (void)telemetry_i32("vision.center1", found1 ? center1 : -1, "pixel");
    (void)telemetry_i32("vision.center2", found2 ? center2 : -1, "pixel");
    (void)telemetry_i32("vision.center3", found3 ? center3 : -1, "pixel");
    (void)telemetry_i32("vision.left1", found1 ? left : -1, "pixel");
    (void)telemetry_i32("vision.right1", found1 ? right : -1, "pixel");
    (void)telemetry_i32("vision.left2", found2 ? left2 : -1, "pixel");
    (void)telemetry_i32("vision.right2", found2 ? right2 : -1, "pixel");
    (void)telemetry_i32("vision.left3", found3 ? left3 : -1, "pixel");
    (void)telemetry_i32("vision.right3", found3 ? right3 : -1, "pixel");
    (void)telemetry_i32("vision.width1", found1 ? width_px : -1, "pixel");
    (void)telemetry_i32("vision.width2", found2 ? width_px2 : -1, "pixel");
    (void)telemetry_i32("vision.width3", found3 ? width_px3 : -1, "pixel");
    (void)telemetry_u32("vision.found1", found1 ? 1U : 0U, "bool");
    (void)telemetry_u32("vision.found2", found2 ? 1U : 0U, "bool");
    (void)telemetry_u32("vision.found3", found3 ? 1U : 0U, "bool");
    (void)telemetry_u32("vision.threshold", threshold, "luma");
}


static void vision_test__update_overlay(uint16_t *frame, uint8_t threshold)
{
    uint16_t text_color = color_rgb565(0U, 0xFFU, 0U);
    uint32_t cpu;

    /* Los pots se muestran en centesimos (50 = 0.50) para no depender de printf-float */
    snprintf(pot_text, sizeof(pot_text), "a:%d b:%d g:%d TH:%u",
             (int)(input_alpha() * 100.0f),
             (int)(input_beta() * 100.0f),
             (int)(input_gamma() * 100.0f),
             (unsigned)threshold);

    frame_draw_text(frame, 3, 3, pot_text, text_color);

    cpu = (frame_processing_microseconds() * 100U) / FRAME_BUDGET_MICROSECONDS;

    snprintf(pot_text, sizeof(pot_text), "CPU:%u%%", (unsigned)cpu);

    frame_draw_text(frame, 3, 13, pot_text, color_rgb565(0U, 192U, 0U));
}
