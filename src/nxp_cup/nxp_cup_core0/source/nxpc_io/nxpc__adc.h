#ifndef NXPC__ADC_H_
#define NXPC__ADC_H_

#include "inttypes.h"

typedef enum{
    BATT_ADC_CH,
    ALPHA_ADC_CH,
    BETA_ADC_CH,
    GAMMA_ADC_CH,
    NUM_ADC_CH
} adc_channels_e;

extern void nxpc__adc_init();


extern float nxpc__read_alpha();
extern float nxpc__read_beta();
extern float nxpc__read_gamma();

extern uint16_t nxpc__read_battery_voltage();

#endif
