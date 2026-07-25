cmake_minimum_required(VERSION 3.24)

# Top-level build orchestrator for the AVC project.
#
# This exists so CMakePresets.json has a single source directory to point at,
# and so PC-side host tools can be added alongside the firmware later without
# moving anything. Build variants are presets, not scripts:
#
#   cmake --preset competition && cmake --build --preset competition
#
# See CMakePresets.json for the list, or run: cmake --list-presets

project(avc NONE)

option(AVC_BUILD_FIRMWARE "Build the MCXN947 firmware" ON)

# Host tools (SDL camera viewer and friends) will land here. They are a separate
# option because they build with the host compiler, not the Arm toolchain, so
# they cannot share a preset with the firmware.
option(AVC_BUILD_HOST_TOOLS "Build PC-side host tools" OFF)

if(AVC_BUILD_FIRMWARE)
    enable_language(C ASM)
    add_subdirectory(src/avc/avc_core0)
endif()

if(AVC_BUILD_HOST_TOOLS)
    message(FATAL_ERROR
        "AVC_BUILD_HOST_TOOLS is reserved for the SDL camera viewer, which is "
        "not implemented yet. See docs/plans/cmake-build-and-toolchain.")
endif()
