#!/usr/bin/env python3
"""Measure the FRDM-MCXN947 NXP Cup CDC debug-display stream."""

from __future__ import annotations

import argparse
import struct
import time
from dataclasses import dataclass, field

import serial


MAGIC = b"AVCU"
PACKET_HEADER_SIZE = 32
RUI_FRAME_CHUNK_HEADER_SIZE = 24
LOG_RECORD_HEADER_SIZE = 12
TELEMETRY_SCALAR_HEADER_SIZE = 16
TELEMETRY_TEXT_MAX_BYTES = 48
STATS_REPORT_SIZE = 76
HELLO_RESPONSE_SIZE = 24
FRAME_BYTES = 320 * 200 * 2
PACKET_PAYLOAD_MAX_BYTES = (16 * 1024) - PACKET_HEADER_SIZE
FRAME_DATA_MAX_BYTES = PACKET_PAYLOAD_MAX_BYTES - RUI_FRAME_CHUNK_HEADER_SIZE
MSG_ID_RUI_WRITE_FRAME_BUFFER_RAW = 0x01000002
MSG_ID_LOG_TEXT = 0x01000200
MSG_ID_STATS_REPORT = 0x01000300
MSG_ID_TELEMETRY_SCALAR = 0x01000500
MSG_ID_CONTROL_HELLO = 0x01000403
MSG_ID_CONTROL_SET_CHANNELS = 0x01000404
MSG_ID_CONTROL_PING = 0x01000405
MSG_ID_CONTROL_CLOSE = 0x01000406
MSG_ID_CONTROL_ERROR = 0x01000407
PACKET_FLAGS_KNOWN = 0x000F
PACKET_FLAG_RESPONSE = 1 << 0
PACKET_FLAG_DROPPED_BEFORE = 1 << 3
PIXEL_FORMAT_RGB565_LE = 1
CHUNK_FLAG_FRAME_START = 1
CHUNK_FLAG_FRAME_END = 2
CHUNK_FLAGS_KNOWN = CHUNK_FLAG_FRAME_START | CHUNK_FLAG_FRAME_END | 4
CHANNEL_FRAMES = 1 << 0
CHANNEL_STATS = 1 << 1
CHANNEL_LOGS = 1 << 2
CHANNEL_TELEMETRY = 1 << 3
STREAM_SOURCE_CAMERA = 0
STREAM_SOURCE_SYNTHETIC = 1
CONTROL_STATUS_OK = 0


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--port", required=True, help="CDC COM port, for example COM15")
    parser.add_argument("--seconds", type=float, default=10.0, help="Measurement duration")
    parser.add_argument("--baud", type=int, default=115200, help="Ignored by USB CDC but required by serial APIs")
    parser.add_argument("--read-size", type=int, default=262144, help="Maximum bytes to read per call")
    parser.add_argument(
        "--start-command",
        default="START",
        help="Source selector. Use START_SYNTH for the synthetic max-rate source.",
    )
    parser.add_argument(
        "--legacy-ascii",
        action="store_true",
        help="Use the pre-session ASCII START/STOP controls with older firmware.",
    )
    parser.add_argument(
        "--rx-buffer-size",
        type=int,
        default=16 * 1024 * 1024,
        help="Requested host serial RX buffer size on platforms that support it",
    )
    parser.add_argument(
        "--prestart-drain-seconds",
        type=float,
        default=1.0,
        help="Maximum time to drain stale target bytes after sending STOP before measurement",
    )
    parser.add_argument(
        "--prestart-quiet-seconds",
        type=float,
        default=0.2,
        help="Required quiet time before issuing START",
    )
    parser.add_argument(
        "--start-read-delay",
        type=float,
        default=0.0,
        help="Delay reads after channel start to exercise device backpressure/drop recovery.",
    )
    parser.add_argument(
        "--control-ping-burst",
        type=int,
        default=0,
        help="Send this many framed PING requests in one write after stream start.",
    )
    parser.add_argument("--error-limit", type=int, default=5, help="Number of parser anomaly summaries to retain")
    return parser.parse_args()


