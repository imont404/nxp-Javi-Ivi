package com.wavenumber.avc.bridge.protocol

import java.nio.ByteBuffer
import java.nio.ByteOrder

data class AvcHelloResponse(
    val capabilities: Long,
    val activeChannels: Long,
    val maxPacketBytes: Long,
    val frameWidth: Int,
    val frameHeight: Int,
    val pixelFormat: Int,
    val sessionId: Long,
)

data class AvcFrameChunk(
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

data class AvcStatsReport(val counters: List<Long>)

data class AvcLogRecord(
    val timestampMs: Long,
    val recordId: Long,
    val level: Int,
    val category: String,
    val text: String,
)

sealed interface AvcTelemetryValue {
    data class Signed(val value: Int) : AvcTelemetryValue
    data class Unsigned(val value: Long) : AvcTelemetryValue
    data class Floating(val value: Float) : AvcTelemetryValue
    data class BooleanValue(val value: Boolean) : AvcTelemetryValue
}

data class AvcTelemetryScalar(
    val timestampMs: Long,
    val sampleId: Long,
    val name: String,
    val units: String,
    val value: AvcTelemetryValue,
)

object AvcPayloadDecoder {
    fun hello(payload: ByteArray): AvcHelloResponse {
        require(payload.size == AvcProtocol.HELLO_RESPONSE_BYTES) { "invalid HELLO payload size" }
        val view = view(payload)
        return AvcHelloResponse(
            capabilities = view.u32(),
            activeChannels = view.u32(),
            maxPacketBytes = view.u32(),
            frameWidth = view.u16(),
            frameHeight = view.u16(),
            pixelFormat = view.u16(),
            sessionId = view.apply { u16() }.u32(),
        )
    }

    fun frameChunk(payload: ByteArray): AvcFrameChunk {
        require(payload.size > AvcProtocol.FRAME_CHUNK_HEADER_BYTES) { "frame chunk has no pixels" }
        val view = view(payload)
        return AvcFrameChunk(
            frameId = view.u32(),
            byteOffset = view.u32(),
            totalFrameBytes = view.u32(),
            width = view.u16(),
            height = view.u16(),
            pixelFormat = view.u16(),
            bufferId = view.u16(),
            chunkFlags = view.u32(),
            pixels = payload.copyOfRange(AvcProtocol.FRAME_CHUNK_HEADER_BYTES, payload.size),
        )
    }

    fun stats(payload: ByteArray): AvcStatsReport {
        require(payload.size == AvcProtocol.STATS_REPORT_BYTES) { "invalid stats payload size" }
        val view = view(payload)
        return AvcStatsReport(List(19) { view.u32() })
    }

    fun log(payload: ByteArray): AvcLogRecord {
        require(payload.size >= AvcProtocol.LOG_RECORD_HEADER_BYTES) { "truncated log header" }
        val view = view(payload)
        val timestampMs = view.u32()
        val recordId = view.u32()
        val textBytes = view.u16()
        val level = view.u8()
        val categoryBytes = view.u8()
        require(level <= 4) { "unknown log level" }
        require(AvcProtocol.LOG_RECORD_HEADER_BYTES + categoryBytes + textBytes == payload.size) {
            "invalid log string lengths"
        }
        val categoryStart = AvcProtocol.LOG_RECORD_HEADER_BYTES
        val textStart = categoryStart + categoryBytes
        return AvcLogRecord(
            timestampMs,
            recordId,
            level,
            payload.copyOfRange(categoryStart, textStart).toString(Charsets.UTF_8),
            payload.copyOfRange(textStart, payload.size).toString(Charsets.UTF_8),
        )
    }

    fun telemetry(payload: ByteArray): AvcTelemetryScalar {
        require(payload.size >= AvcProtocol.TELEMETRY_SCALAR_HEADER_BYTES) { "truncated telemetry header" }
        val view = view(payload)
        val timestampMs = view.u32()
        val sampleId = view.u32()
        val valueBits = view.int
        val nameBytes = view.u16()
        val valueType = view.u8()
        val unitsBytes = view.u8()
        require(nameBytes > 0) { "telemetry name is empty" }
        require(AvcProtocol.TELEMETRY_SCALAR_HEADER_BYTES + nameBytes + unitsBytes == payload.size) {
            "invalid telemetry string lengths"
        }
        val nameStart = AvcProtocol.TELEMETRY_SCALAR_HEADER_BYTES
        val unitsStart = nameStart + nameBytes
        val value = when (valueType) {
            AvcProtocol.TELEMETRY_I32 -> AvcTelemetryValue.Signed(valueBits)
            AvcProtocol.TELEMETRY_U32 -> AvcTelemetryValue.Unsigned(valueBits.toLong() and 0xFFFF_FFFFL)
            AvcProtocol.TELEMETRY_F32 -> AvcTelemetryValue.Floating(Float.fromBits(valueBits))
            AvcProtocol.TELEMETRY_BOOL -> AvcTelemetryValue.BooleanValue(valueBits != 0)
            else -> throw IllegalArgumentException("unknown telemetry type")
        }
        return AvcTelemetryScalar(
            timestampMs,
            sampleId,
            payload.copyOfRange(nameStart, unitsStart).toString(Charsets.UTF_8),
            payload.copyOfRange(unitsStart, payload.size).toString(Charsets.UTF_8),
            value,
        )
    }

    private fun view(payload: ByteArray): ByteBuffer =
        ByteBuffer.wrap(payload).order(ByteOrder.LITTLE_ENDIAN)

    private fun ByteBuffer.u8(): Int = get().toInt() and 0xFF
    private fun ByteBuffer.u16(): Int = short.toInt() and 0xFFFF
    private fun ByteBuffer.u32(): Long = int.toLong() and 0xFFFF_FFFFL
}
