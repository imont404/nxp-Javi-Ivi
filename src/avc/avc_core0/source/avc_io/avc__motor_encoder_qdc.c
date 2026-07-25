#include "avc__motor_encoder_qdc.h"

#include "avc__io.h"
#include "fsl_clock.h"
#include "fsl_inputmux.h"
#include "fsl_inputmux_connections.h"
#include "fsl_port.h"
#include "fsl_reset.h"

#if CONFIG__MOTOR_ENCODER_BACKEND == MOTOR_ENCODER_BACKEND_QDC

#define AVC_QDC_CTRL_W1C_FLAGS \
    (QDC_CTRL_HIRQ_MASK | QDC_CTRL_XIRQ_MASK | QDC_CTRL_DIRQ_MASK | QDC_CTRL_CMPIRQ_MASK)

#define AVC_QDC_CTRL2_W1C_FLAGS \
    (QDC_CTRL2_SABIRQ_MASK | QDC_CTRL2_ROIRQ_MASK | QDC_CTRL2_RUIRQ_MASK)

static QDC_Type *const s_qdc_base[AVC_MOTOR_ENCODER_COUNT] = {QDC0, QDC1};
static uint32_t s_last_position[AVC_MOTOR_ENCODER_COUNT];
static bool s_diag_motors_enabled;

static uint32_t avc__qdc_read_position(QDC_Type *base)
{
    uint32_t position = base->UPOS;

    position <<= 16U;
    position |= base->LPOSH;

    return position;
}

static void avc__qdc_clear_flags(QDC_Type *base)
{
    base->CTRL = (uint16_t)((base->CTRL & (uint16_t)(~(uint16_t)AVC_QDC_CTRL_W1C_FLAGS)) |
                            AVC_QDC_CTRL_W1C_FLAGS);
    base->CTRL2 = (uint16_t)((base->CTRL2 & (uint16_t)(~(uint16_t)AVC_QDC_CTRL2_W1C_FLAGS)) |
                             AVC_QDC_CTRL2_W1C_FLAGS);
}

static void avc__qdc_zero(QDC_Type *base)
{
    base->UINIT = 0U;
    base->LINIT = 0U;
    base->CTRL = (uint16_t)((base->CTRL & (uint16_t)(~(uint16_t)AVC_QDC_CTRL_W1C_FLAGS)) |
                            QDC_CTRL_SWIP_MASK);
}

static void avc__qdc_init_block(QDC_Type *base, bool invert)
{
    /*
     * CTRL[REV] flips the counting direction in hardware, so forward motion
     * reads positive and the position register counts up from zero rather than
     * wrapping down through 0xFFFFFFFF. See CONFIG__MOTOR_ENCODER_INVERT_M0/M1.
     */
    base->CTRL = invert ? QDC_CTRL_REV(1U) : 0U;
    base->FILT = 0U;
    base->WTR = 0U;
    base->POSD = 0U;
    base->REV = 0U;
    base->UINIT = 0U;
    base->LINIT = 0U;
    base->UMOD = 0xFFFFU;
    base->LMOD = 0xFFFFU;
    base->UCOMP = 0xFFFFU;
    base->LCOMP = 0xFFFFU;
    base->TST = 0U;
    base->CTRL2 = 0U;
    base->CTRL3 = 0U;

    avc__qdc_clear_flags(base);
    avc__qdc_zero(base);
}

static void avc__motor_encoder_qdc_configure_pins(void)
{
    const port_pin_config_t encoder_pin_config = {
        .pullSelect = kPORT_PullUp,
        .pullValueSelect = kPORT_LowPullResistor,
        .slewRate = kPORT_FastSlewRate,
        .passiveFilterEnable = kPORT_PassiveFilterDisable,
        .openDrainEnable = kPORT_OpenDrainDisable,
        .driveStrength = kPORT_LowDriveStrength,
        .mux = kPORT_MuxAlt1,
        .inputBuffer = kPORT_InputBufferEnable,
        .invertInput = kPORT_InputNormal,
        .lockRegister = kPORT_UnlockRegister,
    };

    CLOCK_EnableClock(kCLOCK_Gpio1);
    CLOCK_EnableClock(kCLOCK_Gpio2);
    CLOCK_EnableClock(kCLOCK_Port1);
    CLOCK_EnableClock(kCLOCK_Port2);

    PORT_SetPinConfig(PORT1, 0U, &encoder_pin_config);  /* J17/M0 encoder A: P1_0/TRIG_IN0 */
    PORT_SetPinConfig(PORT1, 1U, &encoder_pin_config);  /* J17/M0 encoder B: P1_1/TRIG_IN1 */
    PORT_SetPinConfig(PORT1, 22U, &encoder_pin_config); /* J11/M1 encoder A: P1_22/TRIG_IN3 */
    PORT_SetPinConfig(PORT2, 0U, &encoder_pin_config);  /* J11/M1 encoder B: P2_0/TRIG_IN5 */
}

