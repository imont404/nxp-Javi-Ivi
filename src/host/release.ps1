param(
    [Parameter(Mandatory = $true)]
    [ValidatePattern("^\d+\.\d+\.\d+(?:-[0-9A-Za-z.-]+)?$")]
    [string]$Version,

    [string]$Repository = "wavenumber-eng/nxp_cup",

    [switch]$Publish,

    [switch]$AllowDirty,

    [switch]$Force
)

$ErrorActionPreference = "Stop"

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$repoRoot = (Resolve-Path (Join-Path $scriptDir "../..")).Path
$packageScript = Join-Path $scriptDir "package.ps1"
$releaseTag = "core-tools-v$Version"
$packageName = "nxp-cup-core-tools-win-x64-$Version"

function Invoke-NativeChecked {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Command,

        [Parameter(Mandatory = $true)]
        [string[]]$Arguments,

        [Parameter(Mandatory = $true)]
        [string]$FailureMessage
    )

    & $Command @Arguments
    if ($LASTEXITCODE -ne 0) {
        throw "$FailureMessage (exit code $LASTEXITCODE)"
    }
}

function Test-CoreToolsArchive {
    param(
        [Parameter(Mandatory = $true)]
        [string]$ZipPath,

        [Parameter(Mandatory = $true)]
        [string]$ExpectedVersion,

        [Parameter(Mandatory = $true)]
        [string]$ValidationRoot
    )

    $extractPath = Join-Path $ValidationRoot "archive"
    if (Test-Path -LiteralPath $extractPath) {
        Remove-Item -LiteralPath $extractPath -Recurse -Force
    }
    New-Item -ItemType Directory -Force -Path $extractPath | Out-Null
    Expand-Archive -LiteralPath $ZipPath -DestinationPath $extractPath

    $manifestPath = Join-Path $extractPath "manifest.json"
    if (-not (Test-Path -LiteralPath $manifestPath -PathType Leaf)) {
        throw "Package validation failed: manifest.json is missing"
    }
    $manifest = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
    if ($manifest.releaseVersion -ne $ExpectedVersion) {
        throw "Package validation failed: expected version $ExpectedVersion, found $($manifest.releaseVersion)"
    }
    if (($manifest.platform -ne "windows") -or ($manifest.architecture -ne "x64")) {
        throw "Package validation failed: expected Windows x64 metadata"
    }

    $expectedNames = @(
        "Dear-ImGui-LICENSE.txt",
        "nxpc_tool.exe",
        "nxpc_viewer.exe",
        "rblhost.exe",
        "rblhost-LICENSE.txt",
        "HOST-README.md",
        "SDL2.dll",
        "SDL2-LICENSE.txt"
    )
    $manifestNames = @($manifest.files | ForEach-Object { $_.name } | Sort-Object)
    if (($manifestNames -join "`n") -ne (($expectedNames | Sort-Object) -join "`n")) {
        throw "Package validation failed: manifest payload does not match the release contract"
    }

    foreach ($entry in $manifest.files) {
        $payloadPath = Join-Path $extractPath $entry.name
        if (-not (Test-Path -LiteralPath $payloadPath -PathType Leaf)) {
            throw "Package validation failed: $($entry.name) is missing"
        }
        $file = Get-Item -LiteralPath $payloadPath
        $hash = (Get-FileHash -LiteralPath $payloadPath -Algorithm SHA256).Hash.ToLowerInvariant()
        if (($file.Length -ne $entry.size) -or ($hash -ne $entry.sha256)) {
            throw "Package validation failed: $($entry.name) does not match manifest.json"
        }
    }

    $unexpected = @(
        Get-ChildItem -LiteralPath $extractPath -File |
            Where-Object { $_.Name -ne "manifest.json" -and $_.Name -notin $expectedNames }
    )
    if ($unexpected.Count -gt 0) {
        throw "Package validation failed: unexpected file $($unexpected[0].Name)"
    }

    $tool = Join-Path $extractPath "nxpc_tool.exe"
    Invoke-NativeChecked -Command $tool -Arguments @("selftest") `
        -FailureMessage "Packaged nxpc_tool self-test failed"
}

if (-not [Environment]::Is64BitOperatingSystem) {
    throw "Core-tools releases must be built on 64-bit Windows."
}
if ($Publish -and $AllowDirty) {
    throw "-AllowDirty is available only for local dry runs; publishing requires a clean source tree."
}

foreach ($command in @("git", "cmake", "ninja", "clang++", "npm")) {
    if (-not (Get-Command $command -ErrorAction SilentlyContinue)) {
        throw "Required release command was not found: $command"
    }
}

$sourceCommit = (& git -C $repoRoot rev-parse HEAD 2>&1 | Out-String).Trim()
if (($LASTEXITCODE -ne 0) -or ($sourceCommit -notmatch "^[0-9a-f]{40}$")) {
    throw "Could not resolve the source commit: $sourceCommit"
}
$status = (& git -C $repoRoot status --porcelain=v1 --untracked-files=all | Out-String).Trim()
if ((-not [string]::IsNullOrWhiteSpace($status)) -and (-not $AllowDirty)) {
    throw "The release source tree is not clean. Commit or remove local changes before releasing."
}
if (-not [string]::IsNullOrWhiteSpace($status)) {
    Write-Warning "Local dry run is using a dirty source tree because -AllowDirty was explicit."
}

Write-Host "Building and testing core tools from $sourceCommit..." -ForegroundColor Cyan
& (Join-Path $scriptDir "build.ps1") -Configuration Release -Toolchain Clang
if ($LASTEXITCODE -ne 0) {
    throw "Host build failed with exit code $LASTEXITCODE"
}

$builtTool = Join-Path $repoRoot "out/build/host/runtime/Release/nxpc_tool.exe"
Invoke-NativeChecked -Command $builtTool -Arguments @("selftest") `
    -FailureMessage "Built nxpc_tool self-test failed"
Invoke-NativeChecked -Command "npm" -Arguments @("ci", "--prefix", $scriptDir) `
    -FailureMessage "Could not install the pinned host browser test dependencies"
Invoke-NativeChecked -Command "npm" -Arguments @("test", "--prefix", $scriptDir) `
    -FailureMessage "Host browser tests failed"
Invoke-NativeChecked -Command "git" `
    -Arguments @("-C", $repoRoot, "diff", "--exit-code", "--", "src/host/nxpc_usb_debug_viewer.html") `
    -FailureMessage "Browser build changed the checked-in standalone viewer"

if ($Publish) {
    $postTestStatus = (& git -C $repoRoot status --porcelain=v1 --untracked-files=all | Out-String).Trim()
    if (-not [string]::IsNullOrWhiteSpace($postTestStatus)) {
        throw "Build or test execution left the source tree dirty; refusing to publish."
    }
}

$packageArguments = @{
    Version = $Version
    Configuration = "Release"
    SkipBuild = $true
    Force = $Force
}
$package = & $packageScript @packageArguments
$zipHash = (Get-FileHash -LiteralPath $package.ZipPath -Algorithm SHA256).Hash.ToLowerInvariant()
$checksumLine = (Get-Content -LiteralPath $package.ChecksumPath -Raw).Trim()
if ($checksumLine -ne "$zipHash  $packageName.zip") {
    throw "Outer SHA-256 file does not match the package archive."
}

$validationRoot = Join-Path $repoRoot "out/validation/host-release/$packageName"
if (Test-Path -LiteralPath $validationRoot) {
    Remove-Item -LiteralPath $validationRoot -Recurse -Force
}
New-Item -ItemType Directory -Force -Path $validationRoot | Out-Null
Test-CoreToolsArchive -ZipPath $package.ZipPath -ExpectedVersion $Version `
    -ValidationRoot $validationRoot

$packageManifest = Get-Content -LiteralPath (Join-Path $package.StagingPath "manifest.json") -Raw |
    ConvertFrom-Json
if ($Publish -and ($packageManifest.sourceDirty -or ($packageManifest.sourceCommit -ne $sourceCommit))) {
    throw "Package provenance is not the clean release commit; refusing to publish."
}

if (-not $Publish) {
    Write-Host ""
    Write-Host "Dry run complete; nothing was uploaded." -ForegroundColor Green
    Write-Host "Release tag: $releaseTag"
    Write-Host "Archive:     $($package.ZipPath)"
    Write-Host "Checksum:    $($package.ChecksumPath)"
    Write-Host "Run again from a clean release commit with -Publish to create the GitHub release."
    exit 0
}

if (-not (Get-Command "gh" -ErrorAction SilentlyContinue)) {
    throw "GitHub CLI (gh) is required only when -Publish is used."
}
Invoke-NativeChecked -Command "gh" -Arguments @("auth", "status") `
    -FailureMessage "GitHub CLI authentication is not ready"

$repoJson = (& gh repo view $Repository --json visibility,defaultBranchRef,viewerPermission 2>&1 | Out-String).Trim()
if ($LASTEXITCODE -ne 0) {
    throw "Could not inspect GitHub repository $Repository`: $repoJson"
}
$repo = $repoJson | ConvertFrom-Json
if ($repo.visibility -ne "PUBLIC") {
    throw "Release repository must be public so student setup can download without authentication."
}
if (($null -eq $repo.defaultBranchRef) -or [string]::IsNullOrWhiteSpace($repo.defaultBranchRef.name)) {
    throw "GitHub repository $Repository has no default branch. Push main before publishing."
}

$immutabilityJson = (& gh api "repos/$Repository/immutable-releases" 2>&1 | Out-String).Trim()
if ($LASTEXITCODE -ne 0) {
    throw "Could not inspect immutable-release settings for $Repository`: $immutabilityJson"
}
$immutability = $immutabilityJson | ConvertFrom-Json
if (-not $immutability.enabled) {
    throw "GitHub immutable releases are disabled for $Repository. Enable them before publishing."
}

& gh api "repos/$Repository/git/commits/$sourceCommit" --silent 2>$null
if ($LASTEXITCODE -ne 0) {
    throw "Source commit $sourceCommit is not present on GitHub. Push the release commit before publishing."
}
& gh release view $releaseTag --repo $Repository *> $null
if ($LASTEXITCODE -eq 0) {
    throw "GitHub release $releaseTag already exists; release versions are immutable."
}
$matchingTagsJson = (& gh api "repos/$Repository/git/matching-refs/tags/$releaseTag" 2>&1 | Out-String).Trim()
if ($LASTEXITCODE -ne 0) {
    throw "Could not inspect existing GitHub tags: $matchingTagsJson"
}
$matchingTags = @($matchingTagsJson | ConvertFrom-Json)
$exactTag = @($matchingTags | Where-Object { $_.ref -eq "refs/tags/$releaseTag" })
if ($exactTag.Count -gt 0) {
    throw "GitHub tag $releaseTag already exists without a release; refusing to attach assets to an unknown commit."
}

$releaseNotes = @"
Prebuilt NXP Cup student core tools for Windows x64.

Includes the native viewer, command-line tool, SDL2 runtime, and pinned NXP
ROM-HID programmer. J-Link is not included or required for the normal flow.
"@
Invoke-NativeChecked -Command "gh" -Arguments @(
    "release", "create", $releaseTag,
    $package.ZipPath, $package.ChecksumPath,
    "--repo", $Repository,
    "--target", $sourceCommit,
    "--title", "NXP Cup core tools $Version",
    "--notes", $releaseNotes,
    "--draft"
) -FailureMessage "Could not create the draft GitHub release"

$downloadRoot = Join-Path $validationRoot "github-download"
New-Item -ItemType Directory -Force -Path $downloadRoot | Out-Null
Invoke-NativeChecked -Command "gh" -Arguments @(
    "release", "download", $releaseTag,
    "--repo", $Repository,
    "--pattern", "$packageName.zip",
    "--dir", $downloadRoot
) -FailureMessage "Could not download the draft release asset for verification"
$downloadedZip = Join-Path $downloadRoot "$packageName.zip"
$downloadHash = (Get-FileHash -LiteralPath $downloadedZip -Algorithm SHA256).Hash.ToLowerInvariant()
if ($downloadHash -ne $zipHash) {
    throw "Downloaded GitHub asset hash does not match the local release archive; the draft was not published."
}
Test-CoreToolsArchive -ZipPath $downloadedZip -ExpectedVersion $Version `
    -ValidationRoot (Join-Path $validationRoot "github-package")

Invoke-NativeChecked -Command "gh" -Arguments @(
    "release", "edit", $releaseTag,
    "--repo", $Repository,
    "--draft=false"
) -FailureMessage "The verified draft could not be published"

$publicUrl = "https://github.com/$Repository/releases/download/$releaseTag/$packageName.zip"
$publicDownload = Join-Path $downloadRoot "public-$packageName.zip"
$publicVerified = $false
$lastPublicError = ""
for ($attempt = 1; $attempt -le 6; $attempt++) {
    try {
        if (Test-Path -LiteralPath $publicDownload) {
            Remove-Item -LiteralPath $publicDownload -Force
        }
        Write-Host "Anonymous asset verification attempt $attempt of 6..."
        Invoke-WebRequest -UseBasicParsing -Uri $publicUrl -OutFile $publicDownload
        $publicHash = (Get-FileHash -LiteralPath $publicDownload -Algorithm SHA256).Hash.ToLowerInvariant()
        if ($publicHash -eq $zipHash) {
            $publicVerified = $true
            break
        }
        $lastPublicError = "downloaded SHA-256 was $publicHash"
    } catch {
        $lastPublicError = $_.Exception.Message
    }
    if ($attempt -lt 6) {
        Start-Sleep -Seconds ([Math]::Min(2 * $attempt, 10))
    }
}
if (-not $publicVerified) {
    throw "Release $releaseTag was published and is immutable, but anonymous verification did not complete after 6 attempts: $lastPublicError"
}
$publicHash = (Get-FileHash -LiteralPath $publicDownload -Algorithm SHA256).Hash.ToLowerInvariant()
if ($publicHash -ne $zipHash) {
    throw "The anonymous public download does not match the verified release archive."
}
Write-Host "Published and verified: $publicUrl" -ForegroundColor Green
