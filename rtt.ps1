param(
    [int]$Seconds = 0,
    [switch]$NoTail,
    [switch]$CMake,
    [switch]$Reset,
    [ValidateSet("Debug")]
    [string]$Configuration = "Debug",
    [string]$File,
    [string]$OutFile,
    [ValidateSet("PyLink", "Logger")]
    [string]$Backend = "PyLink",
    [string]$UvPath = "uv",
    [string]$JLinkDllPath = "C:\Program Files\SEGGER\JLink_V940\JLink_x64.dll",
    [string]$JLinkRTTLoggerPath = "C:\Program Files\SEGGER\JLink_V940\JLinkRTTLogger.exe",
    [string]$NmPath = "C:\nxp\MCUXpressoIDE_25.6.136\ide\plugins\com.nxp.mcuxpresso.tools.win32_25.6.0.202501151204\tools\bin\arm-none-eabi-nm.exe",
    [string]$Device = "MCXN947_M33_0",
    [string]$Interface = "SWD",
    [int]$SpeedKHz = 4000,
    # See flash.ps1 / scripts\tools\jlink_common.ps1. Empty resolves automatically.
    [string]$UsbSerial = "",
    [int]$Channel = 0
)

$ErrorActionPreference = "Stop"

. (Join-Path $PSScriptRoot "scripts\tools\jlink_common.ps1")
$UsbSerial = Resolve-JLinkSerial -Requested $UsbSerial

function Resolve-SeggerTool {
    param(
        [Parameter(Mandatory = $true)]
        [string]$ConfiguredPath,
        [Parameter(Mandatory = $true)]
        [string]$ToolName
    )

    if (Test-Path -LiteralPath $ConfiguredPath) {
        return $ConfiguredPath
    }

    $seggerRoot = "C:\Program Files\SEGGER"
    if (Test-Path -LiteralPath $seggerRoot) {
        $candidate = Get-ChildItem -LiteralPath $seggerRoot -Directory -Filter "JLink_V*" |
            Sort-Object -Property Name -Descending |
            ForEach-Object { Join-Path $_.FullName $ToolName } |
            Where-Object { Test-Path -LiteralPath $_ } |
            Select-Object -First 1

        if ($candidate) {
            return $candidate
        }
    }

    return $ConfiguredPath
}

$projectDir = $PSScriptRoot
$mcuxAxf = Join-Path $projectDir "src\avc\avc_core0\$Configuration\avc_core0.axf"
$cmakeAxf = Join-Path $projectDir "build\cmake\avc_core0-$Configuration\avc_core0.axf"
$defaultAxf = if ($CMake) { $cmakeAxf } else { $mcuxAxf }
$axfFile = if ($File) { $File } else { $defaultAxf }
$JLinkDllPath = Resolve-SeggerTool -ConfiguredPath $JLinkDllPath -ToolName "JLink_x64.dll"
$JLinkRTTLoggerPath = Resolve-SeggerTool -ConfiguredPath $JLinkRTTLoggerPath -ToolName "JLinkRTTLogger.exe"

if (-not (Test-Path -LiteralPath $NmPath)) {
    throw "arm-none-eabi-nm not found: $NmPath"
}

if (-not (Test-Path -LiteralPath $axfFile)) {
    throw "Firmware not found: $axfFile. Build first with .\build.ps1"
}

if ($Backend -eq "PyLink") {
    $monitor = Join-Path $projectDir "scripts\tools\rtt_monitor.py"
    if (-not (Test-Path -LiteralPath $monitor)) {
        throw "PyLink RTT monitor not found: $monitor"
    }
    if (-not (Test-Path -LiteralPath $JLinkDllPath)) {
        throw "J-Link DLL not found: $JLinkDllPath"
    }
    if (-not (Get-Command $UvPath -ErrorAction SilentlyContinue)) {
        throw "uv not found. Install uv or pass -UvPath."
    }

    $args = @(
        "run",
        "python",
        $monitor,
        "--file", $axfFile,
        "--nm", $NmPath,
        "--dll", $JLinkDllPath,
        "--device", $Device,
        "--serial", $UsbSerial,
        "--speed", "$SpeedKHz",
        "--channel", "$Channel"
    )

    if ($Seconds -gt 0) {
        $args += @("--timeout", "$Seconds")
    }
    if ($NoTail) {
        $args += "--once"
    }
    if ($Reset) {
        $args += "--reset"
    }

    $previousUvLinkMode = $env:UV_LINK_MODE
    if ([string]::IsNullOrWhiteSpace($env:UV_LINK_MODE)) {
        $env:UV_LINK_MODE = "copy"
    }

    try {
        & $UvPath @args
        exit $LASTEXITCODE
    } finally {
        if ($null -eq $previousUvLinkMode) {
            Remove-Item Env:\UV_LINK_MODE -ErrorAction SilentlyContinue
        } else {
            $env:UV_LINK_MODE = $previousUvLinkMode
        }
    }
}

