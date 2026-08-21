[CmdletBinding()]
param(
    [string]$Serial
)

$ErrorActionPreference = "Stop"
. (Join-Path $PSScriptRoot "android_env.ps1")
$adb = Join-Path $env:ANDROID_HOME "platform-tools\adb.exe"
$apk = Join-Path $script:AvcAndroidRepoRoot "src\android\avc_bridge\app\build\outputs\apk\debug\app-debug.apk"
$packageName = "com.wavenumber.avc.bridge"
$activityName = "$packageName/.MainActivity"

if (-not (Test-Path -LiteralPath $apk)) {
    throw "Debug APK not found. Run scripts\android\build_android.ps1 first."
}

$devices = @(& $adb devices | Select-Object -Skip 1 | Where-Object { $_ -match "\tdevice$" })
if ([string]::IsNullOrWhiteSpace($Serial)) {
    if ($devices.Count -ne 1) {
        throw "Expected exactly one authorized adb target, found $($devices.Count). Use -Serial when needed."
    }
    $Serial = ($devices[0] -split "\s+")[0]
}

& $adb -s $Serial install -r $apk
if ($LASTEXITCODE -ne 0) { throw "APK install failed with exit code $LASTEXITCODE" }

& $adb -s $Serial logcat -c
& $adb -s $Serial shell am force-stop $packageName
& $adb -s $Serial shell am start -W -n $activityName
if ($LASTEXITCODE -ne 0) { throw "App start failed with exit code $LASTEXITCODE" }

for ($attempt = 0; $attempt -lt 60; $attempt++) {
    Start-Sleep -Milliseconds 500
    $health = & $adb -s $Serial logcat -d -s "AVC_BRIDGE_HEALTH:I" "*:S"
    if (
        $health |
            Where-Object {
                $_ -match "state=streaming" -and
                $_ -match "frames=[1-9][0-9]*" -and
                $_ -match "seq_errors=0" -and
                $_ -match "malformed=0"
            }
    ) { break }
    if ($health | Where-Object { $_ -match "state=error" }) { break }
}
if (
    -not (
        $health |
            Where-Object {
                $_ -match "state=streaming" -and
                $_ -match "frames=[1-9][0-9]*" -and
                $_ -match "seq_errors=0" -and
                $_ -match "malformed=0"
            }
    )
) {
    $health | Write-Host
    throw "AVC bridge did not reach an error-free live-frame state. Check USB permission and AVC_BRIDGE_HEALTH."
}
$health | Write-Host
