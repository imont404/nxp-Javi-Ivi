# AVC USB Debug Transport Protocol Draft

Status: draft

This protocol is the planned durable shape for the AVC USB debug-display
transport. The first working firmware/host proof now uses the shared `AVCU`
envelope plus RUI `WRITE_FRAME_BUFFER_RAW` chunks. This draft keeps that
working direction but makes the protocol extensible enough to support remote
display operations, text/log messages, stats, controls, and later embedded
display nodes.

See `AVC_USB_Debug_Display_Current_State.md` for the parked implementation
state, measured rates, host-tool paths, and next backlog after the PCB-revision
viability milestone.

## Design Lineage

The useful prior art is:

- `e_msg`: a generic byte-stream envelope with sync bytes, payload length, and
  CRC16.
- `RUI`: remote UI operations carried inside that envelope, especially
  `write_frame_buffer_raw`.
- `RIO`: remote I/O operations carried as fixed structs with a 32-bit message
  ID namespace.

For AVC, keep the same separation:

- Transport envelope: resync, length, sequence, flags, and message ID.
- Message payload: typed RUI/RIO/log/control structures.

Do not copy `e_msg` directly into the high-speed USB path. Its byte-at-a-time
parser, 16-bit payload length, and required CRC are useful for UART-class links
but are unnecessary overhead for high-throughput USB bulk. USB bulk already has
link-level CRC. The AVC protocol should reserve optional payload CRC for noisy
or non-USB transports but keep it off by default.

## Wire Rules

- All multi-byte fields are little-endian.
- Wire structs use fixed-width integer types, not C enum layout.
- Message IDs are 32-bit values.
- Structs must be packed on firmware and host if mapped directly.
- Unknown message IDs must be skipped using `payload_length`.
- Receivers must be able to resync after malformed or partial data.
- Frame data is latest-frame best-effort debug output, not reliable archival
  transfer.

## Message ID Namespace

Use class ranges inspired by the older RUI/RIO values:

```c
#define AVC_DBG_MSG_CLASS_RUI      0x01000000u
#define AVC_DBG_MSG_CLASS_RIO      0x01000100u
#define AVC_DBG_MSG_CLASS_LOG      0x01000200u
#define AVC_DBG_MSG_CLASS_STATS    0x01000300u
#define AVC_DBG_MSG_CLASS_CONTROL  0x01000400u
```

Initial RUI messages:

```c
#define AVC_DBG_RUI_REQUEST_GEOMETRY        (AVC_DBG_MSG_CLASS_RUI + 0u)
#define AVC_DBG_RUI_REPORT_GEOMETRY         (AVC_DBG_MSG_CLASS_RUI + 1u)
#define AVC_DBG_RUI_WRITE_FRAME_BUFFER_RAW  (AVC_DBG_MSG_CLASS_RUI + 2u)
#define AVC_DBG_RUI_PRESENT                 (AVC_DBG_MSG_CLASS_RUI + 3u)
#define AVC_DBG_RUI_TEXT                    (AVC_DBG_MSG_CLASS_RUI + 4u)
```

Initial log/stats/control messages:

```c
#define AVC_DBG_LOG_TEXT                    (AVC_DBG_MSG_CLASS_LOG + 0u)
#define AVC_DBG_STATS_REPORT                (AVC_DBG_MSG_CLASS_STATS + 0u)
#define AVC_DBG_CONTROL_START_STREAM        (AVC_DBG_MSG_CLASS_CONTROL + 0u)
#define AVC_DBG_CONTROL_STOP_STREAM         (AVC_DBG_MSG_CLASS_CONTROL + 1u)
#define AVC_DBG_CONTROL_SET_STREAM_MODE     (AVC_DBG_MSG_CLASS_CONTROL + 2u)
```

RIO remains reserved for future host-to-device or embedded-node I/O operations.
Do not define concrete RIO operations until there is a consumer.

## Transport Envelope

The current proof uses a 32-byte header. Keep a 32-byte base envelope so USB
bulk packet parsing remains cheap:

```c
typedef struct __attribute__((packed))
{
    uint32_t magic;          /* 'AVCU' little-endian: 0x55435641 */
    uint8_t  version;        /* 1 */
    uint8_t  header_bytes;   /* sizeof(avc_dbg_packet_header_t), normally 32 */
    uint16_t flags;          /* transport flags */
    uint32_t msg_id;         /* AVC_DBG_* */
    uint32_t sequence;       /* increments per packet on this stream */
    uint32_t payload_length; /* bytes after this header */
    uint32_t arg0;           /* message-specific fast path */
    uint32_t arg1;           /* message-specific fast path */
    uint32_t arg2;           /* message-specific fast path */
} avc_dbg_packet_header_t;
```

Suggested transport flags:

```c
#define AVC_DBG_PACKET_FLAG_RESPONSE        (1u << 0)
#define AVC_DBG_PACKET_FLAG_MORE            (1u << 1)
#define AVC_DBG_PACKET_FLAG_PAYLOAD_CRC32   (1u << 2)
#define AVC_DBG_PACKET_FLAG_DROPPED_BEFORE  (1u << 3)
```

For `PAYLOAD_CRC32`, the CRC field should be appended after the payload, not
placed in the 32-byte header. Leave this disabled for the USB CDC proof.

## RUI Geometry

Geometry lets a remote display node, PC app, or browser discover the logical
frame buffer shape.

```c
typedef struct __attribute__((packed))
{
    uint16_t width;
    uint16_t height;
    uint16_t bits_per_pixel;
    uint16_t pixel_format;
    uint16_t buffer_count;
    uint16_t reserved;
    uint32_t framebuffer_bytes;
} avc_dbg_rui_geometry_t;
```

