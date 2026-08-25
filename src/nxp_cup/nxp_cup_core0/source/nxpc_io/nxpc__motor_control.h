#ifndef NXPC__MOTOR_CONTROL_H_
#define NXPC__MOTOR_CONTROL_H_

#include "stdint.h"
#include "stdbool.h"

typedef struct
{
    bool invert_left_motor_direction;
    bool invert_right_motor_direction;
    bool outputs_enable;

    float left_intensity;
    float right_intensity;

} motor_ctrl_info_t;


extern void nxpc__motor_control_init();
extern void nxpc__set_motor_pwm(float left, float right);
extern void nxpc__enable_motor_control();
extern void nxpc__disable_motor_control();
bool nxpc__motor_control_enabled(void);
float nxpc__motor_left_command(void);
float nxpc__motor_right_command(void);
uint16_t nxpc__dc_to_counts(float new_dc, bool inverted_direction);

#endif
