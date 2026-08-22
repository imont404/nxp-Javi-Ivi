#define NOMINMAX
#include <windows.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../common/avc_usb_debug/avc_usb_debug_protocol.h"

namespace
{
constexpr uint8_t kMagic[] = {'A', 'V', 'C', 'U'};
constexpr uint32_t kHeaderSize = AVC_DBG_PACKET_HEADER_BYTES;
constexpr uint32_t kRuiFrameChunkHeaderSize = AVC_DBG_RUI_WRITE_FRAME_BUFFER_RAW_HEADER_BYTES;
constexpr uint32_t kStatsReportSize = AVC_DBG_STATS_REPORT_BYTES;
constexpr uint32_t kFrameBytes = 320U * 200U * 2U;
constexpr uint32_t kPacketPayloadMaxBytes = (16U * 1024U) - kHeaderSize;
constexpr uint32_t kFrameDataMaxBytes = kPacketPayloadMaxBytes - kRuiFrameChunkHeaderSize;
constexpr uint32_t kKnownPacketFlags = AVC_DBG_PACKET_FLAG_RESPONSE | AVC_DBG_PACKET_FLAG_MORE |
                                       AVC_DBG_PACKET_FLAG_PAYLOAD_CRC32 | AVC_DBG_PACKET_FLAG_DROPPED_BEFORE;
constexpr uint32_t kKnownChunkFlags = AVC_DBG_RUI_CHUNK_FRAME_START | AVC_DBG_RUI_CHUNK_FRAME_END |
                                      AVC_DBG_RUI_CHUNK_STALE_OK;

struct Options
{
    std::string port;
    double seconds = 10.0;
    uint32_t baud = 115200U;
    uint32_t readSize = 256U * 1024U;
    uint32_t rxBufferSize = 16U * 1024U * 1024U;
    double prestartDrainSeconds = 1.0;
    double prestartQuietSeconds = 0.2;
    size_t errorLimit = 5U;
    std::string startCommand = "START";
};

struct ControlResponse
{
    uint32_t msgId;
    uint32_t requestSequence;
    uint32_t status;
};

struct Parser
{
    explicit Parser(size_t limit) : errorLimit(limit) {}

    void extend(const uint8_t *data, size_t length)
    {
        if (cursor > 1024U * 1024U)
        {
            compact();
        }

        buffer.insert(buffer.end(), data, data + length);
        parse();

        if ((cursor > 0U) && ((cursor > (buffer.size() / 2U)) || (cursor > 1024U * 1024U)))
        {
            compact();
        }
    }

    void recordError(const std::string &message)
    {
        if (errors.size() < errorLimit)
        {
            errors.push_back(message);
        }
    }

    void compact()
    {
        buffer.erase(buffer.begin(), buffer.begin() + static_cast<std::ptrdiff_t>(cursor));
        cursor = 0U;
    }

    static uint16_t readLe16(const uint8_t *data)
    {
        return static_cast<uint16_t>(data[0]) | (static_cast<uint16_t>(data[1]) << 8U);
    }

    static uint32_t readLe32(const uint8_t *data)
    {
        return static_cast<uint32_t>(data[0]) | (static_cast<uint32_t>(data[1]) << 8U) |
               (static_cast<uint32_t>(data[2]) << 16U) | (static_cast<uint32_t>(data[3]) << 24U);
    }

    size_t findMagic() const
    {
        auto begin = buffer.begin() + static_cast<std::ptrdiff_t>(cursor);
        auto found = std::search(begin, buffer.end(), std::begin(kMagic), std::end(kMagic));
        if (found == buffer.end())
        {
            return std::string::npos;
        }
        return static_cast<size_t>(found - buffer.begin());
    }

