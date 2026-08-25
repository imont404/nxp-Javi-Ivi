package com.wavenumber.nxpc.bridge.protocol

import java.nio.ByteBuffer
import java.nio.ByteOrder

data class NxpCupHelloResponse(
    val capabilities: Long,
    val activeChannels: Long,
    val maxPacketBytes: Long,
    val frameWidth: Int,
    val frameHeight: Int,
    val pixelFormat: Int,
    val sessionId: Long,
)

data class NxpCupFrameChunk(
    val frameId: Long,
    val byteOffset: Long,
    val totalFrameBytes: Long,
    val width: Int,
    val height: Int,
    val pixelFormat: Int,
    val bufferId: Int,
    val chunkFlags: Long,
    val pixels: ByteArray,
)

data class NxpCupStatsReport(val counters: List<Long>)

data class NxpCupLogRecord(
    val timestampMs: Long,
    val recordId: Long,
    val level: Int,
    val category: String,
    val text: String,
)

sealed interface NxpCupTelemetryValue {
    data class Signed(val value: Int) : NxpCupTelemetryValue
    data class Unsigned(val value: Long) : NxpCupTelemetryValue
    data class Floating(val value: Float) : NxpCupTelemetryValue
    data class BooleanValue(val value: Boolean) : NxpCupTelemetryValue
    data class Text(val value: String) : NxpCupTelemetryValue
}

data class NxpCupTelemetryScalar(
    val timestampMs: Long,
    val sampleId: Long,
    val name: String,
    val units: String,
    val value: NxpCupTelemetryValue,
)

object NxpCupPayloadDecoder {
    fun hello(payload: ByteArray): NxpCupHelloResponse {
        require(payload.size == NxpCupProtocol.HELLO_RESPONSE_BYTES) { "invalid HELLO payload size" }
        val view = view(payload)
        return NxpCupHelloResponse(
            capabilities = view.u32(),
            activeChannels = view.u32(),
            maxPacketBytes = view.u32(),
            frameWidth = view.u16(),
            frameHeight = view.u16(),
            pixelFormat = view.u16(),
            sessionId = view.apply { u16() }.u32(),
        )
    }

    fun frameChunk(payload: ByteArray): NxpCupFrameChunk {
        require(payload.size > NxpCupProtocol.FRAME_CHUNK_HEADER_BYTES) { "frame chunk has no pixels" }
        val view = view(payload)
        return NxpCupFrameChunk(
            frameId = view.u32(),
            byteOffset = view.u32(),
            totalFrameBytes = view.u32(),
            width = view.u16(),
            height = view.u16(),
            pixelFormat = view.u16(),
            bufferId = view.u16(),
            chunkFlags = view.u32(),
            pixels = payload.copyOfRange(NxpCupProtocol.FRAME_CHUNK_HEADER_BYTES, payload.size),
        )
    }

    fun stats(payload: ByteArray): NxpCupStatsReport {
        require(payload.size == NxpCupProtocol.STATS_REPORT_BYTES) { "invalid stats payload size" }
        val view = view(payload)
        return NxpCupStatsReport(List(19) { view.u32() })
    }

    fun log(payload: ByteArray): NxpCupLogRecord {
        require(payload.size >= NxpCupProtocol.LOG_RECORD_HEADER_BYTES) { "truncated log header" }
        val view = view(payload)
        val timestampMs = view.u32()
        val recordId = view.u32()
        val textBytes = view.u16()
        val level = view.u8()
        val categoryBytes = view.u8()
        require(level <= 4) { "unknown log level" }
        require(NxpCupProtocol.LOG_RECORD_HEADER_BYTES + categoryBytes + textBytes == payload.size) {
            "invalid log string lengths"
        }
        val categoryStart = NxpCupProtocol.LOG_RECORD_HEADER_BYTES
        val textStart = categoryStart + categoryBytes
        return NxpCupLogRecord(
            timestampMs,
            recordId,
            level,
            payload.copyOfRange(categoryStart, textStart).toString(Charsets.UTF_8),
            payload.copyOfRange(textStart, payload.size).toString(Charsets.UTF_8),
        )
    }

    fun telemetry(payload: ByteArray): NxpCupTelemetryScalar {
        require(payload.size >= NxpCupProtocol.TELEMETRY_SCALAR_HEADER_BYTES) { "truncated telemetry header" }
        val view = view(payload)
        val timestampMs = view.u32()
        val sampleId = view.u32()
        val valueBits = view.int
        val nameBytes = view.u16()
        val valueType = view.u8()
        val unitsBytes = view.u8()
        require(nameBytes > 0) { "telemetry name is empty" }
        val textValue = valueType == NxpCupProtocol.TELEMETRY_TEXT
        val textBytes = if (textValue) valueBits else 0
        require(!textValue || (textBytes in 1..NxpCupProtocol.TELEMETRY_TEXT_MAX_BYTES && unitsBytes == 0)) {
            "invalid telemetry text length"
        }
        require(NxpCupProtocol.TELEMETRY_SCALAR_HEADER_BYTES + nameBytes + unitsBytes + textBytes == payload.size) {
            "invalid telemetry string lengths"
        }
        val nameStart = NxpCupProtocol.TELEMETRY_SCALAR_HEADER_BYTES
        val unitsStart = nameStart + nameBytes
        val textStart = unitsStart + unitsBytes
        val value = when (valueType) {
            NxpCupProtocol.TELEMETRY_I32 -> NxpCupTelemetryValue.Signed(valueBits)
            NxpCupProtocol.TELEMETRY_U32 -> NxpCupTelemetryValue.Unsigned(valueBits.toLong() and 0xFFFF_FFFFL)
            NxpCupProtocol.TELEMETRY_F32 -> NxpCupTelemetryValue.Floating(Float.fromBits(valueBits))
            NxpCupProtocol.TELEMETRY_BOOL -> NxpCupTelemetryValue.BooleanValue(valueBits != 0)
            NxpCupProtocol.TELEMETRY_TEXT -> NxpCupTelemetryValue.Text(
                payload.copyOfRange(textStart, payload.size).toString(Charsets.UTF_8),
            )
            else -> throw IllegalArgumentException("unknown telemetry type")
        }
        return NxpCupTelemetryScalar(
            timestampMs,
            sampleId,
            payload.copyOfRange(nameStart, unitsStart).toString(Charsets.UTF_8),
            payload.copyOfRange(unitsStart, textStart).toString(Charsets.UTF_8),
            value,
        )
    }

    private fun view(payload: ByteArray): ByteBuffer =
        ByteBuffer.wrap(payload).order(ByteOrder.LITTLE_ENDIAN)

    private fun ByteBuffer.u8(): Int = get().toInt() and 0xFF
    private fun ByteBuffer.u16(): Int = short.toInt() and 0xFFFF
    private fun ByteBuffer.u32(): Long = int.toLong() and 0xFFFF_FFFFL
}
