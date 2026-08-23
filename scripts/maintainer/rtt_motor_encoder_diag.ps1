param(
    [switch]$Reset,
    [int]$Seconds = 10,
    # Selects the motors-enabled image. Must match what was flashed.
    [switch]$EnableMotors,
    [string]$File = "",
    [string]$OutFile = "",
    # Passed through to rtt.ps1; empty resolves automatically.
    [string]$UsbSerial = ""
)

$ErrorActionPreference = "Stop"
$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path

if ([string]::IsNullOrWhiteSpace($File)) {
    $variant = if ($EnableMotors) { "MotorEncoderDiag-Motors" } else { "MotorEncoderDiag" }
    $File = Join-Path $repoRoot "build\cmake\nxp_cup_core0-$variant\nxp_cup_core0.axf"
}

if (-not (Test-Path -LiteralPath $File)) {
    throw "Encoder diagnostic image not found: $File. Build it with .\build_motor_encoder_diag.ps1$(if ($EnableMotors) { ' -EnableMotors' })"
}

# Splat a hashtable, not an array - see flash_motor_encoder_diag.ps1.
$rttArgs = @{ File = $File; Seconds = $Seconds }
if ($Reset) { $rttArgs.Reset = $true }
if (-not [string]::IsNullOrWhiteSpace($OutFile)) { $rttArgs.OutFile = $OutFile }
if (-not [string]::IsNullOrWhiteSpace($UsbSerial)) { $rttArgs.UsbSerial = $UsbSerial }

& (Join-Path $repoRoot "rtt.ps1") @rttArgs
exit $LASTEXITCODE
