<#
.SYNOPSIS
    Provision the pinned command-line Android toolchain used by the AVC bridge.

.DESCRIPTION
    Downloads verified archives into out\toolchains\android and installs the
    pinned Android SDK packages. Nothing is installed globally or persisted in
    the user's environment. Re-run safely to verify or repair the local tools.

.EXAMPLE
    .\scripts\android\setup_android.ps1 -AcceptLicenses
#>
[CmdletBinding()]
param(
    [switch]$AcceptLicenses,
    [switch]$Force
)

$ErrorActionPreference = "Stop"
$versions = Import-PowerShellDataFile (Join-Path $PSScriptRoot "toolchain.versions.psd1")
$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
$toolsRoot = Join-Path $repoRoot "out\toolchains\android"
$downloadRoot = Join-Path $toolsRoot "downloads"
$jdkRoot = Join-Path $toolsRoot "jdk-$($versions.JdkVersion)"
$sdkRoot = Join-Path $toolsRoot "sdk"
$commandToolsRoot = Join-Path $sdkRoot "cmdline-tools\$($versions.AndroidCommandLineToolsVersion)"
$gradleRoot = Join-Path $toolsRoot "gradle-$($versions.GradleVersion)"

function Assert-UnderToolsRoot([string]$Path) {
    $fullPath = [IO.Path]::GetFullPath($Path)
    $fullRoot = [IO.Path]::GetFullPath($toolsRoot).TrimEnd('\') + '\'
    if (-not $fullPath.StartsWith($fullRoot, [StringComparison]::OrdinalIgnoreCase)) {
        throw "Refusing to modify path outside Android tool root: $fullPath"
    }
}

function Remove-ToolPath([string]$Path) {
    Assert-UnderToolsRoot $Path
    if (Test-Path -LiteralPath $Path) {
        Remove-Item -LiteralPath $Path -Recurse -Force
    }
}

function Get-VerifiedArchive(
    [string]$Name,
    [string]$Url,
    [string]$Sha256
) {
    New-Item -ItemType Directory -Path $downloadRoot -Force | Out-Null
    $destination = Join-Path $downloadRoot $Name
    $needDownload = $Force -or -not (Test-Path -LiteralPath $destination)
    if (-not $needDownload) {
        $actual = (Get-FileHash -LiteralPath $destination -Algorithm SHA256).Hash.ToLowerInvariant()
        $needDownload = $actual -ne $Sha256.ToLowerInvariant()
    }
    if ($needDownload) {
        if (Test-Path -LiteralPath $destination) {
            Remove-Item -LiteralPath $destination -Force
        }
        Write-Host "Downloading $Name" -ForegroundColor Cyan
        Write-Host "  $Url" -ForegroundColor DarkGray
        $ProgressPreference = "SilentlyContinue"
        Invoke-WebRequest -UseBasicParsing -Uri $Url -OutFile $destination
    }
    $actual = (Get-FileHash -LiteralPath $destination -Algorithm SHA256).Hash.ToLowerInvariant()
    if ($actual -ne $Sha256.ToLowerInvariant()) {
        throw "SHA-256 mismatch for $Name. Expected $Sha256, got $actual."
    }
    Write-Host "Verified $Name" -ForegroundColor Green
    return $destination
}

function Expand-SingleRootArchive(
    [string]$Archive,
    [string]$Destination,
    [string]$ExpectedProbe
) {
    if ((Test-Path -LiteralPath (Join-Path $Destination $ExpectedProbe)) -and -not $Force) {
        Write-Host "Using $Destination" -ForegroundColor DarkGray
        return
    }
    $temporary = Join-Path $toolsRoot ("extract-" + [guid]::NewGuid().ToString("N"))
    Assert-UnderToolsRoot $temporary
    try {
        Remove-ToolPath $Destination
        New-Item -ItemType Directory -Path $temporary -Force | Out-Null
        Expand-Archive -LiteralPath $Archive -DestinationPath $temporary -Force
        $children = @(Get-ChildItem -LiteralPath $temporary -Force)
        if (($children.Count -eq 1) -and $children[0].PSIsContainer) {
            Move-Item -LiteralPath $children[0].FullName -Destination $Destination
        } else {
            New-Item -ItemType Directory -Path $Destination -Force | Out-Null
            foreach ($child in $children) {
                Move-Item -LiteralPath $child.FullName -Destination $Destination
            }
        }
    } finally {
        Remove-ToolPath $temporary
    }
    $probe = Join-Path $Destination $ExpectedProbe
    if (-not (Test-Path -LiteralPath $probe)) {
        throw "Expected tool was not found after extracting $Archive`: $probe"
    }
}

New-Item -ItemType Directory -Path $toolsRoot -Force | Out-Null

$jdkArchive = Get-VerifiedArchive $versions.JdkArchive $versions.JdkUrl $versions.JdkSha256
Expand-SingleRootArchive $jdkArchive $jdkRoot "bin\java.exe"

$gradleArchive = Get-VerifiedArchive $versions.GradleArchive $versions.GradleUrl $versions.GradleSha256
Expand-SingleRootArchive $gradleArchive $gradleRoot "bin\gradle.bat"

$commandArchive = Get-VerifiedArchive `
    $versions.AndroidCommandLineToolsArchive `
    $versions.AndroidCommandLineToolsUrl `
    $versions.AndroidCommandLineToolsSha256

$sdkManagerProbe = Join-Path $commandToolsRoot "bin\sdkmanager.bat"
if ($Force -or -not (Test-Path -LiteralPath $sdkManagerProbe)) {
    $temporary = Join-Path $toolsRoot ("extract-android-" + [guid]::NewGuid().ToString("N"))
    Assert-UnderToolsRoot $temporary
    try {
        Remove-ToolPath $commandToolsRoot
        New-Item -ItemType Directory -Path $temporary -Force | Out-Null
        Expand-Archive -LiteralPath $commandArchive -DestinationPath $temporary -Force
        $archiveRoot = Join-Path $temporary "cmdline-tools"
        if (-not (Test-Path -LiteralPath (Join-Path $archiveRoot "bin\sdkmanager.bat"))) {
            throw "Unexpected Android command-line tools archive layout."
        }
        New-Item -ItemType Directory -Path (Split-Path -Parent $commandToolsRoot) -Force | Out-Null
        Move-Item -LiteralPath $archiveRoot -Destination $commandToolsRoot
    } finally {
        Remove-ToolPath $temporary
    }
}

. (Join-Path $PSScriptRoot "android_env.ps1")
$sdkManager = Join-Path $commandToolsRoot "bin\sdkmanager.bat"

if ($AcceptLicenses) {
    Write-Host "Accepting Android SDK package licenses (explicit -AcceptLicenses)." -ForegroundColor Yellow
    1..20 | ForEach-Object { "y" } | & $sdkManager "--sdk_root=$sdkRoot" --licenses | Out-Host
    if ($LASTEXITCODE -ne 0) {
        throw "sdkmanager --licenses failed with exit code $LASTEXITCODE"
    }
} else {
    Write-Host "Android SDK licenses were not accepted." -ForegroundColor Yellow
    Write-Host "Re-run with -AcceptLicenses after reviewing the Android SDK terms." -ForegroundColor Yellow
}

$packages = @(
    "platform-tools",
    "platforms;$($versions.AndroidPlatform)",
    "build-tools;$($versions.AndroidBuildTools)"
)
& $sdkManager "--sdk_root=$sdkRoot" @packages
if ($LASTEXITCODE -ne 0) {
    throw "sdkmanager package installation failed with exit code $LASTEXITCODE"
}

Write-Host "Android toolchain provisioned under $toolsRoot" -ForegroundColor Green
& (Join-Path $PSScriptRoot "verify_android_toolchain.ps1")