    void parse()
    {
        while ((buffer.size() - cursor) >= kHeaderSize)
        {
            size_t magicIndex = findMagic();
            if (magicIndex == std::string::npos)
            {
                size_t unread = buffer.size() - cursor;
                size_t keep = std::min(unread, sizeof(kMagic) - 1U);
                size_t dropped = unread - keep;
                if (dropped > 0U)
                {
                    if (parsedHeaders == 0U)
                    {
                        initialResyncBytes += dropped;
                    }
                    else
                    {
                        malformedHeaders++;
                        resyncEvents++;
                        resyncBytes += dropped;
                        recordError("resync_no_magic dropped=" + std::to_string(dropped));
                    }
                    cursor += dropped;
                }
                return;
            }

            if (magicIndex > cursor)
            {
                size_t dropped = magicIndex - cursor;
                cursor = magicIndex;
                if (parsedHeaders == 0U)
                {
                    initialResyncBytes += dropped;
                }
                else
                {
                    malformedHeaders++;
                    resyncEvents++;
                    resyncBytes += dropped;
                    recordError("resync_to_magic dropped=" + std::to_string(dropped));
                }
                if ((buffer.size() - cursor) < kHeaderSize)
                {
                    return;
                }
            }

            const uint8_t *header = buffer.data() + cursor;
            uint8_t version = header[4];
            uint8_t headerLen = header[5];
            uint16_t packetFlags = readLe16(header + 6);
            uint32_t msgId = readLe32(header + 8);
            uint32_t sequence = readLe32(header + 12);
            uint32_t payloadLen = readLe32(header + 16);
            uint32_t arg0 = readLe32(header + 20);
            uint32_t arg1 = readLe32(header + 24);
            uint32_t arg2 = readLe32(header + 28);
            bool isFramePacket = msgId == AVC_DBG_RUI_WRITE_FRAME_BUFFER_RAW;
            bool isStatsPacket = msgId == AVC_DBG_STATS_REPORT;
            bool isControlPacket =
                ((packetFlags & AVC_DBG_PACKET_FLAG_RESPONSE) != 0U) &&
                ((msgId == AVC_DBG_CONTROL_HELLO) || (msgId == AVC_DBG_CONTROL_SET_CHANNELS) ||
                 (msgId == AVC_DBG_CONTROL_PING) || (msgId == AVC_DBG_CONTROL_CLOSE) ||
                 (msgId == AVC_DBG_CONTROL_ERROR));

            bool validHeader = (std::memcmp(header, kMagic, sizeof(kMagic)) == 0) && (version == 1U) &&
                               (headerLen == kHeaderSize) && ((packetFlags & ~kKnownPacketFlags) == 0U) &&
                               (payloadLen <= kPacketPayloadMaxBytes) &&
                               ((isFramePacket && (payloadLen > kRuiFrameChunkHeaderSize)) ||
                                (isStatsPacket && (payloadLen == kStatsReportSize)) ||
                                (isControlPacket && (payloadLen <= AVC_DBG_CONTROL_HELLO_RESPONSE_BYTES) &&
                                 ((msgId != AVC_DBG_CONTROL_HELLO) ||
                                  (payloadLen == AVC_DBG_CONTROL_HELLO_RESPONSE_BYTES))));
            if (!validHeader)
            {
                cursor++;
                malformedHeaders++;
                invalidHeaders++;

                std::ostringstream msg;
                msg << "invalid_header version=" << static_cast<uint32_t>(version)
                    << " header_len=" << static_cast<uint32_t>(headerLen) << " flags=0x" << std::hex
                    << std::setw(4) << std::setfill('0') << packetFlags << " msg_id=0x" << std::setw(8)
                    << msgId << std::dec << " sequence=" << sequence << " payload=" << payloadLen;
                recordError(msg.str());
                continue;
            }

            size_t packetLen = static_cast<size_t>(kHeaderSize) + payloadLen;
            size_t packetEnd = cursor + packetLen;
            if (buffer.size() < packetEnd)
            {
                return;
            }

            if (haveExpectedPacket && (sequence != expectedSequence))
            {
                sequenceErrors++;

                std::ostringstream msg;
                msg << "sequence expected_packet=" << expectedSequence << " got=" << sequence;
                recordError(msg.str());
            }
            expectedSequence = sequence + 1U;
            haveExpectedPacket = true;

            if ((packetFlags & AVC_DBG_PACKET_FLAG_DROPPED_BEFORE) != 0U)
            {
                droppedBeforePackets++;
            }

            if (isControlPacket)
            {
                controlResponses.push_back({msgId, arg0, arg1});
                parsedHeaders++;
                cursor = packetEnd;
                continue;
            }

            if (isStatsPacket)
            {
                const uint8_t *payload = buffer.data() + cursor + kHeaderSize;
                lastStats.stream_source = readLe32(payload + 0);
                lastStats.stream_flags = readLe32(payload + 4);
                lastStats.next_sequence = readLe32(payload + 8);
                lastStats.current_frame_id = readLe32(payload + 12);
                lastStats.frames_started = readLe32(payload + 16);
                lastStats.frames_completed = readLe32(payload + 20);
                lastStats.frames_dropped = readLe32(payload + 24);
                lastStats.packets_sent = readLe32(payload + 28);
                lastStats.bytes_sent = readLe32(payload + 32);
                lastStats.endpoint_busy_count = readLe32(payload + 36);
                lastStats.send_error_count = readLe32(payload + 40);
                lastStats.rx_command_count = readLe32(payload + 44);
                lastStats.control_response_drop_count = readLe32(payload + 48);
                lastStats.control_response_queue_high_water = readLe32(payload + 52);
                lastStats.log_drop_count = readLe32(payload + 56);
                lastStats.log_queue_high_water = readLe32(payload + 60);
                lastStats.telemetry_drop_count = readLe32(payload + 64);
                lastStats.telemetry_queue_high_water = readLe32(payload + 68);
                lastStats.telemetry_coalesce_count = readLe32(payload + 72);
                haveStats = true;
                statsPackets++;
                parsedHeaders++;
                cursor = packetEnd;
                continue;
            }

            const uint8_t *chunk = buffer.data() + cursor + kHeaderSize;
            uint32_t frameId = readLe32(chunk + 0);
            uint32_t offset = readLe32(chunk + 4);
            uint32_t frameBytes = readLe32(chunk + 8);
            uint16_t width = readLe16(chunk + 12);
            uint16_t height = readLe16(chunk + 14);
            uint16_t pixelFormat = readLe16(chunk + 16);
            uint16_t bufferId = readLe16(chunk + 18);
            uint32_t chunkFlags = readLe32(chunk + 20);
            uint32_t dataLen = payloadLen - kRuiFrameChunkHeaderSize;

            bool validChunk = (frameBytes == kFrameBytes) && (width == 320U) && (height == 200U) &&
                              (pixelFormat == AVC_DBG_PIXEL_FORMAT_RGB565_LE) && (bufferId == 0U) &&
                              (offset < frameBytes) && ((offset + dataLen) <= frameBytes) &&
                              ((chunkFlags & ~kKnownChunkFlags) == 0U) && (arg0 == frameId) &&
                              (arg1 == offset) && (arg2 == dataLen);
            if (!validChunk)
            {
                cursor++;
                malformedHeaders++;
                invalidHeaders++;

                std::ostringstream msg;
                msg << "invalid_chunk frame=" << frameId << " offset=" << offset << " data=" << dataLen
                    << " frame_bytes=" << frameBytes << " size=" << width << "x" << height
                    << " format=" << pixelFormat << " buffer=" << bufferId << " chunk_flags=0x" << std::hex
                    << std::setw(8) << std::setfill('0') << chunkFlags;
                recordError(msg.str());
                continue;
            }

            if (((packetFlags & AVC_DBG_PACKET_FLAG_DROPPED_BEFORE) != 0U) &&
                ((chunkFlags & AVC_DBG_RUI_CHUNK_FRAME_START) != 0U))
            {
                expectedFrameId = frameId;
                expectedOffset = 0U;
                haveExpected = true;
            }

            if (haveExpected)
            {
                if ((frameId != expectedFrameId) || (offset != expectedOffset))
                {
                    sequenceErrors++;

                    std::ostringstream msg;
                    msg << "sequence expected=" << expectedFrameId << "/" << expectedOffset << " got=" << frameId
                        << "/" << offset;
                    recordError(msg.str());
                }
            }

            if ((offset == 0U) && ((chunkFlags & AVC_DBG_RUI_CHUNK_FRAME_START) == 0U))
            {
                sequenceErrors++;
                recordError("missing_frame_start frame=" + std::to_string(frameId));
            }

            if ((chunkFlags & AVC_DBG_RUI_CHUNK_FRAME_END) != 0U)
            {
                if ((offset + dataLen) != frameBytes)
                {
                    sequenceErrors++;

                    std::ostringstream msg;
                    msg << "early_frame_end frame=" << frameId << " offset=" << offset << " data=" << dataLen;
                    recordError(msg.str());
                }

                expectedFrameId = frameId + 1U;
                expectedOffset = 0U;
            }
            else
            {
                if (dataLen != kFrameDataMaxBytes)
                {
                    sequenceErrors++;

                    std::ostringstream msg;
                    msg << "short_nonfinal frame=" << frameId << " offset=" << offset << " data=" << dataLen;
                    recordError(msg.str());
                }

                expectedFrameId = frameId;
                expectedOffset = offset + dataLen;
            }

            haveExpected = true;
            cursor = packetEnd;
            parsedHeaders++;
            parsedFrameChunks++;
            lastFrameId = frameId;
            lastChunkId = offset / kFrameDataMaxBytes;
            lastOffset = offset;
            haveLastFrame = true;
        }
    }

