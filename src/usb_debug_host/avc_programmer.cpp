#define NOMINMAX

#include "avc_programmer.hpp"

#include "avc_host_core.hpp"

#include <windows.h>
#include <bcrypt.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <thread>
#include <vector>

namespace avc::host
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
                      std::string &error)
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
        case ProgramStage::reset:
            return "reset";
        case ProgramStage::complete:
            return "complete";
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
    if (path.extension() != L".bin")
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

bool resolve_blhost(const std::string &requested_path,
                    std::string &resolved_path,
                    std::string &error)
{
    std::vector<std::filesystem::path> candidates;
    if (!requested_path.empty())
    {
        candidates.push_back(std::filesystem::u8path(requested_path));
    }
    else
    {
        char *environment_path = nullptr;
        size_t environment_bytes = 0u;
        if ((_dupenv_s(&environment_path, &environment_bytes, "AVC_BLHOST_PATH") == 0) &&
            (environment_path != nullptr) && (environment_path[0] != '\0'))
        {
            candidates.push_back(std::filesystem::u8path(environment_path));
        }
        std::free(environment_path);
        std::array<wchar_t, 32768> executable{};
        const DWORD length = GetModuleFileNameW(nullptr,
                                                executable.data(),
                                                static_cast<DWORD>(executable.size()));
        if ((length > 0u) && (length < executable.size()))
        {
            candidates.push_back(std::filesystem::path(executable.data()).parent_path() / L"blhost.exe");
        }
        candidates.emplace_back(L"C:\\nxp\\SEC_Provi_26.06\\bin\\_internal\\tools\\spsdk\\blhost.exe");
    }

    for (const std::filesystem::path &candidate : candidates)
    {
        std::error_code filesystem_error;
        const std::filesystem::path path = std::filesystem::weakly_canonical(candidate, filesystem_error);
        if (!filesystem_error && std::filesystem::is_regular_file(path) &&
            (path.filename() == L"blhost.exe"))
        {
            ProcessResult version;
            if (!run_process(narrow(path.wstring()), {"--version"}, 5000u, version, error))
            {
                return false;
            }
            if ((version.exit_code != 0u) ||
                (version.output.find("version 3.10.0") == std::string::npos))
            {
                error = "unsupported blhost; expected pinned version 3.10.0: " + version.output;
                return false;
            }
            resolved_path = narrow(path.wstring());
            return true;
        }
    }

    error = requested_path.empty()
                ? "pinned blhost 3.10.0 not found beside the tool, in AVC_BLHOST_PATH, or in NXP SEC"
                : "blhost not found: " + requested_path;
    return false;
}

bool program_rom_with_blhost(const std::string &blhost_path,
                             const FirmwareImage &image,
                             const ProgramProgress &progress,
                             std::string &error)
{
    if (progress)
    {
        progress(ProgramStage::validate, image.sha256);
    }
    std::string discovery_error;
    const std::vector<HidDevice> devices =
        find_hid_devices(kAvcUsbVid, kMcxn947RomPid, discovery_error);
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
    if (!run_blhost_stage(blhost_path,
                          arguments({"write-memory", "0x0", image.path}),
                          120000u,
                          "flash write",
                          error))
    {
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
    if (quote_argument(L"C:\\Program Files\\NXP\\blhost.exe") !=
        L"\"C:\\Program Files\\NXP\\blhost.exe\"")
    {
        error = "Windows argument quoting failed";
        return false;
    }
    error.clear();
    return true;
}

} // namespace avc::host
