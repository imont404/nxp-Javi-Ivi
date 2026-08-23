[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$Serial,
    [int]$SettlingSeconds = 10
)

$ErrorActionPreference = "Stop"
. (Join-Path $PSScriptRoot "android_env.ps1")
$adb = Join-Path $env:ANDROID_HOME "platform-tools\adb.exe"

& (Join-Path $PSScriptRoot "verify_android_relay.ps1") `
    -Serial $Serial `
    -MinimumFrames 3 | Out-Host

try {
    & $adb -s $Serial shell input keyevent KEYCODE_SLEEP
    Start-Sleep -Seconds $SettlingSeconds
    $power = & $adb -s $Serial shell dumpsys power
    if (-not ($power | Where-Object { $_ -match "mWakefulness=(Dozing|Asleep)" })) {
        throw "Phone did not enter an expected screen-off power state (Dozing or Asleep)."
    }
    $idle = & $adb -s $Serial shell dumpsys deviceidle
    if (-not ($idle | Where-Object { $_ -match "mScreenOn=false" })) {
        throw "Phone still reports its screen as on."
    }
    & (Join-Path $PSScriptRoot "verify_android_relay.ps1") `
        -Serial $Serial `
        -MinimumFrames 6 | Out-Host
} finally {
    & $adb -s $Serial shell input keyevent KEYCODE_WAKEUP
}

Write-Host "NXP Cup Android locked-screen relay proof passed." -ForegroundColor Green
