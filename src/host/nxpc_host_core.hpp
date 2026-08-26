#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "../common/nxpc_usb_debug/nxpc_usb_debug_protocol.h"

namespace nxpc::host
{

constexpr uint16_t kNxpCupUsbVid = 0x1FC9u;
constexpr uint16_t kNxpCupRuntimePid = 0x0094u;
constexpr uint16_t kMcxn947RomPid = 0x014Fu;

struct SerialDevice
{
    std::string port_name;
    std::string friendly_name;
    std::string hardware_id;
    std::string instance_id;
    uint16_t vid = 0u;
    uint16_t pid = 0u;
};

struct HidDevice
{
    std::string device_path;
    std::string friendly_name;
    std::string hardware_id;
    std::string instance_id;
    uint16_t vid = 0u;
    uint16_t pid = 0u;
};

std::vector<SerialDevice> list_serial_devices(std::string &error);
std::vector<SerialDevice> find_serial_devices(uint16_t vid, uint16_t pid, std::string &error);
bool select_unique_runtime_port(const std::string &requested_port, SerialDevice &selected,
                                std::string &error);
std::vector<HidDevice> list_hid_devices(std::string &error);
std::vector<HidDevice> find_hid_devices(uint16_t vid, uint16_t pid, std::string &error);

class SerialPort
{
  public:
    SerialPort();
    ~SerialPort();

    SerialPort(const SerialPort &) = delete;
    SerialPort &operator=(const SerialPort &) = delete;

    bool open(const std::string &port_name, uint32_t baud, uint32_t rx_buffer_bytes,
              std::string &error);
    int read(uint8_t *buffer, uint32_t buffer_bytes, std::string &error);
    bool write_all(const uint8_t *data, uint32_t data_bytes, std::string &error);
    bool clear_input(std::string &error);
    void close();
    bool is_open() const;

  private:
    void *handle_;
};

struct ControlResponse
{
    uint32_t msg_id = 0u;
    uint32_t request_sequence = 0u;
    uint32_t status = 0u;
    uint32_t detail = 0u;
    std::vector<uint8_t> payload;
};

struct Frame
{
    uint32_t frame_id = 0u;
    uint16_t width = 0u;
    uint16_t height = 0u;
    uint16_t pixel_format = 0u;
    uint64_t generation = 0u;
    std::vector<uint8_t> pixels;
};

struct LogRecord
{
    uint32_t timestamp_ms = 0u;
    uint32_t record_id = 0u;
    uint8_t level = 0u;
    std::string category;
    std::string text;
};

struct TelemetrySample
{
    uint32_t timestamp_ms = 0u;
    uint32_t sample_id = 0u;
    uint32_t value_bits = 0u;
    uint8_t value_type = 0u;
    std::string name;
    std::string units;
    std::string text_value;
};

struct ParserCounters
{
    uint64_t packets = 0u;
    uint64_t frame_chunks = 0u;
    uint64_t frames = 0u;
    uint64_t stats = 0u;
    uint64_t logs = 0u;
    uint64_t telemetry = 0u;
    uint64_t controls = 0u;
    uint64_t unknown = 0u;
    uint64_t malformed = 0u;
    uint64_t resync_bytes = 0u;
    uint64_t dropped_before = 0u;
};

class StreamParser
{
  public:
    StreamParser();
    ~StreamParser();
    StreamParser(StreamParser &&) noexcept;
    StreamParser &operator=(StreamParser &&) noexcept;

    StreamParser(const StreamParser &) = delete;
    StreamParser &operator=(const StreamParser &) = delete;

    void feed(const uint8_t *data, size_t data_bytes);
    bool take_control_response(uint32_t msg_id, uint32_t request_sequence,
                               ControlResponse &response);
    bool latest_frame(uint64_t after_generation, Frame &frame) const;
    bool latest_stats(nxpc_dbg_stats_report_t &stats) const;
    const std::vector<LogRecord> &logs() const;
    const std::vector<TelemetrySample> &telemetry() const;
    const ParserCounters &counters() const;
    const std::string &last_error() const;

  private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

nxpc_dbg_packet_header_t make_control_request(uint32_t request_sequence, uint32_t msg_id,
                                              uint32_t arg0 = 0u, uint32_t arg1 = 0u,
                                              uint32_t arg2 = 0u);
bool send_control_request(SerialPort &port, uint32_t request_sequence, uint32_t msg_id,
                          uint32_t arg0, uint32_t arg1, uint32_t arg2, std::string &error);
bool wait_for_control_response(SerialPort &port, StreamParser &parser, uint32_t msg_id,
                               uint32_t request_sequence, uint32_t timeout_ms,
                               ControlResponse &response, std::string &error);

bool decode_hello(const ControlResponse &response, nxpc_dbg_control_hello_response_t &hello,
                  std::string &error);

bool run_core_self_test(std::string &error);

} // namespace nxpc::host
