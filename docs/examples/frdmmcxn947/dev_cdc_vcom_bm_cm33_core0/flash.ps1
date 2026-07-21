param(
    [switch]$NoReset,
    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Debug",
    [string]$File,
    [string]$JLinkPath = "C:\Program Files\SEGGER\JLink_V940\JLink.exe",
    [string]$Device = "MCXN947_M33_0",
    [string]$Interface = "SWD",
    [int]$SpeedKHz = 4000,
    [string]$UsbSerial = "1066857635"
)

$ErrorActionPreference = "Stop"

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
$defaultAxf = Join-Path $projectDir "$Configuration\frdmmcxn947_dev_cdc_vcom_bm_cm33_core0.axf"
$axfFile = if ($File) { $File } else { $defaultAxf }
$JLinkPath = Resolve-SeggerTool -ConfiguredPath $JLinkPath -ToolName "JLink.exe"

if (-not (Test-Path -LiteralPath $JLinkPath)) {
    throw "J-Link Commander not found: $JLinkPath"
}

if (-not (Test-Path -LiteralPath $axfFile)) {
    throw "Firmware not found: $axfFile. Build first with .\build.ps1"
}

$fileInfo = Get-Item -LiteralPath $axfFile

Write-Host ("=" * 58)
Write-Host " FRDM-MCXN947 USB CDC Test - Flash"
Write-Host ("=" * 58)
Write-Host "Device: $Device"
Write-Host "Interface: $Interface @ $SpeedKHz kHz"
if (-not [string]::IsNullOrWhiteSpace($UsbSerial)) {
    Write-Host "J-Link serial: $UsbSerial"
}
Write-Host "File: $axfFile"
Write-Host "Size: $($fileInfo.Length) bytes"
Write-Host "Modified: $($fileInfo.LastWriteTime)"
Write-Host ""

$cmdFile = Join-Path $env:TEMP "jlink_flash_usb_cdc_mcxn947.jlink"
$selectCommands = @()
if (-not [string]::IsNullOrWhiteSpace($UsbSerial)) {
    $selectCommands += "SelectEmuBySN $UsbSerial"
}

$postLoadCommands = if ($NoReset) {
    @()
} else {
    @("r", "g")
}

$commands = $selectCommands + @(
    "device $Device",
    "si $Interface",
    "speed $SpeedKHz",
    "connect",
    "h",
    "loadfile `"$axfFile`""
) + $postLoadCommands + @("exit")

$commands | Set-Content -Path $cmdFile -Encoding ASCII

try {
    & $JLinkPath -CommandFile $cmdFile
    exit $LASTEXITCODE
} finally {
    Remove-Item -LiteralPath $cmdFile -Force -ErrorAction SilentlyContinue
}
