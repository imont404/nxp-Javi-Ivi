#include "race_mode.h"

#include "nxp_cup.h"

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
    (void)frame;
}
