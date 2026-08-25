[CmdletBinding()]
param()

$ErrorActionPreference = "Stop"
$mockup = Join-Path $PSScriptRoot "docs\mockups\nxp_cup_telemetry_dashboard.html"
if (-not (Test-Path -LiteralPath $mockup)) {
    throw "Dashboard mockup not found: $mockup"
}

Start-Process -FilePath $mockup
Write-Host "Opened projector dashboard mockup: $mockup" -ForegroundColor Green
Write-Host "Press F in the page for fullscreen and D to toggle diagnostics." -ForegroundColor Green
