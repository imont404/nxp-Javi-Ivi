[CmdletBinding()]
param(
    [switch]$Clean,
    [switch]$Offline
)

$ErrorActionPreference = "Stop"
$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
$toolsDir = Join-Path $PSScriptRoot "tools"

& (Join-Path $toolsDir "verify_android_toolchain.ps1")
& (Join-Path $toolsDir "build-project.ps1") -Clean:$Clean -Offline:$Offline

$sourceApk = Join-Path $PSScriptRoot "nxp_cup_bridge\app\build\outputs\apk\debug\app-debug.apk"
if (-not (Test-Path -LiteralPath $sourceApk -PathType Leaf)) {
    throw "Android build completed but the expected APK is missing: $sourceApk"
}

$publishDir = Join-Path $repoRoot "out\artifacts\android"
$publishedApk = Join-Path $publishDir "nxp_cup_bridge.apk"
New-Item -ItemType Directory -Force -Path $publishDir | Out-Null
Copy-Item -LiteralPath $sourceApk -Destination $publishedApk -Force

Write-Host "Built and tested: $sourceApk" -ForegroundColor Green
Write-Host "Android APK: $publishedApk" -ForegroundColor Green

