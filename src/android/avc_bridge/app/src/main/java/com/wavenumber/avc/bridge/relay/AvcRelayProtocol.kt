package com.wavenumber.avc.bridge.relay

import com.wavenumber.avc.bridge.protocol.AvcPacket
import com.wavenumber.avc.bridge.protocol.AvcProtocol
import java.nio.ByteBuffer
import java.nio.ByteOrder

object AvcRelayProtocol {
    const val JPEG_MAGIC = 0x4A435641
    const val JPEG_VERSION = 1
    const val JPEG_HEADER_BYTES = 32
    const val JPEG_FLAG_DROPPED_BEFORE = 1
    const val H264_MAGIC = 0x34435641
    const val H264_VERSION = 1
    const val H264_HEADER_BYTES = 32
    const val H264_FLAG_INITIALIZATION = 1
    const val H264_FLAG_KEY_FRAME = 2
    const val H264_FLAG_DISCONTINUITY = 4
    const val RAW_MAGIC = 0x52435641
    const val RAW_VERSION = 1
    const val RAW_HEADER_BYTES = 32
    const val RAW_FLAG_DROPPED_BEFORE = 1
    const val RAW_PIXEL_FORMAT_RGB565_LE = 1

    fun encodeJpegFrame(frame: AvcRelayFrame): ByteArray =
        ByteBuffer.allocate(JPEG_HEADER_BYTES + frame.byteCount)
            .order(ByteOrder.LITTLE_ENDIAN)
            .putInt(JPEG_MAGIC)
            .put(JPEG_VERSION.toByte())
            .put(JPEG_HEADER_BYTES.toByte())
            .putShort(if (frame.droppedBefore) JPEG_FLAG_DROPPED_BEFORE.toShort() else 0)
            .putInt(frame.frameId.toInt())
            .putShort(frame.width.toShort())
            .putShort(frame.height.toShort())
            .putInt(frame.byteCount)
            .putLong(frame.capturedNs)
            .putInt(0)
            .put(frame.bytes, 0, frame.byteCount)
            .array()

    fun encodeRawFrame(frame: AvcRelayFrame): ByteArray =
        ByteBuffer.allocate(RAW_HEADER_BYTES + frame.byteCount)
            .order(ByteOrder.LITTLE_ENDIAN)
            .putInt(RAW_MAGIC)
            .put(RAW_VERSION.toByte())
            .put(RAW_HEADER_BYTES.toByte())
            .putShort(if (frame.droppedBefore) RAW_FLAG_DROPPED_BEFORE.toShort() else 0)
            .putInt(frame.frameId.toInt())
            .putShort(frame.width.toShort())
            .putShort(frame.height.toShort())
            .putInt(frame.byteCount)
            .putLong(frame.capturedNs)
            .putInt(RAW_PIXEL_FORMAT_RGB565_LE)
            .put(frame.bytes, 0, frame.byteCount)
            .array()

    fun encodeH264Packet(packet: AvcH264RelayPacket): ByteArray {
        var flags = 0
        if (packet.initialization) flags = flags or H264_FLAG_INITIALIZATION
        if (packet.keyFrame) flags = flags or H264_FLAG_KEY_FRAME
        if (packet.discontinuity) flags = flags or H264_FLAG_DISCONTINUITY
        return ByteBuffer.allocate(H264_HEADER_BYTES + packet.bytes.size)
            .order(ByteOrder.LITTLE_ENDIAN)
            .putInt(H264_MAGIC)
            .put(H264_VERSION.toByte())
            .put(H264_HEADER_BYTES.toByte())
            .putShort(flags.toShort())
            .putInt(packet.frameId.toInt())
            .putShort(320.toShort())
            .putShort(200.toShort())
            .putInt(packet.bytes.size)
            .putLong(packet.capturedNs)
            .putInt(packet.codecConfig)
            .put(packet.bytes)
            .array()
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
