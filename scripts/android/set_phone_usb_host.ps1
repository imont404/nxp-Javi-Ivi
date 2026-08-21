[CmdletBinding()]
param(
    [string]$Serial
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

$usbState = & $adb -s $Serial shell dumpsys usb
$portIdLine = $usbState | Where-Object { $_ -match "^\s+id=(port\S+)" } | Select-Object -First 1
if (-not $portIdLine -or $portIdLine -notmatch "id=(\S+)") {
    throw "Android did not report a USB-C port identifier."
}
$portId = $Matches[1]

if (
    ($usbState | Where-Object { $_ -match "^\s+current_mode=dfp$" }) -and
    ($usbState | Where-Object { $_ -match "^\s+power_role=source$" }) -and
    ($usbState | Where-Object { $_ -match "^\s+data_role=host$" })
) {
    Write-Host "Phone USB port $portId is already source/host." -ForegroundColor Green
    exit 0
}

# Android's USB service may print a vendor-specific warning even when the role
# swap succeeds. Treat the read-back state, not that text, as authoritative.
& $adb -s $Serial shell dumpsys usb set-port-roles $portId source host | Write-Host
Start-Sleep -Seconds 3
$usbState = & $adb -s $Serial shell dumpsys usb
if (
    -not ($usbState | Where-Object { $_ -match "^\s+current_mode=dfp$" }) -or
    -not ($usbState | Where-Object { $_ -match "^\s+power_role=source$" }) -or
    -not ($usbState | Where-Object { $_ -match "^\s+data_role=host$" })
) {
    throw "USB-C role swap failed. Use the OTG adapter topology documented in src\android\avc_bridge\README.md."
}

Write-Host "Phone USB port $portId is now source/host." -ForegroundColor Green
