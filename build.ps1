param(
    [string]$Preset = "competition",
    [switch]$Clean
)

$ErrorActionPreference = "Stop"

$presetData = Get-Content -LiteralPath (Join-Path $PSScriptRoot "CMakePresets.json") -Raw |
    ConvertFrom-Json
$knownPresets = @(
    $presetData.configurePresets |
        Where-Object { -not ($_.PSObject.Properties.Name -contains "hidden" -and $_.hidden) } |
        ForEach-Object { $_.name }
)

if ($knownPresets -notcontains $Preset) {
    throw "Unknown preset '$Preset'. Available: $($knownPresets -join ', ')"
}

$configureArguments = @("--preset", $Preset)
if ($Clean) {
    $configureArguments += "--fresh"
}

Write-Host "Configuring NXP Cup firmware preset '$Preset'..." -ForegroundColor Cyan
& cmake @configureArguments
if ($LASTEXITCODE -ne 0) {
    throw "CMake configure failed with exit code $LASTEXITCODE"
}

Write-Host "Building NXP Cup firmware preset '$Preset'..." -ForegroundColor Cyan
& cmake --build --preset $Preset
if ($LASTEXITCODE -ne 0) {
    throw "CMake build failed with exit code $LASTEXITCODE"
}

$output = Join-Path $PSScriptRoot "build\cmake\$Preset\nxp_cup_core0.axf"
if (-not (Test-Path -LiteralPath $output)) {
    throw "Build completed but the expected image is missing: $output"
}

Write-Host "Built: $output" -ForegroundColor Green
