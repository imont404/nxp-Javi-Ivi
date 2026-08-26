param(
    [Parameter(Mandatory = $true)]
    [ValidatePattern("^\d+\.\d+\.\d+(?:-[0-9A-Za-z.-]+)?$")]
    [string]$Version,

    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Release",

    [switch]$SkipBuild,

    [switch]$Force
)

$ErrorActionPreference = "Stop"

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$repoRoot = (Resolve-Path (Join-Path $scriptDir "../..")).Path
$buildScript = Join-Path $scriptDir "build.ps1"

function Get-CommandVersionLine {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Command,

        [string[]]$Arguments = @("--version")
    )

    $commandInfo = Get-Command $Command -ErrorAction Stop | Select-Object -First 1
    $output = (& $commandInfo.Source @Arguments 2>&1 | Out-String).Trim()
    if ($LASTEXITCODE -ne 0) {
        throw "$Command version check failed with exit code $LASTEXITCODE"
    }
    return ($output -split "`r?`n")[0].Trim()
}

function New-DeterministicZip {
    param(
        [Parameter(Mandatory = $true)]
        [string]$SourceDirectory,

        [Parameter(Mandatory = $true)]
        [string]$DestinationPath
    )

    Add-Type -AssemblyName System.IO.Compression
    $archiveStream = [System.IO.File]::Open(
        $DestinationPath,
        [System.IO.FileMode]::CreateNew,
        [System.IO.FileAccess]::Write,
        [System.IO.FileShare]::None
    )
    try {
        $archive = [System.IO.Compression.ZipArchive]::new(
            $archiveStream,
            [System.IO.Compression.ZipArchiveMode]::Create,
            $false
        )
        try {
            $fixedTimestamp = [DateTimeOffset]::new(
                2000, 1, 1, 0, 0, 0, [TimeSpan]::Zero
            )
            foreach ($file in Get-ChildItem -LiteralPath $SourceDirectory -File | Sort-Object Name) {
                $entry = $archive.CreateEntry(
                    $file.Name,
                    [System.IO.Compression.CompressionLevel]::Optimal
                )
                $entry.LastWriteTime = $fixedTimestamp
                $inputStream = [System.IO.File]::OpenRead($file.FullName)
                $outputStream = $entry.Open()
                try {
                    $inputStream.CopyTo($outputStream)
                } finally {
                    $outputStream.Dispose()
                    $inputStream.Dispose()
                }
            }
        } finally {
            $archive.Dispose()
        }
    } finally {
        $archiveStream.Dispose()
    }
}

if (-not $SkipBuild) {
    & $buildScript -Configuration $Configuration -Toolchain Clang
    if ($LASTEXITCODE -ne 0) {
        throw "Host build failed with exit code $LASTEXITCODE"
    }
}

$runtimeDir = Join-Path $repoRoot "out/build/host/runtime/$Configuration"
$required = @(
    "nxpc_viewer.exe",
    "nxpc_tool.exe",
    "SDL2.dll",
    "rblhost.exe",
    "rblhost-LICENSE.txt"
)
foreach ($name in $required) {
    $path = Join-Path $runtimeDir $name
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) {
        throw "Missing runtime artifact: $path"
    }
}

$rblhost = Join-Path $runtimeDir "rblhost.exe"
$rblhostVersion = (& $rblhost --version 2>&1 | Out-String).Trim()
if (($LASTEXITCODE -ne 0) -or ($rblhostVersion -ne "rblhost 0.2.0")) {
    throw "Unexpected packaged programmer version: $rblhostVersion"
}
$rblhostSha256 = (Get-FileHash -LiteralPath $rblhost -Algorithm SHA256).Hash.ToLowerInvariant()
$expectedRblhostSha256 = "6cae03c432489e0bd8a658f91e5899e7d1153b6859be02c7a76798f2885cd2a8"
if ($rblhostSha256 -ne $expectedRblhostSha256) {
    throw "Unexpected rblhost binary SHA-256: $rblhostSha256"
}

