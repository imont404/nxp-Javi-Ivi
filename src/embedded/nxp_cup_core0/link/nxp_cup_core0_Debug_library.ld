/* STATIC CAPTURE 2026-07-25 (cmake-build-and-toolchain/break-ide-dependencies):
 * copied from the MCUXpresso-generated Debug linker scripts and maintained here
 * as the source of truth for the CMake build. To regenerate for comparison, run
 * .\src\embedded\tools\maintainer\build_mcuxpresso.ps1 and diff against
 * src\embedded\nxp_cup_core0\Debug\nxp_cup_core0_Debug*.ld.
 *
 * DELIBERATE EDIT 2026-07-25: the MCUXpresso capture groups "libcr_newlib_nohost.a",
 * which ships only with the IDE and breaks a build using the standalone Arm GNU
 * toolchain. It was replaced with the standard "libnosys.a", present in both the
 * MCUXpresso bundle and standalone Arm GNU 14.2.Rel1. A drift diff against a
 * regenerated MCUXpresso script will show exactly this one library-name line;
 * that is expected.
 */

/*
 * Originally generated; now maintained by hand - see the capture note above.
 * Copyright 2008-2013 Code Red Technologies Ltd,
 * Copyright 2013-2026 NXP
 * SPDX-License-Identifier: BSD-3-Clause
 * Generated linker script file for MCXN947
 * Created from library.ldt by FMCreateLinkLibraries
 * Using Freemarker v2.3.30
 * MCUXpresso IDE v25.6 [Build 136] [2025-06-27] on Jul 8, 2026, 10:16:46 PM
 */

GROUP (
  "libgcc.a"
  "libc_nano.a"
  "libm.a"
  "libnosys.a"
)
