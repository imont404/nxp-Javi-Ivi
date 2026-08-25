param(
    [switch]$Clean
)

$ErrorActionPreference = "Stop"
$preset = "competition"
$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path

$configureArguments = @("--preset", $preset)
if ($Clean) {
    $configureArguments += "--fresh"
}

Push-Location $PSScriptRoot
try {
    Write-Host "Configuring NXP Cup competition firmware..." -ForegroundColor Cyan
    & cmake @configureArguments
    if ($LASTEXITCODE -ne 0) {
        throw "CMake configure failed with exit code $LASTEXITCODE"
    }

    Write-Host "Building NXP Cup competition firmware..." -ForegroundColor Cyan
    $buildArguments = @("--build", "--preset", $preset)
    if ($Clean) {
        $buildArguments += "--clean-first"
    }
    & cmake @buildArguments
    if ($LASTEXITCODE -ne 0) {
        throw "CMake build failed with exit code $LASTEXITCODE"
    }
} finally {
    Pop-Location
}

$compileCommands = Join-Path $repoRoot "out\build\embedded\competition\compile_commands.json"
$flagCheck = Join-Path $PSScriptRoot "tools\maintainer\check_compile_flags.ps1"
& $flagCheck -CompileCommands $compileCommands -ExpectedOptimization "-O2"

$output = Join-Path $repoRoot "out\build\embedded\competition\nxp_cup_core0.axf"
if (-not (Test-Path -LiteralPath $output)) {
    throw "Build completed but the expected image is missing: $output"
}

$binary = Join-Path $repoRoot "out\build\embedded\competition\nxp_cup_core0.bin"
if (-not (Test-Path -LiteralPath $binary)) {
    throw "Build completed but the expected binary is missing: $binary"
}

$publishDir = Join-Path $repoRoot "out\artifacts\embedded"
New-Item -ItemType Directory -Force -Path $publishDir | Out-Null
Copy-Item -LiteralPath $output -Destination (Join-Path $publishDir "nxp_cup_core0.axf") -Force
Copy-Item -LiteralPath $binary -Destination (Join-Path $publishDir "nxp_cup_core0.bin") -Force

Write-Host "Built: $output" -ForegroundColor Green
Write-Host "Firmware bin: $publishDir" -ForegroundColor Green
