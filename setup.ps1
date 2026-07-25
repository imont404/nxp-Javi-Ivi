<#
.SYNOPSIS
    Provision everything needed to build, flash, and check the AVC firmware.

.DESCRIPTION
    One script, run once, on a machine with nothing installed. It provisions:

      1. Arm GNU Toolchain 14.2.Rel1  -> out\toolchains\  (no MCUXpresso needed)
      2. CMake                        -> winget, if not already on PATH
      3. Ninja                        -> winget, if not already on PATH
      4. uv                           -> winget, if not already on PATH
      5. LLVM (clang-format/tidy)     -> winget, if not already on PATH

    The Arm toolchain version deliberately matches the one MCUXpresso 25.6
    bundles, so a build using either is directly comparable.

    Idempotent: re-running skips anything already present. Nothing is added to
    PATH and no environment variables are persisted - the CMake build discovers
    the toolchain under out\toolchains\ on its own.

.EXAMPLE
    .\setup.ps1
    .\setup.ps1 -Force          # re-download the Arm toolchain even if present
    .\setup.ps1 -SkipOptional   # Arm toolchain, CMake, and Ninja only
#>
[CmdletBinding()]
param(
    [switch]$Force,
    [switch]$SkipCMake,
    [switch]$SkipNinja,
    [switch]$SkipUv,
    [switch]$SkipLlvm,
    # Skips uv and LLVM: enough to build and flash, not enough to run the gates.
    [switch]$SkipOptional
)

$ErrorActionPreference = "Stop"
Add-Type -AssemblyName System.IO.Compression.FileSystem

# Must match the Arm GNU Toolchain version MCUXpresso 25.6 bundles, so a build
# with either toolchain can be compared directly.
$ArmVersion = "14.2.rel1"
$ArmZipName = "arm-gnu-toolchain-$ArmVersion-mingw-w64-i686-arm-none-eabi"
$ArmUrl     = "https://developer.arm.com/-/media/Files/downloads/gnu/$ArmVersion/binrel/$ArmZipName.zip"

$RepoRoot = $PSScriptRoot
$ToolsDir = Join-Path $RepoRoot "out\toolchains"
$ArmDir   = Join-Path $ToolsDir $ArmZipName
$ArmGcc   = Join-Path $ArmDir "bin\arm-none-eabi-gcc.exe"

if ($SkipOptional) { $SkipUv = $true; $SkipLlvm = $true }

function Write-Header($text) {
    Write-Host ""
    Write-Host ("=" * 70) -ForegroundColor Cyan
    Write-Host "  $text" -ForegroundColor Cyan
    Write-Host ("=" * 70) -ForegroundColor Cyan
}

function Test-OnPath($name) {
    return [bool](Get-Command $name -ErrorAction SilentlyContinue)
}

function Get-RemoteFile($url, $out) {
    # BITS is dramatically faster than Invoke-WebRequest for a ~290 MB file and
    # shows real progress; fall back where it is unavailable.
    if (Get-Command Start-BitsTransfer -ErrorAction SilentlyContinue) {
        try {
            Start-BitsTransfer -Source $url -Destination $out -DisplayName "Arm GNU Toolchain" -ErrorAction Stop
            return
        } catch {
            Write-Host "  [WARN] BITS transfer failed ($_); falling back to Invoke-WebRequest" -ForegroundColor Yellow
        }
    }
    $ProgressPreference = "SilentlyContinue"
    Invoke-WebRequest -Uri $url -OutFile $out -UseBasicParsing
}

