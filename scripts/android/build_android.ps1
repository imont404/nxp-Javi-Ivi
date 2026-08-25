[CmdletBinding()]
param(
    [switch]$Clean,
    [switch]$Offline
)

$ErrorActionPreference = "Stop"
. (Join-Path $PSScriptRoot "android_env.ps1")
$projectRoot = Join-Path $script:NxpCupAndroidRepoRoot "src\android\nxp_cup_bridge"
$env:GRADLE_USER_HOME = Join-Path $script:NxpCupAndroidToolsRoot "gradle-user-home"

if (-not (Test-Path -LiteralPath $script:NxpCupAndroidGradle)) {
    throw "Pinned Gradle is missing. Run .\scripts\android\setup_android.ps1 -AcceptLicenses."
}

$tasks = @("testDebugUnitTest", "assembleDebug")
if ($Clean) { $tasks = @("clean") + $tasks }
$gradleArguments = @("--no-daemon")
if ($Offline) { $gradleArguments += "--offline" }
$gradleArguments += $tasks

Push-Location $projectRoot
try {
    & $script:NxpCupAndroidGradle @gradleArguments
    if ($LASTEXITCODE -ne 0) {
        throw "Android build failed with exit code $LASTEXITCODE"
    }
} finally {
    Pop-Location
}
