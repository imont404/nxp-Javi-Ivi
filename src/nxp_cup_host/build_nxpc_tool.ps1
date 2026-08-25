param(
    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Release",

    [ValidateSet("Auto", "Clang", "MSVC")]
    [string]$Toolchain = "Auto"
)

$ErrorActionPreference = "Stop"

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$buildScript = Join-Path $scriptDir "build_nxpc_viewer.ps1"
& $buildScript -Configuration $Configuration -Toolchain $Toolchain -ToolOnly