@dataclass
class StreamParser:
    error_limit: int
    buffer: bytearray = field(default_factory=bytearray)
    cursor: int = 0
    parsed_headers: int = 0
    parsed_chunks: int = 0
    stats_packets: int = 0
    log_records: list[tuple[int, int, int, str, str]] = field(default_factory=list)
    telemetry_samples: list[tuple[int, int, int, str, str, object]] = field(default_factory=list)
    dropped_before_packets: int = 0
    initial_resync_bytes: int = 0
    malformed_headers: int = 0
    resync_events: int = 0
    resync_bytes: int = 0
    invalid_headers: int = 0
    sequence_errors: int = 0
    last_frame_id: int = -1
    last_chunk_id: int = -1
    last_offset: int = -1
    expected_sequence: int | None = None
    expected_frame_id: int | None = None
    expected_offset: int = 0
    last_stats: tuple[int, ...] | None = None
    control_responses: list[tuple[int, int, int, int, bytes]] = field(default_factory=list)
    errors: list[str] = field(default_factory=list)

    def extend(self, data: bytes) -> None:
        if self.cursor > 1024 * 1024:
            self._compact()
        self.buffer.extend(data)
        self._parse()
        if self.cursor > 0 and (self.cursor > len(self.buffer) // 2 or self.cursor > 1024 * 1024):
            self._compact()

    def _compact(self) -> None:
        del self.buffer[: self.cursor]
        self.cursor = 0

    def _record_error(self, message: str) -> None:
        if len(self.errors) < self.error_limit:
            self.errors.append(message)

    def _parse(self) -> None:
        while len(self.buffer) - self.cursor >= PACKET_HEADER_SIZE:
            magic_index = self.buffer.find(MAGIC, self.cursor)
            if magic_index < 0:
                unread = len(self.buffer) - self.cursor
                keep = min(unread, len(MAGIC) - 1)
                dropped = unread - keep
                if dropped > 0:
                    if self.parsed_headers == 0:
                        self.initial_resync_bytes += dropped
                    else:
                        self.malformed_headers += 1
                        self.resync_events += 1
                        self.resync_bytes += dropped
                        self._record_error(f"resync_no_magic dropped={dropped}")
                    self.cursor += dropped
                return
            if magic_index > self.cursor:
                dropped = magic_index - self.cursor
                self.cursor = magic_index
                if self.parsed_headers == 0:
                    self.initial_resync_bytes += dropped
                else:
                    self.malformed_headers += 1
                    self.resync_events += 1
                    self.resync_bytes += dropped
                    self._record_error(f"resync_to_magic dropped={dropped}")
                if len(self.buffer) - self.cursor < PACKET_HEADER_SIZE:
                    return

            header = bytes(self.buffer[self.cursor : self.cursor + PACKET_HEADER_SIZE])
            magic, version, header_len, packet_flags, msg_id, sequence, payload_len, arg0, arg1, arg2 = (
                struct.unpack_from("<4sBBHIIIIII", header)
            )
            is_frame_packet = msg_id == MSG_ID_RUI_WRITE_FRAME_BUFFER_RAW
            is_log_packet = msg_id == MSG_ID_LOG_TEXT
            is_stats_packet = msg_id == MSG_ID_STATS_REPORT
            is_telemetry_packet = msg_id == MSG_ID_TELEMETRY_SCALAR
            is_control_packet = (
                msg_id
                in {
                    MSG_ID_CONTROL_HELLO,
                    MSG_ID_CONTROL_SET_CHANNELS,
                    MSG_ID_CONTROL_PING,
                    MSG_ID_CONTROL_CLOSE,
                    MSG_ID_CONTROL_ERROR,
                }
                and (packet_flags & PACKET_FLAG_RESPONSE) != 0
            )
            valid_header = (
                magic == MAGIC
                and version == 1
                and header_len == PACKET_HEADER_SIZE
                and (packet_flags & ~PACKET_FLAGS_KNOWN) == 0
                and payload_len <= PACKET_PAYLOAD_MAX_BYTES
                and (
                    (is_frame_packet and RUI_FRAME_CHUNK_HEADER_SIZE < payload_len)
                    or (is_log_packet and payload_len >= LOG_RECORD_HEADER_SIZE)
                    or (is_stats_packet and payload_len == STATS_REPORT_SIZE)
                    or (is_telemetry_packet and payload_len >= TELEMETRY_SCALAR_HEADER_SIZE)
                    or (
                        is_control_packet
                        and payload_len <= HELLO_RESPONSE_SIZE
                        and (msg_id != MSG_ID_CONTROL_HELLO or payload_len == HELLO_RESPONSE_SIZE)
                    )
                )
            )
            if not valid_header:
                self.cursor += 1
                self.malformed_headers += 1
                self.invalid_headers += 1
                self._record_error(
                    "invalid_header "
                    f"version={version} header_len={header_len} flags=0x{packet_flags:04x} "
                    f"msg_id=0x{msg_id:08x} sequence={sequence} payload={payload_len}"
                )
                continue

            packet_len = PACKET_HEADER_SIZE + payload_len
            packet_end = self.cursor + packet_len
            if len(self.buffer) < packet_end:
                return

            payload_start = self.cursor + PACKET_HEADER_SIZE
            if self.expected_sequence is not None and sequence != self.expected_sequence:
                self.sequence_errors += 1
                self._record_error(f"sequence expected_packet={self.expected_sequence} got={sequence}")
            self.expected_sequence = sequence + 1

            if (packet_flags & PACKET_FLAG_DROPPED_BEFORE) != 0:
                self.dropped_before_packets += 1

            if is_stats_packet:
                self.last_stats = struct.unpack_from("<IIIIIIIIIIIIIIIIIII", self.buffer, payload_start)
                self.stats_packets += 1
                self.parsed_headers += 1
                self.cursor = packet_end
                continue

            if is_telemetry_packet:
                timestamp_ms, sample_id, value_bits, name_bytes, value_type, units_bytes = struct.unpack_from(
                    "<IIIHBB",
                    self.buffer,
                    payload_start,
                )
                name_start = payload_start + TELEMETRY_SCALAR_HEADER_SIZE
                units_start = name_start + name_bytes
                text_start = units_start + units_bytes
                text_bytes = value_bits if value_type == 5 else 0
                text_valid = value_type != 5 or (
                    0 < text_bytes <= TELEMETRY_TEXT_MAX_BYTES and units_bytes == 0
                )
                if (
                    text_start + text_bytes != packet_end
                    or value_type not in {1, 2, 3, 4, 5}
                    or name_bytes == 0
                    or not text_valid
                ):
                    self.invalid_headers += 1
                    self._record_error(
                        f"invalid_telemetry sample={sample_id} type={value_type} name={name_bytes} units={units_bytes}"
                    )
                else:
                    name = bytes(self.buffer[name_start:units_start]).decode("utf-8", errors="replace")
                    units = bytes(self.buffer[units_start:text_start]).decode("utf-8", errors="replace")
                    if value_type == 1:
                        value = struct.unpack("<i", struct.pack("<I", value_bits))[0]
                    elif value_type == 2:
                        value = value_bits
                    elif value_type == 3:
                        value = struct.unpack("<f", struct.pack("<I", value_bits))[0]
                    elif value_type == 4:
                        value = value_bits != 0
                    else:
                        value = bytes(self.buffer[text_start:packet_end]).decode("utf-8", errors="replace")
                    self.telemetry_samples.append((timestamp_ms, sample_id, value_type, name, units, value))
                self.parsed_headers += 1
                self.cursor = packet_end
                continue

            if is_log_packet:
                timestamp_ms, record_id, text_bytes, level, category_bytes = struct.unpack_from(
                    "<IIHBB",
                    self.buffer,
                    payload_start,
                )
                category_start = payload_start + LOG_RECORD_HEADER_SIZE
                text_start = category_start + category_bytes
                if text_start + text_bytes != packet_end or level > 4:
                    self.invalid_headers += 1
                    self._record_error(
                        f"invalid_log record={record_id} level={level} category={category_bytes} text={text_bytes}"
                    )
                else:
                    category = bytes(self.buffer[category_start:text_start]).decode("utf-8", errors="replace")
                    text = bytes(self.buffer[text_start:packet_end]).decode("utf-8", errors="replace")
                    self.log_records.append((timestamp_ms, record_id, level, category, text))
                self.parsed_headers += 1
                self.cursor = packet_end
                continue

            if is_control_packet:
                payload = bytes(self.buffer[payload_start:packet_end])
                self.control_responses.append((msg_id, arg0, arg1, arg2, payload))
                self.parsed_headers += 1
                self.cursor = packet_end
                continue

            (
                frame_id,
                offset,
                frame_bytes,
                width,
                height,
                pixel_format,
                buffer_id,
                chunk_flags,
            ) = struct.unpack_from("<IIIHHHHI", self.buffer, payload_start)
            data_len = payload_len - RUI_FRAME_CHUNK_HEADER_SIZE
            valid_chunk = (
                frame_bytes == FRAME_BYTES
                and width == 320
                and height == 200
                and pixel_format == PIXEL_FORMAT_RGB565_LE
                and buffer_id == 0
                and offset < frame_bytes
                and offset + data_len <= frame_bytes
                and (chunk_flags & ~CHUNK_FLAGS_KNOWN) == 0
                and arg0 == frame_id
                and arg1 == offset
                and arg2 == data_len
            )
            if not valid_chunk:
                self.cursor += 1
                self.malformed_headers += 1
                self.invalid_headers += 1
                self._record_error(
                    "invalid_chunk "
                    f"frame={frame_id} offset={offset} data={data_len} frame_bytes={frame_bytes} "
                    f"size={width}x{height} format={pixel_format} buffer={buffer_id} "
                    f"chunk_flags=0x{chunk_flags:08x}"
                )
                continue

            if (
                (packet_flags & PACKET_FLAG_DROPPED_BEFORE) != 0
                and (chunk_flags & CHUNK_FLAG_FRAME_START) != 0
            ):
                self.expected_frame_id = frame_id
                self.expected_offset = 0

            if self.expected_frame_id is not None:
                if frame_id != self.expected_frame_id or offset != self.expected_offset:
                    self.sequence_errors += 1
                    self._record_error(
                        "sequence "
                        f"expected={self.expected_frame_id}/{self.expected_offset} "
                        f"got={frame_id}/{offset}"
                    )

            if offset == 0 and (chunk_flags & CHUNK_FLAG_FRAME_START) == 0:
                self.sequence_errors += 1
                self._record_error(f"missing_frame_start frame={frame_id}")

            if (chunk_flags & CHUNK_FLAG_FRAME_END) != 0:
                if offset + data_len != frame_bytes:
                    self.sequence_errors += 1
                    self._record_error(
                        f"early_frame_end frame={frame_id} offset={offset} data={data_len}"
                    )
                self.expected_frame_id = frame_id + 1
                self.expected_offset = 0
            else:
                if data_len != FRAME_DATA_MAX_BYTES:
                    self.sequence_errors += 1
                    self._record_error(f"short_nonfinal frame={frame_id} offset={offset} data={data_len}")
                self.expected_frame_id = frame_id
                self.expected_offset = offset + data_len

            self.cursor = packet_end
            self.parsed_headers += 1
            self.parsed_chunks += 1
            self.last_frame_id = frame_id
            self.last_chunk_id = offset // FRAME_DATA_MAX_BYTES
            self.last_offset = offset


def drain_input_until_quiet(port: serial.Serial, max_seconds: float, quiet_seconds: float, read_size: int) -> int:
    drained = 0
    start = time.perf_counter()
    quiet_start = start

    while time.perf_counter() - start < max_seconds:
        waiting = port.in_waiting
        data = port.read(min(max(waiting, 1), read_size))
        now = time.perf_counter()
        if data:
            drained += len(data)
            quiet_start = now
        elif now - quiet_start >= quiet_seconds:
            break

    return drained


def build_control_packet(sequence: int, msg_id: int, arg0: int = 0, arg1: int = 0, arg2: int = 0) -> bytes:
    return struct.pack(
        "<4sBBHIIIIII",
        MAGIC,
        1,
        PACKET_HEADER_SIZE,
        0,
        msg_id,
        sequence,
        0,
        arg0,
        arg1,
        arg2,
    )


def wait_for_control_response(
    port: serial.Serial,
    parser: StreamParser,
    msg_id: int,
    request_sequence: int,
    timeout_seconds: float = 2.0,
) -> tuple[int, int, int, int, bytes]:
    deadline = time.perf_counter() + timeout_seconds
    while time.perf_counter() < deadline:
        for response in parser.control_responses:
            if response[0] == msg_id and response[1] == request_sequence:
                return response
        data = port.read(65536)
        if data:
            parser.extend(data)
    raise RuntimeError(
        f"timed out waiting for control response msg_id=0x{msg_id:08x} request={request_sequence}"
    )


def main() -> int:
    args = parse_args()
    rx_total = 0
    parser = StreamParser(error_limit=args.error_limit)

    with serial.Serial(args.port, args.baud, timeout=0.05, write_timeout=1.0) as port:
        if args.rx_buffer_size > 0 and hasattr(port, "set_buffer_size"):
            port.set_buffer_size(rx_size=args.rx_buffer_size)
        port.dtr = True
        port.rts = True
        time.sleep(0.25)
        if args.legacy_ascii:
            port.reset_input_buffer()
            port.write(b"STOP\n")
            port.flush()
            drain_input_until_quiet(
                port,
                max_seconds=args.prestart_drain_seconds,
                quiet_seconds=args.prestart_quiet_seconds,
                read_size=min(args.read_size, 65536),
            )
            port.reset_input_buffer()
            port.write(args.start_command.encode("ascii") + b"\n")
        else:
            port.reset_input_buffer()
            port.write(build_control_packet(0, MSG_ID_CONTROL_HELLO))
            hello = wait_for_control_response(port, parser, MSG_ID_CONTROL_HELLO, 0)
            if hello[2] != CONTROL_STATUS_OK:
                raise RuntimeError(f"HELLO failed with status {hello[2]}")

            stream_source = (
                STREAM_SOURCE_SYNTHETIC
                if args.start_command.upper() in {"START_SYNTH", "SYNTH"}
                else STREAM_SOURCE_CAMERA
            )
            channels = CHANNEL_FRAMES | CHANNEL_LOGS | CHANNEL_TELEMETRY | (
                CHANNEL_STATS if stream_source == STREAM_SOURCE_CAMERA else 0
            )
            port.write(
                build_control_packet(
                    1,
                    MSG_ID_CONTROL_SET_CHANNELS,
                    arg0=channels,
                    arg1=stream_source,
                )
            )
            if args.control_ping_burst > 0:
                port.write(
                    b"".join(
                        build_control_packet(2 + index, MSG_ID_CONTROL_PING)
                        for index in range(args.control_ping_burst)
                    )
                )

        if args.start_read_delay > 0:
            time.sleep(args.start_read_delay)

        start = time.perf_counter()
        deadline = start + args.seconds
        while time.perf_counter() < deadline:
            data = port.read(args.read_size)
            if not data:
                continue
            rx_total += len(data)
            parser.extend(data)

        elapsed = time.perf_counter() - start
        if args.legacy_ascii:
            port.write(b"STOP\n")
        else:
            stop_sequence = 2 + max(args.control_ping_burst, 0)
            port.write(
                build_control_packet(
                    stop_sequence,
                    MSG_ID_CONTROL_SET_CHANNELS,
                )
            )
            wait_for_control_response(
                port,
                parser,
                MSG_ID_CONTROL_SET_CHANNELS,
                stop_sequence,
            )
            close_sequence = stop_sequence + 1
            port.write(build_control_packet(close_sequence, MSG_ID_CONTROL_CLOSE))
            wait_for_control_response(
                port,
                parser,
                MSG_ID_CONTROL_CLOSE,
                close_sequence,
            )
        time.sleep(0.05)

    mbps = (rx_total / elapsed) / (1024.0 * 1024.0)
    fps = 0.0
    if parser.last_frame_id >= 0:
        fps = (parser.last_frame_id + 1) / elapsed

    print(f"port={args.port}")
    print(f"seconds={elapsed:.3f}")
    print(f"bytes={rx_total}")
    print(f"throughput_mib_s={mbps:.3f}")
    print(f"parsed_packets={parser.parsed_headers}")
    print(f"parsed_chunks={parser.parsed_chunks}")
    print(f"stats_packets={parser.stats_packets}")
    print(f"log_records={len(parser.log_records)}")
    print(f"telemetry_samples={len(parser.telemetry_samples)}")
    print(f"control_responses={len(parser.control_responses)}")
    print(f"dropped_before_packets={parser.dropped_before_packets}")
    print(f"initial_resync_bytes={parser.initial_resync_bytes}")
    print(f"last_frame_id={parser.last_frame_id}")
    print(f"last_chunk_id={parser.last_chunk_id}")
    print(f"last_offset={parser.last_offset}")
    print(f"approx_frame_rate={fps:.2f}")
    print(f"malformed_headers={parser.malformed_headers}")
    print(f"resync_events={parser.resync_events}")
    print(f"resync_bytes={parser.resync_bytes}")
    print(f"invalid_headers={parser.invalid_headers}")
    print(f"sequence_errors={parser.sequence_errors}")
    if parser.last_stats is not None:
        (
            stream_source,
            stream_flags,
            next_sequence,
            current_frame_id,
            frames_started,
            frames_completed,
            frames_dropped,
            packets_sent,
            bytes_sent,
            endpoint_busy_count,
            send_error_count,
            rx_command_count,
            control_response_drop_count,
            control_response_queue_high_water,
            log_drop_count,
            log_queue_high_water,
            telemetry_drop_count,
            telemetry_queue_high_water,
            telemetry_coalesce_count,
        ) = parser.last_stats
        print(f"stats_stream_source={stream_source}")
        print(f"stats_stream_flags=0x{stream_flags:08x}")
        print(f"stats_next_sequence={next_sequence}")
        print(f"stats_current_frame_id={current_frame_id}")
        print(f"stats_frames_started={frames_started}")
        print(f"stats_frames_completed={frames_completed}")
        print(f"stats_frames_dropped={frames_dropped}")
        print(f"stats_packets_sent={packets_sent}")
        print(f"stats_bytes_sent={bytes_sent}")
        print(f"stats_endpoint_busy_count={endpoint_busy_count}")
        print(f"stats_send_error_count={send_error_count}")
        print(f"stats_rx_command_count={rx_command_count}")
        print(f"stats_control_response_drop_count={control_response_drop_count}")
        print(f"stats_control_response_queue_high_water={control_response_queue_high_water}")
        print(f"stats_log_drop_count={log_drop_count}")
        print(f"stats_log_queue_high_water={log_queue_high_water}")
        print(f"stats_telemetry_drop_count={telemetry_drop_count}")
        print(f"stats_telemetry_queue_high_water={telemetry_queue_high_water}")
        print(f"stats_telemetry_coalesce_count={telemetry_coalesce_count}")
    for timestamp_ms, record_id, level, category, text in parser.log_records:
        print(f"log_{record_id}={timestamp_ms},{level},{category},{text}")
    for timestamp_ms, sample_id, value_type, name, units, value in parser.telemetry_samples:
        print(f"telemetry_{sample_id}={timestamp_ms},{value_type},{name},{value},{units}")
    for index, error in enumerate(parser.errors, start=1):
        print(f"parser_error_{index}={error}")

    if rx_total == 0 or parser.parsed_chunks == 0:
        return 2
    if not args.legacy_ascii and len(parser.control_responses) < (4 + max(args.control_ping_burst, 0)):
        return 3
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
