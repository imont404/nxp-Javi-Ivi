+++
type = "plan"
id = "one-cable-host-tool"
status = "active"
created = "2026-08-22"

[[steps]]
id = "lock-scope"
title = "Freeze the one-cable workflow, safety boundaries, USB identities, image format, and host-tool ownership"
status = "done"

[[steps]]
id = "rom-image-artifact"
title = "Make every relevant CMake preset emit a deterministic ROM-programmable binary beside the AXF"
status = "done"
depends_on = ["lock-scope"]

[[steps]]
id = "manual-rom-isp-proof"
title = "Prove physical SW3/reset entry, USB-HID discovery, program, reset, and application recovery through J11"
status = "done"
depends_on = ["rom-image-artifact"]

[[steps]]
id = "host-core"
title = "Extract the native AVC parser, framed session, transport, latest-frame model, and telemetry state into a reusable host core"
status = "done"
depends_on = ["lock-scope"]

[[steps]]
id = "device-discovery"
title = "Discover runtime AVC CDC and MCXN947 ROM HID devices without guessing when multiple matching boards are present"
status = "done"
depends_on = ["host-core", "manual-rom-isp-proof"]

[[steps]]
id = "native-preview"
title = "Build a minimal SDL2 and Dear ImGui Windows viewer with automatic session setup, raw RGB565 preview, connection state, and basic telemetry"
status = "done"
depends_on = ["host-core", "device-discovery"]

[[steps]]
id = "safe-isp-contract"
title = "Define a framed ENTER_ISP request that requires a recognized session, produces a correlated response, and always safe-stops actuators"
status = "done"
depends_on = ["lock-scope", "manual-rom-isp-proof"]

[[steps]]
id = "firmware-rom-entry"
title = "Implement bounded shutdown of streaming and explicit handoff to the MCXN947 ROM USB-HS HID bootloader"
status = "done"
depends_on = ["safe-isp-contract"]

[[steps]]
id = "programmer-backend-selection"
title = "Select the programming backend from proven MCXN947 USB-HID behavior, command coverage, Windows reliability, licensing, and distribution evidence"
status = "done"
depends_on = ["manual-rom-isp-proof"]

[[steps]]
id = "programmer-backend"
title = "Wrap the selected pinned programmer with validated target identity, explicit erase and write stages, captured diagnostics, and reset"
status = "done"
depends_on = ["device-discovery", "programmer-backend-selection"]

[[steps]]
id = "native-program-ui"
title = "Add deliberate image selection and programming controls to the native viewer without making the GUI own the firmware build"
status = "done"
depends_on = ["native-preview", "programmer-backend"]

[[steps]]
id = "cli-program-path"
title = "Expose the same discovery and programming backend through a bounded command-line mode for maintainers and automation"
status = "done"
depends_on = ["programmer-backend"]

[[steps]]
id = "one-cable-cycle"
title = "Prove application preview, software ISP transition, programming, reset, reconnect, and resumed preview using only J11"
status = "done"
depends_on = ["firmware-rom-entry", "native-program-ui"]

[[steps]]
id = "failure-recovery"
title = "Validate unplug, cancellation, bad image, failed write, application crash, physical ISP recovery, and repeated programming cycles"
status = "active"
depends_on = ["one-cable-cycle", "cli-program-path"]

[[steps]]
id = "dependency-hardening"
title = "Pin and locally retain all host build and runtime dependencies, preferring static SDL and compiled-in ImGui, then prove offline rebuild and execution"
status = "done"
depends_on = ["native-preview", "programmer-backend-selection"]

[[steps]]
id = "distribution-handoff"
title = "After the working proof, choose repository copies, pinned downloads, zip, or master setup integration for the programmer and host artifacts"
status = "done"
depends_on = ["dependency-hardening"]

[[steps]]
id = "student-workflow-docs"
title = "Document the one-cable student workflow and physical recovery path without exposing maintainer-only tools or a race solution"
status = "pending"
depends_on = ["failure-recovery", "distribution-handoff"]

[[steps]]
id = "design-doc-intent-audit"
title = "Audit USB, host-tool, build, and student documentation against the implemented one-cable workflow"
status = "pending"
depends_on = ["student-workflow-docs"]

[[steps]]
id = "test-runtime-impact-audit"
title = "Audit host tests, firmware validation, build time, binary size, and competition-loop runtime impact"
status = "done"
depends_on = ["one-cable-cycle"]

[[steps]]
id = "external-review"
title = "Obtain independent review of programming safety, recovery behavior, student boundaries, dependencies, and documentation"
status = "pending"
depends_on = ["design-doc-intent-audit", "test-runtime-impact-audit"]

