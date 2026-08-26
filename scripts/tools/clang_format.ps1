[CmdletBinding()]
param(
    [switch]$Check
)

$ErrorActionPreference = "Stop"
$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
$clangFormat = Get-Command "clang-format" -ErrorAction SilentlyContinue

if (-not $clangFormat) {
    throw "clang-format is unavailable. Run .\setup.ps1 from the repository root."
}

$ownedFiles = @()
$ownedFiles += Get-ChildItem (Join-Path $repoRoot "src\embedded\nxp_cup_core0\source\app\*") `
    -File -Include *.c, *.h
$ownedFiles += Get-ChildItem (Join-Path $repoRoot "src\embedded\nxp_cup_core0\source\*") `
    -File -Include main.c, nxp_cup.h, nxpc_*.c, nxpc_*.h
$ownedFiles += Get-ChildItem (Join-Path $repoRoot "src\embedded\nxp_cup_core0\source\nxpc_io\*") `
    -File -Include *.c, *.h | Where-Object Name -ne "semihost_hardfault.c"
$ownedFiles += Get-ChildItem (Join-Path $repoRoot "src\embedded\nxp_cup_core0\source\nxpc_config\*") `
    -File -Include *.h
$ownedFiles += Get-Item `
    (Join-Path $repoRoot "src\embedded\nxp_cup_core0\source\shared\nxpc__sbrk.c"), `
    (Join-Path $repoRoot "src\embedded\nxp_cup_core0\source\shared\eGFX.h")
$ownedFiles += Get-ChildItem (Join-Path $repoRoot "src\host\*") `
    -File -Include *.cpp, *.hpp
$ownedFiles += Get-ChildItem (Join-Path $repoRoot "src\common\ipc\*") `
    -File -Include *.c, *.h
$ownedFiles += Get-ChildItem (Join-Path $repoRoot "src\common\nxpc_usb_debug\*") `
    -File -Include *.c, *.h

$paths = @(
    $ownedFiles |
        Sort-Object FullName -Unique |
        ForEach-Object { $_.FullName }
)

if ($paths.Count -eq 0) {
    throw "No repository-owned C/C++ files were found."
}

$arguments = @()
if ($Check) {
    $arguments += "--dry-run"
    $arguments += "--Werror"
} else {
    $arguments += "-i"
}
$arguments += $paths

& $clangFormat.Source @arguments
if ($LASTEXITCODE -ne 0) {
    if ($Check) {
        throw "clang-format found repository-owned files that need formatting."
    }
    throw "clang-format failed with exit code $LASTEXITCODE."
}

$action = if ($Check) { "Verified" } else { "Formatted" }
Write-Host "$action $($paths.Count) repository-owned C/C++ files." -ForegroundColor Green
