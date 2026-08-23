param(
    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Release"
)

$ErrorActionPreference = "Stop"

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$repoRoot = Resolve-Path (Join-Path $scriptDir "../..")
$buildScript = Join-Path $scriptDir "build_avc_viewer.ps1"

& $buildScript -Configuration $Configuration
if ($LASTEXITCODE -ne 0) {
    throw "Host build failed with exit code $LASTEXITCODE"
}

$runtimeDir = Join-Path $repoRoot "build/host/usb_debug_host/$Configuration"
$required = @(
    "avc_viewer.exe",
    "avc_tool.exe",
    "SDL2.dll",
    "rblhost.exe",
    "rblhost-LICENSE.txt"
)
foreach ($name in $required) {
    $path = Join-Path $runtimeDir $name
    if (!(Test-Path -LiteralPath $path -PathType Leaf)) {
        throw "Missing runtime artifact: $path"
    }
}

$rblhost = Join-Path $runtimeDir "rblhost.exe"
$version = (& $rblhost --version 2>&1 | Out-String).Trim()
if (($LASTEXITCODE -ne 0) -or ($version -ne "rblhost 0.2.0")) {
    throw "Unexpected packaged programmer version: $version"
}

$stamp = Get-Date -Format "yyyyMMdd-HHmmss"
$distRoot = Join-Path $repoRoot "build/dist"
$packageName = "avc-host-one-cable-$stamp"
$staging = Join-Path $distRoot $packageName
New-Item -ItemType Directory -Force -Path $staging | Out-Null

foreach ($name in $required) {
    Copy-Item -LiteralPath (Join-Path $runtimeDir $name) -Destination (Join-Path $staging $name)
}
Copy-Item -LiteralPath (Join-Path $scriptDir "README.md") `
    -Destination (Join-Path $staging "HOST-README.md")
Copy-Item -LiteralPath (Join-Path $scriptDir "../common/egfx/test/sdl/sdl2/COPYING.txt") `
    -Destination (Join-Path $staging "SDL2-LICENSE.txt")
Copy-Item -LiteralPath (Join-Path $scriptDir "vendor/imgui/LICENSE.txt") `
    -Destination (Join-Path $staging "Dear-ImGui-LICENSE.txt")

$manifest = @(
    "Wavenumber AVC one-cable host runtime"
    "Built: $(Get-Date -Format o)"
    "Configuration: $Configuration"
    "Programmer: $version"
    ""
    "SHA-256:"
)
foreach ($file in Get-ChildItem -LiteralPath $staging -File | Sort-Object Name) {
    $hash = (Get-FileHash -LiteralPath $file.FullName -Algorithm SHA256).Hash
    $manifest += "$hash  $($file.Name)"
}
$manifest | Set-Content -LiteralPath (Join-Path $staging "MANIFEST.txt") -Encoding utf8

$zip = Join-Path $distRoot "$packageName.zip"
Compress-Archive -Path (Join-Path $staging "*") -DestinationPath $zip

Write-Host "Portable runtime: $staging"
Write-Host "Portable zip:     $zip"