[[exit_criteria]]
id = "single-cable"
title = "A Windows host can view, program, reset, and resume an AVC board through J11 without moving the cable to the debug connector"
status = "met"

[[exit_criteria]]
id = "native-viewer"
title = "The native tool automatically establishes an AVC session and displays live 320x200 RGB565 frames and basic telemetry"
status = "met"

[[exit_criteria]]
id = "safe-programming"
title = "Software ISP entry safe-stops the vehicle, validates the selected target and image, and reports each programming stage"
status = "met"

[[exit_criteria]]
id = "recovery"
title = "Physical SW3/reset entry through J11 recovers a board whose application cannot service USB or ENTER_ISP"
status = "met"

[[exit_criteria]]
id = "no-device-guessing"
title = "Exactly one device may auto-connect; multiple matching runtime or ROM devices require an explicit choice"
status = "pending"

[[exit_criteria]]
id = "shared-core"
title = "GUI and command-line programming use the same tested discovery, protocol, and programmer implementation"
status = "met"

[[exit_criteria]]
id = "browser-fallback"
title = "The existing standalone Web Serial viewer remains functional and the wire protocol does not fork for the native tool"
status = "met"

[[exit_criteria]]
id = "offline-dependencies"
title = "The accepted host tool rebuilds and runs without fetching SDL, ImGui, or programmer components from the network"
status = "met"

[[exit_criteria]]
id = "distribution-decision"
title = "The proven tool has a recorded programmer and dependency acquisition strategy suitable for later master setup or packaging work"
status = "met"

[[exit_criteria]]
id = "student-boundary"
title = "The workflow supplies generic viewing, telemetry, and programming infrastructure without supplying a race algorithm"
status = "pending"

[[exit_criteria]]
id = "design-doc-intent-audit"
title = "Design and setup documentation match the implemented workflow and organizer intent"
status = "pending"

[[exit_criteria]]
id = "test-runtime-impact-audit"
title = "New tests and their host, build, firmware-size, and runtime costs are recorded"
status = "met"

[[exit_criteria]]
id = "external-review"
title = "Independent external review is complete"
status = "pending"
+++

# One-Cable AVC Host Viewer and ROM ISP Programmer

## Integration ownership

This plan retains native viewer, ROM-HID programming, reconnect/recovery, host dependencies,
and host validation. `docs/plans/nxp-cup-framework-migration` owns final product/package
naming plus the canonical `flash.ps1` backend interface. This plan supplies backend evidence
and must not independently rename the wire contract or public firmware facade.

## Priority

Make the one-cable development loop work before spending time on an installer or polished
distribution. The first proof uses the high-speed J11 USB connection for both the running
AVC CDC protocol and the MCXN947 ROM USB-HID programmer. A native Windows viewer then
combines automatic device discovery, the already-proven raw camera stream, basic telemetry,
and deliberate firmware programming controls.

Packaging remains downstream. Once the end-to-end loop is reliable, record whether the
master setup should keep approved programmer and SDL artifacts in the repository, download
pinned artifacts, install a pinned tool through setup, or distribute a zip/installer. That
decision must not delay the bench proof.

## Purpose

Remove the practical requirement for two USB cables during normal student development.
The single cable supplied with a kit can remain on J11 while the student alternates between
building firmware, programming it through the ROM bootloader, and observing camera and
telemetry output from the running application.

This is host and platform infrastructure. It must not add line following, edge detection,
PID tuning, or any other completed race solution.

## Verified Starting Point

- The competition firmware enumerates its high-speed CDC application as NXP VID `0x1FC9`
  and PID `0x0094`; host discovery must not depend on its current legacy product string.
- The current framed AVCU session supports `HELLO`, channel subscriptions, camera frames,
  statistics, logs, named telemetry, ping, and close.
- Sustained 320x200 RGB565 receive has already been measured at about 23.43 FPS, so the
  native PC viewer does not need JPEG or H.264 for this workflow.
- `src/usb_debug_host/usb_cdc_stream_read.cpp` already proves native Win32 CDC read/write
  and framed session setup, but currently requires a COM port argument and is structured as
  a test executable rather than a reusable library.
- Bunny Vision provides a read-only reference for SDL2/Dear ImGui rendering, background
  serial receive, latest-frame publication, and Win32 VID/PID COM discovery.
- The MCXN947 ROM bootloader appears on J11 as USB HID VID `0x1FC9`, PID `0x014F`.
- NXP documents `blhost` commands for connection testing, internal-flash erase, binary write
  at address `0x0`, and reset.
