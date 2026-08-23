[CmdletBinding()]
param()

$ErrorActionPreference = "Stop"
. (Join-Path $PSScriptRoot "android_env.ps1")

$versions = $script:NxpCupAndroidVersions
$checks = @(
    (Join-Path $env:JAVA_HOME "bin\java.exe"),
    (Join-Path $env:ANDROID_HOME "platform-tools\adb.exe"),
    (Join-Path $env:ANDROID_HOME "platforms\$($versions.AndroidPlatform)\android.jar"),
    (Join-Path $env:ANDROID_HOME "build-tools\$($versions.AndroidBuildTools)\aapt2.exe"),
    $script:NxpCupAndroidGradle
)

foreach ($path in $checks) {
    if (-not (Test-Path -LiteralPath $path)) {
        throw "Android toolchain component missing: $path"
    }
}

Write-Host "JAVA_HOME=$env:JAVA_HOME"
$savedErrorPreference = $ErrorActionPreference
$ErrorActionPreference = "Continue"
try {
    & (Join-Path $env:JAVA_HOME "bin\java.exe") -version 2>&1 | ForEach-Object { Write-Host $_ }
} finally {
    $ErrorActionPreference = $savedErrorPreference
}
Write-Host "ANDROID_HOME=$env:ANDROID_HOME"
& (Join-Path $env:ANDROID_HOME "platform-tools\adb.exe") version
$gradleVersion = & $script:NxpCupAndroidGradle --version
if ($LASTEXITCODE -ne 0) { throw "Gradle version check failed with exit code $LASTEXITCODE" }
$gradleVersion | Select-Object -First 8
Write-Host "Android platform=$($versions.AndroidPlatform) build-tools=$($versions.AndroidBuildTools)"
