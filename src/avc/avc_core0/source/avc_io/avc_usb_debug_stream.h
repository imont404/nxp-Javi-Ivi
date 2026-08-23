#ifndef AVC_USB_DEBUG_STREAM_H_
#define AVC_USB_DEBUG_STREAM_H_

#include <stdbool.h>
#include <stdint.h>

#include "avc_usb_debug_protocol.h"

void avc_usb_debug_stream__init(void);
void avc_usb_debug_stream__service(void);
void avc_usb_debug_stream__start(void);
void avc_usb_debug_stream__stop(void);
bool avc_usb_debug_stream__is_open(void);
bool avc_usb_debug_stream__session_active(void);
bool avc_usb_debug_stream__camera_frames_active(void);
/* Main-loop-only handoff for a validated, acknowledged ENTER_ISP request. */
bool avc_usb_debug_stream__take_enter_isp_request(void);
bool avc_usb_debug_stream__tx_idle(void);
/* Camera callback generation marker; safe to call from interrupt context. */
void avc_usb_debug_stream__notify_camera_frame(void);
bool avc_usb_debug_stream__publish_frame(const uint16_t *frame);

/* Main-loop/thread context only. Both calls are no-ops until a recognized host
 * session subscribes to logs; records are bounded and may be dropped. */
bool avc_usb_debug_stream__log_text(uint8_t level, const char *category, const char *text);
bool avc_usb_debug_stream__logf(uint8_t level, const char *category, const char *format, ...);
bool avc_usb_debug_stream__telemetry_i32(const char *name, int32_t value, const char *units);
bool avc_usb_debug_stream__telemetry_u32(const char *name, uint32_t value, const char *units);
bool avc_usb_debug_stream__telemetry_f32(const char *name, float value, const char *units);
bool avc_usb_debug_stream__telemetry_bool(const char *name, bool value);

#define AVC_DBG_LOG_TRACE(category, ...) \
    avc_usb_debug_stream__logf(AVC_DBG_LOG_LEVEL_TRACE, (category), __VA_ARGS__)
#define AVC_DBG_LOG_DEBUG(category, ...) \
    avc_usb_debug_stream__logf(AVC_DBG_LOG_LEVEL_DEBUG, (category), __VA_ARGS__)
#define AVC_DBG_LOG_INFO(category, ...) \
    avc_usb_debug_stream__logf(AVC_DBG_LOG_LEVEL_INFO, (category), __VA_ARGS__)
#define AVC_DBG_LOG_WARNING(category, ...) \
    avc_usb_debug_stream__logf(AVC_DBG_LOG_LEVEL_WARNING, (category), __VA_ARGS__)
#define AVC_DBG_LOG_ERROR(category, ...) \
    avc_usb_debug_stream__logf(AVC_DBG_LOG_LEVEL_ERROR, (category), __VA_ARGS__)

#define AVC_DBG_VALUE_I32(name, value, units) avc_usb_debug_stream__telemetry_i32((name), (value), (units))
#define AVC_DBG_VALUE_U32(name, value, units) avc_usb_debug_stream__telemetry_u32((name), (value), (units))
#define AVC_DBG_VALUE_F32(name, value, units) avc_usb_debug_stream__telemetry_f32((name), (value), (units))
#define AVC_DBG_VALUE_BOOL(name, value) avc_usb_debug_stream__telemetry_bool((name), (value))

#endif /* AVC_USB_DEBUG_STREAM_H_ */
