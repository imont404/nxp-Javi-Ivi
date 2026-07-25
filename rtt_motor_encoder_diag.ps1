param(
    [switch]$Reset,
    [int]$Seconds = 10,
    [string]$File = ""
)

$ErrorActionPreference = "Stop"

if ([string]::IsNullOrWhiteSpace($File)) {
    $File = Join-Path $PSScriptRoot "build\cmake\avc_core0-MotorEncoderDiag\avc_core0.axf"
}

$rttArgs = @("-File", $File, "-Seconds", $Seconds)
if ($Reset) {
    $rttArgs += "-Reset"
}

& (Join-Path $PSScriptRoot "rtt.ps1") @rttArgs
exit $LASTEXITCODE
