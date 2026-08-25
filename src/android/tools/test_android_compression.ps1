[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidateSet("jpeg", "h264")]
    [string]$Mode,
    [string]$Serial,
    [ValidateRange(5, 300)]
    [int]$Seconds = 15,
    [ValidateRange(1, 100)]
    [int]$JpegQuality = 70,
    [ValidateRange(100000, 10000000)]
    [int]$H264Bitrate = 750000,
    [switch]$LeaveProbeRunning
)

$ErrorActionPreference = "Stop"
. (Join-Path $PSScriptRoot "android_env.ps1")
$adb = Join-Path $env:ANDROID_HOME "platform-tools\adb.exe"
$packageName = "com.wavenumber.nxpc.bridge"
$activityName = "$packageName/.MainActivity"

if ([string]::IsNullOrWhiteSpace($Serial)) {
    $wirelessDevices = @(
        & $adb devices |
            Select-Object -Skip 1 |
            Where-Object { $_ -match "^\S+:\d+\s+device$" }
    )
    if ($wirelessDevices.Count -ne 1) {
        throw "Expected exactly one wireless adb target, found $($wirelessDevices.Count). Use -Serial when needed."
    }
    $Serial = ($wirelessDevices[0] -split "\s+")[0]
}

$startArguments = @(
    "-s", $Serial,
    "shell", "am", "start", "-W", "-n", $activityName,
    "--es", "compression_probe", $Mode,
    "--ei", "jpeg_quality", $JpegQuality,
    "--ei", "h264_bitrate", $H264Bitrate
)

try {
    & $adb -s $Serial shell am force-stop $packageName | Out-Null
    & $adb -s $Serial logcat -c
    & $adb @startArguments | Write-Host
    if ($LASTEXITCODE -ne 0) { throw "Could not start the compression probe." }

    Start-Sleep -Seconds $Seconds

    $compressionLines = @(& $adb -s $Serial logcat -d -s "NXPC_COMPRESSION:I" "*:S")
    $result = $compressionLines |
        Where-Object { $_ -match "mode=$Mode state=running" } |
        Select-Object -Last 1
    if ($null -eq $result) {
        $compressionLines | Write-Host
        throw "No running $Mode compression result was reported."
    }
    if ($result -notmatch "encoded=([1-9][0-9]*)") {
        throw "The $Mode probe did not encode a frame: $result"
    }
    if ($result -notmatch "fps=([0-9.]+)") {
        throw "The $Mode probe did not report FPS: $result"
    }
    $fps = [double]::Parse($Matches[1], [Globalization.CultureInfo]::InvariantCulture)
    if ($fps -lt 20.0) {
        throw "The $Mode probe fell below 20 FPS: $result"
    }

    $health = @(& $adb -s $Serial logcat -d -s "NXP_CUP_BRIDGE_HEALTH:I" "*:S") |
        Where-Object { $_ -match "state=streaming" } |
        Select-Object -Last 1
    if ($null -eq $health -or $health -notmatch "seq_errors=0" -or $health -notmatch "malformed=0") {
        throw "USB health was not clean during the $Mode probe: $health"
    }

    $meminfo = & $adb -s $Serial shell dumpsys meminfo $packageName |
        Select-String "TOTAL PSS" |
        Select-Object -First 1
    $battery = & $adb -s $Serial shell dumpsys battery |
        Select-String "level:|temperature:|current now:"

    Write-Host $result
    Write-Host $health
    $meminfo | Write-Host
    $battery | Write-Host
    Write-Host "Compression probe passed: mode=$Mode duration_s=$Seconds" -ForegroundColor Green
} finally {
    if (-not $LeaveProbeRunning) {
        & $adb -s $Serial shell am force-stop $packageName | Out-Null
        & $adb -s $Serial shell am start -W -n $activityName | Out-Null
        $phoneAddress = ($Serial -split ":")[0]
        $restored = $false
        for ($attempt = 0; $attempt -lt 20; $attempt++) {
            try {
                $null = Invoke-RestMethod -Uri "http://${phoneAddress}:8765/health" -TimeoutSec 1
                $restored = $true
                break
            } catch {
                Start-Sleep -Milliseconds 250
            }
        }
        if (-not $restored) {
            Write-Warning "The normal app restarted, but its relay was not reachable within five seconds."
        }
    }
}
