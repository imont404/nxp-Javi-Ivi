package com.wavenumber.avc.bridge.relay

import com.wavenumber.avc.bridge.video.AvcJpegFrameView
import com.wavenumber.avc.bridge.video.AvcVideoFrame
import java.nio.ByteBuffer
import java.nio.ByteOrder
import org.junit.Assert.assertArrayEquals
import org.junit.Assert.assertEquals
import org.junit.Assert.assertTrue
import org.junit.Test

class AvcRelayProtocolTest {
    @Test
    fun relayModesParseClientWireNames() {
        assertEquals(AvcRelayVideoMode.RAW, AvcRelayVideoMode.parse("raw"))
        assertEquals(AvcRelayVideoMode.JPEG, AvcRelayVideoMode.parse("JPEG"))
        assertEquals(AvcRelayVideoMode.H264, AvcRelayVideoMode.parse("h264"))
        assertEquals(null, AvcRelayVideoMode.parse("png"))
    }

    @Test
    fun jpegFrameIsEncodedAsOneBoundedWebSocketPayload() {
        val jpeg = byteArrayOf(0xFF.toByte(), 0xD8.toByte(), 1, 2, 0xFF.toByte(), 0xD9.toByte())
        val packet = AvcRelayProtocol.encodeJpegFrame(
            AvcRelayFrame(
                frameId = 55,
                width = 320,
                height = 200,
                capturedNs = 123_456_789,
                bytes = jpeg,
                byteCount = jpeg.size,
                droppedBefore = true,
            ),
        )
        val view = ByteBuffer.wrap(packet).order(ByteOrder.LITTLE_ENDIAN)
        assertEquals(AvcRelayProtocol.JPEG_MAGIC, view.getInt(0))
        assertEquals(AvcRelayProtocol.JPEG_VERSION, view.get(4).toInt())
        assertEquals(AvcRelayProtocol.JPEG_HEADER_BYTES, view.get(5).toInt())
        assertEquals(AvcRelayProtocol.JPEG_FLAG_DROPPED_BEFORE, view.getShort(6).toInt())
        assertEquals(55, view.getInt(8))
        assertEquals(320, view.getShort(12).toInt())
        assertEquals(200, view.getShort(14).toInt())
        assertEquals(jpeg.size, view.getInt(16))
        assertEquals(123_456_789, view.getLong(20))
        assertArrayEquals(jpeg, packet.copyOfRange(AvcRelayProtocol.JPEG_HEADER_BYTES, packet.size))
    }

    @Test
    fun relayMailboxKeepsOnlyNewestEncodedFrame() {
        val mailbox = AvcRelayMailbox(maxFrameBytes = 8)
        for (frameId in 0L..4L) {
            mailbox.noteSourceFrame(frameId)
            mailbox.offerJpegFrame(
                AvcJpegFrameView(frameId, 1, 1, frameId, byteArrayOf(frameId.toByte()), 1),
            )
        }

        val latest = requireNotNull(mailbox.takeLatestFrame())
        assertEquals(4, latest.frameId)
        assertArrayEquals(byteArrayOf(4), latest.bytes.copyOf(latest.byteCount))
        assertTrue(latest.droppedBefore)
        val snapshot = mailbox.snapshot()
        assertEquals(5, snapshot.sourceFrames)
        assertEquals(5, snapshot.selectedFrames)
        assertEquals(4, snapshot.droppedFrames)
        mailbox.releaseSentFrame(latest, sent = true)
        assertEquals(1, mailbox.snapshot().sentFrames)
        assertEquals(1, mailbox.snapshot().sentBytes)
    }

    @Test
    fun failedSendMarksTheNextSelectedFrameAsFollowingADrop() {
        val mailbox = AvcRelayMailbox(maxFrameBytes = 8)
        mailbox.noteSourceFrame(1)
        mailbox.offerJpegFrame(AvcJpegFrameView(1, 1, 1, 1, byteArrayOf(1), 1))
        val failed = requireNotNull(mailbox.takeLatestFrame())
        mailbox.releaseSentFrame(failed, sent = false)
        mailbox.noteSourceFrame(2)
        mailbox.offerJpegFrame(AvcJpegFrameView(2, 1, 1, 2, byteArrayOf(2), 1))

        assertTrue(requireNotNull(mailbox.takeLatestFrame()).droppedBefore)
    }

    @Test
    fun h264InitializationCarriesCodecAndFragmentedMp4() {
        val mp4 = byteArrayOf(0, 0, 0, 8, 'f'.code.toByte(), 't'.code.toByte(), 'y'.code.toByte(), 'p'.code.toByte())
        val encoded = AvcRelayProtocol.encodeH264Packet(
            AvcH264RelayPacket(8, 123, true, true, true, 0x42000d, mp4),
        )
        val view = ByteBuffer.wrap(encoded).order(ByteOrder.LITTLE_ENDIAN)
        assertEquals(AvcRelayProtocol.H264_MAGIC, view.getInt(0))
        assertEquals(
            AvcRelayProtocol.H264_FLAG_INITIALIZATION or
                AvcRelayProtocol.H264_FLAG_KEY_FRAME or
                AvcRelayProtocol.H264_FLAG_DISCONTINUITY,
            view.getShort(6).toInt(),
        )
        assertEquals(0x42000d, view.getInt(28))
        assertArrayEquals(mp4, encoded.copyOfRange(AvcRelayProtocol.H264_HEADER_BYTES, encoded.size))
    }

    @Test
    fun rawRgb565FrameIsCopiedAndEncodedAsOnePayload() {
        val mailbox = AvcRelayMailbox(maxFrameBytes = 8)
        val source = byteArrayOf(0x00, 0xF8.toByte(), 0xE0.toByte(), 0x07)
        mailbox.noteSourceFrame(9)
        mailbox.offerRawFrame(AvcVideoFrame(9, 2, 1, source))
        source.fill(0)

        val frame = requireNotNull(mailbox.takeLatestFrame())
        val packet = AvcRelayProtocol.encodeRawFrame(frame)
        val view = ByteBuffer.wrap(packet).order(ByteOrder.LITTLE_ENDIAN)
        assertEquals(AvcRelayProtocol.RAW_MAGIC, view.getInt(0))
        assertEquals(AvcRelayProtocol.RAW_VERSION, view.get(4).toInt())
        assertEquals(AvcRelayProtocol.RAW_HEADER_BYTES, view.get(5).toInt())
        assertEquals(9, view.getInt(8))
        assertEquals(2, view.getShort(12).toInt())
        assertEquals(1, view.getShort(14).toInt())
        assertEquals(4, view.getInt(16))
        assertEquals(AvcRelayProtocol.RAW_PIXEL_FORMAT_RGB565_LE, view.getInt(28))
        assertArrayEquals(
            byteArrayOf(0x00, 0xF8.toByte(), 0xE0.toByte(), 0x07),
            packet.copyOfRange(AvcRelayProtocol.RAW_HEADER_BYTES, packet.size),
        )
        mailbox.releaseSentFrame(frame, sent = true)
    }
}