if (-not (Test-Path -LiteralPath $JLinkRTTLoggerPath)) {
    throw "J-Link RTT Logger not found: $JLinkRTTLoggerPath"
}

$symbolLine = & $NmPath -n $axfFile | Select-String -Pattern "^\s*([0-9A-Fa-f]+)\s+\w\s+_SEGGER_RTT\s*$" | Select-Object -First 1
if (-not $symbolLine) {
    throw "Could not find _SEGGER_RTT in $axfFile"
}

if ($symbolLine.Line -notmatch "^\s*([0-9A-Fa-f]+)\s+\w\s+_SEGGER_RTT\s*$") {
    throw "Could not parse _SEGGER_RTT symbol line: $($symbolLine.Line)"
}

$rttAddress = "0x$($Matches[1])"
$logFile = if ($OutFile) {
    $OutFile
} else {
    Join-Path $env:TEMP ("avc_rtt_{0}.log" -f (Get-Date -Format "yyyyMMdd_HHmmss"))
}

$loggerStdout = Join-Path $env:TEMP "avc_rtt_logger_stdout.txt"
$loggerStderr = Join-Path $env:TEMP "avc_rtt_logger_stderr.txt"
Remove-Item -LiteralPath $logFile, $loggerStdout, $loggerStderr -Force -ErrorAction SilentlyContinue

Write-Host "Device: $Device"
Write-Host "Interface: $Interface @ $SpeedKHz kHz"
Write-Host "RTT address: $rttAddress"
Write-Host "Channel: $Channel"
Write-Host "Log file: $logFile"
Write-Host ""

$args = @(
    "-Device", $Device,
    "-If", $Interface,
    "-Speed", "$SpeedKHz",
    "-USB", $UsbSerial,
    "-RTTAddress", $rttAddress,
    "-RTTChannel", "$Channel",
    $logFile
)

if ($NoTail) {
    & $JLinkRTTLoggerPath @args
    exit $LASTEXITCODE
}

$process = Start-Process `
    -FilePath $JLinkRTTLoggerPath `
    -ArgumentList $args `
    -WindowStyle Hidden `
    -RedirectStandardOutput $loggerStdout `
    -RedirectStandardError $loggerStderr `
    -PassThru

try {
    $deadline = (Get-Date).AddSeconds(5)
    while ((-not (Test-Path -LiteralPath $logFile)) -and (Get-Date) -lt $deadline) {
        Start-Sleep -Milliseconds 100
    }

    if ($Seconds -gt 0) {
        Start-Sleep -Seconds $Seconds
        if (-not $process.HasExited) {
            Stop-Process -Id $process.Id -Force
        }
        if (Test-Path -LiteralPath $logFile) {
            Get-Content -LiteralPath $logFile
        } else {
            Write-Host "RTT log file was not created."
            if (Test-Path -LiteralPath $loggerStdout) {
                Get-Content -LiteralPath $loggerStdout
            }
            if (Test-Path -LiteralPath $loggerStderr) {
                Get-Content -LiteralPath $loggerStderr
            }
        }
        exit 0
    }

    if (-not (Test-Path -LiteralPath $logFile)) {
        Write-Host "RTT log file was not created yet. Logger output:"
        if (Test-Path -LiteralPath $loggerStdout) {
            Get-Content -LiteralPath $loggerStdout
        }
        if (Test-Path -LiteralPath $loggerStderr) {
            Get-Content -LiteralPath $loggerStderr
        }
        exit 1
    }

    Get-Content -LiteralPath $logFile -Wait
} finally {
    if (-not $process.HasExited) {
        Stop-Process -Id $process.Id -Force
    }
}
