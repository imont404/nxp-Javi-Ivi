#ifndef UTILS_H_
#define UTILS_H_
#include <stdint.h>
#include <stdbool.h>
#include "vision_test.h"
#include "nxp_cup.h"

bool white_center(const color_features_t *scaneline,
                uint16_t width,
                uint8_t threadhold,
                int32_t *center,
                int32_t *left_edge,
                int32_t *right_edge,
                int32_t *track_width);

void draw_filled_circle(uint16_t *frame, int32_t cx, int32_t cy, int32_t radius, uint16_t color);

void motor_control(int32_t center1, int32_t center2, int32_t center3,
                   bool found1, bool found2, bool found3, bool motors_on);

#endif /* UTILS_H_ */
