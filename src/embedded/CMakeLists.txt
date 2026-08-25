cmake_minimum_required(VERSION 3.24)

# Embedded MCXN947 firmware build orchestrator.
#
# This exists so CMakePresets.json has a single source directory to point at.
# Host and Android consumers own independent builds beside this component.
# The one normal firmware preset is:
#
#   cmake --preset competition && cmake --build --preset competition
#
# Maintainer validation uses scripts under tools/maintainer and separate
# build directories instead of appearing as alternate public presets.

project(nxp_cup_embedded NONE)

enable_language(C ASM)
add_subdirectory(nxp_cup_core0)
