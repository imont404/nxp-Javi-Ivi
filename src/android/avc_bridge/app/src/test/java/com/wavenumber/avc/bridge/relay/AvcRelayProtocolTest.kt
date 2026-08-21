package com.wavenumber.avc.bridge.relay

import com.wavenumber.avc.bridge.protocol.AvcProtocol
import com.wavenumber.avc.bridge.video.AvcVideoFrame
import java.nio.ByteBuffer
import java.nio.ByteOrder
import org.junit.Assert.assertArrayEquals
import org.junit.Assert.assertEquals
import org.junit.Assert.assertTrue
import org.junit.Test

class AvcRelayProtocolTest {
    @Test
    fun completeFrameIsRechunkedIntoStandardAvcuPackets() {
        val pixels = ByteArray(320 * 200 * 2) { (it and 0xFF).toByte() }
        val encoded = AvcRelayProtocol.encodeFrame(
            AvcVideoFrame(55, 320, 200, pixels),
            firstSequence = 100,
            droppedBefore = true,
        )
        val reassembled = ByteArray(pixels.size)
        var expectedOffset = 0
        encoded.packets.forEachIndexed { index, packet ->
            val view = ByteBuffer.wrap(packet).order(ByteOrder.LITTLE_ENDIAN)
            assertEquals(AvcProtocol.MAGIC, view.getInt(0))
            assertEquals(AvcProtocol.MSG_RUI_WRITE_FRAME_BUFFER_RAW, view.getInt(8))
            assertEquals(100 + index, view.getInt(12))
            assertEquals(55, view.getInt(20))
            assertEquals(expectedOffset, view.getInt(24))
            val dataBytes = view.getInt(28)
            val payloadOffset = AvcProtocol.HEADER_BYTES
            val chunkFlags = view.getInt(payloadOffset + 20)
            if (index == 0) {
                assertTrue(view.getShort(6).toInt() and AvcProtocol.FLAG_DROPPED_BEFORE != 0)
                assertTrue(chunkFlags and AvcProtocol.CHUNK_FRAME_START != 0)
            }
            if (index == encoded.packets.lastIndex) {
                assertTrue(chunkFlags and AvcProtocol.CHUNK_FRAME_END != 0)
            }
            packet.copyInto(
                reassembled,
                expectedOffset,
                payloadOffset + AvcProtocol.FRAME_CHUNK_HEADER_BYTES,
                payloadOffset + AvcProtocol.FRAME_CHUNK_HEADER_BYTES + dataBytes,
            )
            expectedOffset += dataBytes
        }
        assertEquals(8, encoded.packets.size)
        assertEquals(108, encoded.nextSequence)
        assertArrayEquals(pixels, reassembled)
    }

    @Test
    fun relayMailboxDecimatesAndKeepsOnlyNewestSelectedFrame() {
        val mailbox = AvcRelayMailbox(frameBytes = 2, decimation = 2)
        for (frameId in 0L..4L) {
            mailbox.offerSourceFrame(
                AvcVideoFrame(frameId, 1, 1, byteArrayOf(frameId.toByte(), frameId.toByte())),
            )
        }

        val latest = requireNotNull(mailbox.takeLatestFrame())
        assertEquals(4, latest.frameId)
        assertArrayEquals(byteArrayOf(4, 4), latest.pixels)
        assertTrue(latest.droppedBefore)
        val snapshot = mailbox.snapshot()
        assertEquals(5, snapshot.sourceFrames)
        assertEquals(3, snapshot.selectedFrames)
        assertEquals(2, snapshot.droppedFrames)
        mailbox.releaseSentFrame(latest, sent = true)
        assertEquals(1, mailbox.snapshot().sentFrames)
    }

    @Test
    fun failedSendMarksTheNextSelectedFrameAsFollowingADrop() {
        val mailbox = AvcRelayMailbox(frameBytes = 2, decimation = 1)
        mailbox.offerSourceFrame(AvcVideoFrame(1, 1, 1, byteArrayOf(1, 1)))
        val failed = requireNotNull(mailbox.takeLatestFrame())
        mailbox.releaseSentFrame(failed, sent = false)
        mailbox.offerSourceFrame(AvcVideoFrame(2, 1, 1, byteArrayOf(2, 2)))

        assertTrue(requireNotNull(mailbox.takeLatestFrame()).droppedBefore)
    }
}
