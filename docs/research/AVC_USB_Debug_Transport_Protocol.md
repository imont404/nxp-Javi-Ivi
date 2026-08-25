# AVC USB Debug Transport Protocol

Status: implemented core, named telemetry, and safe ROM-ISP extension

This document describes the durable AVC USB debug-display transport. The Rev A
competition firmware, native Windows receiver, Python receiver, and standalone
Web Serial viewer use the shared `AVCU` envelope, framed control, RUI raw-frame
chunks, statistics, bounded log records, and typed named telemetry. Additional
RUI/RIO operations remain reserved design space rather than implemented claims.

See `AVC_USB_Debug_Display_Current_State.md` for the proven implementation
state, measured rates, and host-tool paths. The native one-cable viewer/programmer
is owned by `docs/plans/one-cable-host-tool/plan.md`; remaining browser telemetry
work is owned by `docs/plans/usb-debug-telemetry/plan.md`.

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
#define AVC_DBG_MSG_CLASS_TELEMETRY 0x01000500u
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
#define AVC_DBG_CONTROL_HELLO               (AVC_DBG_MSG_CLASS_CONTROL + 3u)
#define AVC_DBG_CONTROL_SET_CHANNELS        (AVC_DBG_MSG_CLASS_CONTROL + 4u)
#define AVC_DBG_CONTROL_PING                (AVC_DBG_MSG_CLASS_CONTROL + 5u)
#define AVC_DBG_CONTROL_CLOSE               (AVC_DBG_MSG_CLASS_CONTROL + 6u)
#define AVC_DBG_CONTROL_ERROR               (AVC_DBG_MSG_CLASS_CONTROL + 7u)
#define AVC_DBG_CONTROL_ENTER_ISP           (AVC_DBG_MSG_CLASS_CONTROL + 8u)
#define AVC_DBG_CONTROL_SYSTEM_ACTION       (AVC_DBG_MSG_CLASS_CONTROL + 9u)
#define AVC_DBG_TELEMETRY_SCALAR            (AVC_DBG_MSG_CLASS_TELEMETRY + 0u)
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
    uint32_t control_response_drop_count;
    uint32_t control_response_queue_high_water;
    uint32_t log_drop_count;
    uint32_t log_queue_high_water;
    uint32_t telemetry_drop_count;
    uint32_t telemetry_queue_high_water;
    uint32_t telemetry_coalesce_count;
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

Log records carry bounded UTF-8 category and text bytes immediately after a
fixed header. Neither string is NUL-terminated on the wire:

```c
typedef struct __attribute__((packed))
{
    uint32_t timestamp_ms;
    uint32_t record_id;
    uint16_t text_bytes;
    uint8_t  level;
    uint8_t  category_bytes;
} avc_dbg_log_record_t;
```

The current firmware accepts at most 15 category bytes and 160 text bytes per
record, queues eight fixed records, truncates longer input, and drops rather
than blocks when the queue is full. Levels are TRACE, DEBUG, INFO, WARNING, and
ERROR. Formatting is skipped completely when no recognized session subscribes
to logs. The API is main-loop/thread context only; camera and USB ISRs must not
format log strings.

## Named Telemetry

Telemetry records carry one typed value plus bounded UTF-8 name and units bytes.
The version-1 fixed header remains 16 bytes. The name immediately follows it,
then the optional units. Scalar types end there. Text type 5 appends its value;
none of the strings is NUL-terminated on the wire:

```c
typedef struct __attribute__((packed))
{
    uint32_t timestamp_ms;
    uint32_t sample_id;
    uint32_t value_bits;
    uint16_t name_bytes;
    uint8_t  value_type;
    uint8_t  units_bytes;
} avc_dbg_telemetry_scalar_t;
```

Types 1 through 4 are signed 32-bit integer, unsigned 32-bit integer, IEEE-754
32-bit float, and boolean. Their `value_bits` field carries the little-endian
32-bit representation and their packet bytes are unchanged. Type 5 is a
non-empty UTF-8 text value: `value_bits` is its trailing byte count, units must
be empty, and payload size is `16 + name_bytes + text_bytes`. Firmware accepts
at most 31 name bytes, 15 units bytes, and 48 text bytes. An oversized text value
is rejected rather than truncated, so a sender cannot emit partial UTF-8 bytes.

The sixteen-entry fixed queue is keyed by name. Publishing a name already
pending replaces that queued record with the newest type, value, units,
timestamp, and sample ID; this is counted as a coalesce rather than a drop. A
new name is dropped when the queue is full. Calls return before inspecting names
or values unless a recognized session subscribes to telemetry.

The public API functions are `telemetry_i32`, `telemetry_u32`, `telemetry_f32`,
`telemetry_bool`, and `telemetry_text`. Internal `NXPC_DBG_VALUE_*` macros remain
available to framework code. Six queue positions and six names are reserved for
framework mode, state, and actuator diagnostics, so participant telemetry cannot
replace or starve them.

## Control Direction

Host-to-device control uses the same envelope over CDC bulk OUT. The competition
firmware implements `HELLO`, `SET_CHANNELS`, `PING`, `CLOSE`, and the narrow
`SYSTEM_ACTION` operation. `HELLO`
establishes a recognized telemetry session and reports capabilities and geometry;
`SET_CHANNELS` independently selects frames, stats, logs, and telemetry. These
diagnostic commands do not select vehicle mode or enable motors.

Legacy `START_STREAM`, `STOP_STREAM`, and `SET_STREAM_MODE` IDs remain allocated
for compatibility with older proof images and synthetic ceiling tools, but the
competition firmware does not accept the old ASCII control path. New hosts should
use the framed session commands.

