#ifndef NXPC_USB_DEBUG_STREAM_H_
#define NXPC_USB_DEBUG_STREAM_H_

#include <stdbool.h>
#include <stdint.h>

#include "nxpc_usb_debug_protocol.h"

void nxpc_usb_debug_stream__init(void);
void nxpc_usb_debug_stream__service(void);
void nxpc_usb_debug_stream__start(void);
void nxpc_usb_debug_stream__stop(void);
bool nxpc_usb_debug_stream__is_open(void);
bool nxpc_usb_debug_stream__session_active(void);
bool nxpc_usb_debug_stream__camera_frames_active(void);
/* Main-loop-only handoff for a validated, acknowledged ENTER_ISP request. */
bool nxpc_usb_debug_stream__take_enter_isp_request(void);

typedef struct
{
    uint32_t requestSequence;
    uint32_t action;
} nxpc_usb_system_action_request_t;

/* Main-loop handoff: USB parsing never changes the vehicle state directly. */
bool nxpc_usb_debug_stream__take_system_action(nxpc_usb_system_action_request_t *request);
void nxpc_usb_debug_stream__complete_system_action(uint32_t requestSequence, uint32_t status);
bool nxpc_usb_debug_stream__tx_idle(void);
/* Camera callback generation marker; safe to call from interrupt context. */
void nxpc_usb_debug_stream__notify_camera_frame(void);
bool nxpc_usb_debug_stream__publish_frame(const uint16_t *frame);

/* Main-loop/thread context only. Both calls are no-ops until a recognized host
 * session subscribes to logs; records are bounded and may be dropped. */
bool nxpc_usb_debug_stream__log_text(uint8_t level, const char *category, const char *text);
bool nxpc_usb_debug_stream__logf(uint8_t level, const char *category, const char *format, ...);
bool nxpc_usb_debug_stream__telemetry_i32(const char *name, int32_t value, const char *units);
bool nxpc_usb_debug_stream__telemetry_u32(const char *name, uint32_t value, const char *units);
bool nxpc_usb_debug_stream__telemetry_f32(const char *name, float value, const char *units);
bool nxpc_usb_debug_stream__telemetry_bool(const char *name, bool value);
bool nxpc_usb_debug_stream__telemetry_text(const char *name, const char *value);

/* Framework-only priority path. Six queue positions are reserved for these values. */
bool nxpc_usb_debug_stream__framework_telemetry_f32(const char *name, float value,
                                                    const char *units);
bool nxpc_usb_debug_stream__framework_telemetry_bool(const char *name, bool value);
bool nxpc_usb_debug_stream__framework_telemetry_text(const char *name, const char *value);

#define NXPC_DBG_LOG_TRACE(category, ...)                                                          \
    nxpc_usb_debug_stream__logf(NXPC_DBG_LOG_LEVEL_TRACE, (category), __VA_ARGS__)
#define NXPC_DBG_LOG_DEBUG(category, ...)                                                          \
    nxpc_usb_debug_stream__logf(NXPC_DBG_LOG_LEVEL_DEBUG, (category), __VA_ARGS__)
#define NXPC_DBG_LOG_INFO(category, ...)                                                           \
    nxpc_usb_debug_stream__logf(NXPC_DBG_LOG_LEVEL_INFO, (category), __VA_ARGS__)
#define NXPC_DBG_LOG_WARNING(category, ...)                                                        \
    nxpc_usb_debug_stream__logf(NXPC_DBG_LOG_LEVEL_WARNING, (category), __VA_ARGS__)
#define NXPC_DBG_LOG_ERROR(category, ...)                                                          \
    nxpc_usb_debug_stream__logf(NXPC_DBG_LOG_LEVEL_ERROR, (category), __VA_ARGS__)

#define NXPC_DBG_VALUE_I32(name, value, units)                                                     \
    nxpc_usb_debug_stream__telemetry_i32((name), (value), (units))
#define NXPC_DBG_VALUE_U32(name, value, units)                                                     \
    nxpc_usb_debug_stream__telemetry_u32((name), (value), (units))
#define NXPC_DBG_VALUE_F32(name, value, units)                                                     \
    nxpc_usb_debug_stream__telemetry_f32((name), (value), (units))
#define NXPC_DBG_VALUE_BOOL(name, value) nxpc_usb_debug_stream__telemetry_bool((name), (value))
#define NXPC_DBG_VALUE_TEXT(name, value) nxpc_usb_debug_stream__telemetry_text((name), (value))

#endif /* NXPC_USB_DEBUG_STREAM_H_ */
