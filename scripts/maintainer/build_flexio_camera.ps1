<#
.SYNOPSIS
    Build the FlexIO camera capture image for a chosen pin group.

.DESCRIPTION
    Wraps build_cmake.ps1 for the FlexIO eDMA capture backend.

    -PinGroup Port4 (default) is the original proven group. It needs eleven
    fly-wires because Port 4 is not where the Rev A camera is routed.

    -PinGroup Port1 reuses the Rev A camera wiring and needs only three jumpers
    on J9_EXT:
        J9_EXT.12 (P3_4 cam D4) -> J9_EXT.32 (P1_8)
        J9_EXT.11 (P3_5 cam D5) -> J9_EXT.30 (P1_9)
        J9_EXT.15 (P0_5 PCLK)   -> J9_EXT.2  (P1_14)

    Neither profile changes the normal build. The Rev A competition default
    remains the SmartDMA/EZH backend; see nxpc__master_config.h.

    See docs/research/AVC_Camera_FlexIO_Pin_Migration.md and
    docs/plans/camera-flexio-pin-migration/.

.EXAMPLE
    .\build_flexio_camera.ps1
    .\build_flexio_camera.ps1 -PinGroup Port1
#>
param(
    [ValidateSet("Port4", "Port1")]
    [string]$PinGroup = "Port4",

    [string]$BuildDir = ""
)

$ErrorActionPreference = "Stop"
$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path

$groupValue = if ($PinGroup -eq "Port1") { 2 } else { 1 }

if ([string]::IsNullOrWhiteSpace($BuildDir)) {
    $BuildDir = Join-Path $repoRoot "build\cmake\nxp_cup_core0-FlexIO$PinGroup"
}

Write-Host "Building FlexIO eDMA camera capture, pin group $PinGroup" -ForegroundColor Cyan
if ($PinGroup -eq "Port1") {
    Write-Host "Requires the three J9_EXT jumpers: 12->32, 11->30, 15->2" -ForegroundColor Yellow
}

& (Join-Path $PSScriptRoot "build_cmake.ps1") `
    -BuildDir $BuildDir `
    -Define "CONFIG__CAMERA_CAPTURE_BACKEND=3", "CONFIG__CAMERA_FLEXIO_PIN_GROUP=$groupValue"
