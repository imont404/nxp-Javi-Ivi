#include "vision_test.h"

#include <stdio.h>

#include "nxp_cup.h"
#include "utils.h"
#include "coins.h"


/* Misma configuracion que race_mode.c: un buffer por linea procesada */
static color_features_t line_hsl[CAMERA_WIDTH];
static color_features_t line_hsl2[CAMERA_WIDTH];
static color_features_t line_hsl3[CAMERA_WIDTH];

static bool found1 = false;
static bool found2 = false;
static bool found3 = false;

static uint32_t line_to_process1, line_to_process2, line_to_process3;

/* Mismo umbral fijo que race_mode.c (0.50 * 255) */
#define LUMA_THRESHOLD 127U

static char pot_text[64];

static void vision_test__update_overlay(uint16_t *frame, uint8_t threshold);
static uint16_t coin_marker_color(coin_color_t c);


void vision_test_on_frame(uint16_t *frame)
{
    /* Seleccion de linea: iguales a las de race_mode.c */
    line_to_process1 = 110U;
    line_to_process2 = 150U;
    line_to_process3 = 170U;

    /* Umbral de luminancia fijo: equivale a tener el pot en 0.50 */
    uint8_t threshold = (uint8_t)(input_beta() * 255.0f);

    int32_t center1, left, right, width_px;
    int32_t center2, left2, right2, width_px2;
    int32_t center3, left3, right3, width_px3;

    int32_t adjusted1 = -1, adjusted2 = -1, adjusted3 = -1;
    coin_list_t coins1, coins2, coins3;

    uint16_t center_color = color_rgb565(255U, 0U, 0U);     /* centro de masa normal */
    uint16_t left_color = color_rgb565(0U, 255U, 0U);       /* primer pixel blanco */
    uint16_t right_color = color_rgb565(0U, 128U, 255U);    /* ultimo pixel blanco */
    uint16_t scan_color = color_rgb565(255U, 255U, 0U);     /* marca de la fila escaneada */
    uint16_t adjusted_color = color_rgb565(0U, 255U, 128U); /* centro ya ajustado por monedas */

    coins1.count = 0U;
    coins2.count = 0U;
    coins3.count = 0U;

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

    /* Deteccion de monedas y ajuste del centro por fila */
    if (found1)
    {
        coins_scan_row(line_hsl, CAMERA_WIDTH, &coins1);
        adjusted1 = center1;
        (void)coins_adjust_center(&coins1, center1, left, right, &adjusted1);
    }

    if (found2)
    {
        coins_scan_row(line_hsl2, CAMERA_WIDTH, &coins2);
        adjusted2 = center2;
        (void)coins_adjust_center(&coins2, center2, left2, right2, &adjusted2);
    }

    if (found3)
    {
        coins_scan_row(line_hsl3, CAMERA_WIDTH, &coins3);
        adjusted3 = center3;
        (void)coins_adjust_center(&coins3, center3, left3, right3, &adjusted3);
    }

    /* Las filas se dibujan despues de escanear para no contaminar el dato */
    frame_draw_horizontal_line(frame, 0, CAMERA_WIDTH - 1U, (int32_t)line_to_process1, scan_color);
    frame_draw_horizontal_line(frame, 0, CAMERA_WIDTH - 1U, (int32_t)line_to_process2, scan_color);
    frame_draw_horizontal_line(frame, 0, CAMERA_WIDTH - 1U, (int32_t)line_to_process3, scan_color);

    if (found1)
    {
        draw_filled_circle(frame, center1, (int32_t)line_to_process1, 4, center_color);
        draw_filled_circle(frame, left,    (int32_t)line_to_process1, 3, left_color);
        draw_filled_circle(frame, right,   (int32_t)line_to_process1, 3, right_color);

        for (uint8_t i = 0U; i < coins1.count; i++)
        {
            draw_filled_circle(frame, coins1.items[i].center, (int32_t)line_to_process1,
                                5, coin_marker_color(coins1.items[i].color));
        }
        draw_filled_circle(frame, adjusted1, (int32_t)line_to_process1, 2, adjusted_color);
    }

    if (found2)
    {
        draw_filled_circle(frame, center2, (int32_t)line_to_process2, 4, center_color);
        draw_filled_circle(frame, left2,   (int32_t)line_to_process2, 3, left_color);
        draw_filled_circle(frame, right2,  (int32_t)line_to_process2, 3, right_color);

        for (uint8_t i = 0U; i < coins2.count; i++)
        {
            draw_filled_circle(frame, coins2.items[i].center, (int32_t)line_to_process2,
                                5, coin_marker_color(coins2.items[i].color));
        }
        draw_filled_circle(frame, adjusted2, (int32_t)line_to_process2, 2, adjusted_color);
    }

    if (found3)
    {
        draw_filled_circle(frame, center3, (int32_t)line_to_process3, 4, center_color);
        draw_filled_circle(frame, left3,   (int32_t)line_to_process3, 3, left_color);
        draw_filled_circle(frame, right3,  (int32_t)line_to_process3, 3, right_color);

        for (uint8_t i = 0U; i < coins3.count; i++)
        {
            draw_filled_circle(frame, coins3.items[i].center, (int32_t)line_to_process3,
                                5, coin_marker_color(coins3.items[i].color));
        }
        draw_filled_circle(frame, adjusted3, (int32_t)line_to_process3, 2, adjusted_color);
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
    (void)telemetry_i32("coins.adjusted1", adjusted1, "pixel");
    (void)telemetry_i32("coins.adjusted2", adjusted2, "pixel");
    (void)telemetry_i32("coins.adjusted3", adjusted3, "pixel");
    (void)telemetry_u32("coins.count1", coins1.count, "coins");
    (void)telemetry_u32("coins.count2", coins2.count, "coins");
    (void)telemetry_u32("coins.count3", coins3.count, "coins");
}


static uint16_t coin_marker_color(coin_color_t c)
{
    switch (c)
    {
    case COIN_RED:    return color_rgb565(255U, 0U, 0U);
    case COIN_YELLOW: return color_rgb565(255U, 255U, 0U);
    case COIN_BLUE:   return color_rgb565(0U, 128U, 255U);
    default:          return color_rgb565(128U, 128U, 128U);
    }
}


static void vision_test__update_overlay(uint16_t *frame, uint8_t threshold)
{
    uint16_t text_color = color_rgb565(0U, 0xFFU, 0U);
    uint32_t cpu;

    /* vel=alpha, kd=beta, kp=gamma. En centesimos para no depender de printf-float */
    snprintf(pot_text, sizeof(pot_text), "vel:%d kd:%d kp:%d TH:%u",
             (int)(input_alpha() * 100.0f),
             (int)(input_beta() * 100.0f),
             (int)(input_gamma() * 100.0f),
             (unsigned)threshold);

    frame_draw_text(frame, 3, 3, pot_text, text_color);

    cpu = (frame_processing_microseconds() * 100U) / FRAME_BUDGET_MICROSECONDS;

    snprintf(pot_text, sizeof(pot_text), "CPU:%u%%", (unsigned)cpu);

    frame_draw_text(frame, 3, 13, pot_text, color_rgb565(0U, 192U, 0U));
}