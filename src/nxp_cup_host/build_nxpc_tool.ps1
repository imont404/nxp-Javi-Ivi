param(
    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Release"
)

$ErrorActionPreference = "Stop"

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$outDir = Join-Path $scriptDir "bin"
$exe = Join-Path $outDir "nxpc_tool.exe"
$pdb = Join-Path $outDir "nxpc_tool.pdb"
$coreObject = Join-Path $outDir "nxpc_host_core.obj"
$programmerObject = Join-Path $outDir "nxpc_programmer.obj"
$toolObject = Join-Path $outDir "nxpc_tool.obj"
$coreSource = Join-Path $scriptDir "nxpc_host_core.cpp"
$programmerSource = Join-Path $scriptDir "nxpc_programmer.cpp"
$toolSource = Join-Path $scriptDir "nxpc_tool.cpp"

foreach ($source in @($coreSource, $programmerSource, $toolSource)) {
    if (!(Test-Path -LiteralPath $source)) {
        throw "Missing source: $source"
    }
}

New-Item -ItemType Directory -Force -Path $outDir | Out-Null

$vswhere = Join-Path ${env:ProgramFiles(x86)} "Microsoft Visual Studio\Installer\vswhere.exe"
if (!(Test-Path -LiteralPath $vswhere)) {
    throw "vswhere.exe not found. Install Visual Studio Build Tools with the C++ desktop workload."
}

$installPath = & $vswhere -latest -products * `
    -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 `
    -property installationPath
if ([string]::IsNullOrWhiteSpace($installPath)) {
    throw "Visual Studio C++ tools were not found. Install the C++ desktop workload."
}

$vcvars = Join-Path $installPath "VC\Auxiliary\Build\vcvars64.bat"
if (!(Test-Path -LiteralPath $vcvars)) {
    throw "vcvars64.bat not found under Visual Studio installation: $installPath"
}

$optFlags = if ($Configuration -eq "Debug") { "/Od /Zi" } else { "/O2" }
$compileFlags = "/nologo /c /std:c++17 /EHsc /permissive- /W4 /WX $optFlags"
$linkFlags = if ($Configuration -eq "Debug") { "/DEBUG /PDB:`"$pdb`"" } else { "" }
$command = "`"$vcvars`" >nul " +
    "&& cl $compileFlags /Fo:`"$coreObject`" `"$coreSource`" " +
    "&& cl $compileFlags /Fo:`"$programmerObject`" `"$programmerSource`" " +
    "&& cl $compileFlags /Fo:`"$toolObject`" `"$toolSource`" " +
    "&& link /nologo /OUT:`"$exe`" $linkFlags `"$coreObject`" `"$programmerObject`" `"$toolObject`" setupapi.lib advapi32.lib hid.lib bcrypt.lib"

Write-Host "Building NXP Cup native host tool ($Configuration)..."
Write-Host "Output: $exe"
& cmd.exe /d /s /c $command
if ($LASTEXITCODE -ne 0) {
    throw "cl.exe failed with exit code $LASTEXITCODE"
}

Write-Host "Built: $exe"