    size_t errorLimit;
    std::vector<uint8_t> buffer;
    size_t cursor = 0U;
    uint64_t parsedHeaders = 0U;
    uint64_t parsedFrameChunks = 0U;
    uint64_t statsPackets = 0U;
    uint64_t droppedBeforePackets = 0U;
    uint64_t initialResyncBytes = 0U;
    uint64_t malformedHeaders = 0U;
    uint64_t resyncEvents = 0U;
    uint64_t resyncBytes = 0U;
    uint64_t invalidHeaders = 0U;
    uint64_t sequenceErrors = 0U;
    uint32_t lastFrameId = 0U;
    uint32_t lastChunkId = 0U;
    uint32_t lastOffset = 0U;
    bool haveLastFrame = false;
    bool haveExpectedPacket = false;
    bool haveExpected = false;
    uint32_t expectedSequence = 0U;
    uint32_t expectedFrameId = 0U;
    uint32_t expectedOffset = 0U;
    avc_dbg_stats_report_t lastStats{};
    bool haveStats = false;
    std::vector<ControlResponse> controlResponses;
    std::vector<std::string> errors;
};

std::string getLastErrorMessage()
{
    DWORD error = GetLastError();
    LPSTR message = nullptr;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   nullptr,
                   error,
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                   reinterpret_cast<LPSTR>(&message),
                   0,
                   nullptr);

