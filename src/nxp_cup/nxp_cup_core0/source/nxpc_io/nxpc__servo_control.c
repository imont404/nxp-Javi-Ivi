#include "nxpc__servo_control.h"
#include "fsl_pwm.h"
#include "fsl_port.h"
#include "e_debug.h"

#define PWM_SRC_CLK_FREQ            CLOCK_GetFreq(kCLOCK_BusClk)
#define SERVO_DC_CURVE_SLOPE        (65535/4000)

static float g_servo_command;

/*
 * Servo PWM output selection. See CONFIG__SERVO_PWM_OUTPUT in
 * nxpc__master_config.h and docs/research/AVC_RevB_Servo_PWM_Options.md.
 *
 * Only the submodule, channel, and pin differ; the timing setup below is
 * identical either way.
 */
#if CONFIG__SERVO_PWM_OUTPUT == SERVO_PWM_OUTPUT_P3_20_A3

/* Rev B candidate: PWM1 submodule 3 channel A on P3_20, shield J1.5. */
#define SERVO_PWM_SUBMODULE         kPWM_Module_3
#define SERVO_PWM_CONTROL_SUBMODULE kPWM_Control_Module_3
#define SERVO_PWM_CHANNEL           kPWM_PwmA
#define SERVO_PWM_SUBCTL_CLK_MASK   SYSCON_PWM1SUBCTL_CLK3_EN_MASK
#define SERVO_PWM_PORT              PORT3
#define SERVO_PWM_PIN               (20U)
#define SERVO_PWM_PIN_ALT           kPORT_MuxAlt5
#define SERVO_PWM_NAME              "P3_20/PWM1_A3 (shield J1.5)"

#else

/* Rev A as built: PWM1 submodule 2 channel B on P2_3, shield J3.5. */
#define SERVO_PWM_SUBMODULE         kPWM_Module_2
#define SERVO_PWM_CONTROL_SUBMODULE kPWM_Control_Module_2
#define SERVO_PWM_CHANNEL           kPWM_PwmB
#define SERVO_PWM_SUBCTL_CLK_MASK   SYSCON_PWM1SUBCTL_CLK2_EN_MASK
#define SERVO_PWM_PORT              PORT2
#define SERVO_PWM_PIN               (3U)
#define SERVO_PWM_PIN_ALT           kPORT_MuxAlt5
#define SERVO_PWM_NAME              "P2_3/PWM1_B2 (shield J3.5)"

#endif

void nxpc__servo_control_init()
{
    pwm_config_t pwmConfig;
    pwm_signal_param_t pwmSignal_single;

    /*
     * Mux the output pin. The Rev A pin was already muxed by BOARD_InitPins;
     * doing it here keeps the pin choice with the rest of the servo setup so
     * the two options cannot drift apart.
     */
    CLOCK_EnableClock(kCLOCK_Port2);
    CLOCK_EnableClock(kCLOCK_Port3);
    PORT_SetPinMux(SERVO_PWM_PORT, SERVO_PWM_PIN, SERVO_PWM_PIN_ALT);

    /* Enable PWM1 SUB Clockn */
    SYSCON->PWM1SUBCTL |= SERVO_PWM_SUBCTL_CLK_MASK;

    PWM_GetDefaultConfig(&pwmConfig);
    pwmConfig.reloadLogic = kPWM_ReloadPwmFullCycle;
    pwmConfig.enableDebugMode = true;
    pwmConfig.prescale = kPWM_Prescale_Divide_64;
    pwmConfig.pairOperation   = kPWM_Independent;
    PWM_Init(PWM1, SERVO_PWM_SUBMODULE, &pwmConfig);

    pwmSignal_single.pwmChannel       = SERVO_PWM_CHANNEL;
    pwmSignal_single.level            = kPWM_HighTrue;
    pwmSignal_single.dutyCyclePercent = 5;
    pwmSignal_single.faultState       = kPWM_PwmFaultState0;
    pwmSignal_single.pwmchannelenable = true;

    /*********** PWM1 submodule, configuration, setup 1 channel ************/
    PWM_SetupPwm(PWM1, SERVO_PWM_SUBMODULE, &pwmSignal_single, 1, kPWM_SignedCenterAligned, 50U,
                 PWM_SRC_CLK_FREQ);

    PWM_SetPwmLdok(PWM1, SERVO_PWM_CONTROL_SUBMODULE, true);
    PWM_StartTimer(PWM1, SERVO_PWM_CONTROL_SUBMODULE);

    nxpc__set_servo(0);

    DEBUG("Servo PWM output: " SERVO_PWM_NAME " at 50 Hz\r\n");
}

void nxpc__set_servo(float position)
{
	if(position>1.0f)
		position=1.0f;
	else if(position<-1.0f)
		position=-1.0f;

    g_servo_command = position;

    uint16_t position_counts = (65535 * (((int32_t)(position*100.0f)) + 300)) / 4000;

    PWM_UpdatePwmDutycycleHighAccuracy(PWM1,
                                        SERVO_PWM_SUBMODULE,
                                        SERVO_PWM_CHANNEL,
                                        kPWM_SignedCenterAligned,
                                        position_counts);

    PWM_SetPwmLdok(PWM1, SERVO_PWM_CONTROL_SUBMODULE, true);
}

float nxpc__servo_command(void)
{
    return g_servo_command;
}
