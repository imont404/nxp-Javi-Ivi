param(
    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Release",

    [ValidateSet("Auto", "Clang", "MSVC")]
    [string]$Toolchain = "Auto"
)

$ErrorActionPreference = "Stop"

$buildScript = Join-Path $PSScriptRoot "src\nxp_cup_host\build_nxpc_viewer.ps1"
& $buildScript -Configuration $Configuration -Toolchain $Toolchain
if ($LASTEXITCODE -ne 0) {
    throw "Native viewer build failed with exit code $LASTEXITCODE"
}

$sourceDir = Join-Path $PSScriptRoot "build\host\nxp_cup_host\$Configuration"
$publishDir = Join-Path $PSScriptRoot "bin\host"
$runtimeFiles = @(
    "nxpc_viewer.exe",
    "nxpc_tool.exe",
    "SDL2.dll",
    "rblhost.exe",
    "rblhost-LICENSE.txt"
)

New-Item -ItemType Directory -Force -Path $publishDir | Out-Null
foreach ($name in $runtimeFiles) {
    $source = Join-Path $sourceDir $name
    if (-not (Test-Path -LiteralPath $source -PathType Leaf)) {
        throw "Native viewer build completed but a runtime file is missing: $source"
    }
    Copy-Item -LiteralPath $source -Destination (Join-Path $publishDir $name) -Force
}

Write-Host "Viewer bin: $(Join-Path $publishDir 'nxpc_viewer.exe')" -ForegroundColor Green
Write-Host "CLI bin:    $(Join-Path $publishDir 'nxpc_tool.exe')" -ForegroundColor Green
