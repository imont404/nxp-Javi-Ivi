#pragma once

#include <cstdint>
#include <functional>
#include <string>

namespace nxpc::host
{

struct FirmwareImage
{
    std::string path;
    uint64_t bytes = 0u;
    uint32_t initial_sp = 0u;
    uint32_t reset_pc = 0u;
    std::string sha256;
};

enum class ProgramStage
{
    validate,
    query,
    erase,
    write,
    verify,
    reset,
    complete,
};

enum class ProgrammerBackend
{
    rblhost,
    blhost,
};

struct ProgrammerTool
{
    ProgrammerBackend backend = ProgrammerBackend::rblhost;
    std::string path;
    std::string version;
};

using ProgramProgress = std::function<void(ProgramStage stage, const std::string &detail)>;

const char *program_stage_name(ProgramStage stage);
const char *programmer_backend_name(ProgrammerBackend backend);
bool validate_firmware_image(const std::string &requested_path, FirmwareImage &image,
                             std::string &error);
bool resolve_programmer(const std::string &requested_path, ProgrammerTool &programmer,
                        std::string &error);
bool program_rom(const ProgrammerTool &programmer, const FirmwareImage &image,
                 const ProgramProgress &progress, std::string &error);
bool run_programmer_self_test(std::string &error);

} // namespace nxpc::host