void avc__motor_encoder_qdc_init(void)
{
    avc__motor_encoder_qdc_configure_pins();

    INPUTMUX_Init(INPUTMUX);
    INPUTMUX_AttachSignal(INPUTMUX, 0U, kINPUTMUX_TrigIn0ToQdc0Phasea);
    INPUTMUX_AttachSignal(INPUTMUX, 0U, kINPUTMUX_TrigIn1ToQdc0Phaseb);
    INPUTMUX_AttachSignal(INPUTMUX, 0U, kINPUTMUX_TrigIn3ToQdc1Phasea);
    INPUTMUX_AttachSignal(INPUTMUX, 0U, kINPUTMUX_TrigIn5ToQdc1Phaseb);

    CLOCK_EnableClock(kCLOCK_Qdc0);
    CLOCK_EnableClock(kCLOCK_Qdc1);
    RESET_ReleasePeripheralReset(kQDC0_RST_SHIFT_RSTn);
    RESET_ReleasePeripheralReset(kQDC1_RST_SHIFT_RSTn);

    avc__qdc_init_block(QDC0, CONFIG__MOTOR_ENCODER_INVERT_M0 != 0);
    avc__qdc_init_block(QDC1, CONFIG__MOTOR_ENCODER_INVERT_M1 != 0);

    s_last_position[AVC_MOTOR_ENCODER_M0] = avc__qdc_read_position(QDC0);
    s_last_position[AVC_MOTOR_ENCODER_M1] = avc__qdc_read_position(QDC1);

    DEBUG("Motor encoder QDC route: J17/M0 P1_0/P1_1 -> QDC0, J11/M1 P1_22/P2_0 -> QDC1\r\n");
    DEBUG("Motor encoder QDC polarity: invert m0=%u m1=%u (forward reads positive)\r\n",
          (unsigned int)(CONFIG__MOTOR_ENCODER_INVERT_M0 != 0),
          (unsigned int)(CONFIG__MOTOR_ENCODER_INVERT_M1 != 0));
}

void avc__motor_encoder_qdc_zero(void)
{
    avc__qdc_zero(QDC0);
    avc__qdc_zero(QDC1);

    s_last_position[AVC_MOTOR_ENCODER_M0] = avc__qdc_read_position(QDC0);
    s_last_position[AVC_MOTOR_ENCODER_M1] = avc__qdc_read_position(QDC1);
}

void avc__motor_encoder_qdc_sample(uint32_t elapsed_ms,
                                   avc_motor_encoder_sample_t samples[AVC_MOTOR_ENCODER_COUNT])
{
    if (elapsed_ms == 0U)
    {
        elapsed_ms = 1U;
    }

    for (uint32_t index = 0U; index < AVC_MOTOR_ENCODER_COUNT; index++)
    {
        QDC_Type *base = s_qdc_base[index];
        uint32_t position = avc__qdc_read_position(base);
        int32_t delta = (int32_t)(position - s_last_position[index]);
        int32_t counts_per_second = (int32_t)(((int64_t)delta * 1000) / (int32_t)elapsed_ms);
        int32_t rpm_milli = 0;

#if CONFIG__MOTOR_ENCODER_COUNTS_PER_WHEEL_REV > 0
        rpm_milli = (int32_t)(((int64_t)counts_per_second * 60000) /
                              CONFIG__MOTOR_ENCODER_COUNTS_PER_WHEEL_REV);
#endif

        samples[index].position = position;
        samples[index].delta = delta;
        samples[index].counts_per_second = counts_per_second;
        samples[index].rpm_milli = rpm_milli;
        samples[index].imr = base->IMR;
        samples[index].flags_ctrl = (uint16_t)(base->CTRL & AVC_QDC_CTRL_W1C_FLAGS);
        samples[index].flags_ctrl2 = (uint16_t)(base->CTRL2 & AVC_QDC_CTRL2_W1C_FLAGS);
        samples[index].direction_up = ((base->CTRL2 & QDC_CTRL2_DIR_MASK) != 0U);

        s_last_position[index] = position;
        avc__qdc_clear_flags(base);
    }
}

static void avc__motor_encoder_qdc_diag_set_motors(bool enable)
{
#if CONFIG__MOTOR_ENCODER_DIAG_MOTOR_ENABLE
    if (enable)
    {
        const float pwm_m0 = (float)CONFIG__MOTOR_ENCODER_DIAG_PWM_PERCENT_M0 / 100.0f;
        const float pwm_m1 = (float)CONFIG__MOTOR_ENCODER_DIAG_PWM_PERCENT_M1 / 100.0f;

        avc__enable_motor_control();
        avc__set_motor_pwm(pwm_m0, pwm_m1);
    }
    else
    {
        avc__disable_motor_control();
    }

    s_diag_motors_enabled = enable;
#else
    (void)enable;
    avc__disable_motor_control();
    s_diag_motors_enabled = false;
#endif
}