Initial pixel formats:

```c
#define AVC_DBG_PIXEL_FORMAT_RGB565_LE  1u
```

For the current AVC camera/debug display:

- width: `320`
- height: `200` for camera frame, optionally `240` for composed LCD-style view
- bits per pixel: `16`
- format: `RGB565_LE`

## RUI Raw Framebuffer Write

This is the direct descendant of `rui_write_frame_buffer_raw`, but made more
explicit for streaming and partial updates.

Header:

```c
typedef struct __attribute__((packed))
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
```

The raw pixel bytes immediately follow this 24-byte structure in the packet
payload. They are intentionally not represented as a flexible array member in
the shared C/C++ header so the same header remains portable to MSVC.

Chunk flags:

```c
#define AVC_DBG_RUI_CHUNK_FRAME_START  (1u << 0)
#define AVC_DBG_RUI_CHUNK_FRAME_END    (1u << 1)
#define AVC_DBG_RUI_CHUNK_STALE_OK     (1u << 2)
```

Envelope mapping for this message:

- `msg_id`: `AVC_DBG_RUI_WRITE_FRAME_BUFFER_RAW`
- `sequence`: packet sequence
- `payload_length`: `24 + data_len`
- `arg0`: `frame_id`
- `arg1`: `byte_offset`
- `arg2`: `data_len`

The receiver validates:

- `byte_offset < total_frame_bytes`
- `byte_offset + data_len <= total_frame_bytes`
- non-final chunks do not exceed configured max payload
- `FRAME_END` only appears on the chunk that completes the frame

For the current CDC proof, 16 KiB transfers are still preferred. They fit the
EHCI dTD behavior already measured and are comfortably above the required AVC
frame rate.

## RUI Present

`WRITE_FRAME_BUFFER_RAW` can be enough when `FRAME_END` is set. A separate
`RUI_PRESENT` is reserved for future display-node semantics where writes may
target back buffers and presentation is explicit.

```c
typedef struct __attribute__((packed))
{
    uint32_t frame_id;
    uint16_t buffer_id;
    uint16_t flags;
} avc_dbg_rui_present_t;
```

## Stats Report

`AVC_DBG_STATS_REPORT` is a compact firmware-to-host status packet. It is sent
on camera streams at start and then every 24 completed camera frames. The
synthetic maximum-rate source does not currently emit periodic stats, because
the Windows CDC ceiling path is most useful when it remains a pure bulk-IN
stress stream.

```c
typedef struct __attribute__((packed))
{
    uint32_t stream_source;        /* AVC_DBG_STREAM_SOURCE_* */
    uint32_t stream_flags;         /* AVC_DBG_STATS_FLAG_* */
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
```

Current source values:

```c
#define AVC_DBG_STREAM_SOURCE_CAMERA     0u
#define AVC_DBG_STREAM_SOURCE_SYNTHETIC  1u
```

Current stats flags:

```c
#define AVC_DBG_STATS_FLAG_STREAM_ENABLED  (1u << 0)
#define AVC_DBG_STATS_FLAG_TX_BUSY         (1u << 1)
#define AVC_DBG_STATS_FLAG_FRAME_ACTIVE    (1u << 2)
```

## Text, Logs, and Overlay

Use separate text/log messages instead of drawing a full overlay on the MCU
when the host can compose it.

Text payloads are UTF-8 by default:

```c
typedef struct __attribute__((packed))
{
    uint32_t text_id;
    uint16_t x;
    uint16_t y;
    uint16_t color_rgb565;
    uint16_t flags;
    uint8_t  utf8[];
} avc_dbg_rui_text_t;
```

Log text can be simpler:

```c
typedef struct __attribute__((packed))
{
    uint32_t level;
    uint32_t source;
    uint8_t  utf8[];
} avc_dbg_log_text_t;
```

## Control Direction

Host-to-device control can use the same envelope over CDC bulk OUT:

- start stream
- stop stream
- select synthetic/camera/composed mode
- request stats
- request geometry

The current ASCII `START`/`STOP` commands are acceptable during bring-up, but
should become control messages once this protocol is implemented.

Current bring-up commands:

- `START`: enable camera-frame streaming. Frames are published from the AVC
  main loop after the existing marker/overlay drawing, not from the camera ISR.
- `START_SYNTH` or `SYNTH`: enable the synthetic maximum-rate frame source used
  for transport ceiling tests.
- `STOP` or `0`: disable either source.

## Implementation Notes

- Put the final wire constants and structs in a shared protocol header, likely
  under `src/common`, so firmware and native host tooling can include the same
  source of truth.
- Python and JavaScript receivers may keep mirrored constants initially, but
  once framing is locked they should either be generated from the shared header
  or have a small conformance test against captured packets.
- Keep the existing working `AVCU` proof until the protocol transition is
  implemented on both firmware and host.
- Do not send from camera ISR context. Queue or drop frames from main-loop or
  USB service context.
- The first AVC integration copies each camera chunk into one aligned 16 KiB
  USB TX staging buffer before calling the CDC ACM send API. This is not
  zero-copy, but at `320x200` RGB565 and 24 FPS it is only about 2.93 MiB/s of
  memory copy, which is well below the measured synthetic USB ceiling.
- If a camera frame is published while a previous camera frame is still being
  chunked, the new frame is dropped. The packet header `DROPPED_BEFORE` flag is
  set on the next successfully queued packet after any such drop. The stats
  report carries the cumulative dropped-frame count.
- If CDC ACM's one-transfer-per-endpoint abstraction becomes limiting, replace
  it with a thinner endpoint layer while keeping this message protocol.
