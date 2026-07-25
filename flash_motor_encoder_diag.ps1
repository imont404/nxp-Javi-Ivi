param(
    [switch]$NoReset,
    # Selects the motors-enabled image. Must match how it was built.
    [switch]$EnableMotors,
    [string]$File = ""
)

$ErrorActionPreference = "Stop"

if ([string]::IsNullOrWhiteSpace($File)) {
    $variant = if ($EnableMotors) { "MotorEncoderDiag-Motors" } else { "MotorEncoderDiag" }
    $File = Join-Path $PSScriptRoot "build\cmake\avc_core0-$variant\avc_core0.axf"
    if ($EnableMotors) {
        Write-Host "Flashing the MOTORS-ENABLED encoder diagnostic. Car must be on blocks." -ForegroundColor Yellow
    }
}

$flashArgs = @("-File", $File)
if ($NoReset) {
    $flashArgs += "-NoReset"
}

& (Join-Path $PSScriptRoot "flash.ps1") @flashArgs
exit $LASTEXITCODE
