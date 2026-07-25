#ifndef AVC__MOTOR_ENCODER_QDC_H_
#define AVC__MOTOR_ENCODER_QDC_H_

#include "stdbool.h"
#include "stdint.h"

typedef enum
{
    AVC_MOTOR_ENCODER_M0 = 0,
    AVC_MOTOR_ENCODER_M1 = 1,
    AVC_MOTOR_ENCODER_COUNT = 2
} avc_motor_encoder_id_t;

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
} avc_motor_encoder_sample_t;

void avc__motor_encoder_qdc_init(void);
void avc__motor_encoder_qdc_zero(void);
void avc__motor_encoder_qdc_sample(uint32_t elapsed_ms,
                                   avc_motor_encoder_sample_t samples[AVC_MOTOR_ENCODER_COUNT]);
void avc__motor_encoder_qdc_diag_run(void);

#endif