    std::string result = message ? message : "unknown error";
    if (message)
    {
        LocalFree(message);
    }
    return result;
}

void usage()
{
    std::cerr << "usage: usb_cdc_stream_read.exe --port COM15 [--seconds 10] [--read-size 262144]\n"
              << "                               [--rx-buffer-size 16777216] [--start-command START_SYNTH]\n";
}

uint32_t parseU32(const std::string &value, const char *name)
{
    char *end = nullptr;
    unsigned long parsed = std::strtoul(value.c_str(), &end, 10);
    if ((end == value.c_str()) || (*end != '\0'))
    {
        throw std::runtime_error(std::string("invalid ") + name + ": " + value);
    }
    return static_cast<uint32_t>(parsed);
}

double parseDouble(const std::string &value, const char *name)
{
    char *end = nullptr;
    double parsed = std::strtod(value.c_str(), &end);
    if ((end == value.c_str()) || (*end != '\0'))
    {
        throw std::runtime_error(std::string("invalid ") + name + ": " + value);
    }
    return parsed;
}

Options parseArgs(int argc, char **argv)
{
    Options options;
    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];
        auto needValue = [&](const char *name) -> std::string {
            if ((i + 1) >= argc)
            {
                throw std::runtime_error(std::string("missing value for ") + name);
            }
            i++;
            return argv[i];
        };

        if ((arg == "-h") || (arg == "--help"))
        {
            usage();
            std::exit(0);
        }
        if (arg == "--port")
        {
            options.port = needValue("--port");
        }
        else if (arg == "--seconds")
        {
            options.seconds = parseDouble(needValue("--seconds"), "--seconds");
        }
        else if (arg == "--baud")
        {
            options.baud = parseU32(needValue("--baud"), "--baud");
        }
        else if (arg == "--read-size")
        {
            options.readSize = parseU32(needValue("--read-size"), "--read-size");
        }
        else if (arg == "--rx-buffer-size")
        {
            options.rxBufferSize = parseU32(needValue("--rx-buffer-size"), "--rx-buffer-size");
        }
        else if (arg == "--prestart-drain-seconds")
        {
            options.prestartDrainSeconds = parseDouble(needValue("--prestart-drain-seconds"),
                                                       "--prestart-drain-seconds");
        }
        else if (arg == "--prestart-quiet-seconds")
        {
            options.prestartQuietSeconds = parseDouble(needValue("--prestart-quiet-seconds"),
                                                       "--prestart-quiet-seconds");
        }
        else if (arg == "--error-limit")
        {
            options.errorLimit = parseU32(needValue("--error-limit"), "--error-limit");
        }
        else if (arg == "--start-command")
        {
            options.startCommand = needValue("--start-command");
        }
        else
        {
            throw std::runtime_error("unknown argument: " + arg);
        }
    }

    if (options.port.empty())
    {
        throw std::runtime_error("--port is required");
    }
    if (options.readSize == 0U)
    {
        throw std::runtime_error("--read-size must be positive");
    }
    return options;
}

