#ifndef NXPC_USB_DEBUG_PROTOCOL_H_
#define NXPC_USB_DEBUG_PROTOCOL_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NXPC_DBG_MAGIC (0x55435641u) /* "AVCU" little-endian */
#define NXPC_DBG_VERSION (1u)

#define NXPC_DBG_PACKET_HEADER_BYTES (32u)
#define NXPC_DBG_RUI_WRITE_FRAME_BUFFER_RAW_HEADER_BYTES (24u)
#define NXPC_DBG_LOG_RECORD_HEADER_BYTES (12u)
#define NXPC_DBG_TELEMETRY_SCALAR_HEADER_BYTES (16u)
#define NXPC_DBG_STATS_REPORT_BYTES (76u)
#define NXPC_DBG_CONTROL_HELLO_RESPONSE_BYTES (24u)

#define NXPC_DBG_MSG_CLASS_RUI (0x01000000u)
#define NXPC_DBG_MSG_CLASS_RIO (0x01000100u)
#define NXPC_DBG_MSG_CLASS_LOG (0x01000200u)
#define NXPC_DBG_MSG_CLASS_STATS (0x01000300u)
#define NXPC_DBG_MSG_CLASS_CONTROL (0x01000400u)
#define NXPC_DBG_MSG_CLASS_TELEMETRY (0x01000500u)

#define NXPC_DBG_RUI_REQUEST_GEOMETRY (NXPC_DBG_MSG_CLASS_RUI + 0u)
#define NXPC_DBG_RUI_REPORT_GEOMETRY (NXPC_DBG_MSG_CLASS_RUI + 1u)
#define NXPC_DBG_RUI_WRITE_FRAME_BUFFER_RAW (NXPC_DBG_MSG_CLASS_RUI + 2u)
#define NXPC_DBG_RUI_PRESENT (NXPC_DBG_MSG_CLASS_RUI + 3u)
#define NXPC_DBG_RUI_TEXT (NXPC_DBG_MSG_CLASS_RUI + 4u)

#define NXPC_DBG_LOG_TEXT (NXPC_DBG_MSG_CLASS_LOG + 0u)
#define NXPC_DBG_STATS_REPORT (NXPC_DBG_MSG_CLASS_STATS + 0u)
#define NXPC_DBG_TELEMETRY_SCALAR (NXPC_DBG_MSG_CLASS_TELEMETRY + 0u)

#define NXPC_DBG_CONTROL_START_STREAM (NXPC_DBG_MSG_CLASS_CONTROL + 0u)
#define NXPC_DBG_CONTROL_STOP_STREAM (NXPC_DBG_MSG_CLASS_CONTROL + 1u)
#define NXPC_DBG_CONTROL_SET_STREAM_MODE (NXPC_DBG_MSG_CLASS_CONTROL + 2u)
#define NXPC_DBG_CONTROL_HELLO (NXPC_DBG_MSG_CLASS_CONTROL + 3u)
#define NXPC_DBG_CONTROL_SET_CHANNELS (NXPC_DBG_MSG_CLASS_CONTROL + 4u)
#define NXPC_DBG_CONTROL_PING (NXPC_DBG_MSG_CLASS_CONTROL + 5u)
#define NXPC_DBG_CONTROL_CLOSE (NXPC_DBG_MSG_CLASS_CONTROL + 6u)
#define NXPC_DBG_CONTROL_ERROR (NXPC_DBG_MSG_CLASS_CONTROL + 7u)
#define NXPC_DBG_CONTROL_ENTER_ISP (NXPC_DBG_MSG_CLASS_CONTROL + 8u)

/* Deliberate confirmation required in ENTER_ISP arg0 (ASCII "ISP!"). */
#define NXPC_DBG_ENTER_ISP_CONFIRMATION (0x21505349u)

#define NXPC_DBG_CAPABILITY_FRAMED_CONTROL (1u << 0)
#define NXPC_DBG_CAPABILITY_CAMERA_FRAMES (1u << 1)
#define NXPC_DBG_CAPABILITY_SYNTHETIC_FRAMES (1u << 2)
#define NXPC_DBG_CAPABILITY_STREAM_STATS (1u << 3)
#define NXPC_DBG_CAPABILITY_LOG_TEXT (1u << 4)
#define NXPC_DBG_CAPABILITY_NAMED_TELEMETRY (1u << 5)
#define NXPC_DBG_CAPABILITY_ENTER_ISP (1u << 6)