The Web Serial viewer, Python receiver, and native receiver use framed control. A
response sets `AVC_DBG_PACKET_FLAG_RESPONSE`; `arg0` is the request sequence,
`arg1` is a stable control status, and `arg2` is the current session ID.

`ENTER_ISP` is the deliberately narrow exception to diagnostic-only control. It
is accepted only in a recognized session, requires confirmation value
`0x21505349` (`"ISP!"`) in `arg0` and zero in the other arguments, and returns a
correlated status before handoff. Firmware advertises capability bit 6 only when
this path is present. After it can queue the successful response, firmware stops
new diagnostic publication, disconnects zero-duty motor outputs, centers the
servo, permits at least 100 ms for the response and actuator settling, bounds the
wait at 500 ms, and invokes the MCXN947 ROM API for USB-HS HID interface 5. It is
not a general remote actuator or vehicle-mode command.

`SYSTEM_ACTION` carries a typed action in `arg0`; it never carries a target mode
or raw actuator value. Action 1 is `RACE_START`, action 2 is `STOP`, and `arg2`
must be zero. `RACE_START` additionally requires `0x21214F47` (`"GO!!"`) in
`arg1`. The firmware advertises capability bit 7 when this operation is present.
It accepts start only while the physical TEST input is inactive, the system is in
`RACE_WAITING`, and at least one camera frame has arrived. It reports `NOT_READY`
when the camera gate is unmet and `DENIED` when the requested transition is not
permitted. `STOP` requires zero in `arg1`, is accepted from every system state,
immediately disables both motors and centers steering, and returns
`RACE_RUNNING` to `RACE_WAITING`. The successful response is queued only after
the system state machine has applied the action.

## Transmit Arbitration

The CDC bulk-IN endpoint has exactly one transfer and one aligned staging buffer
in flight. A single bounded dispatcher owns both. Transfer completion schedules
at most one next packet; the main-loop service uses the same dispatcher and
returns immediately when the endpoint is busy.

Current arbitration rules are:

- Control responses use a fixed four-entry FIFO and preempt bulk data between
  packets. The queue never allocates memory or overwrites an unsent response.
- At most four consecutive control responses may pass an already-pending
  non-control packet. This prevents a command burst from starving frame or
  diagnostic traffic while retaining bounded response latency.
- Stream statistics are a coalescing one-slot source. Repeated due events remain
  one pending report carrying the newest cumulative counters.
- Camera frames use latest-frame semantics. A newly published frame is dropped
  while another frame is active, and buffer-generation checks can abort an
  unsafe active frame.
- Every source submits through the same endpoint-busy, sequence, byte-count,
  busy-count, and send-error bookkeeping.
- The log source is an eight-record FIFO and yields to a pending frame after at
  most two consecutive diagnostic packets.
- Named telemetry adds its own fixed-depth/drop-policy source to this dispatcher;
  it does not call the CDC send API directly. Log and telemetry sources alternate
  when both are pending and together yield to a pending frame after two packets.

Queue overflow is observable through `control_response_drop_count`; the highest
observed response depth is `control_response_queue_high_water`. Logs expose the
equivalent `log_drop_count` and `log_queue_high_water` counters.
Telemetry exposes drop, queue high-water, and coalesce counters.

Opening CDC does not establish a telemetry session and never changes vehicle
mode. `HELLO` recognizes the client and returns implemented capabilities and
frame geometry. `SET_CHANNELS` controls diagnostic output only. `CLOSE` stops
output and invalidates the session. Hosts should wait for the correlated
`SET_CHANNELS(0)` and `CLOSE` responses before closing the CDC handle; this
drains the last in-flight frame chunk and prevents stale bytes from crossing
into a rapid reopen. Legacy ASCII start/stop commands are not accepted.

Bulk OUT remains armed even if a generic host writes with DTR low. Those bytes
are discarded while the session is closed, then the endpoint is immediately
re-armed so a later framed `HELLO` does not require a USB bus reset.

## Implementation Notes

- Final wire constants and structs live in
  `src/common/avc_usb_debug/avc_usb_debug_protocol.h`; firmware and native host
  tooling include that source of truth.
- Python, JavaScript, and Kotlin receivers mirror the constants and are exercised against
  conformance fixtures plus real framed traffic. The Android bridge deliberately avoids
  an NDK dependency solely to include a small packed C header.
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
- The two camera buffers have a separate generation guard. If capture reaches
  the reuse horizon during an active USB frame, firmware checks again after the
  staging copy, discards that copy, aborts the incomplete frame, and guarantees
  `DROPPED_BEFORE` on the next `FRAME_START`. Receivers reset partial frame
  assembly at that marker and never present the aborted frame.
- If CDC ACM's one-transfer-per-endpoint abstraction becomes limiting, replace
  it with a thinner endpoint layer while keeping this message protocol.
- Native programming discovers application CDC as `1FC9:0094` and ROM HID as
  `1FC9:014F`. It never treats COM-port disappearance alone as successful ISP
  entry and never exposes fuse, CMPA, security, or program-once commands.

## Android Relay Boundary

Android consumes `AVCU` directly from the car; firmware has no phone-specific mode or
message ID. The phone's one-browser relay preserves generic telemetry as `AVCU` and uses
separate 32-byte little-endian video envelopes only on the Wi-Fi side:

- `AVCJ`: one complete independently decodable JPEG per WebSocket message;
- `AVC4`: ISO BMFF initialization or one fragmented-MP4 H.264 media fragment;
- `AVCR`: one complete 320x200 RGB565 little-endian diagnostic frame.

The sole browser client selects `jpeg`, `h264`, or `raw`; JPEG is the default. These
representations are intentionally downstream of the durable firmware protocol. Their
wire details and measured behavior live in `src/android/avc_bridge/README.md` and
`docs/plans/android-telemetry-bridge/plan.md`.
