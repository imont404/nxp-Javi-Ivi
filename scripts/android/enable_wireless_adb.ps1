[CmdletBinding()]
param(
    [string]$Serial,
    [int]$Port = 5555
)

$ErrorActionPreference = "Stop"
. (Join-Path $PSScriptRoot "android_env.ps1")
$adb = Join-Path $env:ANDROID_HOME "platform-tools\adb.exe"

$devices = @(& $adb devices | Select-Object -Skip 1 | Where-Object { $_ -match "\tdevice$" })
if ([string]::IsNullOrWhiteSpace($Serial)) {
    $usbDevices = @($devices | Where-Object { ($_ -split "\s+")[0] -notmatch ":" })
    if ($usbDevices.Count -ne 1) {
        throw "Expected exactly one authorized USB adb device, found $($usbDevices.Count). Use -Serial when needed."
    }
    $Serial = ($usbDevices[0] -split "\s+")[0]
}

$wifiLine = (& $adb -s $Serial shell ip -f inet addr show wlan0 | Where-Object { $_ -match "\sinet\s" } | Select-Object -First 1)
if ($wifiLine -notmatch "inet\s+([0-9.]+)") {
    throw "Could not determine the phone Wi-Fi address."
}
$wifiAddress = $Matches[1]

& $adb -s $Serial tcpip $Port
if ($LASTEXITCODE -ne 0) { throw "adb tcpip failed with exit code $LASTEXITCODE" }
Start-Sleep -Seconds 2

$target = "${wifiAddress}:$Port"
& $adb connect $target
if ($LASTEXITCODE -ne 0) { throw "adb connect $target failed with exit code $LASTEXITCODE" }

$connected = @(& $adb devices | Where-Object { $_ -match "^$([regex]::Escape($target))\s+device$" })
if ($connected.Count -ne 1) {
    throw "Wireless adb target did not become ready: $target"
}

Write-Host "Wireless adb ready: $target" -ForegroundColor Green
Write-Host "The USB-C cable may now be moved from the PC to the car's J11 MCX HS USB port." -ForegroundColor Green
