package com.wavenumber.nxpc.bridge.protocol

import java.nio.ByteBuffer
import java.nio.ByteOrder
import org.junit.Assert.assertArrayEquals
import org.junit.Assert.assertEquals
import org.junit.Assert.assertTrue
import org.junit.Test

class NxpCupStreamParserTest {
    @Test
    fun fragmentedHelloResponseMatchesWireLayout() {
        val payload = ByteArray(24) { it.toByte() }
        val packet = packet(
            messageId = NxpCupProtocol.MSG_CONTROL_HELLO,
            sequence = 41,
            flags = NxpCupProtocol.FLAG_RESPONSE,
            arg0 = 7,
            arg2 = 0x12345678,
            payload = payload,
        )
        val parsed = mutableListOf<NxpCupPacket>()
        val parser = NxpCupStreamParser(parsed::add)

        parser.push(packet.copyOfRange(0, 3))
        parser.push(packet.copyOfRange(3, 29))
        parser.push(packet.copyOfRange(29, packet.size))

        assertEquals(1, parsed.size)
        assertEquals(NxpCupProtocol.MSG_CONTROL_HELLO, parsed.single().header.messageId)
        assertEquals(41, parsed.single().header.sequence)
        assertEquals(7, parsed.single().header.arg0)
        assertEquals(0x12345678, parsed.single().header.arg2)
        assertArrayEquals(payload, parsed.single().payload)
        assertEquals(0, parser.resyncEvents)
    }

    @Test
    fun garbageRecoveryAndUnknownMessageRemainForwardCompatible() {
        val unknown = packet(messageId = 0x0100FFEE, sequence = 1, payload = byteArrayOf(9, 8, 7))
        val parsed = mutableListOf<NxpCupPacket>()
        val parser = NxpCupStreamParser(parsed::add)

        parser.push(byteArrayOf(0x13, 0x37, 0x41) + unknown)

        assertEquals(1, parsed.size)
        assertEquals(0x0100FFEE, parsed.single().header.messageId)
        assertArrayEquals(byteArrayOf(9, 8, 7), parsed.single().payload)
        assertEquals(1, parser.resyncEvents)
        assertEquals(3, parser.resyncBytes)
    }

    @Test
    fun invalidLengthResynchronizesToFollowingPacket() {
        val invalid = packet(messageId = NxpCupProtocol.MSG_STATS_REPORT, sequence = 2)
        ByteBuffer.wrap(invalid).order(ByteOrder.LITTLE_ENDIAN).putInt(16, NxpCupProtocol.MAX_PAYLOAD_BYTES + 1)
        val valid = packet(messageId = NxpCupProtocol.MSG_CONTROL_PING, sequence = 3)
        val parsed = mutableListOf<NxpCupPacket>()
        val parser = NxpCupStreamParser(parsed::add)

        parser.push(invalid + valid)

        assertEquals(1, parsed.size)
        assertEquals(NxpCupProtocol.MSG_CONTROL_PING, parsed.single().header.messageId)
        assertEquals(1, parser.invalidHeaders)
    }

    @Test
    fun controlBuilderMatchesTheCHeaderOffsets() {
        val bytes = NxpCupControlPacketBuilder.build(
            sequence = 0x11223344,
            messageId = NxpCupProtocol.MSG_CONTROL_SET_CHANNELS,
            arg0 = NxpCupProtocol.CHANNEL_FRAMES or NxpCupProtocol.CHANNEL_TELEMETRY,
            arg1 = 25,
            arg2 = 0x55667788,
        )
        val view = ByteBuffer.wrap(bytes).order(ByteOrder.LITTLE_ENDIAN)

        assertEquals(32, bytes.size)
        assertEquals(NxpCupProtocol.MAGIC, view.getInt(0))
        assertEquals(NxpCupProtocol.VERSION, view.get(4).toInt())
        assertEquals(NxpCupProtocol.HEADER_BYTES, view.get(5).toInt())
        assertEquals(NxpCupProtocol.MSG_CONTROL_SET_CHANNELS, view.getInt(8))
        assertEquals(0x11223344, view.getInt(12))
        assertEquals(0, view.getInt(16))
        assertEquals(NxpCupProtocol.CHANNEL_FRAMES or NxpCupProtocol.CHANNEL_TELEMETRY, view.getInt(20))
        assertEquals(25, view.getInt(24))
        assertEquals(0x55667788, view.getInt(28))
    }