void avc__motor_encoder_qdc_diag_run(void)
{
    uint32_t last_report_ms = e_tick__get_ms();
    avc_motor_encoder_sample_t samples[AVC_MOTOR_ENCODER_COUNT];

    avc__motor_encoder_qdc_zero();
    avc__motor_encoder_qdc_diag_set_motors(false);

    DEBUG("Motor encoder QDC diagnostic active: report_ms=%u cpr=%u\r\n",
          CONFIG__MOTOR_ENCODER_DIAG_REPORT_MS,
          CONFIG__MOTOR_ENCODER_COUNTS_PER_WHEEL_REV);

#if CONFIG__MOTOR_ENCODER_DIAG_MOTOR_ENABLE
    DEBUG("Motor encoder QDC diagnostic motors: center button toggles M0 at %u%% and M1 at %u%% PWM\r\n",
          CONFIG__MOTOR_ENCODER_DIAG_PWM_PERCENT_M0,
          CONFIG__MOTOR_ENCODER_DIAG_PWM_PERCENT_M1);
#else
    DEBUG("Motor encoder QDC diagnostic motors disabled at build time; spin wheels by hand or rebuild with -EnableMotors\r\n");
#endif

#if CONFIG__MOTOR_ENCODER_DIAG_MOTOR_ENABLE && (CONFIG__MOTOR_ENCODER_DIAG_AUTOSTART_MS > 0U)
    uint32_t autostart_ms = e_tick__get_ms();
    bool autostart_pending = true;

    DEBUG("Motor encoder QDC diagnostic motors: auto-start in %u ms; car must be on blocks\r\n",
          CONFIG__MOTOR_ENCODER_DIAG_AUTOSTART_MS);
#endif

    while (1)
    {
#if CONFIG__MOTOR_ENCODER_DIAG_MOTOR_ENABLE && (CONFIG__MOTOR_ENCODER_DIAG_AUTOSTART_MS > 0U)
        if (autostart_pending &&
            e_tick__timeout(&autostart_ms, CONFIG__MOTOR_ENCODER_DIAG_AUTOSTART_MS))
        {
            autostart_pending = false;
            avc__motor_encoder_qdc_diag_set_motors(true);
            DEBUG("motor_encoder_diag autostart motors=1\r\n");
        }
#endif

        if (button__up(&center_btn))
        {
            avc__motor_encoder_qdc_diag_set_motors(!s_diag_motors_enabled);
            DEBUG("motor_encoder_diag motors=%u\r\n", s_diag_motors_enabled ? 1U : 0U);
        }

        if (e_tick__timeout(&last_report_ms, CONFIG__MOTOR_ENCODER_DIAG_REPORT_MS))
        {
            avc__motor_encoder_qdc_sample(CONFIG__MOTOR_ENCODER_DIAG_REPORT_MS, samples);

            DEBUG("enc t_ms=%u motors=%u "
                  "m0 pos=%u d=%d cps=%d rpm_m=%d imr=%04x dir=%u f=%04x/%04x "
                  "m1 pos=%u d=%d cps=%d rpm_m=%d imr=%04x dir=%u f=%04x/%04x\r\n",
                  e_tick__get_ms(),
                  s_diag_motors_enabled ? 1U : 0U,
                  samples[AVC_MOTOR_ENCODER_M0].position,
                  samples[AVC_MOTOR_ENCODER_M0].delta,
                  samples[AVC_MOTOR_ENCODER_M0].counts_per_second,
                  samples[AVC_MOTOR_ENCODER_M0].rpm_milli,
                  samples[AVC_MOTOR_ENCODER_M0].imr,
                  samples[AVC_MOTOR_ENCODER_M0].direction_up ? 1U : 0U,
                  samples[AVC_MOTOR_ENCODER_M0].flags_ctrl,
                  samples[AVC_MOTOR_ENCODER_M0].flags_ctrl2,
                  samples[AVC_MOTOR_ENCODER_M1].position,
                  samples[AVC_MOTOR_ENCODER_M1].delta,
                  samples[AVC_MOTOR_ENCODER_M1].counts_per_second,
                  samples[AVC_MOTOR_ENCODER_M1].rpm_milli,
                  samples[AVC_MOTOR_ENCODER_M1].imr,
                  samples[AVC_MOTOR_ENCODER_M1].direction_up ? 1U : 0U,
                  samples[AVC_MOTOR_ENCODER_M1].flags_ctrl,
                  samples[AVC_MOTOR_ENCODER_M1].flags_ctrl2);
        }

        e__crunch();
        CONFIG__E_WFI;
    }
}

#else

void avc__motor_encoder_qdc_init(void)
{
}

void avc__motor_encoder_qdc_zero(void)
{
}

void avc__motor_encoder_qdc_sample(uint32_t elapsed_ms,
                                   avc_motor_encoder_sample_t samples[AVC_MOTOR_ENCODER_COUNT])
{
    (void)elapsed_ms;
    (void)samples;
}

void avc__motor_encoder_qdc_diag_run(void)
{
    while (1)
    {
        CONFIG__E_WFI;
    }
}

#endif
