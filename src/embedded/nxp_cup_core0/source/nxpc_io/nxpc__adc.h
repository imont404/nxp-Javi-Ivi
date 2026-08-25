#ifndef NXPC__ADC_H_
#define NXPC__ADC_H_

#include "inttypes.h"

typedef enum
{
    BATT_ADC_CH,
    ALPHA_ADC_CH,
    BETA_ADC_CH,
    GAMMA_ADC_CH,
    NUM_ADC_CH
} adc_channels_e;

void nxpc__adc_init(void);

float nxpc__read_alpha(void);
float nxpc__read_beta(void);
float nxpc__read_gamma(void);

uint16_t nxpc__read_battery_voltage(void);

#endif
