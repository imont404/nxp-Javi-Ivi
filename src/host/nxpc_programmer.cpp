#define NOMINMAX

#include "nxpc_programmer.hpp"

#include "nxpc_host_core.hpp"

#include <windows.h>
#include <bcrypt.h>

#include <algorithm>
#include <array>
#include <charconv>
#include <chrono>
#include <cwctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <thread>
#include <vector>

namespace nxpc::host
{
namespace
{

constexpr uint64_t kMaximumApplicationBytes = 1024u * 1024u;
constexpr size_t kMaximumProcessOutputBytes = 1024u * 1024u;

struct ProcessResult
{
    DWORD exit_code = 0u;
    std::string output;
};

std::wstring widen(const std::string &text)
{
    if (text.empty())
    {
        return {};
    }
    const int required = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, nullptr, 0);
    if (required <= 0)
    {
        return {};
    }
    std::vector<wchar_t> buffer(static_cast<size_t>(required));
    if (MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, buffer.data(), required) <= 0)
    {
        return {};
    }
    return std::wstring(buffer.data());
}

std::string narrow(const std::wstring &text)
{
    if (text.empty())
    {
        return {};
    }
    const int required = WideCharToMultiByte(CP_UTF8,
                                              0,
                                              text.c_str(),
                                              -1,
                                              nullptr,
                                              0,
                                              nullptr,
                                              nullptr);
    if (required <= 0)
    {
        return {};
    }
    std::vector<char> buffer(static_cast<size_t>(required));
    if (WideCharToMultiByte(CP_UTF8,
                            0,
                            text.c_str(),
                            -1,
                            buffer.data(),
                            required,
                            nullptr,
                            nullptr) <= 0)
    {
        return {};
    }
    return std::string(buffer.data());
}

std::wstring quote_argument(const std::wstring &argument)
{
    if ((argument.find_first_of(L" \t\"") == std::wstring::npos) && !argument.empty())
    {
        return argument;
    }

    std::wstring quoted = L"\"";
    size_t backslashes = 0u;
    for (const wchar_t character : argument)
    {
        if (character == L'\\')
        {
            ++backslashes;
            continue;
        }
        if (character == L'\"')
        {
            quoted.append(backslashes * 2u + 1u, L'\\');
            quoted.push_back(L'\"');
            backslashes = 0u;
            continue;
        }
        quoted.append(backslashes, L'\\');
        backslashes = 0u;
        quoted.push_back(character);
    }
    quoted.append(backslashes * 2u, L'\\');
    quoted.push_back(L'\"');
    return quoted;
}

