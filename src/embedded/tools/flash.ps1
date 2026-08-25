param(
    [ValidateSet("Ozone", "Rom", "JLink")]
    [string]$Backend = "",

    [switch]$NoReset,

    # Deprecated no-op. CMake is the default now; kept so existing commands and
    # notes do not break.
    [switch]$CMake,

    # Flash the MCUXpresso output directory instead. Only useful when comparing
    # the two build systems.
    [switch]$Mcux,

    [ValidateSet("Debug")]
    [string]$Configuration = "Debug",
    [string]$File,
    [string]$OzonePath = "",
    [string]$JLinkPath = "C:\Program Files\SEGGER\JLink_V940\JLink.exe",
    [string]$Device = "MCXN947_M33_0",
    [string]$Interface = "SWD",
    [int]$SpeedKHz = 4000,

    # J-Link probe serial. Leave empty to resolve automatically:
    # NXPC_JLINK_SERIAL, else auto-detect when exactly one probe is attached.
    # Not hardcoded - every kit has a different serial.
    # See tools\lib\jlink_common.ps1.
    [string]$UsbSerial = ""
)

$ErrorActionPreference = "Stop"

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..\..")).Path
. (Join-Path $PSScriptRoot "lib\nxpc_image_common.ps1")

if ([string]::IsNullOrWhiteSpace($Backend)) {
    throw "Choose a flash backend explicitly: -Backend Ozone, -Backend Rom, or -Backend JLink."
}

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

$axfFile = Resolve-NxpCupImage -RepoRoot $repoRoot -File $File `
                            -Mcux:$Mcux -Configuration $Configuration

if ($Backend -eq "Ozone") {
    if ($File -or $Mcux) {
        throw "The Ozone project is pinned to the competition image. Use -Backend Rom or JLink for an explicit image."
    }

    $ozoneProject = Join-Path $repoRoot "src\embedded\nxp_cup_core0\ozone__core0.jdebug"
    if ([string]::IsNullOrWhiteSpace($OzonePath)) {
        $OzonePath = Get-ChildItem -LiteralPath "C:\Program Files\SEGGER" -Directory -Filter "Ozone*" `
            -ErrorAction SilentlyContinue |
            Sort-Object Name -Descending |
            ForEach-Object { Join-Path $_.FullName "Ozone.exe" } |
            Where-Object { Test-Path -LiteralPath $_ } |
            Select-Object -First 1
    }
    if (-not $OzonePath -or -not (Test-Path -LiteralPath $OzonePath)) {
        throw "SEGGER Ozone was not found. Install it or pass -OzonePath."
    }

    Write-Host "Opening the NXP Cup competition image in Ozone: $axfFile" -ForegroundColor Cyan
    Start-Process -FilePath $OzonePath -ArgumentList @($ozoneProject)
    return
}

if ($Backend -eq "Rom") {
    $binFile = if ([IO.Path]::GetExtension($axfFile) -ieq ".bin") {
        $axfFile
    } else {
        [IO.Path]::ChangeExtension($axfFile, ".bin")
    }
    if (-not (Test-Path -LiteralPath $binFile)) {
        throw "ROM image not found: $binFile. Run .\src\embedded\build.ps1 to create it."
    }

    $hostTool = Join-Path $repoRoot "out\artifacts\host\nxpc_tool.exe"
    if (-not (Test-Path -LiteralPath $hostTool)) {
        throw "NXP Cup host tool not found. Run .\src\host\build.ps1 first."
    }

    & $hostTool program --image $binFile
    exit $LASTEXITCODE
}

. (Join-Path $PSScriptRoot "lib\jlink_common.ps1")
$UsbSerial = Resolve-JLinkSerial -Requested $UsbSerial
$JLinkPath = Resolve-SeggerTool -ConfiguredPath $JLinkPath -ToolName "JLink.exe"

if (-not (Test-Path -LiteralPath $JLinkPath)) {
    throw "J-Link Commander not found: $JLinkPath"
}

$fileInfo = Get-Item -LiteralPath $axfFile

Write-Host ("=" * 50)
Write-Host " NXP Cup MCXN947 - Flash"
Write-Host ("=" * 50)
Write-Host "Device: $Device"
Write-Host "J-Link S/N: $UsbSerial"
Write-Host "Interface: $Interface @ $SpeedKHz kHz"
Write-Host "File: $axfFile"
Write-Host "Size: $($fileInfo.Length) bytes"
Write-Host "Modified: $($fileInfo.LastWriteTime)"
Write-Host ""

$cmdFile = Join-Path $env:TEMP "jlink_flash_nxpc.jlink"
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