std::string normalizePortName(const std::string &port)
{
    if (port.rfind("\\\\", 0) == 0)
    {
        return port;
    }
    return "\\\\.\\" + port;
}

class SerialPort
{
public:
    explicit SerialPort(const Options &options)
    {
        handle_ = CreateFileA(normalizePortName(options.port).c_str(),
                              GENERIC_READ | GENERIC_WRITE,
                              0,
                              nullptr,
                              OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL,
                              nullptr);
        if (handle_ == INVALID_HANDLE_VALUE)
        {
            throw std::runtime_error("CreateFile failed: " + getLastErrorMessage());
        }

        SetupComm(handle_, options.rxBufferSize, 4096U);

        DCB dcb{};
        dcb.DCBlength = sizeof(dcb);
        if (!GetCommState(handle_, &dcb))
        {
            throw std::runtime_error("GetCommState failed: " + getLastErrorMessage());
        }
        dcb.BaudRate = options.baud;
        dcb.ByteSize = 8;
        dcb.Parity = NOPARITY;
        dcb.StopBits = ONESTOPBIT;
        dcb.fBinary = TRUE;
        dcb.fDtrControl = DTR_CONTROL_ENABLE;
        dcb.fRtsControl = RTS_CONTROL_ENABLE;
        if (!SetCommState(handle_, &dcb))
        {
            throw std::runtime_error("SetCommState failed: " + getLastErrorMessage());
        }

        COMMTIMEOUTS timeouts{};
        timeouts.ReadIntervalTimeout = MAXDWORD;
        timeouts.ReadTotalTimeoutMultiplier = 0;
        timeouts.ReadTotalTimeoutConstant = 50;
        timeouts.WriteTotalTimeoutMultiplier = 0;
        timeouts.WriteTotalTimeoutConstant = 1000;
        if (!SetCommTimeouts(handle_, &timeouts))
        {
            throw std::runtime_error("SetCommTimeouts failed: " + getLastErrorMessage());
        }

        EscapeCommFunction(handle_, SETDTR);
        EscapeCommFunction(handle_, SETRTS);
        PurgeComm(handle_, PURGE_RXCLEAR | PURGE_TXCLEAR);
    }

