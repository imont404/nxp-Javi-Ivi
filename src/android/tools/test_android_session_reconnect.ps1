[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$Serial,
    [int]$Iterations = 6
)

$ErrorActionPreference = "Stop"
. (Join-Path $PSScriptRoot "android_env.ps1")
$adb = Join-Path $env:ANDROID_HOME "platform-tools\adb.exe"
$packageName = "com.wavenumber.nxpc.bridge"
$activityName = "$packageName/.MainActivity"
$sessionIds = [System.Collections.Generic.List[long]]::new()

if ($Iterations -lt 2) { throw "Use at least two iterations to prove repeatability." }

for ($iteration = 1; $iteration -le $Iterations; $iteration++) {
    & $adb -s $Serial shell am force-stop $packageName | Out-Null
    Start-Sleep -Milliseconds 250

    & $adb -s $Serial logcat -c
    & $adb -s $Serial shell am start -W -n $activityName | Out-Null
    if ($LASTEXITCODE -ne 0) { throw "Iteration $iteration could not start the bridge app." }

    $health = $null
    for ($attempt = 0; $attempt -lt 60; $attempt++) {
        Start-Sleep -Milliseconds 250
        try {
            $health = Invoke-RestMethod -Uri "http://$($Serial.Split(':')[0]):8765/health" -TimeoutSec 2
        } catch {
            continue
        }
        if (
            $health.state -eq "streaming" -and
            $health.usb_frames -gt 0 -and
            $health.sequence_errors -eq 0 -and
            $health.malformed_chunks -eq 0
        ) { break }
    }
    if (
        $null -eq $health -or
        $health.state -ne "streaming" -or
        $health.usb_frames -le 0 -or
        $health.sequence_errors -ne 0 -or
        $health.malformed_chunks -ne 0
    ) {
        & $adb -s $Serial logcat -d -s "NXP_CUP_BRIDGE_HEALTH:I" "*:S" | Write-Host
        throw "Iteration $iteration did not recover an error-free USB stream after forced process loss."
    }
    if ($sessionIds.Contains([long]$health.session_id)) {
        throw "Iteration $iteration reused USB session ID $($health.session_id)."
    }
    $sessionIds.Add([long]$health.session_id)
    & (Join-Path $PSScriptRoot "verify_android_relay.ps1") `
        -Serial $Serial `
        -MinimumFrames 2 | Out-Host
    Write-Host "Reconnect iteration $iteration/$Iterations passed (forced-stop, session $($health.session_id))."
}

[PSCustomObject]@{
    iterations = $Iterations
    forced_stops = $Iterations
    distinct_sessions = $sessionIds.Count
    session_ids = $sessionIds -join ","
} | Format-List

Write-Host "NXP Cup Android session reconnect proof passed." -ForegroundColor Green
