#ifndef AVC_USB_DEBUG_STREAM_H_
#define AVC_USB_DEBUG_STREAM_H_

#include <stdbool.h>
#include <stdint.h>

void avc_usb_debug_stream__init(void);
void avc_usb_debug_stream__service(void);
void avc_usb_debug_stream__start(void);
void avc_usb_debug_stream__stop(void);
bool avc_usb_debug_stream__is_open(void);
bool avc_usb_debug_stream__publish_frame(const uint16_t *frame);

#endif /* AVC_USB_DEBUG_STREAM_H_ */
