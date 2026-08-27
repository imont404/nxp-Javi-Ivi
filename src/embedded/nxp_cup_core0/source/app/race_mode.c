#include "race_mode.h"

#include "nxp_cup.h"
#include "utils.h"


static color_features_t scanline[CAMERA_WIDTH];

#define WEIGHT_NEAR 0.2f
#define WEIGHT_MID  0.3f
#define WEIGHT_FAR  0.5f



static bool scan_row_center(uint16_t *frame, uint32_t row, int32_t *center) {
    uint8_t luma_min = 255;
    uint8_t luma_max = 0;
    uint8_t threshold;
    int32_t left_edge, right_edge, track_width;

    color_convert_rgb565_to_yhsv(camera_row(frame, row), scanline, CAMERA_WIDTH);
    for (int x=0; x < CAMERA_WIDTH; x++) {
        if (scanline[x].y < luma_min) { luma_min = scanline[x].y; }
        if (scanline[x].y > luma_max) { luma_max = scanline[x].y; }
        
        
    }
    threshold = (uint8_t)(luma_min + (((uint32_t)luma_max - luma_min) * 8U) / 10U);
    return white_center(scanline, CAMERA_WIDTH, threshold, center, &left_edge, &right_edge, &track_width);

}

static bool combines_error(int32_t center_near, bool found_near,
                            int32_t center_mid, bool found_mid,
                            int32_t center_far, bool found_far,
                            float *out_error) {

    float center_img = (float)CAMERA_WIDTH/2.0f;
    float error_sum = 0.0f;
    float weight_sum = 0.0f;

    if (found_near) {
        float error_near = (center_img - (float)center_near)/center_img;
        error_sum += WEIGHT_NEAR*error_near;
        weight_sum+= WEIGHT_NEAR;
    } 
    if (found_mid) {
        float error_mid = (center_img - (float)center_mid)/center_img;
        error_sum += WEIGHT_MID*error_mid;
        weight_sum+= WEIGHT_MID;     
    }
    if (found_far) {
        float error_far = (center_img - (float)center_far)/center_img;
        error_sum += WEIGHT_FAR*error_far;
        weight_sum+= WEIGHT_FAR;   
    }

    if (weight_sum <= 0.0f){
        *out_error = 0.0f;
        return false;
    }

    *out_error = error_sum/weight_sum;
    return true;
}

void race_mode_on_frame(uint16_t *frame)
{
    /*
     * Build one bounded frame-to-control pass here:
     *
     * 1. Choose one or more rows with camera_row().
     * 2. Inspect RGB565 pixels or convert them with the color helpers.
     * 3. Apply your own analysis and vehicle-control decision.
     * 4. Command steering_set() and motors_set_duty().
     * 5. Publish a few useful values with the telemetry helpers.
     *
     * Return promptly. Each accepted motors_set_duty() call renews the 100 ms
     * dead-man lease; the framework stops the car if repeated callbacks exceed
     * the 41 ms frame budget. The starter intentionally supplies no driving
     * decision.
     */
    
    uint32_t row_base = (CAMERA_HEIGHT / 2U) + 8;
    uint32_t row_near = row_base;
    uint32_t row_mid = row_base - 25U;
    uint32_t row_far = row_base - 50U;

    int32_t center_near, center_mid, center_far;
    bool found_near, found_mid, found_far;
    float error;
    bool have_error;

    uint16_t center_color = color_rgb565(255U, 0U, 255U);

    found_near = scan_row_center(frame, row_near, &center_near);
    found_mid = scan_row_center(frame, row_mid, &center_mid);
    found_far = scan_row_center(frame, row_far, &center_far);

    have_error = combines_error(center_near, found_near,
                                 center_mid, found_mid,
                                 center_far, found_far,
                                 &error);
    
        if (!have_error)
    {
        /* Ninguna fila vio pista: por ahora, frenar en vez de adivinar.
         * Mas adelante se puede reemplazar por "mantener el ultimo error
         * conocido" para buscar la pista en vez de detenerse en seco. */
        motors_set_duty(0.0f, 0.0f);
        steering_set(0.0f);

        (void)telemetry_i32("vision.center_near", -1, "pixel");
        (void)telemetry_i32("vision.center_mid", -1, "pixel");
        (void)telemetry_i32("vision.center_far", -1, "pixel");
        (void)telemetry_f32("control.error", 0.0f, "ratio");
        return;
    }
    
    if (found_near) { draw_filled_circle(frame, center_near, (int32_t)row_near, 4, center_color); }
    if (found_mid) { draw_filled_circle(frame, center_mid, (int32_t)row_mid, 4, center_color); }
    if (found_far) { draw_filled_circle(frame, center_far, (int32_t)row_far, 4, center_color); }

    pd_control(error);

    (void)telemetry_i32("vision.center_near", found_near ? center_near : -1, "pixel");
    (void)telemetry_i32("vision.center_mid", found_mid ? center_mid : -1, "pixel");
    (void)telemetry_i32("vision.center_far", found_far ? center_far : -1, "pixel");
    (void)telemetry_f32("control.error", error, "ratio");
}
