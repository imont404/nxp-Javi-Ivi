param(
    [switch]$NoReset,
    # Selects the motors-enabled image. Must match how it was built.
    [switch]$EnableMotors,
    [string]$File = "",
    # Passed through to flash.ps1; empty resolves automatically.
    [string]$UsbSerial = ""
)

$ErrorActionPreference = "Stop"

if ([string]::IsNullOrWhiteSpace($File)) {
    $variant = if ($EnableMotors) { "MotorEncoderDiag-Motors" } else { "MotorEncoderDiag" }
    $File = Join-Path $PSScriptRoot "build\cmake\avc_core0-$variant\avc_core0.axf"
}

if (-not (Test-Path -LiteralPath $File)) {
    throw "Encoder diagnostic image not found: $File. Build it with .\build_motor_encoder_diag.ps1$(if ($EnableMotors) { ' -EnableMotors' })"
}

if ($EnableMotors) {
    Write-Host "MOTORS-ENABLED encoder diagnostic. Car must be on blocks." -ForegroundColor Yellow
} else {
    Write-Host "Motors-off encoder diagnostic - safe to turn wheels by hand." -ForegroundColor Green
}

# Splat a hashtable, not an array. Splatting an array passes elements
# positionally, so "-File" bound to flash.ps1's -Configuration and the call
# failed validation.
$flashArgs = @{ File = $File }
if ($NoReset) { $flashArgs.NoReset = $true }
if (-not [string]::IsNullOrWhiteSpace($UsbSerial)) { $flashArgs.UsbSerial = $UsbSerial }

& (Join-Path $PSScriptRoot "flash.ps1") @flashArgs
exit $LASTEXITCODE