    @Test
    fun allCurrentPayloadClassesMatchThePackedCLayouts() {
        val framePayload = ByteBuffer.allocate(NxpCupProtocol.FRAME_CHUNK_HEADER_BYTES + 4)
            .order(ByteOrder.LITTLE_ENDIAN)
            .putInt(8)
            .putInt(12)
            .putInt(128_000)
            .putShort(320.toShort())
            .putShort(200.toShort())
            .putShort(NxpCupProtocol.PIXEL_FORMAT_RGB565_LE.toShort())
            .putShort(0)
            .putInt(NxpCupProtocol.CHUNK_FRAME_START or NxpCupProtocol.CHUNK_FRAME_END)
            .put(byteArrayOf(1, 2, 3, 4))
            .array()
        val frame = NxpCupPayloadDecoder.frameChunk(framePayload)
        assertEquals(8, frame.frameId)
        assertEquals(12, frame.byteOffset)
        assertEquals(320, frame.width)
        assertArrayEquals(byteArrayOf(1, 2, 3, 4), frame.pixels)

        val statsPayload = ByteBuffer.allocate(NxpCupProtocol.STATS_REPORT_BYTES)
            .order(ByteOrder.LITTLE_ENDIAN)
            .apply { repeat(19) { putInt(it) } }
            .array()
        assertEquals((0L..18L).toList(), NxpCupPayloadDecoder.stats(statsPayload).counters)

        val category = "usb".toByteArray()
        val text = "ready".toByteArray()
        val logPayload = ByteBuffer.allocate(NxpCupProtocol.LOG_RECORD_HEADER_BYTES + category.size + text.size)
            .order(ByteOrder.LITTLE_ENDIAN)
            .putInt(1234)
            .putInt(9)
            .putShort(text.size.toShort())
            .put(2)
            .put(category.size.toByte())
            .put(category)
            .put(text)
            .array()
        val log = NxpCupPayloadDecoder.log(logPayload)
        assertEquals("usb", log.category)
        assertEquals("ready", log.text)

        val name = "servo.position".toByteArray()
        val units = "us".toByteArray()
        val telemetryPayload = ByteBuffer.allocate(NxpCupProtocol.TELEMETRY_SCALAR_HEADER_BYTES + name.size + units.size)
            .order(ByteOrder.LITTLE_ENDIAN)
            .putInt(4321)
            .putInt(17)
            .putInt(1500)
            .putShort(name.size.toShort())
            .put(NxpCupProtocol.TELEMETRY_I32.toByte())
            .put(units.size.toByte())
            .put(name)
            .put(units)
            .array()
        val telemetry = NxpCupPayloadDecoder.telemetry(telemetryPayload)
        assertEquals("servo.position", telemetry.name)
        assertEquals("us", telemetry.units)
        assertEquals(NxpCupTelemetryValue.Signed(1500), telemetry.value)
    }

    @Test
    fun fragmentedMixedTrafficAndSequenceGapStayObservable() {
        val packets = byteArrayOf(0x55) +
            packet(NxpCupProtocol.MSG_LOG_TEXT, 10, payload = validLogPayload()) +
            packet(
                NxpCupProtocol.MSG_STATS_REPORT,
                12,
                flags = NxpCupProtocol.FLAG_DROPPED_BEFORE,
                payload = ByteArray(NxpCupProtocol.STATS_REPORT_BYTES),
            )
        val parsed = mutableListOf<NxpCupPacket>()
        val parser = NxpCupStreamParser(parsed::add)

        packets.toList().chunked(7).forEach { parser.push(it.toByteArray()) }

        assertEquals(2, parsed.size)
        assertEquals(1, parser.sequenceErrors)
        assertEquals(1, parser.resyncBytes)
        assertTrue(parsed.last().header.flags and NxpCupProtocol.FLAG_DROPPED_BEFORE != 0)
    }

    @Test
    fun fragmentedStreamCarriesEveryCurrentMessageClass() {
        val frame = ByteBuffer.allocate(NxpCupProtocol.FRAME_CHUNK_HEADER_BYTES + 2)
            .order(ByteOrder.LITTLE_ENDIAN)
            .putInt(1)
            .putInt(0)
            .putInt(2)
            .putShort(1)
            .putShort(1)
            .putShort(NxpCupProtocol.PIXEL_FORMAT_RGB565_LE.toShort())
            .putShort(0)
            .putInt(NxpCupProtocol.CHUNK_FRAME_START or NxpCupProtocol.CHUNK_FRAME_END)
            .put(byteArrayOf(0x34, 0x12))
            .array()
        val telemetry = ByteBuffer.allocate(NxpCupProtocol.TELEMETRY_SCALAR_HEADER_BYTES + 1)
            .order(ByteOrder.LITTLE_ENDIAN)
            .putInt(5)
            .putInt(6)
            .putInt(1)
            .putShort(1)
            .put(NxpCupProtocol.TELEMETRY_BOOL.toByte())
            .put(0)
            .put('x'.code.toByte())
            .array()
        val wire = packet(NxpCupProtocol.MSG_RUI_WRITE_FRAME_BUFFER_RAW, 20, payload = frame) +
            packet(NxpCupProtocol.MSG_STATS_REPORT, 21, payload = ByteArray(NxpCupProtocol.STATS_REPORT_BYTES)) +
            packet(NxpCupProtocol.MSG_LOG_TEXT, 22, payload = validLogPayload()) +
            packet(NxpCupProtocol.MSG_TELEMETRY_SCALAR, 23, payload = telemetry) +
            packet(NxpCupProtocol.MSG_CONTROL_CLOSE, 24, flags = NxpCupProtocol.FLAG_RESPONSE)
        val parsed = mutableListOf<NxpCupPacket>()
        val parser = NxpCupStreamParser(parsed::add)

        wire.toList().chunked(13).forEach { parser.push(it.toByteArray()) }

        assertEquals(5, parsed.size)
        assertEquals(0, parser.sequenceErrors)
        assertArrayEquals(byteArrayOf(0x34, 0x12), NxpCupPayloadDecoder.frameChunk(parsed[0].payload).pixels)
        assertEquals(19, NxpCupPayloadDecoder.stats(parsed[1].payload).counters.size)
        assertEquals("x", NxpCupPayloadDecoder.log(parsed[2].payload).category)
        assertEquals("y", NxpCupPayloadDecoder.log(parsed[2].payload).text)
        assertEquals(NxpCupTelemetryValue.BooleanValue(true), NxpCupPayloadDecoder.telemetry(parsed[3].payload).value)
        assertEquals(NxpCupProtocol.MSG_CONTROL_CLOSE, parsed[4].header.messageId)
    }

