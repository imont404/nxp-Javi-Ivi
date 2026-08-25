#include "nxpc__adc.h"
#include "fsl_common.h"
#include "fsl_lpadc.h"
#include "fsl_vref.h"
#include "fsl_spc.h"
#include "fsl_debug_console.h"
#include "fsl_ctimer.h"
#include "fsl_inputmux.h"

// ******************* ADC0 required channels *******************
#define BATT_ADC_CHANNEL 0U
#define ALPHA_ADC_CHANNEL 14U
#define BETA_ADC_CHANNEL 10U
#define GAMMA_ADC_CHANNEL 13U

#define LPADC_VREF_SOURCE kLPADC_ReferenceVoltageAlt3
#define VREF_BASE VREF0

#define ADC_MAX_VALUE 4095

// *************************** CTIMER ***************************
#define CTIMER CTIMER0                 /* Timer 0 */
#define CTIMER_MAT_OUT kCTIMER_Match_3 /* Match output 0 */
#define CTIMER_EMT_OUT (1u << kCTIMER_Match_3)
#define CTIMER_CLK_FREQ CLOCK_GetCTimerClkFreq(0U)

static ctimer_config_t config;
static ctimer_match_config_t matchConfig;
static lpadc_conv_result_t mLpadcResultConfigStruct;

const uint32_t g_LpadcFullRange = 4096U;
const uint32_t g_LpadcResultShift = 3U;

#define FILTER_SIZE 16
/* Written by ADC0_IRQHandler and sampled by foreground code. */
static volatile uint16_t adc_values[NUM_ADC_CH][FILTER_SIZE];
static uint32_t filter_idx[NUM_ADC_CH];

void nxpc__adc_init(void)
{
    lpadc_config_t mLpadcConfigStruct;
    lpadc_conv_trigger_config_t mLpadcTriggerConfigStruct;
    lpadc_conv_command_config_t mLpadcCommandConfigStruct[4];
    vref_config_t vrefConfig;

    /* attach FRO HF to ADC0 */
    CLOCK_SetClkDiv(kCLOCK_DivAdc0Clk, 2U);
    CLOCK_AttachClk(kFRO_HF_to_ADC0);

    /* Use FRO HF clock for some of the Ctimers */
    CLOCK_SetClkDiv(kCLOCK_DivCtimer0Clk, 1u);
    CLOCK_AttachClk(kFRO_HF_to_CTIMER0);

    /* enable VREF */
    SPC_EnableActiveModeAnalogModules(SPC0, kSPC_controlVref);
    VREF_GetDefaultConfig(&vrefConfig);
    vrefConfig.bufferMode = kVREF_ModeBandgapOnly;
    VREF_Init(VREF_BASE,
              &vrefConfig); // The VREF module is only used to supply the bias current for LPADC

    LPADC_GetDefaultConfig(&mLpadcConfigStruct);
    mLpadcConfigStruct.enableAnalogPreliminary = true;
    mLpadcConfigStruct.referenceVoltageSource = LPADC_VREF_SOURCE;
    mLpadcConfigStruct.conversionAverageMode = kLPADC_ConversionAverage1024;

    LPADC_Init(ADC0, &mLpadcConfigStruct);
    LPADC_DoOffsetCalibration(
        ADC0); /* Request offset calibration, automatic update OFSTRIM register. */
    LPADC_DoAutoCalibration(ADC0);

    /* Set conversion CMD configuration. */
    LPADC_GetDefaultConvCommandConfig(&mLpadcCommandConfigStruct[0]);
    LPADC_GetDefaultConvCommandConfig(&mLpadcCommandConfigStruct[1]);
    LPADC_GetDefaultConvCommandConfig(&mLpadcCommandConfigStruct[2]);
    LPADC_GetDefaultConvCommandConfig(&mLpadcCommandConfigStruct[3]);

    mLpadcCommandConfigStruct[BATT_ADC_CH].channelNumber = BATT_ADC_CHANNEL;
    mLpadcCommandConfigStruct[BATT_ADC_CH].chainedNextCommandNumber = 2;

    mLpadcCommandConfigStruct[ALPHA_ADC_CH].channelBNumber = ALPHA_ADC_CHANNEL;
    mLpadcCommandConfigStruct[ALPHA_ADC_CH].enableChannelB = 1;
    mLpadcCommandConfigStruct[ALPHA_ADC_CH].sampleChannelMode = kLPADC_SampleChannelSingleEndSideB;
    mLpadcCommandConfigStruct[ALPHA_ADC_CH].sampleTimeMode = kLPADC_SampleTimeADCK67;
    mLpadcCommandConfigStruct[ALPHA_ADC_CH].chainedNextCommandNumber = 3;

    mLpadcCommandConfigStruct[BETA_ADC_CH].channelNumber = BETA_ADC_CHANNEL;
    mLpadcCommandConfigStruct[BETA_ADC_CH].chainedNextCommandNumber = 4;

    mLpadcCommandConfigStruct[GAMMA_ADC_CH].channelNumber = GAMMA_ADC_CHANNEL;

    LPADC_SetConvCommandConfig(ADC0, 1, &mLpadcCommandConfigStruct[BATT_ADC_CH]);
    LPADC_SetConvCommandConfig(ADC0, 2, &mLpadcCommandConfigStruct[ALPHA_ADC_CH]);
    LPADC_SetConvCommandConfig(ADC0, 3, &mLpadcCommandConfigStruct[BETA_ADC_CH]);
    LPADC_SetConvCommandConfig(ADC0, 4, &mLpadcCommandConfigStruct[GAMMA_ADC_CH]);

    // Set trigger configuration.
    LPADC_GetDefaultConvTriggerConfig(&mLpadcTriggerConfigStruct);
    mLpadcTriggerConfigStruct.targetCommandId = 1;
    mLpadcTriggerConfigStruct.enableHardwareTrigger = 1;
    LPADC_SetConvTriggerConfig(ADC0, 0U, &mLpadcTriggerConfigStruct);

    // ADC enable interrupts
    LPADC_EnableInterrupts(ADC0, kLPADC_FIFOWatermarkInterruptEnable);
    EnableIRQ(ADC0_IRQn);

    CTIMER_GetDefaultConfig(&config);
    CTIMER_Init(CTIMER, &config);

    matchConfig.enableCounterReset = true;
    matchConfig.enableCounterStop = false;
    matchConfig.matchValue = CTIMER_CLK_FREQ / 250 / 2;
    matchConfig.outControl = kCTIMER_Output_Toggle;
    matchConfig.outPinInitState = true;
    matchConfig.enableInterrupt = false;
    CTIMER_SetupMatch(CTIMER, CTIMER_MAT_OUT, &matchConfig);
    CTIMER_StartTimer(CTIMER);

    INPUTMUX_Init(INPUTMUX);
    INPUTMUX_AttachSignal(INPUTMUX, 0, kINPUTMUX_Ctimer0M3ToAdc0Trigger);
}

