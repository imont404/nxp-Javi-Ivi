param(
    [switch]$NoReset,
    [string]$File = ""
)

$ErrorActionPreference = "Stop"

if ([string]::IsNullOrWhiteSpace($File)) {
    $File = Join-Path $PSScriptRoot "build\cmake\avc_core0-MotorEncoderDiag\avc_core0.axf"
}

$flashArgs = @("-File", $File)
if ($NoReset) {
    $flashArgs += "-NoReset"
}

& (Join-Path $PSScriptRoot "flash.ps1") @flashArgs
exit $LASTEXITCODE
