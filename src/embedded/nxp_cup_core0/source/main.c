#include "app/race_mode.h"
#include "app/test_mode.h"
#include "nxpc__io.h"
#include "nxpc_framework.h"

#if CONFIG__MOTOR_ENCODER_DIAG_ENABLE
#include "nxpc__motor_encoder_qdc.h"
#endif

int main(void)
{
    nxpc_framework__init();

#if CONFIG__MOTOR_ENCODER_DIAG_ENABLE
    nxpc__motor_encoder_qdc_diag_run();
#endif

    while (1)
    {
        uint16_t *frame;

        nxpc_framework__service();
        frame = nxpc_framework__take_latest_frame();
        if (frame == NULL)
        {
            continue;
        }

        nxpc_framework__begin_callback();
        switch (nxpc_system__mode())
        {
            case NXPC_SYSTEM_MODE_TEST:
                test_mode_on_frame(frame);
                break;

            case NXPC_SYSTEM_MODE_RACE_RUNNING:
                race_mode_on_frame(frame);
                break;

            case NXPC_SYSTEM_MODE_RACE_WAITING:
            default:
                break;
        }
        nxpc_framework__end_callback();
        nxpc_framework__finish_frame(frame);
    }
}
