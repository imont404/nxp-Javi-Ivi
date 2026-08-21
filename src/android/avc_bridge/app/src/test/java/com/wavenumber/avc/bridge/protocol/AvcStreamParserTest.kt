package com.wavenumber.avc.bridge.protocol

import java.nio.ByteBuffer
import java.nio.ByteOrder
import org.junit.Assert.assertArrayEquals
import org.junit.Assert.assertEquals
import org.junit.Assert.assertTrue
import org.junit.Test

class AvcStreamParserTest {
    @Test
    fun fragmentedHelloResponseMatchesWireLayout() {
        val payload = ByteArray(24) { it.toByte() }
        val packet = packet(
            messageId = AvcProtocol.MSG_CONTROL_HELLO,
            sequence = 41,
            flags = AvcProtocol.FLAG_RESPONSE,
            arg0 = 7,
            arg2 = 0x12345678,
            payload = payload,
        )
        val parsed = mutableListOf<AvcPacket>()
        val parser = AvcStreamParser(parsed::add)

        parser.push(packet.copyOfRange(0, 3))
        parser.push(packet.copyOfRange(3, 29))
        parser.push(packet.copyOfRange(29, packet.size))

        assertEquals(1, parsed.size)
        assertEquals(AvcProtocol.MSG_CONTROL_HELLO, parsed.single().header.messageId)
        assertEquals(41, parsed.single().header.sequence)
        assertEquals(7, parsed.single().header.arg0)
        assertEquals(0x12345678, parsed.single().header.arg2)
        assertArrayEquals(payload, parsed.single().payload)
        assertEquals(0, parser.resyncEvents)
    }

    @Test
    fun garbageRecoveryAndUnknownMessageRemainForwardCompatible() {
        val unknown = packet(messageId = 0x0100FFEE, sequence = 1, payload = byteArrayOf(9, 8, 7))
        val parsed = mutableListOf<AvcPacket>()
        val parser = AvcStreamParser(parsed::add)

        parser.push(byteArrayOf(0x13, 0x37, 0x41) + unknown)

        assertEquals(1, parsed.size)
        assertEquals(0x0100FFEE, parsed.single().header.messageId)
        assertArrayEquals(byteArrayOf(9, 8, 7), parsed.single().payload)
        assertEquals(1, parser.resyncEvents)
        assertEquals(3, parser.resyncBytes)
    }

    @Test
    fun invalidLengthResynchronizesToFollowingPacket() {
        val invalid = packet(messageId = AvcProtocol.MSG_STATS_REPORT, sequence = 2)
        ByteBuffer.wrap(invalid).order(ByteOrder.LITTLE_ENDIAN).putInt(16, AvcProtocol.MAX_PAYLOAD_BYTES + 1)
        val valid = packet(messageId = AvcProtocol.MSG_CONTROL_PING, sequence = 3)
        val parsed = mutableListOf<AvcPacket>()
        val parser = AvcStreamParser(parsed::add)

        parser.push(invalid + valid)

        assertEquals(1, parsed.size)
        assertEquals(AvcProtocol.MSG_CONTROL_PING, parsed.single().header.messageId)
        assertEquals(1, parser.invalidHeaders)
    }

    @Test
    fun controlBuilderMatchesTheCHeaderOffsets() {
        val bytes = AvcControlPacketBuilder.build(
            sequence = 0x11223344,
            messageId = AvcProtocol.MSG_CONTROL_SET_CHANNELS,
            arg0 = AvcProtocol.CHANNEL_FRAMES or AvcProtocol.CHANNEL_TELEMETRY,
            arg1 = 25,
            arg2 = 0x55667788,
        )
        val view = ByteBuffer.wrap(bytes).order(ByteOrder.LITTLE_ENDIAN)

        assertEquals(32, bytes.size)
        assertEquals(AvcProtocol.MAGIC, view.getInt(0))
        assertEquals(AvcProtocol.VERSION, view.get(4).toInt())
        assertEquals(AvcProtocol.HEADER_BYTES, view.get(5).toInt())
        assertEquals(AvcProtocol.MSG_CONTROL_SET_CHANNELS, view.getInt(8))
        assertEquals(0x11223344, view.getInt(12))
        assertEquals(0, view.getInt(16))
        assertEquals(AvcProtocol.CHANNEL_FRAMES or AvcProtocol.CHANNEL_TELEMETRY, view.getInt(20))
        assertEquals(25, view.getInt(24))
        assertEquals(0x55667788, view.getInt(28))
    }

