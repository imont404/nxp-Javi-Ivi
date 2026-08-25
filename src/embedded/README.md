# Embedded MCXN947 Firmware

This component builds the Rev A NXP Cup competition firmware for the
FRDM-MCXN947 and FRDM-AVC shield.

```powershell
.\src\embedded\build.ps1
.\src\embedded\flash.ps1 -Backend Rom
```

The normal output is published to `out\artifacts\embedded`. CMake build trees
live under `out\build\embedded`. The MCUXpresso project remains in
`nxp_cup_core0` so its linked shared-source layout stays compatible with the IDE.

Students flash the AXF with SEGGER Ozone using
`nxp_cup_core0\ozone__core0.jdebug`. The adjacent `flash.ps1` wrapper supports
explicit Ozone, ROM-HID, and J-Link backends. Scripts under `tools` contain
maintainer-only RTT, metadata-drift, MCUXpresso, and diagnostic helpers.
