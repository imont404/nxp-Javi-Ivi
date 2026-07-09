param(
    [int]$Seconds = 0,
    [switch]$NoTail,
    [string]$File,
    [string]$OutFile,
    [string]$JLinkRTTLoggerPath = "C:\Program Files\SEGGER\JLink_V940\JLinkRTTLogger.exe",
    [string]$NmPath = "C:\nxp\MCUXpressoIDE_25.6.136\ide\plugins\com.nxp.mcuxpresso.tools.win32_25.6.0.202501151204\tools\bin\arm-none-eabi-nm.exe",
    [string]$Device = "MCXN947_M33_0",
    [string]$Interface = "SWD",
    [int]$SpeedKHz = 4000,
    [string]$UsbSerial = "1066857635",
    [int]$Channel = 0
)

$ErrorActionPreference = "Stop"

$projectDir = $PSScriptRoot
$defaultAxf = Join-Path $projectDir "src\avc\avc_core0\Debug\avc_core0.axf"
$axfFile = if ($File) { $File } else { $defaultAxf }

if (-not (Test-Path -LiteralPath $JLinkRTTLoggerPath)) {
    throw "J-Link RTT Logger not found: $JLinkRTTLoggerPath"
}

if (-not (Test-Path -LiteralPath $NmPath)) {
    throw "arm-none-eabi-nm not found: $NmPath"
}

if (-not (Test-Path -LiteralPath $axfFile)) {
    throw "Firmware not found: $axfFile. Build first with .\build.ps1"
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
