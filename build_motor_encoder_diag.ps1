param(
    [ValidateSet("Debug")]
    [string]$Configuration = "Debug",

    [switch]$EnableMotors,
    [int]$PwmPercent = 12,
    [int]$ReportMs = 250,
    [int]$CountsPerWheelRev = 0
)

$ErrorActionPreference = "Stop"

# Separate output directory per variant. Both images used to land in one folder,
# so the tree did not tell you which was there - a bad property when one of
# them spins the wheels.
$variant = if ($EnableMotors) { "MotorEncoderDiag-Motors" } else { "MotorEncoderDiag" }
$buildDir = Join-Path $PSScriptRoot "build\cmake\avc_core0-$variant"
$motorEnable = if ($EnableMotors) { "1" } else { "0" }

if ($EnableMotors) {
    Write-Host "MOTORS ENABLED - PWM capped at $PwmPercent percent. Car must be on blocks." -ForegroundColor Yellow
} else {
    Write-Host "Motors disabled - encoder read-only, safe to turn wheels by hand." -ForegroundColor Green
}
$defines = @(
    "CONFIG__MOTOR_ENCODER_BACKEND=MOTOR_ENCODER_BACKEND_QDC",
    "CONFIG__MOTOR_ENCODER_DIAG_ENABLE=1",
    "CONFIG__MOTOR_ENCODER_DIAG_MOTOR_ENABLE=$motorEnable",
    "CONFIG__MOTOR_ENCODER_DIAG_PWM_PERCENT=$PwmPercent",
    "CONFIG__MOTOR_ENCODER_DIAG_REPORT_MS=$ReportMs",
    "CONFIG__MOTOR_ENCODER_COUNTS_PER_WHEEL_REV=$CountsPerWheelRev"
)

& (Join-Path $PSScriptRoot "build_cmake.ps1") `
    -Configuration $Configuration `
    -BuildDir $buildDir `
    -Define $defines

exit $LASTEXITCODE

Write-Host "Image: $buildDir\avc_core0.axf"
