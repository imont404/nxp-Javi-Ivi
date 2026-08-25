[CmdletBinding()]
param(
    [switch]$Clean,
    [switch]$Offline
)

$ErrorActionPreference = "Stop"

$verifyScript = Join-Path $PSScriptRoot "scripts\android\verify_android_toolchain.ps1"
$buildScript = Join-Path $PSScriptRoot "scripts\android\build_android.ps1"
& $verifyScript
& $buildScript -Clean:$Clean -Offline:$Offline
if ($LASTEXITCODE -ne 0) {
    throw "Android build failed with exit code $LASTEXITCODE"
}

$sourceApk = Join-Path $PSScriptRoot "src\android\nxp_cup_bridge\app\build\outputs\apk\debug\app-debug.apk"
if (-not (Test-Path -LiteralPath $sourceApk)) {
    throw "Android build completed but the expected APK is missing: $sourceApk"
}

$publishDir = Join-Path $PSScriptRoot "bin\android"
$publishedApk = Join-Path $publishDir "nxp_cup_bridge.apk"
New-Item -ItemType Directory -Force -Path $publishDir | Out-Null
Copy-Item -LiteralPath $sourceApk -Destination $publishedApk -Force

Write-Host "Built and tested: $sourceApk" -ForegroundColor Green
Write-Host "Android APK: $publishedApk" -ForegroundColor Green
