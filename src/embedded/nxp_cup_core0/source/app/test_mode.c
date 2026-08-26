#include "test_mode.h"

#include "camera_test.h"
#include "motor_test.h"
#include "nxp_cup.h"
#include "vision_test.h"

/*
 * The framework calls this automatically for each accepted camera frame while
 * the TEST jumper is installed. Students do not call this function themselves.
 * The framework also owns page navigation and all actuator safety gates.
 */
void test_mode_on_frame(uint16_t *frame)
{
    switch (test_mode_page())
    {
    case TEST_MODE_CAMERA_IO:
        camera_test_on_frame(frame);
        break;

    case TEST_MODE_VISION:
        vision_test_on_frame(frame);
        break;

    case TEST_MODE_MOTORS:
        motor_test_on_frame(frame);
        break;

    default:
        break;
    }
}
