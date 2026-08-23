#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <commdlg.h>

#include "avc_host_core.hpp"
#include "avc_programmer.hpp"

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <SDL.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace
{

struct Options
{
    std::string port;
    uint32_t baud = 115200u;
    uint32_t test_seconds = 0u;
};

struct SharedState
{
    std::mutex mutex;
    std::string status = "Looking for AVC TELEMETRY...";
    std::string error;
    std::string port;
    bool connected = false;
    uint64_t connection_count = 0u;
    avc_dbg_control_hello_response_t hello{};
    avc::host::Frame frame;
    avc::host::ParserCounters counters;
    avc_dbg_stats_report_t stats{};
    bool has_stats = false;
    std::vector<avc::host::TelemetrySample> telemetry;
    bool rom_connected = false;
    bool program_requested = false;
    bool program_busy = false;
    bool program_waiting_reconnect = false;
    bool program_succeeded = false;
    std::string program_image_path;
    std::string program_status = "Ready";
    std::string program_detail;
    std::string program_error;
};

uint32_t parse_u32(const std::string &text, const char *name)
{
    char *end = nullptr;
    const unsigned long parsed = std::strtoul(text.c_str(), &end, 10);
    if (text.empty() || (end == nullptr) || (*end != '\0') || (parsed > 0xFFFFFFFFul))
    {
        throw std::runtime_error(std::string("invalid ") + name + ": " + text);
    }
    return static_cast<uint32_t>(parsed);
}

Options parse_args(int argc, char **argv)
{
    Options options;
    for (int index = 1; index < argc; ++index)
    {
        const std::string argument = argv[index];
        auto value = [&](const char *name) -> std::string {
            if (++index >= argc)
            {
                throw std::runtime_error(std::string("missing value for ") + name);
            }
            return argv[index];
        };

        if (argument == "--port")
        {
            options.port = value("--port");
        }
        else if (argument == "--baud")
        {
            options.baud = parse_u32(value("--baud"), "--baud");
        }
        else if (argument == "--test-seconds")
        {
            options.test_seconds = parse_u32(value("--test-seconds"), "--test-seconds");
        }
        else if ((argument == "--help") || (argument == "-h"))
        {
            std::printf("AVC native camera and telemetry viewer\n\n"
                        "usage: avc_viewer.exe [--port COM34] [--baud 115200]\n"
                        "                      [--test-seconds N]\n\n"
                        "Without --port, exactly one VID_1FC9/PID_0094 device must be present.\n");
            std::exit(0);
        }
        else
        {
            throw std::runtime_error("unknown argument: " + argument);
        }
    }
    return options;
}

void publish_status(SharedState &shared,
                    const std::string &status,
                    const std::string &error,
                    bool connected)
{
    std::lock_guard<std::mutex> lock(shared.mutex);
    shared.status = status;
    shared.error = error;
    shared.connected = connected;
    if (!connected)
    {
        shared.port.clear();
    }
}

bool send_and_wait(avc::host::SerialPort &port,
                   avc::host::StreamParser &parser,
                   uint32_t sequence,
                   uint32_t msg_id,
                   uint32_t arg0,
                   uint32_t arg1,
                   uint32_t arg2,
                   avc::host::ControlResponse &response,
                   std::string &error);

void publish_program_status(SharedState &shared,
                            const std::string &status,
                            const std::string &detail = {})
{
    std::lock_guard<std::mutex> lock(shared.mutex);
    shared.program_status = status;
    shared.program_detail = detail;
}

void fail_program(SharedState &shared, const std::string &error)
{
    std::lock_guard<std::mutex> lock(shared.mutex);
    shared.program_busy = false;
    shared.program_waiting_reconnect = false;
    shared.program_succeeded = false;
    shared.program_status = "Programming failed";
    shared.program_error = error;
}

bool take_program_request(SharedState &shared, std::string &image_path)
{
    std::lock_guard<std::mutex> lock(shared.mutex);
    if (!shared.program_requested)
    {
        return false;
    }
    shared.program_requested = false;
    image_path = shared.program_image_path;
    return true;
}

bool wait_for_rom_device(SharedState &shared, std::string &error)
{
    publish_program_status(shared, "Waiting for MCXN947 ROM HID...");
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);
    while (std::chrono::steady_clock::now() < deadline)
    {
        const std::vector<avc::host::HidDevice> devices =
            avc::host::find_hid_devices(avc::host::kAvcUsbVid,
                                        avc::host::kMcxn947RomPid,
                                        error);
        if (!error.empty())
        {
            return false;
        }
        if (devices.size() > 1u)
        {
            error = "multiple MCXN947 ROM HID devices found; refusing to guess";
            return false;
        }
        if (devices.size() == 1u)
        {
            std::lock_guard<std::mutex> lock(shared.mutex);
            shared.rom_connected = true;
            shared.status = "MCXN947 ROM connected";
            shared.error.clear();
            return true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    error = "ROM HID did not appear within 5 seconds";
    return false;
}

bool run_programmer(const std::string &requested_image, SharedState &shared, std::string &error)
{
    publish_program_status(shared, "Validating firmware image...");
    avc::host::FirmwareImage image;
    if (!avc::host::validate_firmware_image(requested_image, image, error))
    {
        return false;
    }

    avc::host::ProgrammerTool programmer;
    if (!avc::host::resolve_programmer({}, programmer, error))
    {
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(shared.mutex);
        shared.program_detail = std::to_string(image.bytes) + " bytes, SHA-256 " + image.sha256;
    }

    if (!avc::host::program_rom(
            programmer,
            image,
            [&](avc::host::ProgramStage stage, const std::string &detail) {
                publish_program_status(shared,
                                       std::string("Programming: ") +
                                           avc::host::program_stage_name(stage),
                                       detail);
            },
            error))
    {
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(shared.mutex);
        shared.rom_connected = false;
        shared.program_waiting_reconnect = true;
        shared.program_status = "Programming complete; reconnecting...";
        shared.program_detail = image.sha256;
        shared.status = "Waiting for AVC TELEMETRY to reconnect...";
        shared.error.clear();
    }
    return true;
}

bool handle_runtime_program_request(avc::host::SerialPort &port,
                                    avc::host::StreamParser &parser,
                                    const avc_dbg_control_hello_response_t &hello,
                                    const std::string &image_path,
                                    SharedState &shared)
{
    std::string error;
    publish_program_status(shared, "Validating firmware image...");
    avc::host::FirmwareImage image;
    if (!avc::host::validate_firmware_image(image_path, image, error))
    {
        fail_program(shared, error);
        return false;
    }

    avc::host::ProgrammerTool programmer;
    if (!avc::host::resolve_programmer({}, programmer, error))
    {
        fail_program(shared, error);
        return false;
    }
    if ((hello.capability_flags & AVC_DBG_CAPABILITY_ENTER_ISP) == 0u)
    {
        fail_program(shared, "connected firmware does not advertise ENTER_ISP capability");
        return false;
    }

    publish_program_status(shared,
                           "Entering ROM ISP...",
                           std::to_string(image.bytes) + " bytes, SHA-256 " + image.sha256);
    avc::host::ControlResponse response;
    if (!send_and_wait(port,
                       parser,
                       2u,
                       AVC_DBG_CONTROL_ENTER_ISP,
                       AVC_DBG_ENTER_ISP_CONFIRMATION,
                       0u,
                       0u,
                       response,
                       error))
    {
        fail_program(shared, error);
        return false;
    }
    if (response.status != AVC_DBG_CONTROL_STATUS_OK)
    {
        fail_program(shared,
                     "ENTER_ISP rejected with status " + std::to_string(response.status));
        return false;
    }

    port.close();
    publish_status(shared, "Entering MCXN947 ROM...", {}, false);
    if (!wait_for_rom_device(shared, error))
    {
        fail_program(shared, error);
        return false;
    }

    if (!avc::host::program_rom(
            programmer,
            image,
            [&](avc::host::ProgramStage stage, const std::string &detail) {
                publish_program_status(shared,
                                       std::string("Programming: ") +
                                           avc::host::program_stage_name(stage),
                                       detail);
            },
            error))
    {
        fail_program(shared, error);
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(shared.mutex);
        shared.rom_connected = false;
        shared.program_waiting_reconnect = true;
        shared.program_status = "Programming complete; reconnecting...";
        shared.program_detail = image.sha256;
        shared.status = "Waiting for AVC TELEMETRY to reconnect...";
        shared.error.clear();
    }
    return true;
}

bool send_and_wait(avc::host::SerialPort &port,
                   avc::host::StreamParser &parser,
                   uint32_t sequence,
                   uint32_t msg_id,
                   uint32_t arg0,
                   uint32_t arg1,
                   uint32_t arg2,
                   avc::host::ControlResponse &response,
                   std::string &error)
{
    return avc::host::send_control_request(port, sequence, msg_id, arg0, arg1, arg2, error) &&
           avc::host::wait_for_control_response(port,
                                                parser,
                                                msg_id,
                                                sequence,
                                                2000u,
                                                response,
                                                error);
}

bool run_session(const Options &options, SharedState &shared, const std::atomic<bool> &running)
{
    std::string error;
    avc::host::SerialDevice device;
    if (!avc::host::select_unique_runtime_port(options.port, device, error))
    {
        publish_status(shared, "AVC TELEMETRY disconnected", error, false);
        return false;
    }

    publish_status(shared, "Opening " + device.port_name + "...", {}, false);
    avc::host::SerialPort port;
    if (!port.open(device.port_name, options.baud, 16u * 1024u * 1024u, error))
    {
        publish_status(shared, "AVC TELEMETRY disconnected", error, false);
        return false;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    if (!port.clear_input(error))
    {
        publish_status(shared, "AVC TELEMETRY disconnected", error, false);
        return false;
    }

    avc::host::StreamParser parser;
    avc::host::ControlResponse response;
    if (!send_and_wait(port,
                       parser,
                       0u,
                       AVC_DBG_CONTROL_HELLO,
                       0u,
                       0u,
                       0u,
                       response,
                       error))
    {
        publish_status(shared, "AVC TELEMETRY did not answer HELLO", error, false);
        return false;
    }

    avc_dbg_control_hello_response_t hello{};
    if (!avc::host::decode_hello(response, hello, error))
    {
        publish_status(shared, "AVC TELEMETRY rejected HELLO", error, false);
        return false;
    }

    constexpr uint32_t channels = AVC_DBG_CHANNEL_FRAMES | AVC_DBG_CHANNEL_STATS |
                                  AVC_DBG_CHANNEL_LOGS | AVC_DBG_CHANNEL_TELEMETRY;
    if (!send_and_wait(port,
                       parser,
                       1u,
                       AVC_DBG_CONTROL_SET_CHANNELS,
                       channels,
                       AVC_DBG_STREAM_SOURCE_CAMERA,
                       0u,
                       response,
                       error) ||
        (response.status != AVC_DBG_CONTROL_STATUS_OK))
    {
        if (error.empty())
        {
            error = "SET_CHANNELS rejected with status " + std::to_string(response.status);
        }
        publish_status(shared, "AVC TELEMETRY stream setup failed", error, false);
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(shared.mutex);
        shared.status = "AVC TELEMETRY connected";
        shared.error.clear();
        shared.port = device.port_name;
        shared.connected = true;
        ++shared.connection_count;
        shared.rom_connected = false;
        shared.hello = hello;
        if (shared.program_waiting_reconnect)
        {
            shared.program_waiting_reconnect = false;
            shared.program_busy = false;
            shared.program_succeeded = true;
            shared.program_status = "Programming complete; preview reconnected";
        }
    }

    std::vector<uint8_t> read_buffer(256u * 1024u);
    uint64_t published_generation = 0u;
    while (running.load())
    {
        std::string requested_image;
        if (take_program_request(shared, requested_image))
        {
            (void)handle_runtime_program_request(
                port, parser, hello, requested_image, shared);
            return false;
        }

        const int received =
            port.read(read_buffer.data(), static_cast<uint32_t>(read_buffer.size()), error);
        if (received < 0)
        {
            publish_status(shared, "AVC TELEMETRY disconnected", error, false);
            return false;
        }
        if (received == 0)
        {
            continue;
        }

        parser.feed(read_buffer.data(), static_cast<size_t>(received));
        avc::host::Frame frame;
        const bool has_new_frame = parser.latest_frame(published_generation, frame);
        avc_dbg_stats_report_t stats{};
        const bool has_stats = parser.latest_stats(stats);
        {
            std::lock_guard<std::mutex> lock(shared.mutex);
            if (has_new_frame)
            {
                published_generation = frame.generation;
                frame.generation = shared.frame.generation + 1u;
                shared.frame = std::move(frame);
            }
            shared.counters = parser.counters();
            if (has_stats)
            {
                shared.stats = stats;
                shared.has_stats = true;
            }
            shared.telemetry = parser.telemetry();
        }
    }

    // Best effort only: application unplug and shutdown remain recoverable.
    error.clear();
    (void)send_and_wait(port,
                        parser,
                        2u,
                        AVC_DBG_CONTROL_SET_CHANNELS,
                        0u,
                        AVC_DBG_STREAM_SOURCE_CAMERA,
                        0u,
                        response,
                        error);
    error.clear();
    (void)send_and_wait(port,
                        parser,
                        3u,
                        AVC_DBG_CONTROL_CLOSE,
                        0u,
                        0u,
                        0u,
                        response,
                        error);
    publish_status(shared, "Viewer stopped", {}, false);
    return true;
}

void connection_worker(const Options &options, SharedState &shared, const std::atomic<bool> &running)
{
    while (running.load())
    {
        (void)run_session(options, shared, running);

        std::string error;
        const std::vector<avc::host::HidDevice> rom_devices =
            avc::host::find_hid_devices(avc::host::kAvcUsbVid,
                                        avc::host::kMcxn947RomPid,
                                        error);
        {
            std::lock_guard<std::mutex> lock(shared.mutex);
            shared.rom_connected = error.empty() && (rom_devices.size() == 1u);
            if (shared.rom_connected && !shared.program_busy)
            {
                shared.status = "MCXN947 ROM connected";
                shared.error.clear();
            }
        }

        std::string requested_image;
        if (take_program_request(shared, requested_image))
        {
            if (!error.empty())
            {
                fail_program(shared, error);
            }
            else if (rom_devices.size() != 1u)
            {
                fail_program(shared,
                             "expected exactly one MCXN947 ROM HID device; found " +
                                 std::to_string(rom_devices.size()));
            }
            else if (!run_programmer(requested_image, shared, error))
            {
                fail_program(shared, error);
            }
        }

        for (unsigned tick = 0u; running.load() && (tick < 10u); ++tick)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

void rgb565_to_rgba(const avc::host::Frame &frame, std::vector<uint8_t> &rgba)
{
    const size_t pixel_count = static_cast<size_t>(frame.width) * frame.height;
    rgba.assign(pixel_count * 4u, 255u);
    const size_t available = std::min(pixel_count, frame.pixels.size() / 2u);
    for (size_t index = 0u; index < available; ++index)
    {
        const uint16_t value = static_cast<uint16_t>(frame.pixels[index * 2u]) |
                               (static_cast<uint16_t>(frame.pixels[index * 2u + 1u]) << 8u);
        const uint8_t r5 = static_cast<uint8_t>((value >> 11u) & 0x1Fu);
        const uint8_t g6 = static_cast<uint8_t>((value >> 5u) & 0x3Fu);
        const uint8_t b5 = static_cast<uint8_t>(value & 0x1Fu);
        rgba[index * 4u + 0u] = static_cast<uint8_t>((r5 << 3u) | (r5 >> 2u));
        rgba[index * 4u + 1u] = static_cast<uint8_t>((g6 << 2u) | (g6 >> 4u));
        rgba[index * 4u + 2u] = static_cast<uint8_t>((b5 << 3u) | (b5 >> 2u));
    }
}

std::string telemetry_value(const avc::host::TelemetrySample &sample)
{
    switch (sample.value_type)
    {
        case AVC_DBG_TELEMETRY_TYPE_I32:
            return std::to_string(static_cast<int32_t>(sample.value_bits));
        case AVC_DBG_TELEMETRY_TYPE_U32:
            return std::to_string(sample.value_bits);
        case AVC_DBG_TELEMETRY_TYPE_F32:
        {
            float value = 0.0f;
            std::memcpy(&value, &sample.value_bits, sizeof(value));
            char text[32]{};
            std::snprintf(text, sizeof(text), "%.3f", static_cast<double>(value));
            return text;
        }
        case AVC_DBG_TELEMETRY_TYPE_BOOL:
            return sample.value_bits != 0u ? "true" : "false";
        default:
            return "?";
    }
}

void choose_firmware_image(std::array<char, 1024> &path)
{
    OPENFILENAMEA dialog{};
    dialog.lStructSize = sizeof(dialog);
    dialog.lpstrFilter = "AVC firmware image (*.bin)\0*.bin\0All files (*.*)\0*.*\0\0";
    dialog.lpstrFile = path.data();
    dialog.nMaxFile = static_cast<DWORD>(path.size());
    dialog.lpstrTitle = "Select an AVC firmware image";
    dialog.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
    (void)GetOpenFileNameA(&dialog);
}

int viewer_main(const Options &options)
{
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());
    }

    const uint32_t window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI |
                                  ((options.test_seconds > 0u) ? SDL_WINDOW_HIDDEN : 0u);
    SDL_Window *window = SDL_CreateWindow("AVC TELEMETRY",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          1000,
                                          720,
                                          window_flags);
    if (window == nullptr)
    {
        const std::string error = SDL_GetError();
        SDL_Quit();
        throw std::runtime_error("SDL_CreateWindow failed: " + error);
    }

    SDL_Renderer *renderer =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        const std::string error = SDL_GetError();
        SDL_DestroyWindow(window);
        SDL_Quit();
        throw std::runtime_error("SDL_CreateRenderer failed: " + error);
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO().IniFilename = nullptr;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    SharedState shared;
    std::atomic<bool> running{true};
    std::thread worker(connection_worker, std::cref(options), std::ref(shared), std::cref(running));

    SDL_Texture *texture = nullptr;
    int texture_width = 0;
    int texture_height = 0;
    uint64_t rendered_generation = 0u;
    std::vector<uint8_t> rgba;
    avc::host::Frame display_frame;
    std::array<char, 1024> image_path{};
    std::snprintf(image_path.data(),
                  image_path.size(),
                  "%s",
                  "build\\cmake\\competition\\avc_core0.bin");
    bool erase_confirmation = false;
    double display_fps = 0.0;
    unsigned fps_frames = 0u;
    auto fps_epoch = std::chrono::steady_clock::now();
    const auto test_deadline = fps_epoch + std::chrono::seconds(options.test_seconds);

    bool quit = false;
    while (!quit)
    {
        if ((options.test_seconds > 0u) && (std::chrono::steady_clock::now() >= test_deadline))
        {
            quit = true;
        }
        SDL_Event event{};
        while (SDL_PollEvent(&event) != 0)
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }
        }

        std::string status;
        std::string error;
        std::string port;
        bool connected = false;
        uint64_t connection_count = 0u;
        bool rom_connected = false;
        bool program_busy = false;
        bool program_succeeded = false;
        std::string program_status;
        std::string program_detail;
        std::string program_error;
        avc_dbg_control_hello_response_t hello{};
        avc::host::ParserCounters counters;
        avc_dbg_stats_report_t stats{};
        bool has_stats = false;
        std::vector<avc::host::TelemetrySample> telemetry;
        {
            std::lock_guard<std::mutex> lock(shared.mutex);
            status = shared.status;
            error = shared.error;
            port = shared.port;
            connected = shared.connected;
            connection_count = shared.connection_count;
            rom_connected = shared.rom_connected;
            program_busy = shared.program_busy;
            program_succeeded = shared.program_succeeded;
            program_status = shared.program_status;
            program_detail = shared.program_detail;
            program_error = shared.program_error;
            hello = shared.hello;
            counters = shared.counters;
            stats = shared.stats;
            has_stats = shared.has_stats;
            telemetry = shared.telemetry;
            if (shared.frame.generation > rendered_generation)
            {
                display_frame = shared.frame;
            }
        }

        if (!connected && (texture != nullptr))
        {
            SDL_DestroyTexture(texture);
            texture = nullptr;
            texture_width = 0;
            texture_height = 0;
        }

        if (display_frame.generation > rendered_generation)
        {
            rendered_generation = display_frame.generation;
            ++fps_frames;
            if ((display_frame.pixel_format == AVC_DBG_PIXEL_FORMAT_RGB565_LE) &&
                (display_frame.width > 0u) && (display_frame.height > 0u))
            {
                if ((texture == nullptr) || (texture_width != display_frame.width) ||
                    (texture_height != display_frame.height))
                {
                    if (texture != nullptr)
                    {
                        SDL_DestroyTexture(texture);
                    }
                    texture_width = display_frame.width;
                    texture_height = display_frame.height;
                    texture = SDL_CreateTexture(renderer,
                                                SDL_PIXELFORMAT_RGBA32,
                                                SDL_TEXTUREACCESS_STREAMING,
                                                texture_width,
                                                texture_height);
                }
                rgb565_to_rgba(display_frame, rgba);
                if (texture != nullptr)
                {
                    SDL_UpdateTexture(texture, nullptr, rgba.data(), texture_width * 4);
                }
            }
        }

        const auto now = std::chrono::steady_clock::now();
        const double fps_seconds = std::chrono::duration<double>(now - fps_epoch).count();
        if (fps_seconds >= 1.0)
        {
            display_fps = static_cast<double>(fps_frames) / fps_seconds;
            fps_frames = 0u;
            fps_epoch = now;
        }

        SDL_SetRenderDrawColor(renderer, 248, 248, 248, 255);
        SDL_RenderClear(renderer);

        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        const ImVec2 display_size = ImGui::GetIO().DisplaySize;
        constexpr float margin = 10.0f;
        const float sidebar_width = std::min(340.0f, display_size.x * 0.36f);
        const float camera_width = std::max(250.0f, display_size.x - sidebar_width - 3.0f * margin);
        const float panel_height = std::max(250.0f, display_size.y - 2.0f * margin);
        const float status_height = std::max(230.0f, panel_height * 0.44f);

        ImGui::SetNextWindowPos(ImVec2(margin, margin), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(camera_width, panel_height), ImGuiCond_Always);
        ImGui::Begin("Camera",
                     nullptr,
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoCollapse);
        if (connected && (texture != nullptr) && (texture_width > 0) && (texture_height > 0))
        {
            const ImVec2 available = ImGui::GetContentRegionAvail();
            const float scale = std::min(available.x / static_cast<float>(texture_width),
                                         available.y / static_cast<float>(texture_height));
            const ImVec2 image_size(static_cast<float>(texture_width) * scale,
                                    static_cast<float>(texture_height) * scale);
            const ImVec2 cursor = ImGui::GetCursorPos();
            ImGui::SetCursorPos(
                ImVec2(cursor.x + std::max(0.0f, (available.x - image_size.x) * 0.5f),
                       cursor.y + std::max(0.0f, (available.y - image_size.y) * 0.5f)));
            ImGui::Image(reinterpret_cast<ImTextureID>(texture), image_size);
        }
        else
        {
            ImGui::SetWindowFontScale(1.5f);
            ImGui::TextWrapped("VIDEO DISCONNECTED");
            ImGui::SetWindowFontScale(1.0f);
            ImGui::Spacing();
            ImGui::TextWrapped("%s", status.c_str());
            if (!error.empty())
            {
                ImGui::TextWrapped("%s", error.c_str());
            }
        }
        ImGui::End();

        const float sidebar_x = camera_width + 2.0f * margin;
        ImGui::SetNextWindowPos(ImVec2(sidebar_x, margin), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(sidebar_width, status_height), ImGuiCond_Always);
        ImGui::Begin("AVC status",
                     nullptr,
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoCollapse);
        if (connected)
        {
            ImGui::TextColored(ImVec4(0.35f, 1.0f, 0.45f, 1.0f), "%s", status.c_str());
            ImGui::Text("%s  %u x %u RGB565", port.c_str(), hello.frame_width, hello.frame_height);
            ImGui::Text("Preview %.1f FPS", display_fps);
        }
        else
        {
            ImGui::TextColored(ImVec4(1.0f, 0.35f, 0.35f, 1.0f), "%s", status.c_str());
            if (!error.empty())
            {
                ImGui::TextWrapped("%s", error.c_str());
            }
        }
        ImGui::Separator();
        ImGui::Text("Successful connections %llu",
                    static_cast<unsigned long long>(connection_count));
        ImGui::Text("Frames %llu  malformed %llu",
                    static_cast<unsigned long long>(counters.frames),
                    static_cast<unsigned long long>(counters.malformed));
        if (has_stats)
        {
            ImGui::Text("Device complete %lu  dropped %lu",
                        static_cast<unsigned long>(stats.frames_completed),
                        static_cast<unsigned long>(stats.frames_dropped));
        }
        for (const avc::host::TelemetrySample &sample : telemetry)
        {
            const std::string value = telemetry_value(sample);
            ImGui::Text("%s: %s %s", sample.name.c_str(), value.c_str(), sample.units.c_str());
        }
        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(sidebar_x, status_height + 2.0f * margin),
                                ImGuiCond_Always);
        ImGui::SetNextWindowSize(
            ImVec2(sidebar_width, std::max(100.0f, panel_height - status_height - margin)),
            ImGuiCond_Always);
        ImGui::Begin("Program firmware",
                     nullptr,
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoCollapse);
        ImGui::TextWrapped("Build first, then select the generated avc_core0.bin image.");
        ImGui::SetNextItemWidth(-78.0f);
        ImGui::InputText("##Image", image_path.data(), image_path.size());
        ImGui::SameLine();
        if (ImGui::Button("Browse..."))
        {
            choose_firmware_image(image_path);
        }
        ImGui::Checkbox("Erase application flash", &erase_confirmation);

        const bool enter_isp_supported =
            connected && ((hello.capability_flags & AVC_DBG_CAPABILITY_ENTER_ISP) != 0u);
        const bool target_available = enter_isp_supported || rom_connected;
        const bool can_program = target_available && erase_confirmation && !program_busy &&
                                 (image_path[0] != '\0');
        ImGui::BeginDisabled(!can_program);
        if (ImGui::Button("Program and reconnect", ImVec2(-1.0f, 0.0f)))
        {
            std::lock_guard<std::mutex> lock(shared.mutex);
            shared.program_image_path = image_path.data();
            shared.program_requested = true;
            shared.program_busy = true;
            shared.program_waiting_reconnect = false;
            shared.program_succeeded = false;
            shared.program_status = "Programming request queued";
            shared.program_detail.clear();
            shared.program_error.clear();
            erase_confirmation = false;
        }
        ImGui::EndDisabled();

        if (!target_available && !program_busy)
        {
            ImGui::TextWrapped("Connect one AVC runtime device or one MCXN947 ROM device.");
        }
        if (program_succeeded)
        {
            ImGui::TextColored(ImVec4(0.35f, 1.0f, 0.45f, 1.0f),
                               "%s",
                               program_status.c_str());
        }
        else
        {
            ImGui::TextWrapped("%s", program_status.c_str());
        }
        if (!program_detail.empty())
        {
            ImGui::TextWrapped("%s", program_detail.c_str());
        }
        if (!program_error.empty())
        {
            ImGui::TextColored(ImVec4(1.0f, 0.35f, 0.35f, 1.0f),
                               "Error: %s",
                               program_error.c_str());
        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }

    running.store(false);
    if (worker.joinable())
    {
        worker.join();
    }
    bool test_passed = true;
    if (options.test_seconds > 0u)
    {
        std::lock_guard<std::mutex> lock(shared.mutex);
        test_passed = (shared.counters.frames > 0u) && (shared.counters.malformed == 0u);
        std::printf("viewer_test=%s connections=%llu frames=%llu malformed=%llu\n",
                    test_passed ? "ok" : "failed",
                    static_cast<unsigned long long>(shared.connection_count),
                    static_cast<unsigned long long>(shared.counters.frames),
                    static_cast<unsigned long long>(shared.counters.malformed));
    }
    if (texture != nullptr)
    {
        SDL_DestroyTexture(texture);
    }
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return test_passed ? 0 : 2;
}

} // namespace

int main(int argc, char **argv)
{
    try
    {
        return viewer_main(parse_args(argc, argv));
    }
    catch (const std::exception &exception)
    {
        std::fprintf(stderr, "error: %s\n", exception.what());
        return 1;
    }
}