bool run_process(const std::string &application,
                 const std::vector<std::string> &arguments,
                 uint32_t timeout_ms,
                 ProcessResult &result,
                 std::string &error)
{
    SECURITY_ATTRIBUTES security{};
    security.nLength = sizeof(security);
    security.bInheritHandle = TRUE;
    HANDLE read_pipe = nullptr;
    HANDLE write_pipe = nullptr;
    if (!CreatePipe(&read_pipe, &write_pipe, &security, 0u))
    {
        error = "CreatePipe failed";
        return false;
    }
    if (!SetHandleInformation(read_pipe, HANDLE_FLAG_INHERIT, 0u))
    {
        CloseHandle(read_pipe);
        CloseHandle(write_pipe);
        error = "SetHandleInformation failed";
        return false;
    }

    const std::wstring wide_application = widen(application);
    std::wstring command = quote_argument(wide_application);
    for (const std::string &argument : arguments)
    {
        command.push_back(L' ');
        command += quote_argument(widen(argument));
    }
    std::vector<wchar_t> mutable_command(command.begin(), command.end());
    mutable_command.push_back(L'\0');

    STARTUPINFOW startup{};
    startup.cb = sizeof(startup);
    startup.dwFlags = STARTF_USESTDHANDLES;
    startup.hStdOutput = write_pipe;
    startup.hStdError = write_pipe;
    startup.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    PROCESS_INFORMATION process{};
    const BOOL started = CreateProcessW(wide_application.c_str(),
                                        mutable_command.data(),
                                        nullptr,
                                        nullptr,
                                        TRUE,
                                        CREATE_NO_WINDOW,
                                        nullptr,
                                        nullptr,
                                        &startup,
                                        &process);
    CloseHandle(write_pipe);
    if (!started)
    {
        CloseHandle(read_pipe);
        error = "CreateProcessW failed for " + application;
        return false;
    }

    result.output.clear();
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms);
    bool timed_out = false;
    for (;;)
    {
        DWORD available = 0u;
        if (PeekNamedPipe(read_pipe, nullptr, 0u, nullptr, &available, nullptr) && (available > 0u))
        {
            std::array<char, 4096> buffer{};
            DWORD received = 0u;
            const DWORD wanted = std::min<DWORD>(available, static_cast<DWORD>(buffer.size()));
            if (ReadFile(read_pipe, buffer.data(), wanted, &received, nullptr) && (received > 0u) &&
                (result.output.size() < kMaximumProcessOutputBytes))
            {
                const size_t retained = std::min<size_t>(received,
                    kMaximumProcessOutputBytes - result.output.size());
                result.output.append(buffer.data(), retained);
            }
        }

        if (WaitForSingleObject(process.hProcess, 0u) == WAIT_OBJECT_0)
        {
            break;
        }
        if (std::chrono::steady_clock::now() >= deadline)
        {
            timed_out = true;
            (void)TerminateProcess(process.hProcess, ERROR_TIMEOUT);
            (void)WaitForSingleObject(process.hProcess, 2000u);
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    for (;;)
    {
        std::array<char, 4096> buffer{};
        DWORD received = 0u;
        if (!ReadFile(read_pipe,
                      buffer.data(),
                      static_cast<DWORD>(buffer.size()),
                      &received,
                      nullptr) ||
            (received == 0u))
        {
            break;
        }
        if (result.output.size() < kMaximumProcessOutputBytes)
        {
            const size_t retained = std::min<size_t>(received,
                kMaximumProcessOutputBytes - result.output.size());
            result.output.append(buffer.data(), retained);
        }
    }

    (void)GetExitCodeProcess(process.hProcess, &result.exit_code);
    CloseHandle(process.hThread);
    CloseHandle(process.hProcess);
    CloseHandle(read_pipe);
    if (timed_out)
    {
        error = "process timed out after " + std::to_string(timeout_ms) + " ms";
        return false;
    }
    return true;
}

bool blhost_result_ok(const ProcessResult &result)
{
    return (result.exit_code == 0u) &&
           (result.output.find("\"value\": 0") != std::string::npos);
}

bool rblhost_result_ok(const ProcessResult &result)
{
    return (result.exit_code == 0u) &&
           (result.output.find("Response status = 0 (0x0) Success.") != std::string::npos);
}

bool blhost_first_response_u64(const std::string &output, uint64_t &value)
{
    const size_t response = output.find("\"response\"");
    if (response == std::string::npos)
    {
        return false;
    }
    size_t cursor = output.find('[', response);
    if (cursor == std::string::npos)
    {
        return false;
    }
    ++cursor;
    while ((cursor < output.size()) &&
           ((output[cursor] == ' ') || (output[cursor] == '\t') ||
            (output[cursor] == '\r') || (output[cursor] == '\n')))
    {
        ++cursor;
    }
    if ((cursor >= output.size()) || (output[cursor] == ']'))
    {
        return false;
    }

    const char *first = output.data() + cursor;
    const char *last = output.data() + output.size();
    const std::from_chars_result parsed = std::from_chars(first, last, value);
    return parsed.ec == std::errc{};
}

std::string summarize_failure(const std::string &operation, const ProcessResult &result)
{
    std::ostringstream message;
    message << operation << " failed with exit code " << result.exit_code;
    if (!result.output.empty())
    {
        message << ": " << result.output;
    }
    return message.str();
}

bool sha256_file(const std::filesystem::path &path, std::string &digest, std::string &error)
{
    BCRYPT_ALG_HANDLE algorithm = nullptr;
    BCRYPT_HASH_HANDLE hash = nullptr;
    DWORD object_bytes = 0u;
    DWORD hash_bytes = 0u;
    DWORD result_bytes = 0u;
    std::vector<uint8_t> object;
    std::vector<uint8_t> output;
    bool ok = false;

    if (BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM, nullptr, 0u) < 0)
    {
        error = "BCryptOpenAlgorithmProvider(SHA256) failed";
        goto cleanup;
    }
    if ((BCryptGetProperty(algorithm,
                           BCRYPT_OBJECT_LENGTH,
                           reinterpret_cast<PUCHAR>(&object_bytes),
                           sizeof(object_bytes),
                           &result_bytes,
                           0u) < 0) ||
        (BCryptGetProperty(algorithm,
                           BCRYPT_HASH_LENGTH,
                           reinterpret_cast<PUCHAR>(&hash_bytes),
                           sizeof(hash_bytes),
                           &result_bytes,
                           0u) < 0))
    {
        error = "BCryptGetProperty(SHA256) failed";
        goto cleanup;
    }
    object.resize(object_bytes);
    output.resize(hash_bytes);
    if (BCryptCreateHash(algorithm, &hash, object.data(), object_bytes, nullptr, 0u, 0u) < 0)
    {
        error = "BCryptCreateHash(SHA256) failed";
        goto cleanup;
    }
    {
        std::ifstream input(path, std::ios::binary);
        std::array<char, 64u * 1024u> buffer{};
        if (!input)
        {
            error = "cannot open firmware image for SHA-256";
            goto cleanup;
        }
        while (input)
        {
            input.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
            const std::streamsize count = input.gcount();
            if ((count > 0) &&
                (BCryptHashData(hash,
                                reinterpret_cast<PUCHAR>(buffer.data()),
                                static_cast<ULONG>(count),
                                0u) < 0))
            {
                error = "BCryptHashData(SHA256) failed";
                goto cleanup;
            }
        }
    }
    if (BCryptFinishHash(hash, output.data(), hash_bytes, 0u) < 0)
    {
        error = "BCryptFinishHash(SHA256) failed";
        goto cleanup;
    }
    {
        std::ostringstream text;
        text << std::uppercase << std::hex << std::setfill('0');
        for (const uint8_t byte : output)
        {
            text << std::setw(2) << static_cast<unsigned>(byte);
        }
        digest = text.str();
    }
    ok = true;

cleanup:
    if (hash != nullptr)
    {
        (void)BCryptDestroyHash(hash);
    }
    if (algorithm != nullptr)
    {
        (void)BCryptCloseAlgorithmProvider(algorithm, 0u);
    }
    return ok;
}