    @Test
    fun allCurrentPayloadClassesMatchThePackedCLayouts() {
        val framePayload = ByteBuffer.allocate(AvcProtocol.FRAME_CHUNK_HEADER_BYTES + 4)
            .order(ByteOrder.LITTLE_ENDIAN)
            .putInt(8)
            .putInt(12)
            .putInt(128_000)
            .putShort(320.toShort())
            .putShort(200.toShort())
            .putShort(AvcProtocol.PIXEL_FORMAT_RGB565_LE.toShort())
            .putShort(0)
            .putInt(AvcProtocol.CHUNK_FRAME_START or AvcProtocol.CHUNK_FRAME_END)
            .put(byteArrayOf(1, 2, 3, 4))
            .array()
        val frame = AvcPayloadDecoder.frameChunk(framePayload)
        assertEquals(8, frame.frameId)
        assertEquals(12, frame.byteOffset)
        assertEquals(320, frame.width)
        assertArrayEquals(byteArrayOf(1, 2, 3, 4), frame.pixels)

        val statsPayload = ByteBuffer.allocate(AvcProtocol.STATS_REPORT_BYTES)
            .order(ByteOrder.LITTLE_ENDIAN)
            .apply { repeat(19) { putInt(it) } }
            .array()
        assertEquals((0L..18L).toList(), AvcPayloadDecoder.stats(statsPayload).counters)

        val category = "usb".toByteArray()
        val text = "ready".toByteArray()
        val logPayload = ByteBuffer.allocate(AvcProtocol.LOG_RECORD_HEADER_BYTES + category.size + text.size)
            .order(ByteOrder.LITTLE_ENDIAN)
            .putInt(1234)
            .putInt(9)
            .putShort(text.size.toShort())
            .put(2)
            .put(category.size.toByte())
            .put(category)
            .put(text)
            .array()
        val log = AvcPayloadDecoder.log(logPayload)
        assertEquals("usb", log.category)
        assertEquals("ready", log.text)

        val name = "servo.position".toByteArray()
        val units = "us".toByteArray()
        val telemetryPayload = ByteBuffer.allocate(AvcProtocol.TELEMETRY_SCALAR_HEADER_BYTES + name.size + units.size)
            .order(ByteOrder.LITTLE_ENDIAN)
            .putInt(4321)
            .putInt(17)
            .putInt(1500)
            .putShort(name.size.toShort())
            .put(AvcProtocol.TELEMETRY_I32.toByte())
            .put(units.size.toByte())
            .put(name)
            .put(units)
            .array()
        val telemetry = AvcPayloadDecoder.telemetry(telemetryPayload)
        assertEquals("servo.position", telemetry.name)
        assertEquals("us", telemetry.units)
        assertEquals(AvcTelemetryValue.Signed(1500), telemetry.value)
    }

    @Test
    fun fragmentedMixedTrafficAndSequenceGapStayObservable() {
        val packets = byteArrayOf(0x55) +
            packet(AvcProtocol.MSG_LOG_TEXT, 10, payload = validLogPayload()) +
            packet(
                AvcProtocol.MSG_STATS_REPORT,
                12,
                flags = AvcProtocol.FLAG_DROPPED_BEFORE,
                payload = ByteArray(AvcProtocol.STATS_REPORT_BYTES),
            )
        val parsed = mutableListOf<AvcPacket>()
        val parser = AvcStreamParser(parsed::add)

        packets.toList().chunked(7).forEach { parser.push(it.toByteArray()) }

        assertEquals(2, parsed.size)
        assertEquals(1, parser.sequenceErrors)
        assertEquals(1, parser.resyncBytes)
        assertTrue(parsed.last().header.flags and AvcProtocol.FLAG_DROPPED_BEFORE != 0)
    }

