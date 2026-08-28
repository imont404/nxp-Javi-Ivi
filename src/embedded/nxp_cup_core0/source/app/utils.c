#include "utils.h"
#include <stdint.h>
#include <stdbool.h>
#include "vision_test.h"
#include <math.h>


static float p_error = 0.0f;

/*
 * Pots: alpha = velocidad, beta = derivativo (kd), gamma = proporcional (kp).
 * beta 0.0-1.0 se escala a 0.0-KD_MAX porque el pot solo llega a 1.0 y el
 * derivativo que andaba bien (1.2) necesitaba mas rango que eso. Con
 * KD_MAX=3.0f, beta a 0.40 reproduce el 1.2 fijo que tenia antes.
 */
#define KD_MAX 3.0f


bool white_center(const color_features_t *scaneline,
                uint16_t width,
                uint8_t threadhold,
                int32_t *center,
                int32_t *left_edge,
                int32_t *right_edge,
                int32_t *track_width) {

    int sum = 0;
    int count = 0;
    int first = -1;
    int last = -1;

    /* Escanear linea para pixeles blancos */
    for(int x=0; x<width; x++){
        // Detectar primer pixel blanco
        if(scaneline[x].y > threadhold) {
            if(first<0){
                first=x;
            }
            last = x;
            sum+=x;
            count++;
        }

    }

    if (count>0){
        // Centro de masa
        *center = sum/count;
        // Primer pixel blanco
        *left_edge = first;
        // Ultimo pixel blanco
        *right_edge = last;
        // Ancho total de la pista
        *track_width = *right_edge - *left_edge;
        return true;
    } else {
        *center = -1;
        *left_edge = -1;
        *right_edge = -1;
        *track_width = -1;

        return false;
    }

}


void draw_filled_circle(uint16_t *frame, int32_t cx, int32_t cy, int32_t radius, uint16_t color)
{
    for (int32_t dy = -radius; dy <= radius; dy++)
    {
        int32_t y = cy + dy;

        if (y < 0 || y >= (int32_t)CAMERA_HEIGHT)
        {
            continue;
        }

        int32_t dx = (int32_t)((float)radius * (float)sqrt(1.0 - ((double)dy * dy) / ((double)radius * radius)));
        int32_t x0 = cx - dx;
        int32_t x1 = cx + dx;

        if (x0 < 0) { x0 = 0; }
        if (x1 >= (int32_t)CAMERA_WIDTH) { x1 = (int32_t)CAMERA_WIDTH - 1; }

        frame_draw_horizontal_line(frame, (uint32_t)x0, (uint32_t)x1, y, color);
    }
}


void motor_control(int32_t center1, int32_t center2, int32_t center3,
                   bool found1, bool found2, bool found3, bool motors_on)
{
    float kp_turn;

    if (!motors_on){
        motors_set_duty(0.0f, 0.0f);
        return;
    }

    int total_found = (found1 ? 1 : 0) + (found2 ? 1 : 0) + (found3 ? 1 : 0);
    if(total_found == 0){
        motors_set_duty(0.0f, 0.0f);
        return;
    }

    float center_img = (float)(CAMERA_WIDTH / 2);
    float error = 0.0f;
    float weight_sum = 0.0f;

    if (found1) {
        float error1 = (center_img - (float)center1) / center_img;
        error += 0.2f * error1;
        weight_sum += 0.2f;
    }
    if (found2) {
        float error2 = (center_img - (float)center2) / center_img;
        error += 0.3f * error2;
        weight_sum += 0.3f;
    }
    if (found3) {
        float error3 = (center_img - (float)center3) / center_img;
        error += 0.5f * error3;
        weight_sum += 0.5f;
    }

    error /= weight_sum;
    float abs_error = fabsf(error);

    if (abs_error > 0.5f) {
        kp_turn = input_gamma() + 1.8f;
    } else if (abs_error > 0.2f) {
        kp_turn = input_gamma() + 1.2f;
    } else {
        kp_turn = input_gamma() + 1.0f;
    }

    float deriv = error - p_error;
    p_error = error;

    /* beta ajusta el derivativo en vivo */
   // float kd_turn = input_beta() * KD_MAX;
   float kd_turn = input_beta() * KD_MAX;

    float turn = -kp_turn * error - kd_turn * deriv;
    turn *= 1.4f;

    if (turn > 1.0f) turn = 1.0f;
    if (turn < -1.0f) turn = -1.0f;

    steering_set(turn);

    /* Velocidad constante: el carro gira bien a fondo, sin frenar en curva */
    float bs = input_alpha() + 0.2f;

    float left_pwm = bs + turn + 0.1f;
    float right_pwm = bs - turn - 0.1f;

    left_pwm  = (left_pwm > 1.0f) ? 1.0f : ((left_pwm < 0.0f) ? 0.0f : left_pwm);
    right_pwm = (right_pwm > 1.0f) ? 1.0f : ((right_pwm < 0.0f) ? 0.0f : right_pwm);

    motors_set_duty(left_pwm, right_pwm);

    (void)telemetry_f32("control.error", error, "ratio");
    (void)telemetry_f32("control.turn", turn, "ratio");
    (void)telemetry_f32("control.base_speed", bs, "duty");
    (void)telemetry_f32("control.kp", kp_turn, "gain");
    (void)telemetry_f32("control.kd", kd_turn, "gain");
}
