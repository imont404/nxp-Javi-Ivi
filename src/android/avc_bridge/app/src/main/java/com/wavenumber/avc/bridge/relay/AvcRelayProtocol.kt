package com.wavenumber.avc.bridge.relay

import com.wavenumber.avc.bridge.protocol.AvcPacket
import com.wavenumber.avc.bridge.protocol.AvcProtocol
import com.wavenumber.avc.bridge.video.AvcVideoFrame
import java.nio.ByteBuffer
import java.nio.ByteOrder

data class AvcEncodedFrame(
    val packets: List<ByteArray>,
    val nextSequence: Int,
)

object AvcRelayProtocol {
    private const val FRAME_DATA_BYTES =
        AvcProtocol.MAX_PAYLOAD_BYTES - AvcProtocol.FRAME_CHUNK_HEADER_BYTES

    fun encodeFrame(
        frame: AvcVideoFrame,
        firstSequence: Int,
        droppedBefore: Boolean,
    ): AvcEncodedFrame {
        val packets = ArrayList<ByteArray>()
        var sequence = firstSequence
        var offset = 0
        while (offset < frame.pixels.size) {
            val dataBytes = minOf(FRAME_DATA_BYTES, frame.pixels.size - offset)
            val startsFrame = offset == 0
            val endsFrame = offset + dataBytes == frame.pixels.size
            val chunkFlags =
                (if (startsFrame) AvcProtocol.CHUNK_FRAME_START else 0) or
                    (if (endsFrame) AvcProtocol.CHUNK_FRAME_END else 0)
            val payload = ByteBuffer.allocate(AvcProtocol.FRAME_CHUNK_HEADER_BYTES + dataBytes)
                .order(ByteOrder.LITTLE_ENDIAN)
                .putInt(frame.frameId.toInt())
                .putInt(offset)
                .putInt(frame.pixels.size)
                .putShort(frame.width.toShort())
                .putShort(frame.height.toShort())
                .putShort(AvcProtocol.PIXEL_FORMAT_RGB565_LE.toShort())
                .putShort(0)
                .putInt(chunkFlags)
                .put(frame.pixels, offset, dataBytes)
                .array()
            packets.add(
                encodePacket(
                    sequence = sequence,
                    messageId = AvcProtocol.MSG_RUI_WRITE_FRAME_BUFFER_RAW,
                    flags = if (startsFrame && droppedBefore) AvcProtocol.FLAG_DROPPED_BEFORE else 0,
                    arg0 = frame.frameId.toInt(),
                    arg1 = offset,
                    arg2 = dataBytes,
                    payload = payload,
                ),
            )
            sequence++
            offset += dataBytes
        }
        return AvcEncodedFrame(packets, sequence)
    }

    fun encodeDiagnostic(packet: AvcPacket, sequence: Int): ByteArray = encodePacket(
        sequence = sequence,
        messageId = packet.header.messageId,
        flags = packet.header.flags and AvcProtocol.FLAG_DROPPED_BEFORE,
        arg0 = packet.header.arg0,
        arg1 = packet.header.arg1,
        arg2 = packet.header.arg2,
        payload = packet.payload,
    )

    private fun encodePacket(
        sequence: Int,
        messageId: Int,
        flags: Int,
        arg0: Int,
        arg1: Int,
        arg2: Int,
        payload: ByteArray,
    ): ByteArray = ByteBuffer.allocate(AvcProtocol.HEADER_BYTES + payload.size)
        .order(ByteOrder.LITTLE_ENDIAN)
        .putInt(AvcProtocol.MAGIC)
        .put(AvcProtocol.VERSION.toByte())
        .put(AvcProtocol.HEADER_BYTES.toByte())
        .putShort(flags.toShort())
        .putInt(messageId)
        .putInt(sequence)
        .putInt(payload.size)
        .putInt(arg0)
        .putInt(arg1)
        .putInt(arg2)
        .put(payload)
        .array()
}
