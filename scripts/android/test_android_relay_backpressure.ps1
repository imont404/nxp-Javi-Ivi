[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$Serial,
    [int]$Port = 8765,
    [int]$TimeoutSeconds = 12
)

$ErrorActionPreference = "Stop"
. (Join-Path $PSScriptRoot "android_env.ps1")

if ($Serial -notmatch "^(?<host>[^:]+):[0-9]+$") {
    throw "A network adb serial such as 192.168.1.37:5555 is required."
}
$phoneAddress = $Matches.host
$baseUri = "http://${phoneAddress}:$Port"
$adb = Join-Path $env:ANDROID_HOME "platform-tools\adb.exe"

function Get-AppPssKb {
    $meminfo = & $adb -s $Serial shell dumpsys meminfo com.wavenumber.avc.bridge
    $total = $meminfo | Select-String "TOTAL PSS:\s+(?<kb>[0-9]+)" | Select-Object -First 1
    if ($null -eq $total) { throw "Could not read app TOTAL PSS through adb." }
    return [int]$total.Matches[0].Groups["kb"].Value
}

$before = Invoke-RestMethod -Uri "$baseUri/health" -TimeoutSec 5
if ($before.state -ne "streaming") {
    throw "Backpressure proof requires a streaming app."
}
$pssBeforeKb = Get-AppPssKb
$client = [System.Net.Sockets.TcpClient]::new()
$client.ReceiveBufferSize = 1024

try {
    $client.Connect($phoneAddress, $Port)
    $stream = $client.GetStream()
    $keyBytes = New-Object byte[] 16
    $random = [System.Security.Cryptography.RandomNumberGenerator]::Create()
    try {
        $random.GetBytes($keyBytes)
    } finally {
        $random.Dispose()
    }
    $webSocketKey = [Convert]::ToBase64String($keyBytes)
    $request = @(
        "GET /stream HTTP/1.1"
        "Host: ${phoneAddress}:$Port"
        "Upgrade: websocket"
        "Connection: Upgrade"
        "Sec-WebSocket-Key: $webSocketKey"
        "Sec-WebSocket-Version: 13"
        "X-AVC-Replace-Viewer: 1"
        ""
        ""
    ) -join "`r`n"
    $requestBytes = [Text.Encoding]::ASCII.GetBytes($request)
    $stream.Write($requestBytes, 0, $requestBytes.Length)
    $stream.Flush()

    $header = [Text.StringBuilder]::new()
    while (-not $header.ToString().EndsWith("`r`n`r`n")) {
        $value = $stream.ReadByte()
        if ($value -lt 0 -or $header.Length -ge 8192) {
            throw "Relay did not complete the WebSocket handshake."
        }
        [void]$header.Append([char]$value)
    }
    if (-not $header.ToString().StartsWith("HTTP/1.1 101")) {
        throw "Relay rejected the slow-client WebSocket handshake."
    }

    # Deliberately stop reading. The relay must close this socket instead of
    # allowing its network worker to remain blocked indefinitely.
    Start-Sleep -Milliseconds 1000
    $pssDuringKb = Get-AppPssKb
    $deadline = [DateTime]::UtcNow.AddSeconds($TimeoutSeconds)
    do {
        Start-Sleep -Milliseconds 250
        $after = Invoke-RestMethod -Uri "$baseUri/health" -TimeoutSec 5
        if (
            $after.slow_client_disconnects -gt $before.slow_client_disconnects -and
            $after.clients -eq 0
        ) { break }
    } while ([DateTime]::UtcNow -lt $deadline)

    if ($after.slow_client_disconnects -le $before.slow_client_disconnects) {
        throw "The non-reading client was not closed by the send deadline."
    }
    if ($after.usb_frames -le $before.usb_frames) {
        throw "USB frame intake did not advance while the Wi-Fi client was blocked."
    }
    if ($after.sequence_errors -ne 0 -or $after.malformed_chunks -ne 0) {
        throw "USB health degraded during slow-client stress."
    }
    if ($pssDuringKb -gt $pssBeforeKb + 16384) {
        throw "App PSS grew by more than 16 MiB during a bounded slow-client test."
    }
} finally {
    $client.Dispose()
}

& (Join-Path $PSScriptRoot "verify_android_relay.ps1") -Serial $Serial -Port $Port
$pssAfterKb = Get-AppPssKb

[PSCustomObject]@{
    usb_frames_before = $before.usb_frames
    usb_frames_after = $after.usb_frames
    slow_client_disconnects = $after.slow_client_disconnects
    pss_before_kb = $pssBeforeKb
    pss_during_kb = $pssDuringKb
    pss_after_kb = $pssAfterKb
} | Format-List

Write-Host "AVC Android relay backpressure proof passed." -ForegroundColor Green
