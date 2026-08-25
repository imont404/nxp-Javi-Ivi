[CmdletBinding()]
param(
    [string]$Serial,
    [switch]$Clean,
    [switch]$SkipRelay
)

$ErrorActionPreference = "Stop"
. (Join-Path $PSScriptRoot "android_env.ps1")
$adb = Join-Path $env:ANDROID_HOME "platform-tools\adb.exe"

if ([string]::IsNullOrWhiteSpace($Serial)) {
    $wirelessDevices = @(
        & $adb devices |
            Select-Object -Skip 1 |
            Where-Object { $_ -match "^[0-9.]+:[0-9]+\s+device$" }
    )
    if ($wirelessDevices.Count -ne 1) {
        throw "Expected exactly one wireless adb target, found $($wirelessDevices.Count). Use -Serial when needed."
    }
    $Serial = ($wirelessDevices[0] -split "\s+")[0]
}

& (Join-Path $PSScriptRoot "verify_android_toolchain.ps1")

& (Join-Path $PSScriptRoot "build-project.ps1") -Clean:$Clean

& (Join-Path $PSScriptRoot "deploy_android.ps1") -Serial $Serial

if (-not $SkipRelay) {
    & (Join-Path $PSScriptRoot "verify_android_relay.ps1") -Serial $Serial
}

Write-Host "NXP Cup Android loop passed for $Serial" -ForegroundColor Green
