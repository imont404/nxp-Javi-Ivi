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
$receiveBuffer = New-Object byte[] 300000
$segment = [System.ArraySegment[byte]]::new($receiveBuffer)
$completedFrames = 0
$telemetryPackets = 0
$lastFrameId = $null
$expectedSequence = $null
$jpegBytesReceived = 0L
$receiveTimer = [Diagnostics.Stopwatch]::new()

try {
    $socket.Options.SetRequestHeader("X-AVC-Replace-Viewer", "1")
    $socket.ConnectAsync(
        [Uri]"ws://${phoneAddress}:$Port/stream",
        $deadline.Token
    ).GetAwaiter().GetResult() | Out-Null
    $receiveTimer.Start()
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
        if ($packet.Length -lt 32) {
            throw "Relay emitted a message shorter than its fixed header."
        }
        $magic = [BitConverter]::ToUInt32($packet, 0)
        if ($magic -eq 0x4A435641) {
            $version = $packet[4]
            $headerBytes = $packet[5]
            $frameId = [BitConverter]::ToUInt32($packet, 8)
            $width = [BitConverter]::ToUInt16($packet, 12)
            $height = [BitConverter]::ToUInt16($packet, 14)
            $jpegBytes = [BitConverter]::ToUInt32($packet, 16)
            if (
                $version -ne 1 -or
                $headerBytes -ne 32 -or
                $width -ne 320 -or
                $height -ne 200 -or
                $packet.Length -ne 32 + $jpegBytes -or
                $jpegBytes -lt 4 -or
                $packet[32] -ne 0xFF -or
                $packet[33] -ne 0xD8 -or
                $packet[$packet.Length - 2] -ne 0xFF -or
                $packet[$packet.Length - 1] -ne 0xD9
            ) {
                throw "Relay emitted a malformed AVCJ frame."
            }
            if ($null -ne $lastFrameId -and $frameId -le $lastFrameId) {
                throw "Relay JPEG frame IDs did not advance: previous $lastFrameId, received $frameId."
            }
            $lastFrameId = $frameId
            $jpegBytesReceived += $jpegBytes
            $completedFrames++
            continue
        }
        if ($magic -ne 0x55435641) {
            throw "Relay emitted an unknown message magic 0x$('{0:X8}' -f $magic)."
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
        }
    }
} finally {
    $receiveTimer.Stop()
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
if ($healthAfter.relay_mode -ne "jpeg") {
    throw "Relay mode is '$($healthAfter.relay_mode)', expected full-rate JPEG."
}
$frameAge = $healthAfter.last_source_frame_id - $healthAfter.last_sent_frame_id
if ($frameAge -lt 0 -or $frameAge -gt 8) {
    throw "Relay did not converge to a recent complete frame; source/sent frame gap is $frameAge."
}
$receivedFps = if ($receiveTimer.Elapsed.TotalSeconds -gt 0) {
    $completedFrames / $receiveTimer.Elapsed.TotalSeconds
} else { 0 }
$receivedMegabitsPerSecond = if ($receiveTimer.Elapsed.TotalSeconds -gt 0) {
    $jpegBytesReceived * 8 / $receiveTimer.Elapsed.TotalSeconds / 1000000
} else { 0 }

[PSCustomObject]@{
    url = "$baseUri/"
    frames = $completedFrames
    received_fps = [Math]::Round($receivedFps, 3)
    received_mbit_s = [Math]::Round($receivedMegabitsPerSecond, 3)
    telemetry_packets = $telemetryPackets
    usb_fps = $healthAfter.usb_fps
    usb_mib_s = $healthAfter.usb_mib_s
    relay_sent_frames = $healthAfter.relay_sent_frames
    relay_mbit_s = $healthAfter.relay_mbit_s
    relay_last_sent_age_ms = $healthAfter.relay_last_sent_age_ms
    relay_dropped_frames = $healthAfter.relay_dropped_frames
    source_sent_frame_gap = $frameAge
} | Format-List

Write-Host "AVC Android relay verification passed." -ForegroundColor Green
