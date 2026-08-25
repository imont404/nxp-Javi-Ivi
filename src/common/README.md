# Shared Libraries

This directory contains linked libraries used by the embedded firmware and host
tools, including `e`, `egfx`, bunny-build support, IPC definitions, and the
shared NXP Cup USB protocol.

It is not a standalone product and therefore has no top-level `build.ps1`.
Build and test these sources through the consuming component. Moving this folder
or changing its depth relative to `src/embedded/nxp_cup_core0` requires a fresh
MCUXpresso linked-resource and CMake metadata audit.