#define NXPC_DBG_CHANNEL_FRAMES (1u << 0)
#define NXPC_DBG_CHANNEL_STATS (1u << 1)
#define NXPC_DBG_CHANNEL_LOGS (1u << 2)
#define NXPC_DBG_CHANNEL_TELEMETRY (1u << 3)

#define NXPC_DBG_CONTROL_STATUS_OK (0u)
#define NXPC_DBG_CONTROL_STATUS_BAD_PACKET (1u)
#define NXPC_DBG_CONTROL_STATUS_UNSUPPORTED (2u)
#define NXPC_DBG_CONTROL_STATUS_SESSION_REQUIRED (3u)
#define NXPC_DBG_CONTROL_STATUS_BUSY (4u)
#define NXPC_DBG_CONTROL_STATUS_BAD_ARGUMENT (5u)

#define NXPC_DBG_PACKET_FLAG_RESPONSE (1u << 0)
#define NXPC_DBG_PACKET_FLAG_MORE (1u << 1)
#define NXPC_DBG_PACKET_FLAG_PAYLOAD_CRC32 (1u << 2)
#define NXPC_DBG_PACKET_FLAG_DROPPED_BEFORE (1u << 3)

#define NXPC_DBG_RUI_CHUNK_FRAME_START (1u << 0)
#define NXPC_DBG_RUI_CHUNK_FRAME_END (1u << 1)
#define NXPC_DBG_RUI_CHUNK_STALE_OK (1u << 2)

#define NXPC_DBG_PIXEL_FORMAT_RGB565_LE (1u)

#define NXPC_DBG_STREAM_SOURCE_CAMERA (0u)
#define NXPC_DBG_STREAM_SOURCE_SYNTHETIC (1u)

#define NXPC_DBG_STATS_FLAG_STREAM_ENABLED (1u << 0)
#define NXPC_DBG_STATS_FLAG_TX_BUSY (1u << 1)
#define NXPC_DBG_STATS_FLAG_FRAME_ACTIVE (1u << 2)

#define NXPC_DBG_LOG_LEVEL_TRACE (0u)
#define NXPC_DBG_LOG_LEVEL_DEBUG (1u)
#define NXPC_DBG_LOG_LEVEL_INFO (2u)
#define NXPC_DBG_LOG_LEVEL_WARNING (3u)
#define NXPC_DBG_LOG_LEVEL_ERROR (4u)

#define NXPC_DBG_TELEMETRY_TYPE_I32 (1u)
#define NXPC_DBG_TELEMETRY_TYPE_U32 (2u)
#define NXPC_DBG_TELEMETRY_TYPE_F32 (3u)
#define NXPC_DBG_TELEMETRY_TYPE_BOOL (4u)

#if defined(_MSC_VER)
#define NXPC_DBG_PACKED_BEGIN __pragma(pack(push, 1))
#define NXPC_DBG_PACKED_END __pragma(pack(pop))
#define NXPC_DBG_PACKED
#else
#define NXPC_DBG_PACKED_BEGIN
#define NXPC_DBG_PACKED_END
#define NXPC_DBG_PACKED __attribute__((packed))
#endif

NXPC_DBG_PACKED_BEGIN

typedef struct NXPC_DBG_PACKED nxpc_dbg_packet_header
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
} nxpc_dbg_packet_header_t;

typedef struct NXPC_DBG_PACKED nxpc_dbg_rui_geometry
{
    uint16_t width;
    uint16_t height;
    uint16_t bits_per_pixel;
    uint16_t pixel_format;
    uint16_t buffer_count;
    uint16_t reserved;
    uint32_t framebuffer_bytes;
} nxpc_dbg_rui_geometry_t;

typedef struct NXPC_DBG_PACKED nxpc_dbg_rui_write_frame_buffer_raw
{
    uint32_t frame_id;
    uint32_t byte_offset;
    uint32_t total_frame_bytes;
    uint16_t width;
    uint16_t height;
    uint16_t pixel_format;
    uint16_t buffer_id;
    uint32_t chunk_flags;
} nxpc_dbg_rui_write_frame_buffer_raw_t;

typedef struct NXPC_DBG_PACKED nxpc_dbg_rui_present
{
    uint32_t frame_id;
    uint16_t buffer_id;
    uint16_t flags;
} nxpc_dbg_rui_present_t;