bool run_blhost_stage(const std::string &blhost,
                      const std::vector<std::string> &arguments,
                      uint32_t timeout_ms,
                      const std::string &operation,
                      std::string &error,
                      uint64_t *first_response = nullptr)
{
    ProcessResult result;
    if (!run_process(blhost, arguments, timeout_ms, result, error))
    {
        error = operation + ": " + error;
        return false;
    }
    if (!blhost_result_ok(result))
    {
        error = summarize_failure(operation, result);
        return false;
    }
    if ((first_response != nullptr) &&
        !blhost_first_response_u64(result.output, *first_response))
    {
        error = operation + " succeeded but did not report a numeric response";
        return false;
    }
    return true;
}

bool run_rblhost_stage(const std::string &rblhost,
                       const std::vector<std::string> &arguments,
                       uint32_t timeout_ms,
                       const std::string &operation,
                       std::string &output,
                       std::string &error)
{
    ProcessResult result;
    if (!run_process(rblhost, arguments, timeout_ms, result, error))
    {
        error = operation + ": " + error;
        return false;
    }
    if (!rblhost_result_ok(result))
    {
        error = summarize_failure(operation, result);
        return false;
    }
    output = std::move(result.output);
    return true;
}

bool program_rom_with_rblhost(const std::string &rblhost_path,
                              const FirmwareImage &image,
                              const ProgramProgress &progress,
                              std::string &error)
{
    const std::vector<std::string> usb = {"-u", "0x1FC9,0x014F", "--"};
    auto arguments = [&](std::initializer_list<std::string> command) {
        std::vector<std::string> result = usb;
        result.insert(result.end(), command.begin(), command.end());
        return result;
    };
    std::string output;

    if (progress)
    {
        progress(ProgramStage::query, "get-property 1");
    }
    if (!run_rblhost_stage(rblhost_path,
                           arguments({"get-property", "1"}),
                           10000u,
                           "ROM identity query",
                           output,
                           error))
    {
        return false;
    }
    if (progress)
    {
        progress(ProgramStage::erase, "internal application flash");
    }
    if (!run_rblhost_stage(rblhost_path,
                           arguments({"flash-erase-all"}),
                           30000u,
                           "flash erase",
                           output,
                           error))
    {
        return false;
    }
    if (progress)
    {
        progress(ProgramStage::write, std::to_string(image.bytes) + " bytes");
    }
    if (!run_rblhost_stage(rblhost_path,
                           arguments({"write-memory", "0x0", image.path}),
                           120000u,
                           "flash write",
                           output,
                           error))
    {
        return false;
    }

    std::array<wchar_t, 32768> temporary_directory{};
    const DWORD temporary_length =
        GetTempPathW(static_cast<DWORD>(temporary_directory.size()), temporary_directory.data());
    if ((temporary_length == 0u) || (temporary_length >= temporary_directory.size()))
    {
        error = "cannot resolve a temporary directory for flash readback";
        return false;
    }
    const std::filesystem::path readback =
        std::filesystem::path(temporary_directory.data()) /
        (L"nxp-cup-rblhost-readback-" + std::to_wstring(GetCurrentProcessId()) + L"-" +
         std::to_wstring(GetTickCount64()) + L".bin");
    if (progress)
    {
        progress(ProgramStage::verify, "full flash readback");
    }
    const bool read_ok = run_rblhost_stage(
        rblhost_path,
        arguments({"read-memory", "0x0", std::to_string(image.bytes), narrow(readback.wstring())}),
        120000u,
        "flash readback",
        output,
        error);
    if (!read_ok)
    {
        std::error_code remove_error;
        (void)std::filesystem::remove(readback, remove_error);
        return false;
    }

    std::error_code filesystem_error;
    const uint64_t readback_bytes = std::filesystem::file_size(readback, filesystem_error);
    std::string readback_sha256;
    const bool hash_ok = !filesystem_error &&
                         sha256_file(readback, readback_sha256, error);
    std::error_code remove_error;
    (void)std::filesystem::remove(readback, remove_error);
    if (!hash_ok)
    {
        if (error.empty())
        {
            error = "cannot inspect flash readback";
        }
        return false;
    }
    if ((readback_bytes != image.bytes) || (readback_sha256 != image.sha256))
    {
        error = "flash readback mismatch: bytes=" + std::to_string(readback_bytes) +
                " SHA-256=" + readback_sha256;
        return false;
    }
    if (progress)
    {
        progress(ProgramStage::verify, "SHA-256 " + readback_sha256);
    }

    if (progress)
    {
        progress(ProgramStage::reset, "reset to application");
    }
    if (!run_rblhost_stage(rblhost_path,
                           arguments({"reset"}),
                           10000u,
                           "ROM reset",
                           output,
                           error))
    {
        return false;
    }
    if (progress)
    {
        progress(ProgramStage::complete, "programming and readback verification complete");
    }
    return true;
}

} // namespace

