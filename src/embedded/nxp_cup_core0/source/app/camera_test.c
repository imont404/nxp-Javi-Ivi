#include "camera_test.h"

#include "nxp_cup.h"

void camera_test_on_frame(uint16_t *frame)
{
    (void)frame;

    /*
     * CAMERA / IO is a safe place to inspect the common input API. The
     * framework draws the camera and status strip; these values are also
     * useful to host tools while checking a car.
     */
    (void)telemetry_f32("input.alpha", input_alpha(), "ratio");
    (void)telemetry_f32("input.beta", input_beta(), "ratio");
    (void)telemetry_f32("input.gamma", input_gamma(), "ratio");
    (void)telemetry_f32("battery.voltage", battery_voltage(), "V");
}
