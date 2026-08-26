#ifndef NXPC__IO_H_
#define NXPC__IO_H_

#include "e.h"
#include "nxpc__master_config.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "board.h"
#include "fsl_device_registers.h"
#include "fsl_mailbox.h"
#include "fsl_gpio.h"
#include "fsl_lpflexcomm.h"
#include "fsl_lpuart.h"
#include "ipc.h"
#include "fsl_lpspi_edma.h"
#include "button.h"

#include "st7789.h"
#include "lpspi1.h"
#include "nxpc_display.h"

#include "nxpc__adc.h"
#include "nxpc__motor_control.h"
#include "nxpc__servo_control.h"

#include "bv_camera__interface.h"

#define IN_PORT 3
#define LEFT_BTN_PIN 21
#define RIGHT_BTN_PIN 19
#define CENTER_BTN_PIN 17
#define TEST_SW_PIN 18

void nxpc__init(void);

#define CYCLE_COUNTER DWT->CYCCNT

#define INIT_CYCLE_COUNTER                                                                         \
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;                                                \
    DWT->CYCCNT = 0;                                                                               \
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk

#endif /* NXPC__IO_H_ */
