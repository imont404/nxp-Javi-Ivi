package com.wavenumber.avc.bridge.relay

import com.wavenumber.avc.bridge.protocol.AvcPacket
import com.wavenumber.avc.bridge.video.AvcVideoFrame
import java.util.ArrayDeque

data class AvcRelayFrame(
    val frameId: Long,
    val width: Int,
    val height: Int,
    val pixels: ByteArray,
    val droppedBefore: Boolean,
)

data class AvcRelaySnapshot(
    val sourceFrames: Long,
    val selectedFrames: Long,
    val sentFrames: Long,
    val droppedFrames: Long,
    val diagnosticDrops: Long,
    val slowClientDisconnects: Long,
    val clients: Int,
    val lastSourceFrameId: Long,
    val lastSentFrameId: Long,
)

class AvcRelayMailbox(
    private val frameBytes: Int,
    private val decimation: Int = 4,
    frameBufferCount: Int = 3,
    private val diagnosticCapacity: Int = 32,
) {
    init {
        require(frameBytes > 0)
        require(decimation > 0)
        require(frameBufferCount >= 3)
        require(diagnosticCapacity > 0)
    }

    private val freeFrames = ArrayDeque<ByteArray>().apply {
        repeat(frameBufferCount) { addLast(ByteArray(frameBytes)) }
    }
    private val diagnostics = ArrayDeque<AvcPacket>()
    private var latestFrame: AvcRelayFrame? = null
    private var sourceFrames = 0L
    private var selectedFrames = 0L
    private var sentFrames = 0L
    private var droppedFrames = 0L
    private var diagnosticDrops = 0L
    private var slowClientDisconnects = 0L
    private var clients = 0
    private var lastSourceFrameId = -1L
    private var lastSentFrameId = -1L
    private var dropPending = false

    @Synchronized
    fun offerSourceFrame(frame: AvcVideoFrame) {
        sourceFrames++
        lastSourceFrameId = frame.frameId
        if ((sourceFrames - 1) % decimation != 0L) return
        selectedFrames++

        val destination = freeFrames.pollFirst() ?: latestFrame?.let {
            latestFrame = null
            droppedFrames++
            dropPending = true
            it.pixels
        } ?: run {
            droppedFrames++
            dropPending = true
            return
        }
        frame.pixels.copyInto(destination)
        val replaced = latestFrame
        if (replaced != null) {
            droppedFrames++
            dropPending = true
            freeFrames.addLast(replaced.pixels)
        }
        latestFrame = AvcRelayFrame(
            frame.frameId,
            frame.width,
            frame.height,
            destination,
            droppedBefore = dropPending,
        )
        dropPending = false
    }

    @Synchronized
    fun takeLatestFrame(): AvcRelayFrame? = latestFrame.also { latestFrame = null }

    @Synchronized
    fun releaseSentFrame(frame: AvcRelayFrame, sent: Boolean) {
        if (sent) {
            sentFrames++
            lastSentFrameId = frame.frameId
        } else {
            droppedFrames++
            dropPending = true
        }
        freeFrames.addLast(frame.pixels)
    }

    @Synchronized
    fun offerDiagnostic(packet: AvcPacket) {
        if (diagnostics.size == diagnosticCapacity) {
            diagnostics.removeFirst()
            diagnosticDrops++
        }
        diagnostics.addLast(packet.copy(payload = packet.payload.copyOf()))
    }

    @Synchronized
    fun takeDiagnostics(maxCount: Int): List<AvcPacket> {
        val result = ArrayList<AvcPacket>(minOf(maxCount, diagnostics.size))
        repeat(minOf(maxCount, diagnostics.size)) { result.add(diagnostics.removeFirst()) }
        return result
    }

    @Synchronized
    fun setClients(value: Int) {
        clients = value
    }

    @Synchronized
    fun noteSlowClientDisconnect() {
        slowClientDisconnects++
    }

    @Synchronized
    fun snapshot(): AvcRelaySnapshot = AvcRelaySnapshot(
        sourceFrames,
        selectedFrames,
        sentFrames,
        droppedFrames,
        diagnosticDrops,
        slowClientDisconnects,
        clients,
        lastSourceFrameId,
        lastSentFrameId,
    )
}