- The MCXN947 device-specific runbootloader API exposes application-controlled ISP entry and
  identifies USB HS HID as boot interface `5`. This exact mapping must be taken from the
  imported device driver and verified on the bench rather than copied from generic examples.
- The application descriptor currently has no USB serial-number string. Multiple identical
  boards therefore require explicit selection; the tool must never silently choose the first.
- The CMake firmware target currently emits an AXF but not the raw binary needed by the
  documented ROM write-memory flow.

## Intended Architecture

```text
AVC application on J11                  MCXN947 ROM on J11
CDC 1FC9:0094                           HID 1FC9:014F
        |                                      ^
        | framed ENTER_ISP + safe stop         |
        +--------------------------------------+
                                               |
                              programmer erase/write/reset
                                               |
        +--------------------------------------+
        v
application re-enumerates; host performs HELLO and resumes preview
```

Build one shared native host core with narrow front ends:

1. Windows device discovery for runtime CDC and ROM HID identities.
2. Serial transport, bounded AVCU parsing, correlated control responses, and session state.
3. Latest-complete-frame ownership plus telemetry/log state for the UI.
4. A programmer backend that invokes a pinned, validated tool and captures machine-readable
   results without shell command construction from untrusted strings.
5. An SDL2/Dear ImGui GUI for preview and deliberate programming actions.
6. A command-line mode using the same programmer backend for maintainers and automation.

The existing standalone Web Serial viewer remains the zero-install fallback. The native
tool consumes the same protocol and must not introduce a firmware fork.

## Programmer Backend Decision

NXP SPSDK `blhost` 3.10.0 established the first MCXN947 USB-HID proof and remains a tested
installed fallback. The packaged default is now NXP Rust `rblhost` 0.2.0. Its Windows x64
release was built from tagged commit `7a775dde2c44bd345a1ac067698afa999bd71be0`, copied and
run without a sibling runtime tree, and bench-proven against `1FC9:014F` for property query,
internal-flash erase, write at address zero, full-length readback, reset, and recovered camera
preview. Full readback size and SHA-256 must match the selected image before reset. The pinned
binary, SHA-256, source record, and BSD-3-Clause license are retained under
`src/usb_debug_host/vendor/rblhost`.

Do not reimplement the Mboot protocol for the first working version. The selected backend
must remain behind a narrow process-runner interface so it can be replaced without changing
the GUI, CLI, or firmware protocol.

## Programming Safety Contract

`ENTER_ISP` is not a general remote-control facility. It must be a framed command available
only after a recognized AVC session, with a correlated ACK or NACK. Before leaving the
application, firmware must put motor and steering outputs into the defined safe state, stop
new frame publication, and bound any wait for outstanding USB work. The host must not infer
that a disappearing COM port means programming succeeded; it must identify the ROM HID
device and query it before erasing anything.

The programmer must:

- accept only an explicitly selected, existing firmware artifact;
- verify the expected runtime or ROM USB identity;
- refuse ambiguous multi-board cases;
- expose erase, write, reset, and reconnect as separate reported states;
- treat cancellation or disconnect as a recoverable failure;
- retain physical SW3/reset entry as the documented recovery path;
- avoid fuse, CMPA, security, or program-once commands entirely;
- start with NXP's documented internal-flash procedure on a maintainer board and record the
  exact validated commands before automating them.

The GUI does not own the build in the initial version. Students build with the existing
CMake preset flow and select or accept the known preset output. Build-and-flash integration
can be considered only after the programming cycle is reliable.

## Host UI Boundary

The MVP UI is intentionally small:

- a large raw RGB565 camera preview;
- clear disconnected, application-connected, entering-ISP, ROM-connected, programming,
  reconnecting, and error states;
- selected device and firmware artifact;
- frame rate and basic named telemetry;
- a bounded firmware debug-log view using the existing timestamped
  `AVC_DBG_LOG_TEXT` records;
- one deliberate program action with confirmation and visible progress.

The accepted 2026-08-23 layout uses a black SDL workspace with independently
movable and resizable Dear ImGui `Camera`, `AVC status`, `Program firmware`, and
`Debug log` panels. Initial placement remains useful at 1000x720, but it is not
reapplied every frame, so the operator can rearrange and resize panels. This is
a host-only presentation change; subscription, framing, bounded parser storage,
and firmware runtime behavior are unchanged.

Do not add dashboard styling, compression, native plotting libraries, actuator controls, or
a simulator to the critical path. Those can consume the shared host core later.

## Dependency Reliability

The final host tool must not depend on a live package server, GitHub checkout, PATH setting,
or a preinstalled developer environment merely to run. Pin exact versions and retain the
approved artifacts needed to reproduce the build.

