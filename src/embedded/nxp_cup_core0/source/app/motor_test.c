#include "motor_test.h"

#include "nxp_cup.h"

static float clamp_input(float value)
{
    if ((value != value) || (value < 0.0f))
    {
        return 0.0f;
    }
    if (value > 1.0f)
    {
        return 1.0f;
    }
    return value;
}

static float signed_input(float value)
{
    return (clamp_input(value) * 2.0f) - 1.0f;
}

void motor_test_on_frame(uint16_t *frame)
{
    float left = signed_input(input_alpha());
    float steering = signed_input(input_beta());
    float right = signed_input(input_gamma());

    (void)frame;

    /*
     * This is the same public actuator API used in race_mode.c. These calls
     * cannot move the car until the framework has selected MOTORS, observed a
     * deliberate EXE release, and confirmed that all three pots were centered.
     * The framework also caps TEST motor duty and enforces the command lease.
     */
    motors_set_duty(left, right);
    steering_set(steering);
}
