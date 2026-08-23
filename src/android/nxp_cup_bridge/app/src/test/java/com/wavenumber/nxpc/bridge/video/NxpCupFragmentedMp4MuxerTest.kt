package com.wavenumber.nxpc.bridge.video

import java.nio.ByteBuffer
import java.nio.ByteOrder
import org.junit.Assert.assertEquals
import org.junit.Assert.assertTrue
import org.junit.Test

class NxpCupFragmentedMp4MuxerTest {
    private val sps = byteArrayOf(
        0, 0, 0, 1, 0x67, 0x42, 0x00, 0x0d, 0x8d.toByte(), 0x8d.toByte(), 0x40,
    )
    private val pps = byteArrayOf(0, 0, 0, 1, 0x68, 0xca.toByte(), 0x43, 0xc8.toByte())

    @Test
    fun writesMseInitializationAndLengthPrefixedMediaFragment() {
        val muxer = NxpCupFragmentedMp4Muxer()
        val initialization = muxer.initialization(NxpCupH264FormatView(sps, pps))
        assertEquals(0x42000d, initialization.codecConfig)
        assertEquals(listOf("ftyp", "moov"), topLevelBoxes(initialization.bytes).map { it.first })
        assertTrue(indexOfAscii(initialization.bytes, "avcC") > 0)
        assertTrue(indexOfAscii(initialization.bytes, "mvex") > 0)

        val nal = byteArrayOf(0x65, 1, 2, 3, 4)
        val fragment = muxer.fragment(
            NxpCupH264AccessUnitView(7, 99, 0, true, byteArrayOf(0, 0, 0, 1) + nal),
        )
        val boxes = topLevelBoxes(fragment.bytes)
        assertEquals(listOf("moof", "mdat"), boxes.map { it.first })
        val moofSize = boxes[0].second
        val trun = indexOfAscii(fragment.bytes, "trun") - 4
        assertEquals(moofSize + 8, readInt(fragment.bytes, trun + 16))
        assertEquals(nal.size, readInt(fragment.bytes, moofSize + 8))
        assertEquals(nal.toList(), fragment.bytes.copyOfRange(moofSize + 12, fragment.bytes.size).toList())
    }

    private fun topLevelBoxes(bytes: ByteArray): List<Pair<String, Int>> {
        val result = mutableListOf<Pair<String, Int>>()
        var offset = 0
        while (offset < bytes.size) {
            val size = readInt(bytes, offset)
            require(size >= 8 && offset + size <= bytes.size)
            result += String(bytes, offset + 4, 4, Charsets.US_ASCII) to size
            offset += size
        }
        return result
    }

    private fun indexOfAscii(bytes: ByteArray, value: String): Int {
        val target = value.toByteArray(Charsets.US_ASCII)
        return bytes.indices.firstOrNull { start ->
            start + target.size <= bytes.size && target.indices.all { bytes[start + it] == target[it] }
        } ?: -1
    }

    private fun readInt(bytes: ByteArray, offset: Int): Int =
        ByteBuffer.wrap(bytes, offset, 4).order(ByteOrder.BIG_ENDIAN).int
}