const char *program_stage_name(ProgramStage stage)
{
    switch (stage)
    {
        case ProgramStage::validate:
            return "validate";
        case ProgramStage::query:
            return "query";
        case ProgramStage::erase:
            return "erase";
        case ProgramStage::write:
            return "write";
        case ProgramStage::verify:
            return "verify";
        case ProgramStage::reset:
            return "reset";
        case ProgramStage::complete:
            return "complete";
        default:
            return "unknown";
    }
}

const char *programmer_backend_name(ProgrammerBackend backend)
{
    switch (backend)
    {
        case ProgrammerBackend::rblhost:
            return "rblhost";
        case ProgrammerBackend::blhost:
            return "blhost";
        default:
            return "unknown";
    }
}

bool validate_firmware_image(const std::string &requested_path,
                             FirmwareImage &image,
                             std::string &error)
{
    error.clear();
    std::error_code filesystem_error;
    const std::filesystem::path requested = std::filesystem::u8path(requested_path);
    const std::filesystem::path path = std::filesystem::weakly_canonical(requested, filesystem_error);
    if (filesystem_error || !std::filesystem::is_regular_file(path))
    {
        error = "firmware image is not an existing regular file: " + requested_path;
        return false;
    }
    const uint64_t bytes = std::filesystem::file_size(path, filesystem_error);
    if (filesystem_error || (bytes < 8u) || (bytes > kMaximumApplicationBytes))
    {
        error = "firmware image size must be between 8 bytes and 1 MiB";
        return false;
    }
    std::wstring extension = path.extension().wstring();
    std::transform(extension.begin(), extension.end(), extension.begin(), [](wchar_t value) {
        return static_cast<wchar_t>(std::towlower(value));
    });
    if (extension != L".bin")
    {
        error = "ROM programmer requires a .bin image";
        return false;
    }

    std::ifstream input(path, std::ios::binary);
    std::array<uint8_t, 8> vectors{};
    input.read(reinterpret_cast<char *>(vectors.data()), static_cast<std::streamsize>(vectors.size()));
    if (input.gcount() != static_cast<std::streamsize>(vectors.size()))
    {
        error = "firmware image vector table cannot be read";
        return false;
    }
    const uint32_t initial_sp = static_cast<uint32_t>(vectors[0]) |
                                (static_cast<uint32_t>(vectors[1]) << 8u) |
                                (static_cast<uint32_t>(vectors[2]) << 16u) |
                                (static_cast<uint32_t>(vectors[3]) << 24u);
    const uint32_t reset_pc = static_cast<uint32_t>(vectors[4]) |
                              (static_cast<uint32_t>(vectors[5]) << 8u) |
                              (static_cast<uint32_t>(vectors[6]) << 16u) |
                              (static_cast<uint32_t>(vectors[7]) << 24u);
    if ((initial_sp < 0x20000000u) || (initial_sp >= 0x20100000u))
    {
        error = "firmware initial stack pointer is outside MCXN947 SRAM";
        return false;
    }
    if (((reset_pc & 1u) == 0u) || ((reset_pc & ~1u) >= bytes))
    {
        error = "firmware reset vector is not a Thumb address inside the image";
        return false;
    }

    image.path = narrow(path.wstring());
    image.bytes = bytes;
    image.initial_sp = initial_sp;
    image.reset_pc = reset_pc;
    if (!sha256_file(path, image.sha256, error))
    {
        return false;
    }
    return true;
}

