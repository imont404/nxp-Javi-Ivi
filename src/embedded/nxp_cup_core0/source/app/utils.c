#include "utils.h"
#include <stdint.h>
#include <stdbool.h>
#include "vision_test.h"
#include <math.h>


#define BASE_SPEED 0.2f
#define KP_HIGH 1.8f  /* abs_error > 0.5 */
#define KP_MID  1.2f  /* abs_error > 0.2 */
#define KP_LOW  1.0f  /* abs_error <= 0.2 */
#define KD_TURN 1.0f

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
        // Borde negro izquierda
        *left_edge = first;
        // Borde negro derecha
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

void pd_control(float error)
{
    static float p_error = 0.0f;

    float abs_error = fabsf(error);
    float kp_turn;
    float deriv;
    float turn;
    float left_pwm;
    float right_pwm;

    if (abs_error > 0.5f)
    {
        kp_turn = KP_HIGH;
    }
    else if (abs_error > 0.2f)
    {
        kp_turn = KP_MID;
    }
    else
    {
        kp_turn = KP_LOW;
    }

    deriv = error - p_error;
    p_error = error;

    turn = -kp_turn * error - KD_TURN * deriv - 0.1f;

    if (turn > 1.0f) { turn = 1.0f; }
    if (turn < -1.0f) { turn = -1.0f; }

    steering_set(turn);

    left_pwm = input_alpha() + turn + 0.1f;
    right_pwm = input_alpha() - turn - 0.1f;

    left_pwm = (left_pwm > 1.0f) ? 1.0f : ((left_pwm < 0.0f) ? 0.0f : left_pwm);
    right_pwm = (right_pwm > 1.0f) ? 1.0f : ((right_pwm < 0.0f) ? 0.0f : right_pwm);

    motors_set_duty(left_pwm, right_pwm);
}