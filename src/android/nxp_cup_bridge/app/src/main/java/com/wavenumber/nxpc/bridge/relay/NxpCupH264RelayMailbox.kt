package com.wavenumber.nxpc.bridge.relay

import com.wavenumber.nxpc.bridge.video.NxpCupMp4Fragment
import com.wavenumber.nxpc.bridge.video.NxpCupMp4Initialization
import java.util.ArrayDeque

data class NxpCupH264RelayPacket(
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
class NxpCupH264RelayMailbox(private val capacity: Int = 32) {
    init {
        require(capacity >= 2)
    }

    private val packets = ArrayDeque<NxpCupH264RelayPacket>()
    private var initialization: NxpCupMp4Initialization? = null
    private var waitingForKeyFrame = true
    private var discontinuityPending = true

    @Synchronized
    fun offerInitialization(value: NxpCupMp4Initialization) {
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
    fun offerFragment(fragment: NxpCupMp4Fragment): Int {
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
                NxpCupH264RelayPacket(
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
            NxpCupH264RelayPacket(
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
    fun takePacket(): NxpCupH264RelayPacket? = packets.pollFirst()
}
