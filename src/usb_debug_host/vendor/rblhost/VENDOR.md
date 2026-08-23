# NXP rblhost vendor record

- Upstream: `https://github.com/nxp-mcuxpresso/rblhost`
- Version: `v0.2.0`
- Source commit: `7a775dde2c44bd345a1ac067698afa999bd71be0`
- Binary SHA-256: `6CAE03C432489E0BD8A658F91E5899E7D1153B6859BE02C7A76798F2885CD2A8`
- Toolchain used: Rust/Cargo 1.96.1, release profile with the upstream lockfile
- License: BSD-3-Clause; see `LICENSE` in this directory.
- Imported and bench-validated: 2026-08-23

The retained x64 Windows executable is the default ROM programmer packaged
beside `avc_viewer.exe` and `avc_tool.exe`. On FRDM-MCXN947 J11 it has passed
USB-HID property query, erase-all, write-memory at address zero, full-length
readback, reset, and application/camera recovery using VID/PID `1FC9:014F`.
