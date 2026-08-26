# Embedded MCXN947 Firmware

This component builds the Rev A NXP Cup competition firmware for the
FRDM-MCXN947 and FRDM-AVC shield.

```powershell
.\src\embedded\build.ps1
.\src\embedded\flash.ps1
```

The normal output is published to `out\artifacts\embedded`. CMake build trees
live under `out\build\embedded`. The MCUXpresso project remains in
`nxp_cup_core0` so its linked shared-source layout stays compatible with the IDE.

The adjacent `flash.ps1` wrapper first tries the preferred ROM-HID path and
falls back to J-Link Commander when ROM flashing is unavailable. `-Backend Rom`
and `-Backend JLink` force one command-line path. Scripts under `tools` contain
maintainer-only RTT, metadata-drift, MCUXpresso, and diagnostic helpers.

Students normally work in `nxp_cup_core0\source\app\vision_test.c` and
`race_mode.c`. `test_mode.c` is a short automatic dispatcher used while the TEST
jumper is installed. Its page implementations are separate: CAMERA / IO
demonstrates inputs and telemetry, VISION is the camera-processing sandbox, and
MOTORS demonstrates the public motor and steering API. Navigation and all
motion-safety gates remain framework-owned.

Additional student modules are just another `.c`/`.h` pair placed directly in
`nxp_cup_core0\source\app`. The normal CMake build automatically compiles every
`.c` file there; see that folder's README for the two-line workflow.

J-Link Commander is found automatically from the newest SEGGER installation
under Program Files. Override discovery with `-JLinkPath` or the
`NXPC_JLINK_PATH` environment variable. A single attached probe is selected
automatically; use `-UsbSerial` or `NXPC_JLINK_SERIAL` when several are attached.
