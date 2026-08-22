[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidateSet("raw", "jpeg", "h264")]
    [string]$Mode,
    [string]$Serial,
    [ValidateRange(1024, 65535)]
    [int]$Port = 8765,
    [ValidateRange(1, 100)]
    [int]$JpegQuality = 70,
    [ValidateRange(100000, 10000000)]
    [int]$H264Bitrate = 750000
)

$ErrorActionPreference = "Stop"
. (Join-Path $PSScriptRoot "android_env.ps1")
$adb = Join-Path $env:ANDROID_HOME "platform-tools\adb.exe"
$packageName = "com.wavenumber.avc.bridge"
$activityName = "$packageName/.MainActivity"

if ([string]::IsNullOrWhiteSpace($Serial)) {
    $devices = @(
        & $adb devices |
            Select-Object -Skip 1 |
            Where-Object { $_ -match "\s+device$" }
    )
    if ($devices.Count -ne 1) {
        throw "Expected exactly one authorized adb target, found $($devices.Count). Use -Serial when needed."
    }
    $Serial = ($devices[0] -split "\s+")[0]
}

& $adb -s $Serial shell am force-stop $packageName | Out-Null
& $adb -s $Serial shell am start -W -n $activityName `
    --es relay_video $Mode `
    --ei relay_port $Port `
    --ei jpeg_quality $JpegQuality `
    --ei h264_bitrate $H264Bitrate | Write-Host
if ($LASTEXITCODE -ne 0) { throw "Could not start the $Mode relay." }

if ($Serial -notmatch "^(?<host>[^:]+):[0-9]+$") {
    Write-Warning "Relay started, but its URL cannot be derived from non-network adb target '$Serial'."
    return
}
$baseUri = "http://$($Matches.host):$Port"
for ($attempt = 0; $attempt -lt 20; $attempt++) {
    try {
        $health = Invoke-RestMethod -Uri "$baseUri/health" -TimeoutSec 1
        if ($health.state -eq "streaming" -and $health.relay_mode -eq $Mode) {
            Write-Host "Relay ready: $baseUri/?video=$Mode&replace=1" -ForegroundColor Green
            return
        }
    } catch {
    }
    Start-Sleep -Milliseconds 250
}
throw "The app did not report a streaming $Mode relay at $baseUri."
