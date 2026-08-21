$script:AvcAndroidVersions = Import-PowerShellDataFile (Join-Path $PSScriptRoot "toolchain.versions.psd1")
$script:AvcAndroidRepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
$script:AvcAndroidToolsRoot = Join-Path $script:AvcAndroidRepoRoot "out\toolchains\android"

$env:JAVA_HOME = Join-Path $script:AvcAndroidToolsRoot "jdk-$($script:AvcAndroidVersions.JdkVersion)"
$env:ANDROID_HOME = Join-Path $script:AvcAndroidToolsRoot "sdk"
$env:ANDROID_SDK_ROOT = $env:ANDROID_HOME

$androidPaths = @(
    (Join-Path $env:JAVA_HOME "bin"),
    (Join-Path $env:ANDROID_HOME "platform-tools"),
    (Join-Path $env:ANDROID_HOME "cmdline-tools\$($script:AvcAndroidVersions.AndroidCommandLineToolsVersion)\bin")
)

$existingPaths = $env:Path -split [IO.Path]::PathSeparator
$newPaths = $androidPaths | Where-Object { $_ -notin $existingPaths }
if ($newPaths.Count -gt 0) {
    $env:Path = (($newPaths + $existingPaths) -join [IO.Path]::PathSeparator)
}

$script:AvcAndroidGradle = Join-Path $script:AvcAndroidToolsRoot "gradle-$($script:AvcAndroidVersions.GradleVersion)\bin\gradle.bat"