typedef struct NXPC_DBG_PACKED nxpc_dbg_stats_report
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
    uint32_t control_response_drop_count;
    uint32_t control_response_queue_high_water;
    uint32_t log_drop_count;
    uint32_t log_queue_high_water;
    uint32_t telemetry_drop_count;
    uint32_t telemetry_queue_high_water;
    uint32_t telemetry_coalesce_count;
} nxpc_dbg_stats_report_t;

typedef struct NXPC_DBG_PACKED nxpc_dbg_log_record
{
    uint32_t timestamp_ms;
    uint32_t record_id;
    uint16_t text_bytes;
    uint8_t level;
    uint8_t category_bytes;
} nxpc_dbg_log_record_t;

typedef struct NXPC_DBG_PACKED nxpc_dbg_telemetry_scalar
{
    uint32_t timestamp_ms;
    uint32_t sample_id;
    uint32_t value_bits;
    uint16_t name_bytes;
    uint8_t value_type;
    uint8_t units_bytes;
} nxpc_dbg_telemetry_scalar_t;

typedef struct NXPC_DBG_PACKED nxpc_dbg_control_hello_response
{
    uint32_t capability_flags;
    uint32_t active_channel_flags;
    uint32_t max_packet_bytes;
    uint16_t frame_width;
    uint16_t frame_height;
    uint16_t pixel_format;
    uint16_t reserved;
    uint32_t session_id;
} nxpc_dbg_control_hello_response_t;

NXPC_DBG_PACKED_END

#if defined(__cplusplus)
static_assert(sizeof(nxpc_dbg_packet_header_t) == NXPC_DBG_PACKET_HEADER_BYTES,
              "NXP Cup debug packet header must stay 32 bytes");
static_assert(sizeof(nxpc_dbg_rui_write_frame_buffer_raw_t) ==
                  NXPC_DBG_RUI_WRITE_FRAME_BUFFER_RAW_HEADER_BYTES,
              "NXP Cup debug RUI raw framebuffer header must stay 24 bytes");
static_assert(sizeof(nxpc_dbg_stats_report_t) == NXPC_DBG_STATS_REPORT_BYTES,
              "NXP Cup debug stats report wire size changed unexpectedly");
static_assert(sizeof(nxpc_dbg_log_record_t) == NXPC_DBG_LOG_RECORD_HEADER_BYTES,
              "NXP Cup debug log record header wire size changed unexpectedly");
static_assert(sizeof(nxpc_dbg_telemetry_scalar_t) == NXPC_DBG_TELEMETRY_SCALAR_HEADER_BYTES,
              "NXP Cup debug telemetry scalar header wire size changed unexpectedly");
static_assert(sizeof(nxpc_dbg_control_hello_response_t) ==
                  NXPC_DBG_CONTROL_HELLO_RESPONSE_BYTES,
              "NXP Cup debug hello response must stay 24 bytes");
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
_Static_assert(sizeof(nxpc_dbg_packet_header_t) == NXPC_DBG_PACKET_HEADER_BYTES,
               "NXP Cup debug packet header must stay 32 bytes");
_Static_assert(sizeof(nxpc_dbg_rui_write_frame_buffer_raw_t) ==
                   NXPC_DBG_RUI_WRITE_FRAME_BUFFER_RAW_HEADER_BYTES,
               "NXP Cup debug RUI raw framebuffer header must stay 24 bytes");
_Static_assert(sizeof(nxpc_dbg_stats_report_t) == NXPC_DBG_STATS_REPORT_BYTES,
               "NXP Cup debug stats report wire size changed unexpectedly");
_Static_assert(sizeof(nxpc_dbg_log_record_t) == NXPC_DBG_LOG_RECORD_HEADER_BYTES,
               "NXP Cup debug log record header wire size changed unexpectedly");
_Static_assert(sizeof(nxpc_dbg_telemetry_scalar_t) == NXPC_DBG_TELEMETRY_SCALAR_HEADER_BYTES,
               "NXP Cup debug telemetry scalar header wire size changed unexpectedly");
_Static_assert(sizeof(nxpc_dbg_control_hello_response_t) ==
                   NXPC_DBG_CONTROL_HELLO_RESPONSE_BYTES,
               "NXP Cup debug hello response must stay 24 bytes");
#endif

#undef NXPC_DBG_PACKED_BEGIN
#undef NXPC_DBG_PACKED_END
#undef NXPC_DBG_PACKED

#ifdef __cplusplus
}
#endif

#endif /* NXPC_USB_DEBUG_PROTOCOL_H_ */
