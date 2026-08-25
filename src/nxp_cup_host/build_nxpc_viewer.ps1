param(
    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Release",

    [string]$ImGuiDir = "",

    [ValidateSet("Auto", "Clang", "MSVC")]
    [string]$Toolchain = "Auto",

    [switch]$ToolOnly
)

$ErrorActionPreference = "Stop"

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$repoRoot = Resolve-Path (Join-Path $scriptDir "../..")
$runtimeRoot = Join-Path $repoRoot "build/host/nxp_cup_host"

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

if ($Toolchain -eq "Auto") {
    if ($clang -and $ninja) {
        $Toolchain = "Clang"
    } elseif ($msvcAvailable) {
        $Toolchain = "MSVC"
    } else {
        throw "No supported native host compiler found. Run .\setup.ps1 to install LLVM-MinGW, or install the Visual Studio C++ desktop workload."
    }
}

if ($Toolchain -eq "Clang") {
    if (-not $clang) {
        throw "clang++ was not found. Run .\setup.ps1 to install LLVM-MinGW."
    }
    if (-not $ninja) {
        throw "Ninja was not found. Run .\setup.ps1."
    }
    $buildDir = Join-Path $repoRoot "build/host/nxp_cup_host-clang-build"
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
    $buildDir = Join-Path $repoRoot "build/host/nxp_cup_host-msvc"
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