    @Test
    fun fragmentedStreamCarriesEveryCurrentMessageClass() {
        val frame = ByteBuffer.allocate(AvcProtocol.FRAME_CHUNK_HEADER_BYTES + 2)
            .order(ByteOrder.LITTLE_ENDIAN)
            .putInt(1)
            .putInt(0)
            .putInt(2)
            .putShort(1)
            .putShort(1)
            .putShort(AvcProtocol.PIXEL_FORMAT_RGB565_LE.toShort())
            .putShort(0)
            .putInt(AvcProtocol.CHUNK_FRAME_START or AvcProtocol.CHUNK_FRAME_END)
            .put(byteArrayOf(0x34, 0x12))
            .array()
        val telemetry = ByteBuffer.allocate(AvcProtocol.TELEMETRY_SCALAR_HEADER_BYTES + 1)
            .order(ByteOrder.LITTLE_ENDIAN)
            .putInt(5)
            .putInt(6)
            .putInt(1)
            .putShort(1)
            .put(AvcProtocol.TELEMETRY_BOOL.toByte())
            .put(0)
            .put('x'.code.toByte())
            .array()
        val wire = packet(AvcProtocol.MSG_RUI_WRITE_FRAME_BUFFER_RAW, 20, payload = frame) +
            packet(AvcProtocol.MSG_STATS_REPORT, 21, payload = ByteArray(AvcProtocol.STATS_REPORT_BYTES)) +
            packet(AvcProtocol.MSG_LOG_TEXT, 22, payload = validLogPayload()) +
            packet(AvcProtocol.MSG_TELEMETRY_SCALAR, 23, payload = telemetry) +
            packet(AvcProtocol.MSG_CONTROL_CLOSE, 24, flags = AvcProtocol.FLAG_RESPONSE)
        val parsed = mutableListOf<AvcPacket>()
        val parser = AvcStreamParser(parsed::add)

        wire.toList().chunked(13).forEach { parser.push(it.toByteArray()) }

        assertEquals(5, parsed.size)
        assertEquals(0, parser.sequenceErrors)
        assertArrayEquals(byteArrayOf(0x34, 0x12), AvcPayloadDecoder.frameChunk(parsed[0].payload).pixels)
        assertEquals(19, AvcPayloadDecoder.stats(parsed[1].payload).counters.size)
        assertEquals("x", AvcPayloadDecoder.log(parsed[2].payload).category)
        assertEquals("y", AvcPayloadDecoder.log(parsed[2].payload).text)
        assertEquals(AvcTelemetryValue.BooleanValue(true), AvcPayloadDecoder.telemetry(parsed[3].payload).value)
        assertEquals(AvcProtocol.MSG_CONTROL_CLOSE, parsed[4].header.messageId)
    }

    @Test(expected = IllegalArgumentException::class)
    fun malformedTelemetryStringLengthsAreRejected() {
        val payload = ByteBuffer.allocate(AvcProtocol.TELEMETRY_SCALAR_HEADER_BYTES)
            .order(ByteOrder.LITTLE_ENDIAN)
            .putInt(0)
            .putInt(0)
            .putInt(0)
            .putShort(4)
            .put(AvcProtocol.TELEMETRY_U32.toByte())
            .put(0)
            .array()
        AvcPayloadDecoder.telemetry(payload)
    }

    @Test
    fun newSequenceWindowExcludesPreSessionDiscontinuity() {
        val parsed = mutableListOf<AvcPacket>()
        val parser = AvcStreamParser(parsed::add)
        parser.push(packet(AvcProtocol.MSG_CONTROL_HELLO, 10))
        parser.push(packet(AvcProtocol.MSG_CONTROL_SET_CHANNELS, 12))
        assertEquals(1, parser.sequenceErrors)

        parser.beginSequenceWindow()
        parser.push(packet(AvcProtocol.MSG_RUI_WRITE_FRAME_BUFFER_RAW, 200))
        parser.push(packet(AvcProtocol.MSG_RUI_WRITE_FRAME_BUFFER_RAW, 201))

        assertEquals(0, parser.sequenceErrors)
    }

    private fun validLogPayload(): ByteArray = ByteBuffer.allocate(AvcProtocol.LOG_RECORD_HEADER_BYTES + 2)
        .order(ByteOrder.LITTLE_ENDIAN)
        .putInt(1)
        .putInt(2)
        .putShort(1)
        .put(1)
        .put(1)
        .put('x'.code.toByte())
        .put('y'.code.toByte())
        .array()

    private fun packet(
        messageId: Int,
        sequence: Int,
        flags: Int = 0,
        arg0: Int = 0,
        arg1: Int = 0,
        arg2: Int = 0,
        payload: ByteArray = byteArrayOf(),
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
