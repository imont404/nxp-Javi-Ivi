# Embedded MCXN947 Firmware

This component builds the Rev A NXP Cup competition firmware for the
FRDM-MCXN947 and FRDM-AVC shield.

```powershell
.\src\embedded\build.ps1
```

The normal output is published to `out\artifacts\embedded`. CMake build trees
live under `out\build\embedded`. The MCUXpresso project remains in
`nxp_cup_core0` so its linked shared-source layout stays compatible with the IDE.

Students flash the AXF with SEGGER Ozone using
`nxp_cup_core0\ozone__core0.jdebug`. Scripts under `tools` are maintainer-only
flash, RTT, metadata-drift, MCUXpresso, and diagnostic helpers.

