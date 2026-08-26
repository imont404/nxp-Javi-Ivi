# Windows and Browser Host Tools

This component contains the native Windows camera/telemetry viewer, the
one-cable programming CLI, and the direct WebSerial viewer.

## Native build

Run from the repository root:

```powershell
.\src\host\build.ps1
```

LLVM-MinGW is the canonical compiler installed by root
`setup.ps1 -IncludeMaintainerTools`. Ordinary setup installs the pinned prebuilt
runtime instead. The maintainer build produces the viewer and CLI, copies SDL2
and the pinned ROM programmer beside them, and publishes a runnable bundle under
`out\artifacts\host`.

Useful commands:

```powershell
.\out\artifacts\host\nxpc_tool.exe selftest
.\out\artifacts\host\nxpc_tool.exe devices
.\out\artifacts\host\nxpc_tool.exe probe --frame --seconds 3
.\out\artifacts\host\nxpc_tool.exe program --image .\out\artifacts\embedded\nxp_cup_core0.bin
.\out\artifacts\host\nxpc_viewer.exe --test-seconds 5
```

The viewer test command expects a connected telemetry device; the CLI
`selftest` is the bench-free host smoke test. The viewer's firmware field
automatically resolves the published embedded image even when the viewer is
launched from `out\artifacts\host` instead of the repository root.

Create a deterministic, versioned portable zip and checksum with:

```powershell
.\src\host\package.ps1 -Version 1.0.0
```

Packaging refuses to replace an existing version unless `-Force` is explicit.
Normal releases use `release.ps1`, which adds source-tree and release checks.

## Maintainer release

One command builds and tests the native and browser tools, packages the Windows
x64 runtime, verifies its manifest and checksum, and stops before upload:

```powershell
.\src\host\release.ps1 -Version 1.0.0
```

Run the same command from a clean commit with `-Publish` to create a draft
GitHub release, download and verify its archive, and then make it public. The
script does not push branches or change remotes. For local validation while
developing the script, `-AllowDirty` is accepted only without `-Publish`.

## Browser viewer and tests

```powershell
.\src\host\tools\build-web.ps1
.\src\host\serve.ps1
npm test --prefix .\src\host
```

`nxpc_usb_debug_viewer.html` is the generated standalone handoff and must not
depend on external assets. The browser and native tools consume the same
session-gated `AVCU` v1 firmware protocol.

Until the shared-dashboard extraction lands, `webserial_viewer.html`,
`webserial_viewer.css`, and `webserial_viewer.js` are the authoritative Formula
One-style WebSerial sources; do not hand-edit generated `nxpc_usb_debug_viewer.html`.
The target structure in `docs/design/shared-web-dashboard.md` moves only presentation
assets into `src/web`. Serial discovery, `AVCU` parsing, control requests, race-action
gating, and direct RGB565 transport remain host-owned adapter behavior.

Obsolete standalone USB receiver diagnostics and their one-off build wrappers
have been removed; normal native development uses the CMake/LLVM build above.
