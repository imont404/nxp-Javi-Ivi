package com.wavenumber.nxpc.bridge.protocol

import java.nio.ByteBuffer
import java.nio.ByteOrder

class NxpCupStreamParser(
    private val onPacket: (NxpCupPacket) -> Unit,
) {
    companion object {
        private const val BUFFER_CAPACITY = 64 * 1024
        private val MAGIC_BYTES = byteArrayOf(0x41, 0x56, 0x43, 0x55)
    }

    private val buffer = ByteArray(BUFFER_CAPACITY)
    private var start = 0
    private var end = 0
    private var lastSequence: Int? = null

    var parsedPackets: Long = 0
        private set
    var resyncEvents: Long = 0
        private set
    var resyncBytes: Long = 0
        private set
    var invalidHeaders: Long = 0
        private set
    var sequenceErrors: Long = 0
        private set

    fun reset() {
        start = 0
        end = 0
        lastSequence = null
    }

    fun beginSequenceWindow() {
        lastSequence = null
        sequenceErrors = 0
    }

    fun push(input: ByteArray, offset: Int = 0, length: Int = input.size - offset) {
        require(offset >= 0 && length >= 0 && offset + length <= input.size)
        require(length <= BUFFER_CAPACITY) { "input exceeds parser capacity" }
        ensureWritable(length)
        input.copyInto(buffer, end, offset, offset + length)
        end += length
        parse()
    }

    private fun ensureWritable(length: Int) {
        if (end + length <= buffer.size) return
        if (start > 0) {
            buffer.copyInto(buffer, 0, start, end)
            end -= start
            start = 0
        }
        if (end + length > buffer.size) {
            throw IllegalStateException("AVCU parser buffer exhausted")
        }
    }

    private fun parse() {
        while (end - start >= NxpCupProtocol.HEADER_BYTES) {
            if (!hasMagicAt(start)) {
                val next = findMagic(start + 1)
                val discard = if (next >= 0) next - start else (end - start - 3)
                if (discard <= 0) return
                start += discard
                resyncEvents++
                resyncBytes += discard.toLong()
                continue
            }

            val headerBuffer = ByteBuffer.wrap(buffer, start, NxpCupProtocol.HEADER_BYTES)
                .order(ByteOrder.LITTLE_ENDIAN)
            val magic = headerBuffer.int
            val version = headerBuffer.get().toInt() and 0xFF
            val headerBytes = headerBuffer.get().toInt() and 0xFF
            val flags = headerBuffer.short.toInt() and 0xFFFF
            val messageId = headerBuffer.int
            val sequence = headerBuffer.int
            val payloadLength = headerBuffer.int
            val arg0 = headerBuffer.int
            val arg1 = headerBuffer.int
            val arg2 = headerBuffer.int

            if (
                magic != NxpCupProtocol.MAGIC ||
                version != NxpCupProtocol.VERSION ||
                headerBytes != NxpCupProtocol.HEADER_BYTES ||
                payloadLength < 0 ||
                payloadLength > NxpCupProtocol.MAX_PAYLOAD_BYTES
            ) {
                start++
                invalidHeaders++
                resyncEvents++
                resyncBytes++
                continue
            }

            val packetBytes = NxpCupProtocol.HEADER_BYTES + payloadLength
            if (end - start < packetBytes) return

            lastSequence?.let { previous ->
                if (sequence != previous + 1) sequenceErrors++
            }
            lastSequence = sequence

            val payloadStart = start + NxpCupProtocol.HEADER_BYTES
            val payload = buffer.copyOfRange(payloadStart, payloadStart + payloadLength)
            onPacket(
                NxpCupPacket(
                    NxpCupPacketHeader(flags, messageId, sequence, payloadLength, arg0, arg1, arg2),
                    payload,
                ),
            )
            parsedPackets++
            start += packetBytes
        }

        if (start == end) {
            start = 0
            end = 0
        }
    }

    private fun hasMagicAt(index: Int): Boolean =
        index + MAGIC_BYTES.size <= end && MAGIC_BYTES.indices.all { buffer[index + it] == MAGIC_BYTES[it] }

    private fun findMagic(from: Int): Int {
        val last = end - MAGIC_BYTES.size
        for (index in from..last) {
            if (hasMagicAt(index)) return index
        }
        return -1
    }
}
