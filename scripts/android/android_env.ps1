$script:NxpCupAndroidVersions = Import-PowerShellDataFile (Join-Path $PSScriptRoot "toolchain.versions.psd1")
$script:NxpCupAndroidRepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
$script:NxpCupAndroidToolsRoot = Join-Path $script:NxpCupAndroidRepoRoot "out\toolchains\android"

$env:JAVA_HOME = Join-Path $script:NxpCupAndroidToolsRoot "jdk-$($script:NxpCupAndroidVersions.JdkVersion)"
$env:ANDROID_HOME = Join-Path $script:NxpCupAndroidToolsRoot "sdk"
$env:ANDROID_SDK_ROOT = $env:ANDROID_HOME

$androidPaths = @(
    (Join-Path $env:JAVA_HOME "bin"),
    (Join-Path $env:ANDROID_HOME "platform-tools"),
    (Join-Path $env:ANDROID_HOME "cmdline-tools\$($script:NxpCupAndroidVersions.AndroidCommandLineToolsVersion)\bin")
)

$existingPaths = $env:Path -split [IO.Path]::PathSeparator
$newPaths = $androidPaths | Where-Object { $_ -notin $existingPaths }
if ($newPaths.Count -gt 0) {
    $env:Path = (($newPaths + $existingPaths) -join [IO.Path]::PathSeparator)
}

$script:NxpCupAndroidGradle = Join-Path $script:NxpCupAndroidToolsRoot "gradle-$($script:NxpCupAndroidVersions.GradleVersion)\bin\gradle.bat"
