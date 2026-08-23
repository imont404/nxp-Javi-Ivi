#pragma once

#include <cstdint>
#include <functional>
#include <string>

namespace avc::host
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
    reset,
    complete,
};

using ProgramProgress = std::function<void(ProgramStage stage, const std::string &detail)>;

const char *program_stage_name(ProgramStage stage);
bool validate_firmware_image(const std::string &requested_path,
                             FirmwareImage &image,
                             std::string &error);
bool resolve_blhost(const std::string &requested_path,
                    std::string &resolved_path,
                    std::string &error);
bool program_rom_with_blhost(const std::string &blhost_path,
                             const FirmwareImage &image,
                             const ProgramProgress &progress,
                             std::string &error);
bool run_programmer_self_test(std::string &error);

} // namespace avc::host
