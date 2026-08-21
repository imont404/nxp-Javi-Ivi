[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$Serial,
    [int]$Port = 8765,
    [int]$MinimumFrames = 3,
    [int]$TimeoutSeconds = 15
)

$ErrorActionPreference = "Stop"

if ($Serial -notmatch "^(?<host>[^:]+):[0-9]+$") {
    throw "A network adb serial such as 192.168.1.37:5555 is required."
}
$phoneAddress = $Matches.host
$baseUri = "http://${phoneAddress}:$Port"
$healthBefore = Invoke-RestMethod -Uri "$baseUri/health" -TimeoutSec 5
if ($healthBefore.state -ne "streaming") {
    throw "Relay health reports USB state '$($healthBefore.state)', expected 'streaming'."
}
if ($healthBefore.sequence_errors -ne 0 -or $healthBefore.malformed_chunks -ne 0) {
    throw "USB health is not clean: sequence=$($healthBefore.sequence_errors), malformed=$($healthBefore.malformed_chunks)."
}

$socket = [System.Net.WebSockets.ClientWebSocket]::new()
$deadline = [System.Threading.CancellationTokenSource]::new()
$deadline.CancelAfter([TimeSpan]::FromSeconds($TimeoutSeconds))
$receiveBuffer = New-Object byte[] 20000
$segment = [System.ArraySegment[byte]]::new($receiveBuffer)
$completedFrames = 0
$telemetryPackets = 0
$activeFrameId = $null
$nextOffset = 0
$expectedSequence = $null

try {
    $socket.ConnectAsync(
        [Uri]"ws://${phoneAddress}:$Port/stream",
        $deadline.Token
    ).GetAwaiter().GetResult() | Out-Null
    while ($completedFrames -lt $MinimumFrames) {
        $message = [System.IO.MemoryStream]::new()
        do {
            $result = $socket.ReceiveAsync($segment, $deadline.Token).GetAwaiter().GetResult()
            if ($result.MessageType -eq [System.Net.WebSockets.WebSocketMessageType]::Close) {
                throw "Relay WebSocket closed before $MinimumFrames complete frames arrived."
            }
            $message.Write($receiveBuffer, 0, $result.Count)
        } while (-not $result.EndOfMessage)

        $packet = $message.ToArray()
        if ($packet.Length -lt 32 -or [BitConverter]::ToUInt32($packet, 0) -ne 0x55435641) {
            throw "Relay emitted a malformed AVCU packet."
        }
        $sequence = [BitConverter]::ToUInt32($packet, 12)
        if ($null -ne $expectedSequence -and $sequence -ne $expectedSequence) {
            throw "Relay sequence discontinuity: expected $expectedSequence, received $sequence."
        }
        $expectedSequence = [uint32]($sequence + 1)
        $payloadBytes = [BitConverter]::ToUInt32($packet, 16)
        if ($packet.Length -ne 32 + $payloadBytes) {
            throw "Relay AVCU payload length does not match the WebSocket message."
        }

        $messageId = [BitConverter]::ToUInt32($packet, 8)
        if ($messageId -eq 0x01000500) {
            $telemetryPackets++
            continue
        }
        if ($messageId -ne 0x01000002 -or $payloadBytes -lt 24) { continue }

        $frameId = [BitConverter]::ToUInt32($packet, 32)
        $offset = [BitConverter]::ToUInt32($packet, 36)
        $totalBytes = [BitConverter]::ToUInt32($packet, 40)
        $width = [BitConverter]::ToUInt16($packet, 44)
        $height = [BitConverter]::ToUInt16($packet, 46)
        $pixelFormat = [BitConverter]::ToUInt16($packet, 48)
        $chunkFlags = [BitConverter]::ToUInt32($packet, 52)
        $dataBytes = $payloadBytes - 24
        if ($totalBytes -ne 128000 -or $width -ne 320 -or $height -ne 200 -or $pixelFormat -ne 1) {
            throw "Relay emitted unsupported frame metadata."
        }
        if (($chunkFlags -band 1) -ne 0) {
            if ($offset -ne 0) { throw "Frame start did not begin at offset zero." }
            $activeFrameId = $frameId
            $nextOffset = 0
        }
        if ($null -eq $activeFrameId -or $frameId -ne $activeFrameId -or $offset -ne $nextOffset) {
            throw "Relay emitted non-contiguous frame chunks."
        }
        $nextOffset += $dataBytes
        if (($chunkFlags -band 2) -ne 0) {
            if ($nextOffset -ne 128000) { throw "Relay ended a partial frame at $nextOffset bytes." }
            $completedFrames++
            $activeFrameId = $null
            $nextOffset = 0
        }
    }
} finally {
    if ($socket.State -eq [System.Net.WebSockets.WebSocketState]::Open) {
        $socket.CloseOutputAsync(
            [System.Net.WebSockets.WebSocketCloseStatus]::NormalClosure,
            "verification complete",
            [System.Threading.CancellationToken]::None
        ).GetAwaiter().GetResult() | Out-Null
    }
    $socket.Dispose()
    $deadline.Dispose()
}

$healthAfter = Invoke-RestMethod -Uri "$baseUri/health" -TimeoutSec 5
if ($healthAfter.sequence_errors -ne 0 -or $healthAfter.malformed_chunks -ne 0) {
    throw "USB health degraded during relay: sequence=$($healthAfter.sequence_errors), malformed=$($healthAfter.malformed_chunks)."
}

[PSCustomObject]@{
    url = "$baseUri/"
    frames = $completedFrames
    telemetry_packets = $telemetryPackets
    usb_fps = $healthAfter.usb_fps
    usb_mib_s = $healthAfter.usb_mib_s
    relay_sent_frames = $healthAfter.relay_sent_frames
    relay_dropped_frames = $healthAfter.relay_dropped_frames
} | Format-List

Write-Host "AVC Android relay verification passed." -ForegroundColor Green