function Expand-ZipStreaming($zipPath, $destDir) {
    # Expand-Archive is pathologically slow on the ~13k-file Arm toolchain zip -
    # minutes, with no output, which looks like a hang. Stream it instead.
    $archive = [System.IO.Compression.ZipFile]::OpenRead($zipPath)
    try {
        $total = $archive.Entries.Count
        $i = 0
        $lastPct = -10
        foreach ($entry in $archive.Entries) {
            $i++
            $target = Join-Path $destDir $entry.FullName
            if ($entry.FullName.EndsWith("/")) {
                New-Item -ItemType Directory -Path $target -Force | Out-Null
                continue
            }
            $parent = Split-Path -Parent $target
            if (-not (Test-Path $parent)) { New-Item -ItemType Directory -Path $parent -Force | Out-Null }
            [System.IO.Compression.ZipFileExtensions]::ExtractToFile($entry, $target, $true)

            $pct = [int](100 * $i / $total)
            if ($pct -ge $lastPct + 10) {
                Write-Host "    $pct% ($i/$total files)" -ForegroundColor DarkGray
                $lastPct = $pct
            }
        }
    } finally {
        $archive.Dispose()
    }
}

function Install-ViaWinget($displayName, $wingetId, $probeCommand) {
    if (Test-OnPath $probeCommand) {
        Write-Host "  [SKIP] Already on PATH" -ForegroundColor DarkGray
        return $true
    }
    if (-not (Test-OnPath "winget")) {
        Write-Host "  [WARN] winget is not available on this machine." -ForegroundColor Yellow
        Write-Host "         Install $displayName manually, then re-run this script." -ForegroundColor White
        return $false
    }
    Write-Host "  Installing $displayName via winget..." -ForegroundColor White
    winget install $wingetId --silent --accept-package-agreements --accept-source-agreements
    if ($LASTEXITCODE -ne 0) {
        Write-Host "  [WARN] winget could not install $displayName (exit $LASTEXITCODE)." -ForegroundColor Yellow
        Write-Host "         This is common on managed/locked-down laptops." -ForegroundColor White
        Write-Host "         Install $displayName manually, then re-run this script." -ForegroundColor White
        return $false
    }
    Write-Host "  [OK] Installed $displayName" -ForegroundColor Green
    Write-Host "  Note: a new terminal may be needed before it appears on PATH." -ForegroundColor DarkGray
    return $true
}

Write-Header "AVC Toolchain Setup"
Write-Host "  Repository: $RepoRoot" -ForegroundColor DarkGray
Write-Host "  Tools:      $ToolsDir  (git-ignored, safe to delete and re-run)" -ForegroundColor DarkGray

$warnings = @()

# ---------------------------------------------------------------- 1. Arm GCC
Write-Host ""
Write-Host "[1/5] Arm GNU Toolchain $ArmVersion" -ForegroundColor Yellow
if ((Test-Path $ArmGcc) -and (-not $Force)) {
    $ver = & $ArmGcc -dumpversion
    Write-Host "  [SKIP] Already present (gcc $ver): $ArmDir" -ForegroundColor DarkGray
} else {
    New-Item -ItemType Directory -Path $ToolsDir -Force | Out-Null
    $zip = Join-Path $ToolsDir "$ArmZipName.zip"
    Write-Host "  Downloading (~290 MB). This is the slow step." -ForegroundColor White
    Write-Host "    $ArmUrl" -ForegroundColor DarkGray
    try {
        Get-RemoteFile $ArmUrl $zip
    } catch {
        Write-Host "  [ERROR] Download failed: $_" -ForegroundColor Red
        Write-Host "  Download it manually from:" -ForegroundColor White
        Write-Host "    https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads" -ForegroundColor White
        Write-Host "  Pick arm-none-eabi, Windows mingw-w64-i686, version $ArmVersion," -ForegroundColor White
        Write-Host "  and extract it to: $ArmDir" -ForegroundColor White
        exit 1
    }

    Write-Host "  Extracting (~13k files)..." -ForegroundColor White
    if (Test-Path $ArmDir) { Remove-Item -Recurse -Force $ArmDir }
    New-Item -ItemType Directory -Path $ArmDir -Force | Out-Null
    Expand-ZipStreaming $zip $ArmDir

    # Some Arm releases nest under a single versioned folder; flatten if so.
    if (-not (Test-Path $ArmGcc)) {
        $inner = Get-ChildItem $ArmDir -Directory -ErrorAction SilentlyContinue |
            Where-Object { Test-Path (Join-Path $_.FullName "bin\arm-none-eabi-gcc.exe") } |
            Select-Object -First 1
        if ($inner) {
            Get-ChildItem $inner.FullName -Force | Move-Item -Destination $ArmDir -Force
            Remove-Item $inner.FullName -Recurse -Force
        }
    }
    Remove-Item $zip -Force -ErrorAction SilentlyContinue

    if (-not (Test-Path $ArmGcc)) {
        Write-Host "  [ERROR] arm-none-eabi-gcc.exe not found after extraction:" -ForegroundColor Red
        Write-Host "          $ArmGcc" -ForegroundColor Red
        exit 1
    }
    $ver = & $ArmGcc -dumpversion
    Write-Host "  [OK] Installed arm-none-eabi-gcc $ver" -ForegroundColor Green
}

