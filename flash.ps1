param(
    [switch]$NoReset,

    # Which image to flash. The CMake preset flow is the default; the preset
    # name is the normal way to choose. See CMakePresets.json for the list.
    [string]$Preset = "competition",

    # Deprecated no-op. CMake is the default now; kept so existing commands and
    # notes do not break.
    [switch]$CMake,

    # Flash the MCUXpresso output directory instead. Only useful when comparing
    # the two build systems.
    [switch]$Mcux,

    [ValidateSet("Debug")]
    [string]$Configuration = "Debug",
    [string]$File,
    [string]$JLinkPath = "C:\Program Files\SEGGER\JLink_V940\JLink.exe",
    [string]$Device = "MCXN947_M33_0",
    [string]$Interface = "SWD",
    [int]$SpeedKHz = 4000,

    # J-Link probe serial. Leave empty to resolve automatically:
    # AVC_JLINK_SERIAL, else auto-detect when exactly one probe is attached.
    # Not hardcoded - every kit has a different serial.
    # See scripts\tools\jlink_common.ps1.
    [string]$UsbSerial = ""
)

$ErrorActionPreference = "Stop"

. (Join-Path $PSScriptRoot "scripts\tools\jlink_common.ps1")
. (Join-Path $PSScriptRoot "scripts\tools\avc_image_common.ps1")
$UsbSerial = Resolve-JLinkSerial -Requested $UsbSerial

function Resolve-SeggerTool {
    param(
        [Parameter(Mandatory = $true)]
        [string]$ConfiguredPath,
        [Parameter(Mandatory = $true)]
        [string]$ToolName
    )

    if (Test-Path -LiteralPath $ConfiguredPath) {
        return $ConfiguredPath
    }

    $seggerRoot = "C:\Program Files\SEGGER"
    if (Test-Path -LiteralPath $seggerRoot) {
        $candidate = Get-ChildItem -LiteralPath $seggerRoot -Directory -Filter "JLink_V*" |
            Sort-Object -Property Name -Descending |
            ForEach-Object { Join-Path $_.FullName $ToolName } |
            Where-Object { Test-Path -LiteralPath $_ } |
            Select-Object -First 1

        if ($candidate) {
            return $candidate
        }
    }

    return $ConfiguredPath
}

$projectDir = $PSScriptRoot
$axfFile = Resolve-AvcImage -RepoRoot $projectDir -Preset $Preset -File $File `
                            -Mcux:$Mcux -Configuration $Configuration
$JLinkPath = Resolve-SeggerTool -ConfiguredPath $JLinkPath -ToolName "JLink.exe"

if (-not (Test-Path -LiteralPath $JLinkPath)) {
    throw "J-Link Commander not found: $JLinkPath"
}

$fileInfo = Get-Item -LiteralPath $axfFile

Write-Host ("=" * 50)
Write-Host " AVC MCXN947 - Flash"
Write-Host ("=" * 50)
Write-Host "Device: $Device"
Write-Host "J-Link S/N: $UsbSerial"
Write-Host "Interface: $Interface @ $SpeedKHz kHz"
Write-Host "File: $axfFile"
Write-Host "Size: $($fileInfo.Length) bytes"
Write-Host "Modified: $($fileInfo.LastWriteTime)"
Write-Host ""

$cmdFile = Join-Path $env:TEMP "jlink_flash_avc.jlink"
$postLoadCommands = if ($NoReset) {
    @()
} else {
    @("r", "g")
}

$commands = @(
    "device $Device",
    "si $Interface",
    "speed $SpeedKHz",
    "connect",
    "h",
    "loadfile `"$axfFile`""
) + $postLoadCommands + @("exit")

$commands | Set-Content -Path $cmdFile -Encoding ASCII

$jlinkArgs = @()
if (-not [string]::IsNullOrWhiteSpace($UsbSerial)) {
    $jlinkArgs += @("-SelectEmuBySN", $UsbSerial)
}
$jlinkArgs += @("-CommandFile", $cmdFile)

try {
    & $JLinkPath @jlinkArgs
    exit $LASTEXITCODE
} finally {
    Remove-Item -LiteralPath $cmdFile -Force -ErrorAction SilentlyContinue
}