- Prefer a static SDL2 library so the GUI cannot fail because `SDL2.dll` is missing or from
  the wrong version. If static linking proves impractical, retain the pinned DLL and always
  install or extract it beside the executable.
- Compile the pinned Dear ImGui sources directly into the application. Do not leave Bunny
  Vision's configure-time `FetchContent` download in the accepted workflow.
- Keep the chosen programmer version and license evidence reproducible. A companion
  executable is acceptable if it is pinned, checksummed, installed beside the GUI, and
  invoked by absolute path.
- Prove both a clean offline rebuild and execution on a machine that does not already have
  SDL, ImGui, SPSDK, or the programmer on PATH.

The most bulletproof deliverable may be a single downloaded installer or zip even if the
installed runtime contains more than one file. Do not distort the implementation merely to
produce one Windows PE file; eliminate missing-version and discovery failures instead.

## Validation

Host-only tests should cover fragmented and concatenated AVCU packets, malformed lengths,
unknown message IDs, stale responses, latest-frame takeover, disconnects, and device-list
ambiguity. The programmer process runner should be tested with recorded success and failure
output before it is allowed to erase hardware.

Bench validation must cover:

- physical ISP entry and recovery using only J11;
- application-controlled ISP entry with camera streaming active;
- the competition and camera-USB-bench images;
- repeated program/reset/reconnect/preview cycles;
- unplug during application streaming, ROM discovery, erase, write, and reconnect;
- invalid and missing image files;
- no application response and no ROM response;
- exactly one board and multiple attached matching boards;
- proof that motors remain safe throughout every transition and failure.

Current 2026-08-23 bench state: the normal application-stream unplug/replug
case has passed repeated physical J11 detach/attach cycles. The running viewer
hid its stale image, re-enumerated `COM34`, negotiated a new session, and
resumed live preview; the final captured state showed four successful
connections, 821 complete frames, and zero malformed packets. Invalid image and
target requests are also proven to fail before ISP, and repeated complete
program/reset/reconnect cycles have passed. Keep `failure-recovery` active:
mid-erase/write cable removal, cancellation, application-crash simulation,
multi-board selection, and repeated destructive cycles remain. Those tests are
deferred until a suitable bench board is available rather than risk the travel
hardware immediately before departure.

## Distribution Handoff

The interim portable distribution handoff is complete even though the remaining
destructive `failure-recovery` cases are still active. The repository retains pinned
Dear ImGui source/license and standalone `rblhost` binary/license, uses its existing SDL2
package, and builds a versioned zip with a SHA-256 manifest. A fresh extraction passed
self-test and live preview. The 2026-08-23 travel build with floating panels and the
firmware debug-log view received 114 complete frames in five seconds with zero malformed
packets. This gives the trip a reproducible tool without changing the student Ozone workflow.

Later master-setup or installer work may still choose one of these without changing the
host architecture:

- keep approved tool and runtime artifacts in the repository;
- download pinned, checksummed artifacts during master setup;
- install a pinned tool through the existing `uv`-based setup;
- publish a versioned zip or later NSIS installer containing the native tool and dependencies.

The master setup documentation remains authoritative. The generated zip is deliberately
ignored by Git; copy it separately or regenerate it from
`src/usb_debug_host/package_avc_host.ps1`. A packaging follow-up may implement an NSIS or
equivalent installer after race-week behavior is accepted.

## Explicitly Out of Scope

- Installer implementation before the one-cable proof is reliable.
- Reimplementing the NXP Mboot USB-HID protocol in C++, Rust, or JavaScript.
- WebHID firmware programming.
- Replacing the standalone Web Serial viewer.
- JPEG or H.264 compression for the directly attached PC viewer.
- General remote motor, steering, or race-mode control.
- A completed student vision, steering, speed-control, or race algorithm.

## Source Material

- `docs/research/AVC_USB_Debug_Display_Current_State.md`
- `docs/research/AVC_USB_Debug_Transport_Protocol.md`
- `docs/plans/usb-debug-telemetry/plan.md`
- `docs/plans/cmake-build-and-toolchain/plan.md`
- `docs/setup.html`
- `src/common/avc_usb_debug/avc_usb_debug_protocol.h`
- `src/avc/avc_core0/source/usb_device_descriptor.c`
- `src/usb_debug_host/usb_cdc_stream_read.cpp`
- Bunny Vision host reference:
  `D:/prj/wavenumber/bunny_vision/bunny_vision_firmware-west/bunny_vision_sw`
- NXP AN14460, *How to program MCX N series internal flash through ISP*
- NXP MCUXpresso SDK MCXN947 `fsl_runbootloader.h` and runbootloader example
- NXP SPSDK `blhost` documentation and license
