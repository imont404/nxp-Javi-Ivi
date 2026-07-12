param(
    [switch]$NoReset,
    [switch]$CMake,
    [ValidateSet("Debug")]
    [string]$Configuration = "Debug",
    [string]$File,
    [string]$JLinkPath = "C:\Program Files\SEGGER\JLink_V940\JLink.exe",
    [string]$Device = "MCXN947_M33_0",
    [string]$Interface = "SWD",
    [int]$SpeedKHz = 4000
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
$mcuxAxf = Join-Path $projectDir "src\avc\avc_core0\$Configuration\avc_core0.axf"
$cmakeAxf = Join-Path $projectDir "build\cmake\avc_core0-$Configuration\avc_core0.axf"
$defaultAxf = if ($CMake) { $cmakeAxf } else { $mcuxAxf }
$axfFile = if ($File) { $File } else { $defaultAxf }
$JLinkPath = Resolve-SeggerTool -ConfiguredPath $JLinkPath -ToolName "JLink.exe"

if (-not (Test-Path -LiteralPath $JLinkPath)) {
    throw "J-Link Commander not found: $JLinkPath"
}

if (-not (Test-Path -LiteralPath $axfFile)) {
    throw "Firmware not found: $axfFile. Build first with .\build.ps1"
}

$fileInfo = Get-Item -LiteralPath $axfFile

Write-Host ("=" * 50)
Write-Host " AVC MCXN947 - Flash"
Write-Host ("=" * 50)
Write-Host "Device: $Device"
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

try {
    & $JLinkPath -CommandFile $cmdFile
    exit $LASTEXITCODE
} finally {
    Remove-Item -LiteralPath $cmdFile -Force -ErrorAction SilentlyContinue
}
