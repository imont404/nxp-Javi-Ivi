param(
    [int]$Port = 8765
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $MyInvocation.MyCommand.Path
$url = "http://127.0.0.1:$Port/webserial_viewer.html"

Write-Host "Serving $root"
Write-Host "Open $url"
python -m http.server $Port --bind 127.0.0.1 --directory $root
