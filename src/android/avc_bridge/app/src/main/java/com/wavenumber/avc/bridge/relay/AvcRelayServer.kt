package com.wavenumber.avc.bridge.relay

import com.wavenumber.avc.bridge.protocol.AvcPacket
import com.wavenumber.avc.bridge.protocol.AvcProtocol
import com.wavenumber.avc.bridge.usb.AvcUsbHealth
import com.wavenumber.avc.bridge.usb.AvcUsbState
import com.wavenumber.avc.bridge.video.AvcJpegFrameView
import java.io.BufferedInputStream
import java.io.BufferedOutputStream
import java.io.ByteArrayOutputStream
import java.io.EOFException
import java.io.InputStream
import java.io.IOException
import java.io.OutputStream
import java.net.Inet4Address
import java.net.InetAddress
import java.net.InetSocketAddress
import java.net.NetworkInterface
import java.net.ServerSocket
import java.net.Socket
import java.net.SocketTimeoutException
import java.nio.charset.StandardCharsets
import java.security.MessageDigest
import java.util.Base64
import java.util.Locale
import java.util.concurrent.atomic.AtomicLong

class AvcRelayServer(
    private val viewerHtml: ByteArray,
    private val port: Int = 8765,
) {
    companion object {
        private const val MAX_HTTP_HEADER_BYTES = 8 * 1024
        private const val MAX_CLIENT_PAYLOAD_BYTES = 4 * 1024
        private const val CLIENT_POLL_TIMEOUT_MS = 5
        private const val SEND_DEADLINE_NS = 2_000_000_000L
        private const val SEND_WATCHDOG_PERIOD_MS = 100L
        private const val WEBSOCKET_GUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
    }

    private val mailbox = AvcRelayMailbox(maxFrameBytes = 320 * 200 * 4)
    private val clientLock = Any()

    @Volatile
    private var running = false
    @Volatile
    private var serverSocket: ServerSocket? = null
    @Volatile
    private var activeClient: Socket? = null
    @Volatile
    private var usbHealth = AvcUsbHealth(AvcUsbState.IDLE, "not connected")
    @Volatile
    private var serverError: String? = null
    private var acceptThread: Thread? = null

    fun start() {
        if (running) return
        serverError = null
        running = true
        acceptThread = Thread(::acceptLoop, "avc-relay-accept").also { it.start() }
    }

    fun stop() {
        running = false
        activeClient?.closeQuietly()
        serverSocket?.closeQuietly()
        acceptThread?.interrupt()
    }

    fun noteSourceFrame(frameId: Long) = mailbox.noteSourceFrame(frameId)

    fun offerJpegFrame(frame: AvcJpegFrameView) = mailbox.offerJpegFrame(frame)

    fun offerDiagnostic(packet: AvcPacket) {
        if (packet.header.messageId == AvcProtocol.MSG_TELEMETRY_SCALAR) {
            mailbox.offerDiagnostic(packet)
        }
    }

    fun updateUsbHealth(health: AvcUsbHealth) {
        usbHealth = health
    }

    fun localUrl(): String = "http://${wifiIpv4Address() ?: "127.0.0.1"}:$port/"

    fun snapshot(): AvcRelaySnapshot = mailbox.snapshot()

    private fun acceptLoop() {
        try {
            System.setProperty("java.net.preferIPv4Stack", "true")
            val bindAddress = wifiIpv4Address() ?: "127.0.0.1"
            val listener = ServerSocket().apply {
                reuseAddress = true
                bind(InetSocketAddress(InetAddress.getByName(bindAddress), port))
            }
            serverSocket = listener
            while (running) {
                val socket = try {
                    listener.accept()
                } catch (error: Throwable) {
                    if (running) throw error
                    break
                }
                Thread({ handleClient(socket) }, "avc-relay-client").start()
            }
        } catch (error: Throwable) {
            if (running) serverError = error.message ?: error.javaClass.simpleName
        } finally {
            serverSocket?.closeQuietly()
            serverSocket = null
        }
    }

    private fun handleClient(socket: Socket) {
        try {
            socket.use { client ->
                client.soTimeout = 2_000
                client.tcpNoDelay = true
                val input = BufferedInputStream(client.getInputStream())
                val output = BufferedOutputStream(client.getOutputStream())
                val request = readHttpRequest(input) ?: return
                when {
                    request.path == "/" || request.path == "/index.html" ->
                        writeHttpResponse(output, 200, "OK", "text/html; charset=utf-8", viewerHtml)
                    request.path == "/health" ->
                        writeHttpResponse(
                            output,
                            200,
                            "OK",
                            "application/json; charset=utf-8",
                            healthJson().toByteArray(StandardCharsets.UTF_8),
                        )
                    request.path == "/stream" && request.headers["upgrade"]?.lowercase(Locale.US) == "websocket" ->
                        serveWebSocket(client, input, output, request.headers, request.replaceViewer)
                    else -> writeHttpResponse(
                        output,
                        404,
                        "Not Found",
                        "text/plain; charset=utf-8",
                        "not found\n".toByteArray(StandardCharsets.UTF_8),
                    )
                }
            }
        } catch (_: IOException) {
            // Disconnects, resets, and the send-deadline watchdog are client-local.
        }
    }

    private fun serveWebSocket(
        socket: Socket,
        input: BufferedInputStream,
        output: BufferedOutputStream,
        headers: Map<String, String>,
        replaceViewer: Boolean,
    ) {
        val key = headers["sec-websocket-key"] ?: run {
            writeHttpResponse(output, 400, "Bad Request", "text/plain", "missing websocket key\n".toByteArray())
            return
        }
        synchronized(clientLock) {
            if (activeClient != null) {
                if (replaceViewer || headers["x-avc-replace-viewer"] == "1") {
                    activeClient?.closeQuietly()
                    activeClient = null
                } else {
                    writeHttpResponse(output, 503, "Busy", "text/plain", "one viewer already connected\n".toByteArray())
                    return
                }
            }
            activeClient = socket
            mailbox.setClients(1)
        }

        try {
            val accept = Base64.getEncoder().encodeToString(
                MessageDigest.getInstance("SHA-1").digest((key.trim() + WEBSOCKET_GUID).toByteArray()),
            )
            output.write(
                (
                    "HTTP/1.1 101 Switching Protocols\r\n" +
                        "Upgrade: websocket\r\n" +
                        "Connection: Upgrade\r\n" +
                        "Sec-WebSocket-Accept: $accept\r\n\r\n"
                    ).toByteArray(StandardCharsets.US_ASCII),
            )
            output.flush()
            socket.soTimeout = CLIENT_POLL_TIMEOUT_MS
            socket.sendBufferSize = 64 * 1024
            val outboundWriteStartedNs = AtomicLong(0)
            val sendWatchdog = Thread(
                {
                    while (running && !socket.isClosed) {
                        val startedNs = outboundWriteStartedNs.get()
                        if (startedNs != 0L && System.nanoTime() - startedNs >= SEND_DEADLINE_NS) {
                            mailbox.noteSlowClientDisconnect()
                            socket.closeQuietly()
                            break
                        }
                        try {
                            Thread.sleep(SEND_WATCHDOG_PERIOD_MS)
                        } catch (_: InterruptedException) {
                            break
                        }
                    }
                },
                "avc-relay-send-watchdog",
            ).apply {
                isDaemon = true
                start()
            }
            var relaySequence = 0
            try {
                while (running && !socket.isClosed) {
                    val diagnostics = mailbox.takeDiagnostics(4)
                    val frame = mailbox.takeLatestFrame()
                    if (diagnostics.isNotEmpty() || frame != null) {
                        var frameSent = false
                        outboundWriteStartedNs.set(System.nanoTime())
                        try {
                            diagnostics.forEach { packet ->
                                writeWebSocketFrame(
                                    output,
                                    0x2,
                                    AvcRelayProtocol.encodeDiagnostic(packet, relaySequence++),
                                )
                            }
                            if (frame != null) {
                                writeWebSocketFrame(output, 0x2, AvcRelayProtocol.encodeJpegFrame(frame))
                            }
                            output.flush()
                            frameSent = frame != null
                        } finally {
                            outboundWriteStartedNs.set(0)
                            if (frame != null) mailbox.releaseSentFrame(frame, frameSent)
                        }
                    }

                    try {
                        if (!readClientFrame(input, output)) break
                    } catch (_: SocketTimeoutException) {
                        // Expected polling boundary; outbound work remains latest-only.
                    }
                }
            } finally {
                sendWatchdog.interrupt()
            }
        } finally {
            synchronized(clientLock) {
                if (activeClient === socket) {
                    activeClient = null
                    mailbox.setClients(0)
                }
            }
        }
    }

    private fun readClientFrame(input: InputStream, output: OutputStream): Boolean {
        val first = input.read()
        if (first < 0) return false
        val second = input.read()
        if (second < 0) return false
        val opcode = first and 0x0F
        val masked = second and 0x80 != 0
        var length = second and 0x7F
        if (length == 126) {
            length = (readRequired(input) shl 8) or readRequired(input)
        } else if (length == 127) {
            var extended = 0L
            repeat(8) { extended = (extended shl 8) or readRequired(input).toLong() }
            if (extended > MAX_CLIENT_PAYLOAD_BYTES) return false
            length = extended.toInt()
        }
        if (!masked || length > MAX_CLIENT_PAYLOAD_BYTES) return false
        val mask = ByteArray(4)
        readFully(input, mask)
        val payload = ByteArray(length)
        readFully(input, payload)
        payload.indices.forEach { payload[it] = (payload[it].toInt() xor mask[it % 4].toInt()).toByte() }
        return when (opcode) {
            0x8 -> false
            0x9 -> {
                writeWebSocketFrame(output, 0xA, payload)
                output.flush()
                true
            }
            else -> true
        }
    }

    private fun readHttpRequest(input: InputStream): HttpRequest? {
        val bytes = ByteArrayOutputStream()
        var matched = 0
        while (bytes.size() < MAX_HTTP_HEADER_BYTES) {
            val value = input.read()
            if (value < 0) return null
            bytes.write(value)
            matched = when {
                matched == 0 && value == '\r'.code -> 1
                matched == 1 && value == '\n'.code -> 2
                matched == 2 && value == '\r'.code -> 3
                matched == 3 && value == '\n'.code -> 4
                value == '\r'.code -> 1
                else -> 0
            }
            if (matched == 4) break
        }
        if (matched != 4) return null
        val lines = bytes.toString(StandardCharsets.ISO_8859_1.name()).split("\r\n")
        val requestParts = lines.firstOrNull()?.split(' ') ?: return null
        if (requestParts.size < 2 || requestParts[0] != "GET") return null
        val headers = mutableMapOf<String, String>()
        lines.drop(1).forEach { line ->
            val colon = line.indexOf(':')
            if (colon > 0) headers[line.substring(0, colon).trim().lowercase(Locale.US)] = line.substring(colon + 1).trim()
        }
        val target = requestParts[1]
        val replaceViewer = target.substringAfter('?', "")
            .split('&')
            .any { it.substringBefore('=') == "replace" }
        return HttpRequest(target.substringBefore('?'), headers, replaceViewer)
    }

    private fun writeHttpResponse(
        output: OutputStream,
        status: Int,
        reason: String,
        contentType: String,
        body: ByteArray,
    ) {
        output.write(
            (
                "HTTP/1.1 $status $reason\r\n" +
                    "Content-Type: $contentType\r\n" +
                    "Content-Length: ${body.size}\r\n" +
                    "Cache-Control: no-store\r\n" +
                    "Connection: close\r\n\r\n"
                ).toByteArray(StandardCharsets.US_ASCII),
        )
        output.write(body)
        output.flush()
    }

    private fun writeWebSocketFrame(output: OutputStream, opcode: Int, payload: ByteArray) {
        output.write(0x80 or opcode)
        when {
            payload.size <= 125 -> output.write(payload.size)
            payload.size <= 0xFFFF -> {
                output.write(126)
                output.write(payload.size ushr 8)
                output.write(payload.size and 0xFF)
            }
            else -> {
                output.write(127)
                repeat(4) { output.write(0) }
                output.write(payload.size ushr 24)
                output.write(payload.size ushr 16)
                output.write(payload.size ushr 8)
                output.write(payload.size and 0xFF)
            }
        }
        output.write(payload)
    }

    private fun healthJson(): String {
        val relay = mailbox.snapshot()
        val usb = usbHealth
        val elapsedSeconds = (relay.rateWindowLastNs - relay.rateWindowFirstNs) / 1_000_000_000.0
        val relayMegabitsPerSecond =
            if (elapsedSeconds > 0) relay.rateWindowBytes * 8.0 / elapsedSeconds / 1_000_000.0 else 0.0
        val lastSentAgeMs = if (relay.lastSentCapturedNs > 0) {
            (System.nanoTime() - relay.lastSentCapturedNs).coerceAtLeast(0) / 1_000_000.0
        } else {
            0.0
        }
        return """{"state":"${usb.state.wireName}","session_id":${usb.sessionId},"usb_frames":${usb.frames},"usb_fps":${format(usb.framesPerSecond)},"usb_mib_s":${format(usb.mebibytesPerSecond)},"sequence_errors":${usb.sequenceErrors},"malformed_chunks":${usb.malformedChunks},"relay_mode":"jpeg","relay_source_frames":${relay.sourceFrames},"relay_selected_frames":${relay.selectedFrames},"relay_sent_frames":${relay.sentFrames},"relay_sent_bytes":${relay.sentBytes},"relay_mbit_s":${format(relayMegabitsPerSecond)},"relay_last_sent_age_ms":${format(lastSentAgeMs)},"relay_dropped_frames":${relay.droppedFrames},"diagnostic_drops":${relay.diagnosticDrops},"slow_client_disconnects":${relay.slowClientDisconnects},"clients":${relay.clients},"last_source_frame_id":${relay.lastSourceFrameId},"last_sent_frame_id":${relay.lastSentFrameId},"server_error":${serverError?.let { "\"${jsonEscape(it)}\"" } ?: "null"}}"""
    }

    private fun format(value: Double): String = String.format(Locale.US, "%.3f", value)

    private fun jsonEscape(value: String): String = value.replace("\\", "\\\\").replace("\"", "\\\"")

    private fun wifiIpv4Address(): String? = try {
        NetworkInterface.getByName("wlan0")?.inetAddresses?.toList()
            ?.filterIsInstance<Inet4Address>()
            ?.firstOrNull { !it.isLoopbackAddress }
            ?.hostAddress
    } catch (_: Throwable) {
        null
    }

    private fun readRequired(input: InputStream): Int = input.read().also {
        if (it < 0) throw EOFException("unexpected EOF")
    }

    private fun readFully(input: InputStream, destination: ByteArray) {
        var offset = 0
        while (offset < destination.size) {
            val count = input.read(destination, offset, destination.size - offset)
            if (count < 0) throw EOFException("unexpected EOF")
            offset += count
        }
    }

    private fun Socket.closeQuietly() = try {
        close()
    } catch (_: Throwable) {
    }

    private fun ServerSocket.closeQuietly() = try {
        close()
    } catch (_: Throwable) {
    }

    private data class HttpRequest(
        val path: String,
        val headers: Map<String, String>,
        val replaceViewer: Boolean,
    )
}
