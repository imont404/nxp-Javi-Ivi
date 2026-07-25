# Shared image and toolchain resolution for flash.ps1 and rtt.ps1.
#
# The CMake preset flow is the default. A preset name is the normal way to say
# which image you mean:
#
#     .\flash.ps1                       # the competition preset
#     .\flash.ps1 -Preset flexio-port1
#     .\rtt.ps1   -Preset encoder-diag
#
# -File still takes an explicit path and wins over everything, and -Mcux selects
# the MCUXpresso output directory for the rare case of comparing the two.

Set-StrictMode -Version Latest

function Get-AvcPresetNames {
    <#
    .SYNOPSIS
    Visible configure preset names, read from CMakePresets.json.
    #>
    param([Parameter(Mandatory)][string]$RepoRoot)

    $presetFile = Join-Path $RepoRoot "CMakePresets.json"
    if (-not (Test-Path -LiteralPath $presetFile)) {
        return @()
    }

    try {
        $data = Get-Content -LiteralPath $presetFile -Raw | ConvertFrom-Json
    } catch {
        return @()
    }

    return @(
        $data.configurePresets |
            Where-Object { -not ($_.PSObject.Properties.Name -contains "hidden" -and $_.hidden) } |
            ForEach-Object { $_.name }
    )
}

function Resolve-AvcImage {
    <#
    .SYNOPSIS
    Work out which .axf to flash or read symbols from.

    .DESCRIPTION
    Resolution order:
      1. -File, an explicit path
      2. -Mcux, the MCUXpresso output directory (legacy)
      3. the named preset's build directory (the default)

    Throws with the command to run when the image is not there, rather than
    letting a stale binary from an earlier build get flashed by accident.
    #>
    param(
        [Parameter(Mandatory)][string]$RepoRoot,
        [string]$Preset = "competition",
        [string]$File,
        [switch]$Mcux,
        [string]$Configuration = "Debug"
    )

    if ($File) {
        if (-not (Test-Path -LiteralPath $File)) {
            throw "Firmware not found: $File"
        }
        return (Resolve-Path -LiteralPath $File).Path
    }

    if ($Mcux) {
        $axf = Join-Path $RepoRoot "src\avc\avc_core0\$Configuration\avc_core0.axf"
        if (-not (Test-Path -LiteralPath $axf)) {
            throw @"
Firmware not found: $axf

That is the MCUXpresso output directory, selected by -Mcux. The default build
flow is CMake:

    cmake --preset $Preset
    cmake --build --preset $Preset
"@
        }
        return $axf
    }

    $known = Get-AvcPresetNames -RepoRoot $RepoRoot
    if ($known.Count -gt 0 -and $known -notcontains $Preset) {
        throw "Unknown preset '$Preset'. Available: $($known -join ', ')"
    }

    $axf = Join-Path $RepoRoot "build\cmake\$Preset\avc_core0.axf"
    if (-not (Test-Path -LiteralPath $axf)) {
        throw @"
Firmware not found: $axf

Build the '$Preset' preset first:

    cmake --preset $Preset
    cmake --build --preset $Preset
"@
    }

    return $axf
}

function Resolve-AvcArmTool {
    <#
    .SYNOPSIS
    Find an arm-none-eabi-* tool, preferring the toolchain setup.ps1 installed.

    .DESCRIPTION
    Mirrors the discovery order in src/avc/avc_core0/cmake/mcuxpresso-toolchain.cmake
    so the scripts and the build agree about which toolchain is in use:
      1. AVC_ARM_TOOLCHAIN_DIR
      2. out/toolchains/arm-gnu-toolchain-*-arm-none-eabi/bin
      3. PATH
      4. the caller's configured path, typically inside MCUXpresso
    #>
    param(
        [Parameter(Mandatory)][string]$RepoRoot,
        [Parameter(Mandatory)][string]$ToolName,
        [string]$ConfiguredPath
    )

    $exe = if ($ToolName -like "*.exe") { $ToolName } else { "$ToolName.exe" }

    if ($env:AVC_ARM_TOOLCHAIN_DIR) {
        $candidate = Join-Path $env:AVC_ARM_TOOLCHAIN_DIR $exe
        if (Test-Path -LiteralPath $candidate) { return $candidate }
    }

    $localRoot = Join-Path $RepoRoot "out\toolchains"
    if (Test-Path -LiteralPath $localRoot) {
        $candidate = Get-ChildItem -LiteralPath $localRoot -Directory -ErrorAction SilentlyContinue |
            Where-Object { $_.Name -like "arm-gnu-toolchain-*-arm-none-eabi" } |
            Sort-Object Name -Descending |
            ForEach-Object { Join-Path $_.FullName "bin\$exe" } |
            Where-Object { Test-Path -LiteralPath $_ } |
            Select-Object -First 1
        if ($candidate) { return $candidate }
    }

    $onPath = Get-Command $ToolName -ErrorAction SilentlyContinue
    if ($onPath) { return $onPath.Source }

    if ($ConfiguredPath -and (Test-Path -LiteralPath $ConfiguredPath)) {
        return $ConfiguredPath
    }

    throw @"
$ToolName not found.

Run .\setup.ps1 to provision the Arm GNU toolchain, or set
AVC_ARM_TOOLCHAIN_DIR to a toolchain bin directory.
"@
}
