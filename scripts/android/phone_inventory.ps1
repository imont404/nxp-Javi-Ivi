[CmdletBinding()]
param(
    [string]$Serial
)

$ErrorActionPreference = "Stop"
. (Join-Path $PSScriptRoot "android_env.ps1")
$adb = Join-Path $env:ANDROID_HOME "platform-tools\adb.exe"

$devices = @(& $adb devices | Select-Object -Skip 1 | Where-Object { $_ -match "\tdevice$" })
if ([string]::IsNullOrWhiteSpace($Serial)) {
    if ($devices.Count -ne 1) {
        throw "Expected exactly one authorized adb device, found $($devices.Count). Use -Serial when needed."
    }
    $Serial = ($devices[0] -split "\s+")[0]
}

function Get-Prop([string]$Name) {
    return (& $adb -s $Serial shell getprop $Name).Trim()
}

$features = & $adb -s $Serial shell pm list features
$usbHost = [bool]($features | Where-Object { $_ -match "android\.hardware\.usb\.host" })
$wifiLine = (& $adb -s $Serial shell ip -f inet addr show wlan0 | Where-Object { $_ -match "\sinet\s" } | Select-Object -First 1)
$wifiAddress = if ($wifiLine -match "inet\s+([0-9.]+)") { $Matches[1] } else { "unavailable" }

[pscustomobject]@{
    Serial = $Serial
    Manufacturer = Get-Prop "ro.product.manufacturer"
    Model = Get-Prop "ro.product.model"
    Product = Get-Prop "ro.product.name"
    AndroidRelease = Get-Prop "ro.build.version.release"
    ApiLevel = Get-Prop "ro.build.version.sdk"
    BuildFingerprint = Get-Prop "ro.build.fingerprint"
    UsbHostFeature = $usbHost
    WifiAddress = $wifiAddress
} | Format-List

& $adb -s $Serial shell dumpsys battery | Select-String "level:|status:|temperature:|USB powered:|AC powered:"
