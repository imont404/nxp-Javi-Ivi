param(
    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Debug",

    [switch]$Clean,
    [switch]$ResetWorkspace,

    [string]$IdePath = "C:\nxp\MCUXpressoIDE_25.6.136\ide\mcuxpressoidec.exe",
    [string]$Workspace = (Join-Path (Split-Path $PSScriptRoot -Parent) ".mcux_workspace_cdc_vcom"),
    [string]$ProjectPath = $PSScriptRoot,
    [string]$ProjectName = "frdmmcxn947_dev_cdc_vcom_bm_cm33_core0"
)

$ErrorActionPreference = "Stop"

function Resolve-McuxpressoIde {
    param(
        [Parameter(Mandatory = $true)]
        [string]$ConfiguredPath
    )

    if (Test-Path -LiteralPath $ConfiguredPath) {
        return $ConfiguredPath
    }

    $nxpRoot = "C:\nxp"
    if (Test-Path -LiteralPath $nxpRoot) {
        $candidate = Get-ChildItem -LiteralPath $nxpRoot -Directory -Filter "MCUXpressoIDE_*" |
            Sort-Object -Property Name -Descending |
            ForEach-Object { Join-Path $_.FullName "ide\mcuxpressoidec.exe" } |
            Where-Object { Test-Path -LiteralPath $_ } |
            Select-Object -First 1

        if ($candidate) {
            return $candidate
        }
    }

    return $ConfiguredPath
}

$IdePath = Resolve-McuxpressoIde -ConfiguredPath $IdePath

if (-not (Test-Path -LiteralPath $IdePath)) {
    throw "MCUXpresso headless executable not found: $IdePath"
}

if (-not (Test-Path -LiteralPath $ProjectPath)) {
    throw "MCUXpresso project not found: $ProjectPath"
}

if ($ResetWorkspace -and (Test-Path -LiteralPath $Workspace)) {
    $workspacePath = (Resolve-Path -LiteralPath $Workspace).Path
    $examplesRoot = (Resolve-Path -LiteralPath (Split-Path $PSScriptRoot -Parent)).Path
    if (-not $workspacePath.StartsWith($examplesRoot, [StringComparison]::OrdinalIgnoreCase)) {
        throw "Refusing to remove workspace outside the FRDM examples folder: $workspacePath"
    }
    Remove-Item -LiteralPath $workspacePath -Recurse -Force
}

$buildVerb = if ($Clean) { "-cleanBuild" } else { "-build" }
$target = "$ProjectName/$Configuration"

& $IdePath `
    -nosplash `
    --launcher.suppressErrors `
    -application org.eclipse.cdt.managedbuilder.core.headlessbuild `
    -data $Workspace `
    -import $ProjectPath `
    $buildVerb $target

exit $LASTEXITCODE
