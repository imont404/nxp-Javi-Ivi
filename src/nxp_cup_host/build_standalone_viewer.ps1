param(
    [string]$Output = "nxpc_usb_debug_viewer.html"
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $MyInvocation.MyCommand.Path
$templatePath = Join-Path $root "webserial_viewer.html"
$cssPath = Join-Path $root "webserial_viewer.css"
$scriptPath = Join-Path $root "webserial_viewer.js"
$outputPath = Join-Path $root $Output

$html = Get-Content -LiteralPath $templatePath -Raw
$css = Get-Content -LiteralPath $cssPath -Raw
$script = Get-Content -LiteralPath $scriptPath -Raw

$styleTag = "<style>`r`n$css`r`n</style>"
$scriptTag = "<script>`r`n$script`r`n</script>"
$html = $html.Replace('<link rel="stylesheet" href="./webserial_viewer.css">', $styleTag)
$html = $html.Replace('<script src="./webserial_viewer.js"></script>', $scriptTag)

if (($html -match 'webserial_viewer\.(css|js)') -or ($html -notmatch '<style>') -or ($html -notmatch '"serial" in navigator'))
{
    throw "Standalone viewer generation failed: external assets remain or required content is missing."
}

[System.IO.File]::WriteAllText($outputPath, $html, [System.Text.UTF8Encoding]::new($false))
Write-Host "Built standalone viewer: $outputPath"
