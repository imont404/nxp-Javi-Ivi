param(
    [ValidateSet("Debug")]
    [string]$Configuration = "Debug",

    [switch]$EnableMotors,
    [int]$PwmPercent = 12,
    # Per-motor duty. Defaults to -PwmPercent for both. Setting them differently
    # proves the two QDC channels are independent and correctly associated -
    # driving both at one duty would hide a cross-wiring or duplicate-read fault.
    [int]$PwmPercentM0 = -1,
    [int]$PwmPercentM1 = -1,
    # Auto-start motors this many ms after the diagnostic begins. 0 = button only.
    # Removes the race between a scripted capture window and a button press.
    [int]$AutoStartMs = 0,
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

if ($PwmPercentM0 -lt 0) { $PwmPercentM0 = $PwmPercent }
if ($PwmPercentM1 -lt 0) { $PwmPercentM1 = $PwmPercent }

if ($EnableMotors) {
    Write-Host "MOTORS ENABLED - M0 at $PwmPercentM0 percent, M1 at $PwmPercentM1 percent. Car must be on blocks." -ForegroundColor Yellow
} else {
    Write-Host "Motors disabled - encoder read-only, safe to turn wheels by hand." -ForegroundColor Green
}
$defines = @(
    "CONFIG__MOTOR_ENCODER_BACKEND=MOTOR_ENCODER_BACKEND_QDC",
    "CONFIG__MOTOR_ENCODER_DIAG_ENABLE=1",
    "CONFIG__MOTOR_ENCODER_DIAG_MOTOR_ENABLE=$motorEnable",
    "CONFIG__MOTOR_ENCODER_DIAG_PWM_PERCENT=$PwmPercent",
    "CONFIG__MOTOR_ENCODER_DIAG_PWM_PERCENT_M0=$PwmPercentM0",
    "CONFIG__MOTOR_ENCODER_DIAG_PWM_PERCENT_M1=$PwmPercentM1",
    "CONFIG__MOTOR_ENCODER_DIAG_AUTOSTART_MS=$AutoStartMs",
    "CONFIG__MOTOR_ENCODER_DIAG_REPORT_MS=$ReportMs",
    "CONFIG__MOTOR_ENCODER_COUNTS_PER_WHEEL_REV=$CountsPerWheelRev"
)

& (Join-Path $PSScriptRoot "build_cmake.ps1") `
    -Configuration $Configuration `
    -BuildDir $buildDir `
    -Define $defines

exit $LASTEXITCODE

Write-Host "Image: $buildDir\avc_core0.axf"
