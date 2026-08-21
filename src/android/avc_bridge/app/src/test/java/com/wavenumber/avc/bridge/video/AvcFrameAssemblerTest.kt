package com.wavenumber.avc.bridge.video

import com.wavenumber.avc.bridge.protocol.AvcPacket
import com.wavenumber.avc.bridge.protocol.AvcPacketHeader
import com.wavenumber.avc.bridge.protocol.AvcProtocol
import java.nio.ByteBuffer
import java.nio.ByteOrder
import org.junit.Assert.assertArrayEquals
import org.junit.Assert.assertEquals
import org.junit.Assert.assertNull
import org.junit.Test

class AvcFrameAssemblerTest {
    @Test
    fun twoContiguousChunksPublishOneCompleteFrame() {
        val mailbox = LatestFrameMailbox(frameBytes = 8)
        val assembler = AvcFrameAssembler(mailbox, width = 2, height = 2)

        assembler.accept(chunk(frameId = 7, offset = 0, total = 8, flags = AvcProtocol.CHUNK_FRAME_START, data = byteArrayOf(1, 2, 3)))
        assembler.accept(chunk(frameId = 7, offset = 3, total = 8, flags = AvcProtocol.CHUNK_FRAME_END, data = byteArrayOf(4, 5, 6, 7, 8)))

        val frame = requireNotNull(mailbox.takeLatest())
        assertEquals(7, frame.frameId)
        assertArrayEquals(byteArrayOf(1, 2, 3, 4, 5, 6, 7, 8), frame.pixels)
        assertEquals(1, assembler.completedFrames)
        assertEquals(0, assembler.malformedChunks)
        mailbox.release(frame)
    }

    @Test
    fun discontinuityAbortsPartialFrameAndNeverPublishesIt() {
        val mailbox = LatestFrameMailbox(frameBytes = 8)
        val assembler = AvcFrameAssembler(mailbox, width = 2, height = 2)

        assembler.accept(chunk(frameId = 1, offset = 0, total = 8, flags = AvcProtocol.CHUNK_FRAME_START, data = byteArrayOf(1, 2)))
        assembler.accept(chunk(frameId = 1, offset = 4, total = 8, flags = AvcProtocol.CHUNK_FRAME_END, data = byteArrayOf(3, 4, 5, 6)))

        assertNull(mailbox.takeLatest())
        assertEquals(1, assembler.malformedChunks)
        assertEquals(1, assembler.abortedFrames)
    }

    @Test
    fun mailboxKeepsOnlyNewestCompleteFrame() {
        val mailbox = LatestFrameMailbox(frameBytes = 2)
        val firstBuffer = requireNotNull(mailbox.acquireForAssembly()).apply { fill(1) }
        val secondBuffer = requireNotNull(mailbox.acquireForAssembly()).apply { fill(2) }
        mailbox.publish(AvcVideoFrame(1, 1, 1, firstBuffer))
        mailbox.publish(AvcVideoFrame(2, 1, 1, secondBuffer))

        val latest = requireNotNull(mailbox.takeLatest())
        assertEquals(2, latest.frameId)
        assertArrayEquals(byteArrayOf(2, 2), latest.pixels)
        assertEquals(1, mailbox.supersededFrames)
        mailbox.release(latest)
    }

    private fun chunk(
        frameId: Int,
        offset: Int,
        total: Int,
        flags: Int,
        data: ByteArray,
    ): AvcPacket {
        val payload = ByteBuffer.allocate(AvcProtocol.FRAME_CHUNK_HEADER_BYTES + data.size)
            .order(ByteOrder.LITTLE_ENDIAN)
            .putInt(frameId)
            .putInt(offset)
            .putInt(total)
            .putShort(2)
            .putShort(2)
            .putShort(AvcProtocol.PIXEL_FORMAT_RGB565_LE.toShort())
            .putShort(0)
            .putInt(flags)
            .put(data)
            .array()
        return AvcPacket(
            AvcPacketHeader(
                flags = 0,
                messageId = AvcProtocol.MSG_RUI_WRITE_FRAME_BUFFER_RAW,
                sequence = 0,
                payloadLength = payload.size,
                arg0 = frameId,
                arg1 = offset,
                arg2 = data.size,
            ),
            payload,
        )
    }
}
