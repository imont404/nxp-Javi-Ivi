package com.wavenumber.avc.bridge.relay

import com.wavenumber.avc.bridge.video.AvcMp4Fragment
import com.wavenumber.avc.bridge.video.AvcMp4Initialization
import java.util.ArrayDeque

data class AvcH264RelayPacket(
    val frameId: Long,
    val capturedNs: Long,
    val initialization: Boolean,
    val keyFrame: Boolean,
    val discontinuity: Boolean,
    val codecConfig: Int,
    val bytes: ByteArray,
)

/**
 * A bounded dependent-frame queue. Any discontinuity discards inter frames until the
 * next IDR, then re-sends the MP4 initialization segment before resuming media.
 */
class AvcH264RelayMailbox(private val capacity: Int = 32) {
    init {
        require(capacity >= 2)
    }

    private val packets = ArrayDeque<AvcH264RelayPacket>()
    private var initialization: AvcMp4Initialization? = null
    private var waitingForKeyFrame = true
    private var discontinuityPending = true

    @Synchronized
    fun offerInitialization(value: AvcMp4Initialization) {
        initialization = value.copy(bytes = value.bytes.copyOf())
        resetForViewer()
    }

    @Synchronized
    fun resetForViewer() {
        packets.clear()
        waitingForKeyFrame = true
        discontinuityPending = true
    }

    @Synchronized
    fun offerFragment(fragment: AvcMp4Fragment): Int {
        val init = initialization ?: return 1
        if (packets.size >= capacity) {
            val dropped = packets.count { !it.initialization }
            packets.clear()
            waitingForKeyFrame = true
            discontinuityPending = true
            if (!fragment.keyFrame) return dropped + 1
        }
        if (waitingForKeyFrame) {
            if (!fragment.keyFrame) return 1
            packets.addLast(
                AvcH264RelayPacket(
                    frameId = fragment.frameId,
                    capturedNs = fragment.capturedNs,
                    initialization = true,
                    keyFrame = true,
                    discontinuity = discontinuityPending,
                    codecConfig = init.codecConfig,
                    bytes = init.bytes,
                ),
            )
            waitingForKeyFrame = false
            discontinuityPending = false
        }
        packets.addLast(
            AvcH264RelayPacket(
                frameId = fragment.frameId,
                capturedNs = fragment.capturedNs,
                initialization = false,
                keyFrame = fragment.keyFrame,
                discontinuity = false,
                codecConfig = init.codecConfig,
                bytes = fragment.bytes,
            ),
        )
        return 0
    }

    @Synchronized
    fun takePacket(): AvcH264RelayPacket? = packets.pollFirst()
}
