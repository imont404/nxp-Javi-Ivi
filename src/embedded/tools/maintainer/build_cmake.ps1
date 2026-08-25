param(
    [ValidateSet("Debug")]
    [string]$Configuration = "Debug",

    [string]$BuildDir = "",

    # The checked-in cmake/mcuxpresso_debug.cmake is the source of truth for the
    # source list. Regeneration is opt-in because it requires Python and the
    # MCUXpresso .cproject/.project metadata, neither of which a normal build
    # should depend on.
    [switch]$Regenerate,

    # Regenerate to a temporary file and diff it against the committed one, so
    # drift after an MCUXpresso project change is visible rather than silent.
    [switch]$CheckDrift,

    # Deprecated no-op: not regenerating is now the default.
    [switch]$SkipGenerate,

    # Python tooling runs through uv against pyproject.toml - never a bare
    # interpreter. setup.ps1 provisions uv.
    [string]$UvPath = "uv",
    [string[]]$Define = @(),

    [string]$ProjectPath = ""
)

$ErrorActionPreference = "Stop"
$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..\..\..")).Path

if ([string]::IsNullOrWhiteSpace($ProjectPath)) {
    $ProjectPath = Join-Path $repoRoot "src\embedded\nxp_cup_core0"
}

if ([string]::IsNullOrWhiteSpace($BuildDir)) {
    $BuildDir = Join-Path $repoRoot "out\build\embedded\diagnostics\cmake-$Configuration"
}

$generator = Join-Path $ProjectPath "cmake\generate_mcuxpresso_cmake.py"
$generatedInclude = Join-Path $ProjectPath "cmake\mcuxpresso_debug.cmake"
$toolchain = Join-Path $ProjectPath "cmake\mcuxpresso-toolchain.cmake"

function Invoke-Checked {
    param(
        [Parameter(Mandatory = $true)]
        [scriptblock]$Command,
        [Parameter(Mandatory = $true)]
        [string]$Description
    )

    & $Command
    if ($LASTEXITCODE -ne 0) {
        throw "$Description failed with exit code $LASTEXITCODE"
    }
}

if (-not (Test-Path -LiteralPath $ProjectPath)) {
    throw "CMake project not found: $ProjectPath"
}

if ($SkipGenerate) {
    Write-Host "-SkipGenerate is now the default and can be dropped." -ForegroundColor DarkGray
}

if ($CheckDrift -or $Regenerate) {
    if (-not (Get-Command $UvPath -ErrorAction SilentlyContinue)) {
        throw "uv not found; it is required only for -CheckDrift and -Regenerate. Run .\setup.ps1, or pass -UvPath."
    }
}

if ($CheckDrift) {
    Write-Host "Checking the committed source list against MCUXpresso project metadata..." -ForegroundColor Cyan
    $tmp = Join-Path ([System.IO.Path]::GetTempPath()) "nxpc_mcuxpresso_drift.cmake"
    Invoke-Checked {
        & $UvPath run python $generator --project-dir $ProjectPath --config $Configuration --output $tmp
    } "Source list drift check"
    $diff = Compare-Object (Get-Content $generatedInclude) (Get-Content $tmp)
    Remove-Item $tmp -Force -ErrorAction SilentlyContinue
    if ($diff) {
        Write-Host "  [DRIFT] The committed source list differs from the regenerated one:" -ForegroundColor Yellow
        $diff | ForEach-Object { Write-Host ("    {0} {1}" -f $_.SideIndicator, $_.InputObject) -ForegroundColor Yellow }
        Write-Host "  Re-run with -Regenerate to accept, then review and commit." -ForegroundColor White
        exit 1
    }
    Write-Host "  [OK] No drift." -ForegroundColor Green
}
elseif ($Regenerate) {
    Write-Host "Regenerating CMake settings from MCUXpresso $Configuration project metadata..." -ForegroundColor Cyan
    Invoke-Checked {
        & $UvPath run python $generator --project-dir $ProjectPath --config $Configuration --output $generatedInclude
    } "CMake settings generation"
    Write-Host "  Review and commit the change to $generatedInclude" -ForegroundColor White
}

if (-not (Test-Path -LiteralPath $generatedInclude)) {
    throw "Source list not found: $generatedInclude. It is normally committed; run with -Regenerate to rebuild it."
}

$cachePath = Join-Path $BuildDir "CMakeCache.txt"
if (Test-Path -LiteralPath $cachePath) {
    $cacheText = Get-Content -Raw -LiteralPath $cachePath
    $staleReasons = New-Object System.Collections.Generic.List[string]

    if ($cacheText -match '(?m)^CMAKE_HOME_DIRECTORY:INTERNAL=(.+)$') {
        $cachedSource = $Matches[1].Trim()
        $expectedSource = [System.IO.Path]::GetFullPath($ProjectPath)
        $cachedComparable = ($cachedSource -replace '\\', '/').TrimEnd('/')
        $expectedComparable = ($expectedSource -replace '\\', '/').TrimEnd('/')
        if ($cachedComparable -ne $expectedComparable) {
            $staleReasons.Add("source directory changed from $cachedSource")
        }
    }

    if ($cacheText -match '(?m)^CMAKE_C_COMPILER:FILEPATH=(.+)$') {
        $cachedCompiler = $Matches[1].Trim()
        if ($cachedCompiler -and -not (Test-Path -LiteralPath $cachedCompiler)) {
            $staleReasons.Add("compiler is unavailable: $cachedCompiler")
        }
    }

    if ($staleReasons.Count -gt 0) {
        Write-Host "Removing stale CMake cache: $($staleReasons -join '; ')" -ForegroundColor Yellow
        Remove-Item -LiteralPath $cachePath -Force
        $cacheFilesDir = Join-Path $BuildDir "CMakeFiles"
        if (Test-Path -LiteralPath $cacheFilesDir) {
            Remove-Item -LiteralPath $cacheFilesDir -Recurse -Force
        }
    }
}

Write-Host "Configuring CMake/Ninja build..." -ForegroundColor Cyan
$configureArgs = @("-S", $ProjectPath, "-B", $BuildDir, "-G", "Ninja")
if (-not (Test-Path -LiteralPath $cachePath)) {
    $configureArgs += "-DCMAKE_TOOLCHAIN_FILE=$toolchain"
}
if ($Define.Count -gt 0) {
    $configureArgs += "-DNXPC_EXTRA_DEFINES=$($Define -join ';')"
}

Invoke-Checked {
    cmake @configureArgs
} "CMake configure"

Write-Host "Building with Ninja..." -ForegroundColor Cyan
Invoke-Checked {
    cmake --build $BuildDir
} "CMake build"

Write-Host "CMake build output: $BuildDir" -ForegroundColor Green
