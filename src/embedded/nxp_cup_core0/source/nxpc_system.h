#ifndef NXPC_SYSTEM_H_
#define NXPC_SYSTEM_H_

#include <stdbool.h>

#include "button.h"

#define NXPC_SYSTEM_CAMERA_FRESH_MS (250U)

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
    NXPC_TEST_PAGE_CAMERA_IO = 0,
    NXPC_TEST_PAGE_VISION,
    NXPC_TEST_PAGE_MOTORS,
    NXPC_TEST_PAGE_COUNT
} nxpc_test_page_t;

typedef enum
{
    NXPC_SYSTEM_FAULT_NONE = 0,
    NXPC_SYSTEM_FAULT_CAMERA_STARTUP,
    NXPC_SYSTEM_FAULT_CAMERA_LOST,
    NXPC_SYSTEM_FAULT_CALLBACK_OVERRUN,
    NXPC_SYSTEM_FAULT_BOOTLOADER_RETURNED
} nxpc_system_fault_t;

/*
 * A typed, presentation-independent snapshot of the current operating state.
 * These values are internal firmware identifiers, not USB wire values.
 */
typedef enum
{
    NXPC_SYSTEM_STATE_INITIALIZING = 0,
    NXPC_SYSTEM_STATE_TEST_DISARMED,
    NXPC_SYSTEM_STATE_TEST_CENTER_POTS,
    NXPC_SYSTEM_STATE_TEST_ARMED,
    NXPC_SYSTEM_STATE_RACE_WAITING_CAMERA,
    NXPC_SYSTEM_STATE_RACE_READY,
    NXPC_SYSTEM_STATE_RACE_RUNNING,
    NXPC_SYSTEM_STATE_ENTERING_ISP,
    NXPC_SYSTEM_STATE_FAULT_CAMERA_STARTUP,
    NXPC_SYSTEM_STATE_FAULT_CAMERA_LOST,
    NXPC_SYSTEM_STATE_FAULT_CALLBACK_OVERRUN,
    NXPC_SYSTEM_STATE_FAULT_BOOTLOADER_RETURNED,
    NXPC_SYSTEM_STATE_SAFE_FAULT
} nxpc_system_state_t;

typedef enum
{
    NXPC_SYSTEM_ACTION_RACE_START = 1,
    NXPC_SYSTEM_ACTION_STOP = 2
} nxpc_system_action_t;

typedef enum
{
    NXPC_SYSTEM_ACTION_ACCEPTED = 0,
    NXPC_SYSTEM_ACTION_NOT_READY,
    NXPC_SYSTEM_ACTION_DENIED
} nxpc_system_action_result_t;

/*
 * Perform the existing one-time platform initialization, then select TEST or
 * RACE_WAITING from the physical TEST input. USB state is deliberately not an
 * input to this decision.
 */
void nxpc_system__init(void);

/* Advance bounded mode transitions and enforce safe actuator transitions. */
void nxpc_system__service(const button_snapshot_t *buttons);

/* Framework-only TEST page transition. Hardware is made safe before page changes. */
bool nxpc_system__select_test_page(nxpc_test_page_t page,
                                   const button_snapshot_t *buttons);

/* Request a validated high-level action; callers cannot assign modes directly. */
nxpc_system_action_result_t nxpc_system__request_action(nxpc_system_action_t action);

/* Called by the camera frame callback. Safe to call from interrupt context. */
void nxpc_system__notify_camera_frame(void);

/* Enter a latched safe fault. Recovery policy is added by a later plan step. */
void nxpc_system__enter_fault(nxpc_system_fault_t fault);

nxpc_system_mode_t nxpc_system__mode(void);
nxpc_system_fault_t nxpc_system__fault(void);
nxpc_system_state_t nxpc_system__state(void);
bool nxpc_system__camera_frame_seen(void);
bool nxpc_system__camera_live(void);
bool nxpc_system__outputs_allowed(void);
bool nxpc_system__test_outputs_armed(void);
bool nxpc_system__test_arm_pending(void);
nxpc_test_page_t nxpc_system__test_page(void);
const char *nxpc_system__test_page_label(nxpc_test_page_t page);
void nxpc_system__notify_test_motor_lease_expired(void);
const char *nxpc_system__mode_label(nxpc_system_mode_t mode);
const char *nxpc_system__state_label(nxpc_system_state_t state);

#endif /* NXPC_SYSTEM_H_ */