bool resolve_programmer(const std::string &requested_path,
                        ProgrammerTool &programmer,
                        std::string &error)
{
    std::vector<std::filesystem::path> candidates;
    if (!requested_path.empty())
    {
        candidates.push_back(std::filesystem::u8path(requested_path));
    }
    else
    {
        auto add_environment = [&](const char *name) {
            char *environment_path = nullptr;
            size_t environment_bytes = 0u;
            if ((_dupenv_s(&environment_path, &environment_bytes, name) == 0) &&
                (environment_path != nullptr) && (environment_path[0] != '\0'))
            {
                candidates.push_back(std::filesystem::u8path(environment_path));
            }
            std::free(environment_path);
        };
        add_environment("NXPC_PROGRAMMER_PATH");
        std::array<wchar_t, 32768> executable{};
        const DWORD length = GetModuleFileNameW(nullptr,
                                                executable.data(),
                                                static_cast<DWORD>(executable.size()));
        if ((length > 0u) && (length < executable.size()))
        {
            const std::filesystem::path directory =
                std::filesystem::path(executable.data()).parent_path();
            candidates.push_back(directory / L"rblhost.exe");
            candidates.push_back(directory / L"blhost.exe");
        }
        add_environment("NXPC_RBLHOST_PATH");
        add_environment("NXPC_BLHOST_PATH");
        candidates.emplace_back(L"C:\\nxp\\SEC_Provi_26.06\\bin\\_internal\\tools\\spsdk\\blhost.exe");
    }

    for (const std::filesystem::path &candidate : candidates)
    {
        std::error_code filesystem_error;
        const std::filesystem::path path =
            std::filesystem::weakly_canonical(candidate, filesystem_error);
        if (!filesystem_error && std::filesystem::is_regular_file(path))
        {
            ProcessResult version;
            std::string version_error;
            if (!run_process(narrow(path.wstring()),
                             {"--version"},
                             5000u,
                             version,
                             version_error))
            {
                if (!requested_path.empty())
                {
                    error = version_error;
                    return false;
                }
                continue;
            }
            if ((version.exit_code == 0u) &&
                (version.output.find("rblhost 0.2.0") != std::string::npos))
            {
                programmer.backend = ProgrammerBackend::rblhost;
                programmer.path = narrow(path.wstring());
                programmer.version = "0.2.0";
                error.clear();
                return true;
            }
            if ((version.exit_code == 0u) &&
                (version.output.find("version 3.10.0") != std::string::npos))
            {
                programmer.backend = ProgrammerBackend::blhost;
                programmer.path = narrow(path.wstring());
                programmer.version = "3.10.0";
                error.clear();
                return true;
            }
            if (!requested_path.empty())
            {
                error = "unsupported programmer; expected rblhost 0.2.0 or blhost 3.10.0: " +
                        version.output;
                return false;
            }
        }
    }

    error = requested_path.empty()
                ? "no pinned ROM programmer found; expected rblhost 0.2.0 beside the tool or blhost 3.10.0"
                : "programmer not found: " + requested_path;
    return false;
}