#define ADC_NORMALIZE(x) (1.0f - (((float)(x) / (float)ADC_MAX_VALUE)))

static uint32_t filter_channel(adc_channels_e channel)
{
    uint32_t interrupt_state;
    uint32_t sum = 0U;

    /* Take one coherent rolling-window snapshot. The 16 reads keep this
     * critical section
     * bounded while preventing O2 from caching ISR data. */
    interrupt_state = DisableGlobalIRQ();
    for (uint32_t i = 0U; i < FILTER_SIZE; i++)
    {
        sum += adc_values[channel][i];
    }
    EnableGlobalIRQ(interrupt_state);

    return sum / FILTER_SIZE;
}
// Return a [0 to 1.0]
float nxpc__read_alpha(void)
{
    return ADC_NORMALIZE(filter_channel(ALPHA_ADC_CH));
}

// Return a [0 to 1.0]
float nxpc__read_beta(void)
{
    return ADC_NORMALIZE(filter_channel(BETA_ADC_CH));
}

// Return a [0 to 1.0]
float nxpc__read_gamma(void)
{
    return ADC_NORMALIZE(filter_channel(GAMMA_ADC_CH));
}

uint16_t nxpc__read_battery_voltage(void)
{
    uint16_t v_adc;
    uint16_t v_batt;

    v_adc = (filter_channel(BATT_ADC_CH) * 330) / 4095; // Adc voltage multiplied by 100
    v_batt = v_adc * 11;                                // Because of voltag divider

    return v_batt;
}

void ADC0_IRQHandler(void)
{
    uint32_t channel;
    if (LPADC_GetConvResult(ADC0, &mLpadcResultConfigStruct, 0))
    {
        channel = mLpadcResultConfigStruct.commandIdSource - 1U;
        if (channel < NUM_ADC_CH)
        {
            adc_values[channel][filter_idx[channel]] =
                (uint16_t)(mLpadcResultConfigStruct.convValue >> g_LpadcResultShift);
            filter_idx[channel]++;
            if (filter_idx[channel] >= FILTER_SIZE)
            {
                filter_idx[channel] = 0U;
            }
        }
    }

    SDK_ISR_EXIT_BARRIER;
}
