param(
    [ValidateSet("Debug")]
    [string]$Configuration = "Debug",

    [string]$BuildDir = "",
    [switch]$SkipGenerate,
    [string[]]$Define = @(),

    [string]$ProjectPath = (Join-Path $PSScriptRoot "src\avc\avc_core0")
)

$ErrorActionPreference = "Stop"

if ([string]::IsNullOrWhiteSpace($BuildDir)) {
    $BuildDir = Join-Path $PSScriptRoot "build\cmake\avc_core0-$Configuration"
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

if (-not $SkipGenerate) {
    Write-Host "Generating CMake settings from MCUXpresso $Configuration project metadata..." -ForegroundColor Cyan
    Invoke-Checked {
        python $generator --project-dir $ProjectPath --config $Configuration --output $generatedInclude
    } "CMake settings generation"
}

if (-not (Test-Path -LiteralPath $generatedInclude)) {
    throw "Generated CMake include not found: $generatedInclude. Run without -SkipGenerate first."
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
    $configureArgs += "-DAVC_EXTRA_DEFINES=$($Define -join ';')"
}

Invoke-Checked {
    cmake @configureArgs
} "CMake configure"

Write-Host "Building with Ninja..." -ForegroundColor Cyan
Invoke-Checked {
    cmake --build $BuildDir
} "CMake build"

Write-Host "CMake build output: $BuildDir" -ForegroundColor Green
