param([switch]$Check)

$ErrorActionPreference = "Stop"
$sharedBuild = (Resolve-Path (Join-Path $PSScriptRoot "..\..\web\build.ps1")).Path
& $sharedBuild -Check:$Check
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
