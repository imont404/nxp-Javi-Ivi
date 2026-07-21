#ifndef AVC_USB_DEBUG_PROTOCOL_H_
#define AVC_USB_DEBUG_PROTOCOL_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AVC_DBG_MAGIC (0x55435641u) /* "AVCU" little-endian */
#define AVC_DBG_VERSION (1u)

#define AVC_DBG_PACKET_HEADER_BYTES (32u)
#define AVC_DBG_RUI_WRITE_FRAME_BUFFER_RAW_HEADER_BYTES (24u)
#define AVC_DBG_STATS_REPORT_BYTES (48u)

#define AVC_DBG_MSG_CLASS_RUI (0x01000000u)
#define AVC_DBG_MSG_CLASS_RIO (0x01000100u)
#define AVC_DBG_MSG_CLASS_LOG (0x01000200u)
#define AVC_DBG_MSG_CLASS_STATS (0x01000300u)
#define AVC_DBG_MSG_CLASS_CONTROL (0x01000400u)

#define AVC_DBG_RUI_REQUEST_GEOMETRY (AVC_DBG_MSG_CLASS_RUI + 0u)
#define AVC_DBG_RUI_REPORT_GEOMETRY (AVC_DBG_MSG_CLASS_RUI + 1u)
#define AVC_DBG_RUI_WRITE_FRAME_BUFFER_RAW (AVC_DBG_MSG_CLASS_RUI + 2u)
#define AVC_DBG_RUI_PRESENT (AVC_DBG_MSG_CLASS_RUI + 3u)
#define AVC_DBG_RUI_TEXT (AVC_DBG_MSG_CLASS_RUI + 4u)

#define AVC_DBG_LOG_TEXT (AVC_DBG_MSG_CLASS_LOG + 0u)
#define AVC_DBG_STATS_REPORT (AVC_DBG_MSG_CLASS_STATS + 0u)

#define AVC_DBG_CONTROL_START_STREAM (AVC_DBG_MSG_CLASS_CONTROL + 0u)
#define AVC_DBG_CONTROL_STOP_STREAM (AVC_DBG_MSG_CLASS_CONTROL + 1u)
#define AVC_DBG_CONTROL_SET_STREAM_MODE (AVC_DBG_MSG_CLASS_CONTROL + 2u)

#define AVC_DBG_PACKET_FLAG_RESPONSE (1u << 0)
#define AVC_DBG_PACKET_FLAG_MORE (1u << 1)
#define AVC_DBG_PACKET_FLAG_PAYLOAD_CRC32 (1u << 2)
#define AVC_DBG_PACKET_FLAG_DROPPED_BEFORE (1u << 3)

#define AVC_DBG_RUI_CHUNK_FRAME_START (1u << 0)
#define AVC_DBG_RUI_CHUNK_FRAME_END (1u << 1)
#define AVC_DBG_RUI_CHUNK_STALE_OK (1u << 2)

#define AVC_DBG_PIXEL_FORMAT_RGB565_LE (1u)

#define AVC_DBG_STREAM_SOURCE_CAMERA (0u)
#define AVC_DBG_STREAM_SOURCE_SYNTHETIC (1u)

#define AVC_DBG_STATS_FLAG_STREAM_ENABLED (1u << 0)
#define AVC_DBG_STATS_FLAG_TX_BUSY (1u << 1)
#define AVC_DBG_STATS_FLAG_FRAME_ACTIVE (1u << 2)

#if defined(_MSC_VER)
#define AVC_DBG_PACKED_BEGIN __pragma(pack(push, 1))
#define AVC_DBG_PACKED_END __pragma(pack(pop))
#define AVC_DBG_PACKED
#else
#define AVC_DBG_PACKED_BEGIN
#define AVC_DBG_PACKED_END
#define AVC_DBG_PACKED __attribute__((packed))
#endif

AVC_DBG_PACKED_BEGIN

typedef struct AVC_DBG_PACKED avc_dbg_packet_header
{
    uint32_t magic;
    uint8_t version;
    uint8_t header_bytes;
    uint16_t flags;
    uint32_t msg_id;
    uint32_t sequence;
    uint32_t payload_length;
    uint32_t arg0;
    uint32_t arg1;
    uint32_t arg2;
} avc_dbg_packet_header_t;

typedef struct AVC_DBG_PACKED avc_dbg_rui_geometry
{
    uint16_t width;
    uint16_t height;
    uint16_t bits_per_pixel;
    uint16_t pixel_format;
    uint16_t buffer_count;
    uint16_t reserved;
    uint32_t framebuffer_bytes;
} avc_dbg_rui_geometry_t;

typedef struct AVC_DBG_PACKED avc_dbg_rui_write_frame_buffer_raw
{
    uint32_t frame_id;
    uint32_t byte_offset;
    uint32_t total_frame_bytes;
    uint16_t width;
    uint16_t height;
    uint16_t pixel_format;
    uint16_t buffer_id;
    uint32_t chunk_flags;
} avc_dbg_rui_write_frame_buffer_raw_t;

typedef struct AVC_DBG_PACKED avc_dbg_rui_present
{
    uint32_t frame_id;
    uint16_t buffer_id;
    uint16_t flags;
} avc_dbg_rui_present_t;

typedef struct AVC_DBG_PACKED avc_dbg_stats_report
{
    uint32_t stream_source;
    uint32_t stream_flags;
    uint32_t next_sequence;
    uint32_t current_frame_id;
    uint32_t frames_started;
    uint32_t frames_completed;
    uint32_t frames_dropped;
    uint32_t packets_sent;
    uint32_t bytes_sent;
    uint32_t endpoint_busy_count;
    uint32_t send_error_count;
    uint32_t rx_command_count;
} avc_dbg_stats_report_t;

AVC_DBG_PACKED_END

#if defined(__cplusplus)
static_assert(sizeof(avc_dbg_packet_header_t) == AVC_DBG_PACKET_HEADER_BYTES,
              "AVC debug packet header must stay 32 bytes");
static_assert(sizeof(avc_dbg_rui_write_frame_buffer_raw_t) ==
                  AVC_DBG_RUI_WRITE_FRAME_BUFFER_RAW_HEADER_BYTES,
              "AVC debug RUI raw framebuffer header must stay 24 bytes");
static_assert(sizeof(avc_dbg_stats_report_t) == AVC_DBG_STATS_REPORT_BYTES,
              "AVC debug stats report must stay 48 bytes");
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
_Static_assert(sizeof(avc_dbg_packet_header_t) == AVC_DBG_PACKET_HEADER_BYTES,
               "AVC debug packet header must stay 32 bytes");
_Static_assert(sizeof(avc_dbg_rui_write_frame_buffer_raw_t) ==
                   AVC_DBG_RUI_WRITE_FRAME_BUFFER_RAW_HEADER_BYTES,
               "AVC debug RUI raw framebuffer header must stay 24 bytes");
_Static_assert(sizeof(avc_dbg_stats_report_t) == AVC_DBG_STATS_REPORT_BYTES,
               "AVC debug stats report must stay 48 bytes");
#endif

#undef AVC_DBG_PACKED_BEGIN
#undef AVC_DBG_PACKED_END
#undef AVC_DBG_PACKED

#ifdef __cplusplus
}
#endif

#endif /* AVC_USB_DEBUG_PROTOCOL_H_ */
