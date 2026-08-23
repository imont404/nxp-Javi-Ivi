package com.wavenumber.nxpc.bridge.relay

import com.wavenumber.nxpc.bridge.protocol.NxpCupPacket
import com.wavenumber.nxpc.bridge.video.NxpCupJpegFrameView
import com.wavenumber.nxpc.bridge.video.NxpCupVideoFrame
import java.util.ArrayDeque

data class NxpCupRelayFrame(
    val frameId: Long,
    val width: Int,
    val height: Int,
    val capturedNs: Long,
    val bytes: ByteArray,
    val byteCount: Int,
    val droppedBefore: Boolean,
)

data class NxpCupRelaySnapshot(
    val sourceFrames: Long,
    val selectedFrames: Long,
    val sentFrames: Long,
    val sentBytes: Long,
    val rateWindowBytes: Long,
    val rateWindowFirstNs: Long,
    val rateWindowLastNs: Long,
    val droppedFrames: Long,
    val diagnosticDrops: Long,
    val slowClientDisconnects: Long,
    val clients: Int,
    val lastSourceFrameId: Long,
    val lastSentFrameId: Long,
    val lastSentCapturedNs: Long,
)

private data class SentSample(val timestampNs: Long, val bytes: Int)

class NxpCupRelayMailbox(
    private val maxFrameBytes: Int,
    frameBufferCount: Int = 3,
    private val diagnosticCapacity: Int = 32,
) {
    init {
        require(maxFrameBytes > 0)
        require(frameBufferCount >= 3)
        require(diagnosticCapacity > 0)
    }

    private val freeFrames = ArrayDeque<ByteArray>().apply {
        repeat(frameBufferCount) { addLast(ByteArray(maxFrameBytes)) }
    }
    private val diagnostics = ArrayDeque<NxpCupPacket>()
    private val sentSamples = ArrayDeque<SentSample>()
    private var latestFrame: NxpCupRelayFrame? = null
    private var sourceFrames = 0L
    private var selectedFrames = 0L
    private var sentFrames = 0L
    private var sentBytes = 0L
    private var rateWindowBytes = 0L
    private var droppedFrames = 0L
    private var diagnosticDrops = 0L
    private var slowClientDisconnects = 0L
    private var clients = 0
    private var lastSourceFrameId = -1L
    private var lastSelectedFrameId = -1L
    private var lastSentFrameId = -1L
    private var lastSentCapturedNs = 0L
    private var dropPending = false

    @Synchronized
    fun noteSourceFrame(frameId: Long) {
        sourceFrames++
        lastSourceFrameId = frameId
    }

    @Synchronized
    fun offerJpegFrame(frame: NxpCupJpegFrameView) {
        require(frame.byteCount in 1..maxFrameBytes)
        offerFrame(
            frame.frameId,
            frame.width,
            frame.height,
            frame.capturedNs,
            frame.bytes,
            frame.byteCount,
        )
    }

    @Synchronized
    fun offerRawFrame(frame: NxpCupVideoFrame) {
        require(frame.pixels.size <= maxFrameBytes)
        offerFrame(
            frame.frameId,
            frame.width,
            frame.height,
            System.nanoTime(),
            frame.pixels,
            frame.pixels.size,
        )
    }

    private fun offerFrame(
        frameId: Long,
        width: Int,
        height: Int,
        capturedNs: Long,
        source: ByteArray,
        byteCount: Int,
    ) {
        selectedFrames++
        if (lastSelectedFrameId >= 0 && frameId > lastSelectedFrameId + 1) {
            droppedFrames += frameId - lastSelectedFrameId - 1
            dropPending = true
        }
        lastSelectedFrameId = frameId

        val destination = freeFrames.pollFirst() ?: latestFrame?.let {
            latestFrame = null
            droppedFrames++
            dropPending = true
            it.bytes
        } ?: run {
            droppedFrames++
            dropPending = true
            return
        }
        source.copyInto(destination, endIndex = byteCount)
        val replaced = latestFrame
        if (replaced != null) {
            droppedFrames++
            dropPending = true
            freeFrames.addLast(replaced.bytes)
        }
        latestFrame = NxpCupRelayFrame(
            frameId,
            width,
            height,
            capturedNs,
            destination,
            byteCount,
            droppedBefore = dropPending,
        )
        dropPending = false
    }

    @Synchronized
    fun discardLatestFrame() {
        latestFrame?.let { freeFrames.addLast(it.bytes) }
        latestFrame = null
        dropPending = true
    }

    @Synchronized
    fun noteEncodedFrameSelected(frameId: Long, dropped: Int) {
        selectedFrames++
        lastSelectedFrameId = frameId
        if (dropped > 0) {
            droppedFrames += dropped
            dropPending = true
        }
    }

    @Synchronized
    fun noteEncodedFrameSent(frameId: Long, capturedNs: Long, bytes: Int) {
        val nowNs = System.nanoTime()
        sentFrames++
        sentBytes += bytes
        sentSamples.addLast(SentSample(nowNs, bytes))
        rateWindowBytes += bytes
        while (
            sentSamples.size > 1 &&
            nowNs - checkNotNull(sentSamples.peekFirst()).timestampNs > 5_000_000_000L
        ) {
            rateWindowBytes -= sentSamples.removeFirst().bytes
        }
        lastSentFrameId = frameId
        lastSentCapturedNs = capturedNs
    }

    @Synchronized
    fun takeLatestFrame(): NxpCupRelayFrame? = latestFrame.also { latestFrame = null }

    @Synchronized
    fun releaseSentFrame(frame: NxpCupRelayFrame, sent: Boolean) {
        if (sent) {
            val nowNs = System.nanoTime()
            sentFrames++
            sentBytes += frame.byteCount
            sentSamples.addLast(SentSample(nowNs, frame.byteCount))
            rateWindowBytes += frame.byteCount
            while (
                sentSamples.size > 1 &&
                nowNs - checkNotNull(sentSamples.peekFirst()).timestampNs > 5_000_000_000L
            ) {
                rateWindowBytes -= sentSamples.removeFirst().bytes
            }
            lastSentFrameId = frame.frameId
            lastSentCapturedNs = frame.capturedNs
        } else {
            droppedFrames++
            dropPending = true
        }
        freeFrames.addLast(frame.bytes)
    }

    @Synchronized
    fun offerDiagnostic(packet: NxpCupPacket) {
        if (diagnostics.size == diagnosticCapacity) {
            diagnostics.removeFirst()
            diagnosticDrops++
        }
        diagnostics.addLast(packet.copy(payload = packet.payload.copyOf()))
    }

    @Synchronized
    fun takeDiagnostics(maxCount: Int): List<NxpCupPacket> {
        val result = ArrayList<NxpCupPacket>(minOf(maxCount, diagnostics.size))
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
    fun snapshot(): NxpCupRelaySnapshot = NxpCupRelaySnapshot(
        sourceFrames,
        selectedFrames,
        sentFrames,
        sentBytes,
        rateWindowBytes,
        sentSamples.peekFirst()?.timestampNs ?: 0L,
        sentSamples.peekLast()?.timestampNs ?: 0L,
        droppedFrames,
        diagnosticDrops,
        slowClientDisconnects,
        clients,
        lastSourceFrameId,
        lastSentFrameId,
        lastSentCapturedNs,
    )
}