    ~SerialPort()
    {
        if (handle_ != INVALID_HANDLE_VALUE)
        {
            CloseHandle(handle_);
        }
    }

    void writeAll(const char *data, size_t length)
    {
        const uint8_t *cursor = reinterpret_cast<const uint8_t *>(data);
        size_t remaining = length;
        while (remaining > 0U)
        {
            DWORD written = 0;
            DWORD chunk = static_cast<DWORD>(std::min<size_t>(remaining, 4096U));
            if (!WriteFile(handle_, cursor, chunk, &written, nullptr))
            {
                throw std::runtime_error("WriteFile failed: " + getLastErrorMessage());
            }
            cursor += written;
            remaining -= written;
        }
    }

    DWORD read(uint8_t *data, DWORD length)
    {
        DWORD received = 0;
        if (!ReadFile(handle_, data, length, &received, nullptr))
        {
            throw std::runtime_error("ReadFile failed: " + getLastErrorMessage());
        }
        return received;
    }

    void clearInput()
    {
        PurgeComm(handle_, PURGE_RXCLEAR);
    }

private:
    HANDLE handle_ = INVALID_HANDLE_VALUE;
};

avc_dbg_packet_header_t buildControlPacket(uint32_t requestSequence,
                                           uint32_t msgId,
                                           uint32_t arg0 = 0U,
                                           uint32_t arg1 = 0U,
                                           uint32_t arg2 = 0U)
{
    avc_dbg_packet_header_t packet{};
    packet.magic = AVC_DBG_MAGIC;
    packet.version = AVC_DBG_VERSION;
    packet.header_bytes = AVC_DBG_PACKET_HEADER_BYTES;
    packet.msg_id = msgId;
    packet.sequence = requestSequence;
    packet.arg0 = arg0;
    packet.arg1 = arg1;
    packet.arg2 = arg2;
    return packet;
}

void sendControl(SerialPort &port,
                 uint32_t requestSequence,
                 uint32_t msgId,
                 uint32_t arg0 = 0U,
                 uint32_t arg1 = 0U,
                 uint32_t arg2 = 0U)
{
    avc_dbg_packet_header_t packet = buildControlPacket(requestSequence, msgId, arg0, arg1, arg2);
    port.writeAll(reinterpret_cast<const char *>(&packet), sizeof(packet));
}

ControlResponse waitForControlResponse(SerialPort &port,
                                       Parser &parser,
                                       uint32_t msgId,
                                       uint32_t requestSequence,
                                       double timeoutSeconds = 2.0)
{
    std::array<uint8_t, 65536U> readBuffer{};
    auto deadline = std::chrono::steady_clock::now() + std::chrono::duration<double>(timeoutSeconds);

    while (std::chrono::steady_clock::now() < deadline)
    {
        for (const ControlResponse &response : parser.controlResponses)
        {
            if ((response.msgId == msgId) && (response.requestSequence == requestSequence))
            {
                return response;
            }
        }

        DWORD received = port.read(readBuffer.data(), static_cast<DWORD>(readBuffer.size()));
        if (received > 0U)
        {
            parser.extend(readBuffer.data(), received);
        }
    }

    std::ostringstream message;
    message << "control response timeout msg_id=0x" << std::hex << msgId << std::dec
            << " request=" << requestSequence;
    throw std::runtime_error(message.str());
}

