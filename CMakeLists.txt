cmake_minimum_required(VERSION 3.24)

# Top-level build orchestrator for the NXP Cup project.
#
# This exists so CMakePresets.json has a single source directory to point at,
# and so PC-side host tools can be added alongside the firmware later without
# moving anything. The one normal firmware preset is:
#
#   cmake --preset competition && cmake --build --preset competition
#
# Maintainer diagnostics use scripts under scripts/maintainer and separate
# build directories instead of appearing as alternate public presets.

project(nxp_cup NONE)

option(NXPC_BUILD_FIRMWARE "Build the MCXN947 firmware" ON)

# Host tools (SDL camera viewer and friends) will land here. They are a separate
# option because they build with the host compiler, not the Arm toolchain, so
# they cannot share a preset with the firmware.
option(NXPC_BUILD_HOST_TOOLS "Build PC-side host tools" OFF)

if(NXPC_BUILD_FIRMWARE)
    enable_language(C ASM)
    add_subdirectory(src/nxp_cup/nxp_cup_core0)
endif()

if(NXPC_BUILD_HOST_TOOLS)
    message(FATAL_ERROR
        "NXPC_BUILD_HOST_TOOLS is reserved for the SDL camera viewer, which is "
        "not implemented yet. See docs/plans/cmake-build-and-toolchain.")
endif()
