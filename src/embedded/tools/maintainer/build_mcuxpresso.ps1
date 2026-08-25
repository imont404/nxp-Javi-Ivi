param(
    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Debug",
    [switch]$Clean,
    [switch]$ResetWorkspace,
    [string]$IdePath = "C:\nxp\MCUXpressoIDE_25.6.136\ide\mcuxpressoidec.exe",
    [string]$Workspace = (Join-Path (Resolve-Path (Join-Path $PSScriptRoot "..\..\..\..")) "out\build\embedded\mcuxpresso-workspace"),
    [string]$ProjectPath = (Join-Path (Resolve-Path (Join-Path $PSScriptRoot "..\..\..\..")) "src\embedded\nxp_cup_core0")
)

$ErrorActionPreference = "Stop"

if (-not (Test-Path -LiteralPath $IdePath)) {
    throw "MCUXpresso headless executable not found: $IdePath"
}
if (-not (Test-Path -LiteralPath $ProjectPath)) {
    throw "MCUXpresso project not found: $ProjectPath"
}
if ($ResetWorkspace -and (Test-Path -LiteralPath $Workspace)) {
    Remove-Item -LiteralPath $Workspace -Recurse -Force
}

$buildVerb = if ($Clean) { "-cleanBuild" } else { "-build" }
$target = "nxp_cup_core0/$Configuration"

& $IdePath `
    -nosplash `
    --launcher.suppressErrors `
    -application org.eclipse.cdt.managedbuilder.core.headlessbuild `
    -data $Workspace `
    -import $ProjectPath `
    $buildVerb $target

exit $LASTEXITCODE
