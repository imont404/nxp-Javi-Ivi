param(
    [string]$Configuration = "Release"
)

$ErrorActionPreference = "Stop"

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$source = Join-Path $scriptDir "usb_cdc_stream_read.cpp"
$outDir = Join-Path $scriptDir "bin"
$exe = Join-Path $outDir "usb_cdc_stream_read.exe"
$obj = Join-Path $outDir "usb_cdc_stream_read.obj"
$pdb = Join-Path $outDir "usb_cdc_stream_read.pdb"

if (!(Test-Path $source)) {
    throw "Missing source: $source"
}

New-Item -ItemType Directory -Force -Path $outDir | Out-Null

$vswhere = Join-Path ${env:ProgramFiles(x86)} "Microsoft Visual Studio\Installer\vswhere.exe"
if (!(Test-Path $vswhere)) {
    throw "vswhere.exe not found. Install Visual Studio Build Tools with the C++ desktop workload, or add cl.exe to PATH."
}

$installPath = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if ([string]::IsNullOrWhiteSpace($installPath)) {
    throw "Visual Studio C++ tools were not found. Install Visual Studio Build Tools with the C++ desktop workload."
}

$vcvars = Join-Path $installPath "VC\Auxiliary\Build\vcvars64.bat"
if (!(Test-Path $vcvars)) {
    throw "vcvars64.bat not found under Visual Studio installation: $installPath"
}

$optFlags = if ($Configuration -eq "Debug") { "/Od /Zi" } else { "/O2" }
$command = "`"$vcvars`" >nul && cl /nologo /std:c++17 /EHsc /W4 $optFlags /Fe:`"$exe`" /Fo:`"$obj`" /Fd:`"$pdb`" `"$source`""

Write-Host "Building native USB CDC receiver..."
Write-Host "Source: $source"
Write-Host "Output: $exe"

& cmd.exe /d /s /c $command
if ($LASTEXITCODE -ne 0) {
    throw "cl.exe failed with exit code $LASTEXITCODE"
}

Write-Host "Built: $exe"