# ------------------------------------------------------------------ 2. CMake
Write-Host ""
Write-Host "[2/5] CMake" -ForegroundColor Yellow
if ($SkipCMake) { Write-Host "  [SKIP] -SkipCMake" -ForegroundColor DarkGray }
elseif (-not (Install-ViaWinget "CMake" "Kitware.CMake" "cmake")) { $warnings += "CMake" }

# ------------------------------------------------------------------ 3. Ninja
Write-Host ""
Write-Host "[3/5] Ninja" -ForegroundColor Yellow
if ($SkipNinja) { Write-Host "  [SKIP] -SkipNinja" -ForegroundColor DarkGray }
elseif (-not (Install-ViaWinget "Ninja" "Ninja-build.Ninja" "ninja")) { $warnings += "Ninja" }

# --------------------------------------------------------------------- 4. uv
Write-Host ""
Write-Host "[4/5] uv (Python tooling for the quality gates)" -ForegroundColor Yellow
if ($SkipUv) { Write-Host "  [SKIP] -SkipUv" -ForegroundColor DarkGray }
elseif (-not (Install-ViaWinget "uv" "astral-sh.uv" "uv")) { $warnings += "uv" }

# ------------------------------------------------------------------- 5. LLVM
Write-Host ""
Write-Host "[5/5] LLVM (clang-format and clang-tidy)" -ForegroundColor Yellow
if ($SkipLlvm) { Write-Host "  [SKIP] -SkipLlvm" -ForegroundColor DarkGray }
elseif (-not (Install-ViaWinget "LLVM" "LLVM.LLVM" "clang-format")) { $warnings += "LLVM" }

# ----------------------------------------------------------------- Summary
Write-Header "Setup Complete"
Write-Host "  Arm toolchain: $ArmDir" -ForegroundColor Green
Write-Host "  The CMake build finds it automatically - nothing was added to PATH." -ForegroundColor White

if ($warnings.Count -gt 0) {
    Write-Host ""
    Write-Host "  Not installed: $($warnings -join ', ')" -ForegroundColor Yellow
    Write-Host "  The firmware may still build if CMake and Ninja are present." -ForegroundColor White
    Write-Host "  Install the rest manually and re-run this script to confirm." -ForegroundColor White
}

Write-Host ""
Write-Host "  Next:" -ForegroundColor White
Write-Host "    cmake --preset competition           # configure" -ForegroundColor Green
Write-Host "    cmake --build --preset competition   # build" -ForegroundColor Green
Write-Host "    .\flash.ps1 -File build\cmake\competition\avc_core0.axf" -ForegroundColor Green
Write-Host ""
Write-Host "  Full instructions:    docs\setup.html" -ForegroundColor White
Write-Host "  Other build variants: cmake --list-presets" -ForegroundColor White
Write-Host ""
