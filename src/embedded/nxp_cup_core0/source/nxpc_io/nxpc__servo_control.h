#ifndef NXPC__SERVO_CONTROL_H_
#define NXPC__SERVO_CONTROL_H_

#include "stdint.h"

extern void nxpc__servo_control_init();

// position is -1.0f to 1.0f
void nxpc__set_servo(float position);
float nxpc__servo_command(void);

#endif