static bool program_rom_with_blhost_internal(const std::string &blhost_path,
                                             const FirmwareImage &image,
                                             const ProgramProgress &progress,
                                             std::string &error)
{
    const std::vector<std::string> usb = {"-u", "0x1FC9:0x014F", "-j"};
    auto arguments = [&](std::initializer_list<std::string> command) {
        std::vector<std::string> result = usb;
        result.insert(result.end(), command.begin(), command.end());
        return result;
    };

    if (progress)
    {
        progress(ProgramStage::query, "get-property 1");
    }
    if (!run_blhost_stage(blhost_path,
                          arguments({"get-property", "1"}),
                          10000u,
                          "ROM identity query",
                          error))
    {
        return false;
    }
    if (progress)
    {
        progress(ProgramStage::erase, "internal application flash");
    }
    if (!run_blhost_stage(blhost_path,
                          arguments({"flash-erase-all"}),
                          30000u,
                          "flash erase",
                          error))
    {
        return false;
    }
    if (progress)
    {
        progress(ProgramStage::write, std::to_string(image.bytes) + " bytes");
    }
    uint64_t written_bytes = 0u;
    if (!run_blhost_stage(blhost_path,
                          arguments({"write-memory", "0x0", image.path}),
                          120000u,
                          "flash write",
                          error,
                          &written_bytes))
    {
        return false;
    }
    if (written_bytes != image.bytes)
    {
        error = "flash write reported " + std::to_string(written_bytes) +
                " bytes; expected " + std::to_string(image.bytes);
        return false;
    }
    if (progress)
    {
        progress(ProgramStage::reset, "reset to application");
    }
    if (!run_blhost_stage(blhost_path,
                          arguments({"reset"}),
                          10000u,
                          "ROM reset",
                          error))
    {
        return false;
    }
    if (progress)
    {
        progress(ProgramStage::complete, "programming complete");
    }
    return true;
}

bool program_rom(const ProgrammerTool &programmer,
                 const FirmwareImage &image,
                 const ProgramProgress &progress,
                 std::string &error)
{
    if (progress)
    {
        progress(ProgramStage::validate,
                 std::string(programmer_backend_name(programmer.backend)) + " " +
                     programmer.version + ", SHA-256 " + image.sha256);
    }
    std::string discovery_error;
    const std::vector<HidDevice> devices =
        find_hid_devices(kNxpCupUsbVid, kMcxn947RomPid, discovery_error);
    if (!discovery_error.empty())
    {
        error = discovery_error;
        return false;
    }
    if (devices.size() != 1u)
    {
        error = "expected exactly one MCXN947 ROM HID; found " + std::to_string(devices.size());
        return false;
    }

    if (programmer.backend == ProgrammerBackend::rblhost)
    {
        return program_rom_with_rblhost(programmer.path, image, progress, error);
    }
    return program_rom_with_blhost_internal(programmer.path, image, progress, error);
}

bool run_programmer_self_test(std::string &error)
{
    ProcessResult success;
    success.exit_code = 0u;
    success.output = R"({"status":{"description":"0 Success","value": 0}})";
    ProcessResult failure = success;
    failure.output = R"({"status":{"description":"5 Failure","value": 5}})";
    if (!blhost_result_ok(success) || blhost_result_ok(failure))
    {
        error = "blhost JSON status classification failed";
        return false;
    }
    ProcessResult rust_success;
    rust_success.exit_code = 0u;
    rust_success.output = "Response status = 0 (0x0) Success.\n";
    ProcessResult rust_failure = rust_success;
    rust_failure.output = "Response status = 5 (0x5) Failure.\n";
    if (!rblhost_result_ok(rust_success) || rblhost_result_ok(rust_failure))
    {
        error = "rblhost status classification failed";
        return false;
    }
    uint64_t response_value = 0u;
    const std::string write_response =
        R"({"command":"write-memory","response":[370740],"status":{"value": 0}})";
    if (!blhost_first_response_u64(write_response, response_value) ||
        (response_value != 370740u) ||
        blhost_first_response_u64(R"({"response":[],"status":{"value": 0}})",
                                  response_value))
    {
        error = "blhost numeric response parsing failed";
        return false;
    }
    if (quote_argument(L"C:\\Program Files\\NXP\\blhost.exe") !=
        L"\"C:\\Program Files\\NXP\\blhost.exe\"")
    {
        error = "Windows argument quoting failed";
        return false;
    }
    error.clear();
    return true;
}

} // namespace nxpc::host
