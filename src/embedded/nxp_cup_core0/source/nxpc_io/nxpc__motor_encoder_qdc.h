#ifndef NXPC__MOTOR_ENCODER_QDC_H_
#define NXPC__MOTOR_ENCODER_QDC_H_

#include "stdbool.h"
#include "stdint.h"

typedef enum
{
    NXPC_MOTOR_ENCODER_M0 = 0,
    NXPC_MOTOR_ENCODER_M1 = 1,
    NXPC_MOTOR_ENCODER_COUNT = 2
} nxpc_motor_encoder_id_t;

typedef struct
{
    uint32_t position;
    int32_t delta;
    int32_t counts_per_second;
    int32_t rpm_milli;
    uint16_t imr;
    uint16_t flags_ctrl;
    uint16_t flags_ctrl2;
    bool direction_up;
} nxpc_motor_encoder_sample_t;

void nxpc__motor_encoder_qdc_init(void);
void nxpc__motor_encoder_qdc_zero(void);
void nxpc__motor_encoder_qdc_sample(uint32_t elapsed_ms,
                                    nxpc_motor_encoder_sample_t samples[NXPC_MOTOR_ENCODER_COUNT]);

#endif
