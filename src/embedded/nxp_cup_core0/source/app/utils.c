#include "utils.h"
#include <stdint.h>
#include <stdbool.h>
#include "vision_test.h"
#include <math.h>


static float p_error = 0.0f;

/*
 * Pots: alpha = velocidad, beta = derivativo (kd), gamma = proporcional (kp).
 * beta 0.0-1.0 se escala a 0.0-KD_MAX (beta en 0.40 da el 1.2 de antes).
 */
#define KD_MAX 3.0f

/*
 * Velocidad variable.
 *
 * alpha marca la velocidad MAXIMA (la de recta). La fila lejana (center1) ve
 * un tramo que el carro todavia no alcanzo, asi que su error avisa de la
 * curva ANTES que las cercanas y permite bajar un poco al entrar.
 *
 * Para desactivar el frenado en curva: SPEED_MIN_SCALE en 1.0f.
 */
#define SPEED_MAX_SCALE   1.0f  /* recta: exactamente la velocidad del pot */
#define SPEED_MIN_SCALE   0.85f /* curva: fraccion del pot. 1.0 = sin frenar */
#define SPEED_CURVE_GAIN  0.4f  /* cuanto frena por unidad de error adelante */
#define SPEED_ACCEL_RATE  0.35f /* que tan rapido acelera (frenar es inmediato) */
#define SPEED_DEADBAND_PX 5.0f  /* margen en pixeles a cada lado: se considera recta */

/*
 * Ventana a la que se mapea alpha. El techo deja lugar para el +0.1 del
 * diferencial: SPEED_POT_MAX + 0.1 debe quedar en 1.0 o menos.
 */
#define SPEED_POT_MIN     0.15f /* velocidad maxima con alpha al minimo */
#define SPEED_POT_MAX     0.90f /* velocidad maxima con alpha al maximo */

static float speed_smooth = 1.0f;


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


void motor_control(int32_t center1, int32_t center2, int32_t center3, int32_t width_px,
                   bool found1, bool found2, bool found3, bool motors_on)
{
    float kp_turn;

    (void)width_px;

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
    float error_far = 0.0f;

    if (found1) {
        float error1 = (center_img - (float)center1) / center_img;
        error += 0.2f * error1;
        weight_sum += 0.2f;
        error_far = error1;
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
    float kd_turn = input_beta() * KD_MAX;

    float turn = -kp_turn * error - kd_turn * deriv;
    turn *= 1.4f;

    if (turn > 1.0f) turn = 1.0f;
    if (turn < -1.0f) turn = -1.0f;

    steering_set(turn);

    /*
     * Si la fila lejana ve pista, su error decide la velocidad. Si no la ve
     * (curva muy cerrada: la pista se sale del cuadro por arriba), se usa el
     * error combinado, que en ese caso tambien esta indicando curva.
     */
    float look_ahead = found1 ? fabsf(error_far) : abs_error;

    /* Margen: no hace falta estar alineado al 100% para acelerar.
     * El error viene normalizado contra center_img, asi que los pixeles
     * del margen se convierten a esa misma escala. */
    float deadband = SPEED_DEADBAND_PX / center_img;

    if (look_ahead < deadband) {
        look_ahead = 0.0f;
    } else {
        look_ahead -= deadband;
    }

    float speed_scale = SPEED_MAX_SCALE - (SPEED_CURVE_GAIN * look_ahead);

    if (speed_scale > SPEED_MAX_SCALE) { speed_scale = SPEED_MAX_SCALE; }
    if (speed_scale < SPEED_MIN_SCALE) { speed_scale = SPEED_MIN_SCALE; }

    /* Frenar es inmediato; acelerar es gradual para no patinar al salir */
    if (speed_scale < speed_smooth) {
        speed_smooth = speed_scale;
    } else {
        speed_smooth += SPEED_ACCEL_RATE * (speed_scale - speed_smooth);
    }

    /* alpha mapeado a la ventana de velocidad: todo el recorrido del pot sirve */
    float top_speed = input_alpha() + 0.2f;
    
    float bs = top_speed ;

    float left_pwm = bs + turn + 0.1f;
    float right_pwm = bs - turn - 0.1f;

    left_pwm  = (left_pwm > 1.0f) ? 1.0f : ((left_pwm < 0.0f) ? 0.0f : left_pwm);
    right_pwm = (right_pwm > 1.0f) ? 1.0f : ((right_pwm < 0.0f) ? 0.0f : right_pwm);

    motors_set_duty(left_pwm, right_pwm);

    (void)telemetry_f32("control.error", error, "ratio");
    (void)telemetry_f32("control.turn", turn, "ratio");
    (void)telemetry_f32("control.look_ahead", look_ahead, "ratio");
    (void)telemetry_f32("control.speed_scale", speed_smooth, "ratio");
    (void)telemetry_f32("control.base_speed", bs, "duty");
    (void)telemetry_f32("control.kp", kp_turn, "gain");
    (void)telemetry_f32("control.kd", kd_turn, "gain");
}