uint64_t drainUntilQuiet(SerialPort &port, double maxSeconds, double quietSeconds, uint32_t readSize)
{
    std::vector<uint8_t> buffer(std::min<uint32_t>(readSize, 65536U));
    uint64_t drained = 0U;
    auto start = std::chrono::steady_clock::now();
    auto quietStart = start;

    while (true)
    {
        auto now = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double>(now - start).count();
        double quietElapsed = std::chrono::duration<double>(now - quietStart).count();
        if ((elapsed >= maxSeconds) || (quietElapsed >= quietSeconds))
        {
            break;
        }

        DWORD received = port.read(buffer.data(), static_cast<DWORD>(buffer.size()));
        now = std::chrono::steady_clock::now();
        if (received > 0U)
        {
            drained += received;
            quietStart = now;
        }
    }

    return drained;
}
} // namespace

int main(int argc, char **argv)
{
    try
    {
        Options options = parseArgs(argc, argv);
        SerialPort port(options);
        std::vector<uint8_t> readBuffer(options.readSize);
        Parser parser(options.errorLimit);

        Sleep(250);
        port.clearInput();
        sendControl(port, 0U, AVC_DBG_CONTROL_HELLO);
        ControlResponse hello = waitForControlResponse(port, parser, AVC_DBG_CONTROL_HELLO, 0U);
        if (hello.status != AVC_DBG_CONTROL_STATUS_OK)
        {
            throw std::runtime_error("HELLO rejected with status " + std::to_string(hello.status));
        }

        bool synthetic = (options.startCommand == "START_SYNTH") || (options.startCommand == "SYNTH");
        uint32_t streamSource = synthetic ? AVC_DBG_STREAM_SOURCE_SYNTHETIC : AVC_DBG_STREAM_SOURCE_CAMERA;
        uint32_t channels = AVC_DBG_CHANNEL_FRAMES | (synthetic ? 0U : AVC_DBG_CHANNEL_STATS);
        sendControl(port, 1U, AVC_DBG_CONTROL_SET_CHANNELS, channels, streamSource);
        ControlResponse startResponse =
            waitForControlResponse(port, parser, AVC_DBG_CONTROL_SET_CHANNELS, 1U);
        if (startResponse.status != AVC_DBG_CONTROL_STATUS_OK)
        {
            throw std::runtime_error("SET_CHANNELS rejected with status " +
                                     std::to_string(startResponse.status));
        }

        uint64_t rxTotal = 0U;
        auto start = std::chrono::steady_clock::now();
        auto deadline = start + std::chrono::duration<double>(options.seconds);
        while (std::chrono::steady_clock::now() < deadline)
        {
            DWORD received = port.read(readBuffer.data(), static_cast<DWORD>(readBuffer.size()));
            if (received == 0U)
            {
                continue;
            }
            rxTotal += received;
            parser.extend(readBuffer.data(), received);
        }
        auto end = std::chrono::steady_clock::now();

        sendControl(port, 2U, AVC_DBG_CONTROL_SET_CHANNELS);
        (void)waitForControlResponse(port, parser, AVC_DBG_CONTROL_SET_CHANNELS, 2U);
        sendControl(port, 3U, AVC_DBG_CONTROL_CLOSE);
        (void)waitForControlResponse(port, parser, AVC_DBG_CONTROL_CLOSE, 3U);

        double elapsed = std::chrono::duration<double>(end - start).count();
        double mibPerSecond = (static_cast<double>(rxTotal) / elapsed) / (1024.0 * 1024.0);
        double fps = parser.haveLastFrame ? (static_cast<double>(parser.lastFrameId) + 1.0) / elapsed : 0.0;

        std::cout << std::fixed << std::setprecision(3);
        std::cout << "port=" << options.port << "\n";
        std::cout << "seconds=" << elapsed << "\n";
        std::cout << "bytes=" << rxTotal << "\n";
        std::cout << "throughput_mib_s=" << mibPerSecond << "\n";
        std::cout << "parsed_packets=" << parser.parsedHeaders << "\n";
        std::cout << "parsed_chunks=" << parser.parsedFrameChunks << "\n";
        std::cout << "stats_packets=" << parser.statsPackets << "\n";
        std::cout << "control_responses=" << parser.controlResponses.size() << "\n";
        std::cout << "dropped_before_packets=" << parser.droppedBeforePackets << "\n";
        std::cout << "initial_resync_bytes=" << parser.initialResyncBytes << "\n";
        if (parser.haveLastFrame)
        {
            std::cout << "last_frame_id=" << parser.lastFrameId << "\n";
            std::cout << "last_chunk_id=" << parser.lastChunkId << "\n";
            std::cout << "last_offset=" << parser.lastOffset << "\n";
        }
        else
        {
            std::cout << "last_frame_id=-1\n";
            std::cout << "last_chunk_id=-1\n";
            std::cout << "last_offset=-1\n";
        }
        std::cout << "approx_frame_rate=" << std::setprecision(2) << fps << "\n";
        std::cout << "malformed_headers=" << parser.malformedHeaders << "\n";
        std::cout << "resync_events=" << parser.resyncEvents << "\n";
        std::cout << "resync_bytes=" << parser.resyncBytes << "\n";
        std::cout << "invalid_headers=" << parser.invalidHeaders << "\n";
        std::cout << "sequence_errors=" << parser.sequenceErrors << "\n";
        if (parser.haveStats)
        {
            std::cout << "stats_stream_source=" << parser.lastStats.stream_source << "\n";
            std::cout << "stats_stream_flags=0x" << std::hex << std::setw(8) << std::setfill('0')
                      << parser.lastStats.stream_flags << std::dec << std::setfill(' ') << "\n";
            std::cout << "stats_next_sequence=" << parser.lastStats.next_sequence << "\n";
            std::cout << "stats_current_frame_id=" << parser.lastStats.current_frame_id << "\n";
            std::cout << "stats_frames_started=" << parser.lastStats.frames_started << "\n";
            std::cout << "stats_frames_completed=" << parser.lastStats.frames_completed << "\n";
            std::cout << "stats_frames_dropped=" << parser.lastStats.frames_dropped << "\n";
            std::cout << "stats_packets_sent=" << parser.lastStats.packets_sent << "\n";
            std::cout << "stats_bytes_sent=" << parser.lastStats.bytes_sent << "\n";
            std::cout << "stats_endpoint_busy_count=" << parser.lastStats.endpoint_busy_count << "\n";
            std::cout << "stats_send_error_count=" << parser.lastStats.send_error_count << "\n";
            std::cout << "stats_rx_command_count=" << parser.lastStats.rx_command_count << "\n";
            std::cout << "stats_control_response_drop_count="
                      << parser.lastStats.control_response_drop_count << "\n";
            std::cout << "stats_control_response_queue_high_water="
                      << parser.lastStats.control_response_queue_high_water << "\n";
            std::cout << "stats_log_drop_count=" << parser.lastStats.log_drop_count << "\n";
            std::cout << "stats_log_queue_high_water=" << parser.lastStats.log_queue_high_water << "\n";
            std::cout << "stats_telemetry_drop_count=" << parser.lastStats.telemetry_drop_count << "\n";
            std::cout << "stats_telemetry_queue_high_water="
                      << parser.lastStats.telemetry_queue_high_water << "\n";
            std::cout << "stats_telemetry_coalesce_count="
                      << parser.lastStats.telemetry_coalesce_count << "\n";
        }
        for (size_t index = 0U; index < parser.errors.size(); index++)
        {
            std::cout << "parser_error_" << (index + 1U) << "=" << parser.errors[index] << "\n";
        }

        return (rxTotal == 0U || parser.parsedFrameChunks == 0U) ? 2 : 0;
    }
    catch (const std::exception &ex)
    {
        std::cerr << "error: " << ex.what() << "\n";
        usage();
        return 1;
    }
}
