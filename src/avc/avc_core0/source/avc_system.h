#ifndef AVC_SYSTEM_H_
#define AVC_SYSTEM_H_

#include <stdbool.h>

typedef enum
{
    AVC_SYSTEM_MODE_STARTUP = 0,
    AVC_SYSTEM_MODE_TEST,
    AVC_SYSTEM_MODE_RACE_WAITING,
    AVC_SYSTEM_MODE_STUDENT_RUNNING,
    AVC_SYSTEM_MODE_SAFE_FAULT
} avc_system_mode_t;

typedef enum
{
    AVC_SYSTEM_FAULT_NONE = 0,
    AVC_SYSTEM_FAULT_CAMERA_STARTUP
} avc_system_fault_t;

/*
 * Perform the existing one-time platform initialization, then select TEST or
 * RACE_WAITING from the physical TEST input. USB state is deliberately not an
 * input to this decision.
 */
void avc_system__init(void);

/* Advance bounded mode transitions and enforce safe actuator transitions. */
void avc_system__service(void);

/* Called by the camera frame callback. Safe to call from interrupt context. */
void avc_system__notify_camera_frame(void);

/* Enter a latched safe fault. Recovery policy is added by a later plan step. */
void avc_system__enter_fault(avc_system_fault_t fault);

avc_system_mode_t avc_system__mode(void);
avc_system_fault_t avc_system__fault(void);
bool avc_system__camera_frame_seen(void);
const char *avc_system__mode_label(avc_system_mode_t mode);

#endif /* AVC_SYSTEM_H_ */