$sourceCommit = (& git -C $repoRoot rev-parse HEAD 2>&1 | Out-String).Trim()
if (($LASTEXITCODE -ne 0) -or ($sourceCommit -notmatch "^[0-9a-f]{40}$")) {
    throw "Could not resolve the source commit: $sourceCommit"
}
$sourceStatus = (& git -C $repoRoot status --porcelain=v1 --untracked-files=all | Out-String).Trim()
$sourceDirty = -not [string]::IsNullOrWhiteSpace($sourceStatus)

$distRoot = Join-Path $repoRoot "out/artifacts/host/packages"
$packageName = "nxp-cup-core-tools-win-x64-$Version"
$staging = Join-Path $distRoot $packageName
$zip = Join-Path $distRoot "$packageName.zip"
$checksum = "$zip.sha256"

foreach ($existingPath in @($staging, $zip, $checksum)) {
    if (Test-Path -LiteralPath $existingPath) {
        if (-not $Force) {
            throw "Release output already exists: $existingPath. Pass -Force to replace this exact version."
        }
        Remove-Item -LiteralPath $existingPath -Recurse -Force
    }
}
New-Item -ItemType Directory -Force -Path $staging | Out-Null

foreach ($name in $required) {
    Copy-Item -LiteralPath (Join-Path $runtimeDir $name) -Destination (Join-Path $staging $name)
}
Copy-Item -LiteralPath (Join-Path $scriptDir "PACKAGE-README.md") `
    -Destination (Join-Path $staging "HOST-README.md")
Copy-Item -LiteralPath (Join-Path $scriptDir "../common/egfx/test/sdl/sdl2/COPYING.txt") `
    -Destination (Join-Path $staging "SDL2-LICENSE.txt")
Copy-Item -LiteralPath (Join-Path $scriptDir "vendor/imgui/LICENSE.txt") `
    -Destination (Join-Path $staging "Dear-ImGui-LICENSE.txt")

$manifestFiles = @(
    Get-ChildItem -LiteralPath $staging -File |
        Sort-Object Name |
        ForEach-Object {
            [ordered]@{
                name = $_.Name
                size = $_.Length
                sha256 = (Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
            }
        }
)
$manifest = [ordered]@{
    schemaVersion = 1
    releaseVersion = $Version
    sourceCommit = $sourceCommit
    sourceDirty = $sourceDirty
    platform = "windows"
    architecture = "x64"
    configuration = $Configuration
    tools = [ordered]@{
        cmake = Get-CommandVersionLine -Command "cmake"
        ninja = Get-CommandVersionLine -Command "ninja"
        compiler = Get-CommandVersionLine -Command "clang++"
        programmer = [ordered]@{
            name = "rblhost"
            version = "0.2.0"
            upstream = "https://github.com/nxp-mcuxpresso/rblhost"
            sourceCommit = "7a775dde2c44bd345a1ac067698afa999bd71be0"
            binarySha256 = $rblhostSha256
            toolchain = "Rust/Cargo 1.96.1; upstream lockfile; release profile"
        }
    }
    files = $manifestFiles
}
$manifestJson = ($manifest | ConvertTo-Json -Depth 8) + "`n"
$utf8NoBom = [System.Text.UTF8Encoding]::new($false)
[System.IO.File]::WriteAllText(
    (Join-Path $staging "manifest.json"),
    $manifestJson,
    $utf8NoBom
)

New-DeterministicZip -SourceDirectory $staging -DestinationPath $zip
$zipHash = (Get-FileHash -LiteralPath $zip -Algorithm SHA256).Hash.ToLowerInvariant()
[System.IO.File]::WriteAllText(
    $checksum,
    "$zipHash  $packageName.zip`n",
    $utf8NoBom
)

Write-Host "Portable runtime: $staging"
Write-Host "Portable zip:     $zip"
Write-Host "SHA-256:          $zipHash"

[pscustomobject]@{
    PackageName = $packageName
    StagingPath = $staging
    ZipPath = $zip
    ChecksumPath = $checksum
    Sha256 = $zipHash
}
