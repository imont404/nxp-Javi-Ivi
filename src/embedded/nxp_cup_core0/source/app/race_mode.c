#include "race_mode.h"

#include <stdio.h>

#include "nxp_cup.h"
#include "utils.h"
#include "coins.h"


/* Un buffer por linea procesada, igual que line_hsl / line_hsl2 / line_hsl3 */
static color_features_t line_hsl[CAMERA_WIDTH];
static color_features_t line_hsl2[CAMERA_WIDTH];
static color_features_t line_hsl3[CAMERA_WIDTH];

static bool found1 = false;
static bool found2 = false;
static bool found3 = false;

static uint32_t line_to_process1, line_to_process2, line_to_process3;

/* Umbral de blanco fijo (0.50 * 255). La luz de pista ya no se ajusta con pot. */
#define LUMA_THRESHOLD 127U

static char pot_text[64];

static void race_mode__update_overlay(uint16_t *frame);


void race_mode_on_frame(uint16_t *frame)
{
    /* Seleccion de linea */
    line_to_process1 = 110U;
    line_to_process2 = 150U;
    line_to_process3 = 170U;

    /* Umbral de luminancia fijo: equivale a tener el pot en 0.50 */
    uint8_t threshold = LUMA_THRESHOLD;

    /* left/right delimitan la pista blanca: ahora si se usan, porque
     * coins_adjust_center los necesita para no mandar el objetivo fuera. */
    int32_t center1, left, right, width_px;
    int32_t center2, left2, right2, width_px2;
    int32_t center3, left3, right3, width_px3;

    coin_list_t coins1, coins2, coins3;
    int32_t adj;

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

    /* Ajuste por monedas: corre el centro hacia las amarillas/azules y lo
     * aparta de las rojas. Si la situacion es ambigua, coins_adjust_center
     * devuelve false y el centro original queda intacto. */
    if (found1)
    {
        coins_scan_row(line_hsl, CAMERA_WIDTH, &coins1);
        if (coins_adjust_center(&coins1, center1, left, right, &adj))
        {
            center1 = adj;
        }
    }

    if (found2)
    {
        coins_scan_row(line_hsl2, CAMERA_WIDTH, &coins2);
        if (coins_adjust_center(&coins2, center2, left2, right2, &adj))
        {
            center2 = adj;
        }
    }

    if (found3)
    {
        coins_scan_row(line_hsl3, CAMERA_WIDTH, &coins3);
        if (coins_adjust_center(&coins3, center3, left3, right3, &adj))
        {
            center3 = adj;
        }
    }

    /* Los circulos ya muestran el centro ajustado: sirve para ver en vivo
     * si el carro esta apuntando a las monedas buenas. */
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

    (void)width_px;
    (void)width_px2;
    (void)width_px3;

    motor_control(center1, center2, center3, found1, found2, found3, true);

    race_mode__update_overlay(frame);

    (void)telemetry_i32("vision.center1", found1 ? center1 : -1, "pixel");
    (void)telemetry_i32("vision.center2", found2 ? center2 : -1, "pixel");
    (void)telemetry_i32("vision.center3", found3 ? center3 : -1, "pixel");
    (void)telemetry_u32("vision.threshold", threshold, "luma");
    (void)telemetry_u32("coins.count1", coins1.count, "coins");
    (void)telemetry_u32("coins.count2", coins2.count, "coins");
    (void)telemetry_u32("coins.count3", coins3.count, "coins");
}

static void race_mode__update_overlay(uint16_t *frame)
{
    uint16_t text_color = color_rgb565(0U, 0xFFU, 0U);
    uint32_t cpu;

    /* vel=alpha, kd=beta, kp=gamma. En centesimos para no depender de printf-float */
    snprintf(pot_text, sizeof(pot_text), "vel:%d kd:%d kp:%d",
             (int)(input_alpha() * 100.0f),
             (int)(input_beta() * 100.0f),
             (int)(input_gamma() * 100.0f));

    frame_draw_text(frame, 3, 3, pot_text, text_color);

    cpu = (frame_processing_microseconds() * 100U) / FRAME_BUDGET_MICROSECONDS;

    snprintf(pot_text, sizeof(pot_text), "CPU:%u%%", (unsigned)cpu);

    frame_draw_text(frame, 3, 13, pot_text, color_rgb565(0U, 192U, 0U));
}