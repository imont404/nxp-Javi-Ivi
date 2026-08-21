[CmdletBinding()]
param(
    [switch]$Clean
)

$ErrorActionPreference = "Stop"
. (Join-Path $PSScriptRoot "android_env.ps1")
$projectRoot = Join-Path $script:AvcAndroidRepoRoot "src\android\avc_bridge"
$gradleWrapper = Join-Path $projectRoot "gradlew.bat"
$env:GRADLE_USER_HOME = Join-Path $script:AvcAndroidToolsRoot "gradle-user-home"

if (-not (Test-Path -LiteralPath $gradleWrapper)) {
    throw "Gradle wrapper is missing. Run scripts\android\setup_android.ps1 and regenerate it."
}

$tasks = @("testDebugUnitTest", "assembleDebug")
if ($Clean) { $tasks = @("clean") + $tasks }

Push-Location $projectRoot
try {
    & $gradleWrapper --no-daemon @tasks
    if ($LASTEXITCODE -ne 0) {
        throw "Android build failed with exit code $LASTEXITCODE"
    }
} finally {
    Pop-Location
}
