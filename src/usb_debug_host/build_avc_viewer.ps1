param(
    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Release",

    [string]$ImGuiDir = ""
)

$ErrorActionPreference = "Stop"

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$repoRoot = Resolve-Path (Join-Path $scriptDir "../..")
$buildDir = Join-Path $repoRoot "build/host/usb_debug_host"

$configureArgs = @(
    "-S", $scriptDir,
    "-B", $buildDir,
    "-G", "Visual Studio 17 2022",
    "-A", "x64"
)
if (![string]::IsNullOrWhiteSpace($ImGuiDir)) {
    $resolvedImGui = Resolve-Path -LiteralPath $ImGuiDir
    $configureArgs += "-DAVC_HOST_IMGUI_DIR=$resolvedImGui"
}

Write-Host "Configuring AVC native viewer..."
& cmake @configureArgs
if ($LASTEXITCODE -ne 0) {
    throw "CMake configure failed with exit code $LASTEXITCODE"
}

Write-Host "Building AVC native viewer ($Configuration)..."
& cmake --build $buildDir --config $Configuration --target avc_viewer avc_tool
if ($LASTEXITCODE -ne 0) {
    throw "CMake build failed with exit code $LASTEXITCODE"
}

$outputDir = Join-Path $buildDir $Configuration
Write-Host "Viewer: $(Join-Path $outputDir 'avc_viewer.exe')"
Write-Host "CLI:    $(Join-Path $outputDir 'avc_tool.exe')"
