param(
    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Release",

    [string]$ImGuiDir = "",

    [ValidateSet("Clang", "MSVC")]
    [string]$Toolchain = "Clang",

    [switch]$ToolOnly
)

$ErrorActionPreference = "Stop"

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$repoRoot = Resolve-Path (Join-Path $scriptDir "../..")
$runtimeRoot = Join-Path $repoRoot "out/build/host/runtime"

$clang = Get-Command "clang++" -ErrorAction SilentlyContinue | Select-Object -First 1
$ninja = Get-Command "ninja" -ErrorAction SilentlyContinue | Select-Object -First 1
$vswhere = Join-Path ${env:ProgramFiles(x86)} "Microsoft Visual Studio/Installer/vswhere.exe"
$msvcAvailable = $false
if (Test-Path -LiteralPath $vswhere) {
    $msvcPath = & $vswhere -latest -products * `
        -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 `
        -property installationPath
    $msvcAvailable = -not [string]::IsNullOrWhiteSpace($msvcPath)
}

if ($Toolchain -eq "Clang") {
    if (-not $clang) {
        throw "clang++ was not found. Run .\setup.ps1 to install LLVM-MinGW."
    }
    if (-not $ninja) {
        throw "Ninja was not found. Run .\setup.ps1."
    }
    $buildDir = Join-Path $repoRoot "out/build/host/cmake-clang"
    $configureArgs = @(
        "-S", $scriptDir,
        "-B", $buildDir,
        "-G", "Ninja Multi-Config",
        "-DCMAKE_CXX_COMPILER=$($clang.Source)",
        "-DCMAKE_RUNTIME_OUTPUT_DIRECTORY=$runtimeRoot"
    )
} else {
    if (-not $msvcAvailable) {
        throw "Visual Studio C++ tools were not found. Use -Toolchain Clang after running .\setup.ps1, or install the C++ desktop workload."
    }
    $buildDir = Join-Path $repoRoot "out/build/host/cmake-msvc"
    $configureArgs = @(
        "-S", $scriptDir,
        "-B", $buildDir,
        "-G", "Visual Studio 17 2022",
        "-A", "x64",
        "-DCMAKE_RUNTIME_OUTPUT_DIRECTORY=$runtimeRoot"
    )
}
if ([string]::IsNullOrWhiteSpace($ImGuiDir)) {
    $ImGuiDir = Join-Path $scriptDir "vendor/imgui"
}
$resolvedImGui = Resolve-Path -LiteralPath $ImGuiDir
$configureArgs += "-DNXPC_HOST_IMGUI_DIR=$resolvedImGui"

Write-Host "Configuring NXP Cup native host ($Toolchain)..."
& cmake @configureArgs
if ($LASTEXITCODE -ne 0) {
    throw "CMake configure failed with exit code $LASTEXITCODE"
}

$targets = @(if ($ToolOnly) { "nxpc_tool" } else { "nxpc_viewer"; "nxpc_tool" })
Write-Host "Building NXP Cup native host ($Configuration, $Toolchain)..."
& cmake --build $buildDir --config $Configuration --target @targets
if ($LASTEXITCODE -ne 0) {
    throw "CMake build failed with exit code $LASTEXITCODE"
}

$outputDir = Join-Path $runtimeRoot $Configuration
if (-not $ToolOnly) {
    Write-Host "Viewer: $(Join-Path $outputDir 'nxpc_viewer.exe')"
}
Write-Host "CLI:    $(Join-Path $outputDir 'nxpc_tool.exe')"

$publishDir = Join-Path $repoRoot "out/artifacts/host"
$runtimeFiles = if ($ToolOnly) {
    @("nxpc_tool.exe", "rblhost.exe", "rblhost-LICENSE.txt")
} else {
    @("nxpc_viewer.exe", "nxpc_tool.exe", "SDL2.dll", "rblhost.exe", "rblhost-LICENSE.txt")
}
New-Item -ItemType Directory -Force -Path $publishDir | Out-Null
foreach ($name in $runtimeFiles) {
    $source = Join-Path $outputDir $name
    if (-not (Test-Path -LiteralPath $source -PathType Leaf)) {
        throw "Native host build completed but a runtime file is missing: $source"
    }
    Copy-Item -LiteralPath $source -Destination (Join-Path $publishDir $name) -Force
}
Write-Host "Artifacts: $publishDir" -ForegroundColor Green
