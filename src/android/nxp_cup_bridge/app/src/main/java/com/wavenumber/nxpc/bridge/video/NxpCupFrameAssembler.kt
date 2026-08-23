package com.wavenumber.nxpc.bridge.video

import com.wavenumber.nxpc.bridge.protocol.NxpCupPacket
import com.wavenumber.nxpc.bridge.protocol.NxpCupPayloadDecoder
import com.wavenumber.nxpc.bridge.protocol.NxpCupProtocol
import java.util.ArrayDeque

data class NxpCupVideoFrame(
    val frameId: Long,
    val width: Int,
    val height: Int,
    val pixels: ByteArray,
)

data class NxpCupFrameAssemblerStats(
    val chunks: Long,
    val completedFrames: Long,
    val malformedChunks: Long,
    val abortedFrames: Long,
    val droppedBeforeSignals: Long,
    val noBufferDrops: Long,
)

class LatestFrameMailbox(
    private val frameBytes: Int,
    bufferCount: Int = 3,
) {
    init {
        require(frameBytes > 0)
        require(bufferCount >= 3)
    }

    private val free = ArrayDeque<ByteArray>().apply {
        repeat(bufferCount) { addLast(ByteArray(frameBytes)) }
    }
    private var latest: NxpCupVideoFrame? = null

    @Volatile
    var supersededFrames: Long = 0
        private set

    @Synchronized
    internal fun acquireForAssembly(): ByteArray? {
        free.pollFirst()?.let { return it }
        val stale = latest ?: return null
        latest = null
        supersededFrames++
        return stale.pixels
    }

    @Synchronized
    internal fun publish(frame: NxpCupVideoFrame) {
        require(frame.pixels.size == frameBytes)
        latest?.let {
            supersededFrames++
            free.addLast(it.pixels)
        }
        latest = frame
    }

    @Synchronized
    fun takeLatest(): NxpCupVideoFrame? = latest.also { latest = null }

    @Synchronized
    fun release(frame: NxpCupVideoFrame) {
        require(frame.pixels.size == frameBytes)
        free.addLast(frame.pixels)
    }

    @Synchronized
    internal fun reset() {
        latest?.let { free.addLast(it.pixels) }
        latest = null
        supersededFrames = 0
    }
}

class NxpCupFrameAssembler(
    private val mailbox: LatestFrameMailbox,
    private val width: Int = 320,
    private val height: Int = 200,
    private val onCompletedFrame: ((NxpCupVideoFrame) -> Unit)? = null,
) {
    private val frameBytes = width * height * 2
    private var activeFrameId = -1L
    private var activeBytes: ByteArray? = null
    private var nextOffset = 0

    var chunks: Long = 0
        private set
    var completedFrames: Long = 0
        private set
    var malformedChunks: Long = 0
        private set
    var abortedFrames: Long = 0
        private set
    var droppedBeforeSignals: Long = 0
        private set
    var noBufferDrops: Long = 0
        private set

    fun accept(packet: NxpCupPacket) {
        if (packet.header.messageId != NxpCupProtocol.MSG_RUI_WRITE_FRAME_BUFFER_RAW) return
        chunks++

        if (packet.header.flags and NxpCupProtocol.FLAG_DROPPED_BEFORE != 0) {
            droppedBeforeSignals++
            abortActive()
        }

        val chunk = try {
            NxpCupPayloadDecoder.frameChunk(packet.payload)
        } catch (_: IllegalArgumentException) {
            malformedChunks++
            abortActive()
            return
        }
        val dataBytes = chunk.pixels.size
        val knownChunkFlags =
            NxpCupProtocol.CHUNK_FRAME_START or NxpCupProtocol.CHUNK_FRAME_END or NxpCupProtocol.CHUNK_STALE_OK
        val headerFrameId = packet.header.arg0.toLong() and 0xFFFF_FFFFL
        val headerOffset = packet.header.arg1.toLong() and 0xFFFF_FFFFL
        val headerDataBytes = packet.header.arg2.toLong() and 0xFFFF_FFFFL
        if (
            chunk.totalFrameBytes != frameBytes.toLong() ||
            chunk.width != width ||
            chunk.height != height ||
            chunk.pixelFormat != NxpCupProtocol.PIXEL_FORMAT_RGB565_LE ||
            chunk.bufferId != 0 ||
            chunk.chunkFlags and knownChunkFlags.toLong().inv() != 0L ||
            chunk.frameId != headerFrameId ||
            chunk.byteOffset != headerOffset ||
            dataBytes.toLong() != headerDataBytes ||
            chunk.byteOffset + dataBytes > frameBytes
        ) {
            malformedChunks++
            abortActive()
            return
        }

        val startsFrame = chunk.chunkFlags and NxpCupProtocol.CHUNK_FRAME_START.toLong() != 0L
        val endsFrame = chunk.chunkFlags and NxpCupProtocol.CHUNK_FRAME_END.toLong() != 0L
        if (startsFrame) {
            if (chunk.byteOffset != 0L) {
                malformedChunks++
                abortActive()
                return
            }
            abortActive()
            activeBytes = mailbox.acquireForAssembly()
            if (activeBytes == null) {
                noBufferDrops++
                return
            }
            activeFrameId = chunk.frameId
            nextOffset = 0
        }

        val destination = activeBytes ?: return
        if (chunk.frameId != activeFrameId || chunk.byteOffset != nextOffset.toLong()) {
            malformedChunks++
            abortActive()
            return
        }
        chunk.pixels.copyInto(destination, nextOffset)
        nextOffset += dataBytes

        if (endsFrame) {
            if (nextOffset != frameBytes) {
                malformedChunks++
                abortActive()
                return
            }
            val completedFrame = NxpCupVideoFrame(activeFrameId, width, height, destination)
            try {
                onCompletedFrame?.invoke(completedFrame)
            } catch (_: RuntimeException) {
                // An optional consumer must never interrupt the USB ingest path.
            }
            mailbox.publish(completedFrame)
            completedFrames++
            activeBytes = null
            activeFrameId = -1L
            nextOffset = 0
        } else if (nextOffset >= frameBytes) {
            malformedChunks++
            abortActive()
        }
    }

    fun abortActive() {
        activeBytes?.let {
            mailbox.release(NxpCupVideoFrame(activeFrameId, width, height, it))
            abortedFrames++
        }
        activeBytes = null
        activeFrameId = -1L
        nextOffset = 0
    }

    fun stats(): NxpCupFrameAssemblerStats = NxpCupFrameAssemblerStats(
        chunks,
        completedFrames,
        malformedChunks,
        abortedFrames,
        droppedBeforeSignals,
        noBufferDrops,
    )
}
