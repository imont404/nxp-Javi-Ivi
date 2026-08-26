# Host Agent Guide

This component owns the Windows viewer/CLI, direct WebSerial viewer, discovery,
ROM-HID programming integration, reconnect behavior, packaging, and host tests.

Use `build.ps1` with the explicit LLVM-MinGW default. Do not add a second native
build path or restore the old direct `cl.exe` wrapper. Keep vendor source,
executables, and license files pinned and preserve the portable package manifest.

The host consumes `AVCU` v1 and may not independently rename wire bytes, USB
identities, or public firmware telemetry. Reads, parsing, frame publication,
network/browser work, and logs must remain bounded. Multiple matching boards
must require explicit selection rather than silently choosing the first.

Run `npm test` for the standalone viewer and lesson/browser fixtures. Generated
artifacts belong under `out`; npm `node_modules` and Playwright output remain
ignored component-local caches.

For the browser viewer, edit the WebSerial adapter under `web` and shared presentation
under `src/web`, then run `src/web/build.ps1`; never edit generated
`nxpc_usb_debug_viewer.html` directly. WebSerial, `AVCU`, control actions, and browser
serial lifecycle stay owned here. Follow `docs/design/shared-web-dashboard.md`.