    @Test(expected = IllegalArgumentException::class)
    fun malformedTelemetryStringLengthsAreRejected() {
        val payload = ByteBuffer.allocate(NxpCupProtocol.TELEMETRY_SCALAR_HEADER_BYTES)
            .order(ByteOrder.LITTLE_ENDIAN)
            .putInt(0)
            .putInt(0)
            .putInt(0)
            .putShort(4)
            .put(NxpCupProtocol.TELEMETRY_U32.toByte())
            .put(0)
            .array()
        NxpCupPayloadDecoder.telemetry(payload)
    }

    @Test
    fun textTelemetryAcceptsTheFortyEightByteBoundary() {
        val name = "system.state".toByteArray()
        val text = "x".repeat(NxpCupProtocol.TELEMETRY_TEXT_MAX_BYTES).toByteArray()
        val payload = ByteBuffer.allocate(NxpCupProtocol.TELEMETRY_SCALAR_HEADER_BYTES + name.size + text.size)
            .order(ByteOrder.LITTLE_ENDIAN)
            .putInt(123)
            .putInt(7)
            .putInt(text.size)
            .putShort(name.size.toShort())
            .put(NxpCupProtocol.TELEMETRY_TEXT.toByte())
            .put(0)
            .put(name)
            .put(text)
            .array()

        val decoded = NxpCupPayloadDecoder.telemetry(payload)
        assertEquals("system.state", decoded.name)
        assertEquals("", decoded.units)
        assertEquals(NxpCupTelemetryValue.Text("x".repeat(48)), decoded.value)
    }

    @Test(expected = IllegalArgumentException::class)
    fun textTelemetryRejectsValuesOverFortyEightBytes() {
        val name = "system.state".toByteArray()
        val text = "x".repeat(NxpCupProtocol.TELEMETRY_TEXT_MAX_BYTES + 1).toByteArray()
        val payload = ByteBuffer.allocate(NxpCupProtocol.TELEMETRY_SCALAR_HEADER_BYTES + name.size + text.size)
            .order(ByteOrder.LITTLE_ENDIAN)
            .putInt(123)
            .putInt(7)
            .putInt(text.size)
            .putShort(name.size.toShort())
            .put(NxpCupProtocol.TELEMETRY_TEXT.toByte())
            .put(0)
            .put(name)
            .put(text)
            .array()

        NxpCupPayloadDecoder.telemetry(payload)
    }

    @Test
    fun newSequenceWindowExcludesPreSessionDiscontinuity() {
        val parsed = mutableListOf<NxpCupPacket>()
        val parser = NxpCupStreamParser(parsed::add)
        parser.push(packet(NxpCupProtocol.MSG_CONTROL_HELLO, 10))
        parser.push(packet(NxpCupProtocol.MSG_CONTROL_SET_CHANNELS, 12))
        assertEquals(1, parser.sequenceErrors)

        parser.beginSequenceWindow()
        parser.push(packet(NxpCupProtocol.MSG_RUI_WRITE_FRAME_BUFFER_RAW, 200))
        parser.push(packet(NxpCupProtocol.MSG_RUI_WRITE_FRAME_BUFFER_RAW, 201))

        assertEquals(0, parser.sequenceErrors)
    }

    private fun validLogPayload(): ByteArray = ByteBuffer.allocate(NxpCupProtocol.LOG_RECORD_HEADER_BYTES + 2)
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
    ): ByteArray = ByteBuffer.allocate(NxpCupProtocol.HEADER_BYTES + payload.size)
        .order(ByteOrder.LITTLE_ENDIAN)
        .putInt(NxpCupProtocol.MAGIC)
        .put(NxpCupProtocol.VERSION.toByte())
        .put(NxpCupProtocol.HEADER_BYTES.toByte())
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
