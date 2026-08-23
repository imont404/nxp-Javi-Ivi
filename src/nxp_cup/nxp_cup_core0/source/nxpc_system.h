#ifndef NXPC_SYSTEM_H_
#define NXPC_SYSTEM_H_

#include <stdbool.h>

typedef enum
{
    NXPC_SYSTEM_MODE_STARTUP = 0,
    NXPC_SYSTEM_MODE_TEST,
    NXPC_SYSTEM_MODE_RACE_WAITING,
    NXPC_SYSTEM_MODE_RACE_RUNNING,
    NXPC_SYSTEM_MODE_ENTERING_ISP,
    NXPC_SYSTEM_MODE_SAFE_FAULT
} nxpc_system_mode_t;

typedef enum
{
    NXPC_SYSTEM_FAULT_NONE = 0,
    NXPC_SYSTEM_FAULT_CAMERA_STARTUP,
    NXPC_SYSTEM_FAULT_CAMERA_LOST,
    NXPC_SYSTEM_FAULT_CALLBACK_OVERRUN,
    NXPC_SYSTEM_FAULT_BOOTLOADER_RETURNED
} nxpc_system_fault_t;

/*
 * Perform the existing one-time platform initialization, then select TEST or
 * RACE_WAITING from the physical TEST input. USB state is deliberately not an
 * input to this decision.
 */
void nxpc_system__init(void);

/* Advance bounded mode transitions and enforce safe actuator transitions. */
void nxpc_system__service(void);

/* Called by the camera frame callback. Safe to call from interrupt context. */
void nxpc_system__notify_camera_frame(void);

/* Enter a latched safe fault. Recovery policy is added by a later plan step. */
void nxpc_system__enter_fault(nxpc_system_fault_t fault);

nxpc_system_mode_t nxpc_system__mode(void);
nxpc_system_fault_t nxpc_system__fault(void);
bool nxpc_system__camera_frame_seen(void);
bool nxpc_system__outputs_allowed(void);
bool nxpc_system__test_outputs_armed(void);
bool nxpc_system__test_arm_pending(void);
const char *nxpc_system__mode_label(nxpc_system_mode_t mode);

#endif /* NXPC_SYSTEM_H_ */
