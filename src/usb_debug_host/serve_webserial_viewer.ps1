param(
    [int]$Port = 8765
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $MyInvocation.MyCommand.Path
$viewer = "avc_usb_debug_viewer.html"
$url = "http://127.0.0.1:$Port/$viewer"

& (Join-Path $root "build_standalone_viewer.ps1") -Output $viewer

Write-Host "Serving $root"
Write-Host "Open $url"
python -m http.server $Port --bind 127.0.0.1 --directory $root
