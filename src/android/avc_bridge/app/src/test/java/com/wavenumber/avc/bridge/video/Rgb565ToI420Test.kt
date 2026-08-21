package com.wavenumber.avc.bridge.video

import org.junit.Assert.assertArrayEquals
import org.junit.Test

class Rgb565ToI420Test {
    @Test
    fun convertsBlackAndWhiteBlocksToExpectedStudioRange() {
        val input = byteArrayOf(
            0x00, 0x00,
            0x00, 0x00,
            0xFF.toByte(), 0xFF.toByte(),
            0xFF.toByte(), 0xFF.toByte(),
        )
        val output = ByteArray(6)

        Rgb565ToI420(2, 2).convert(input, output)

        assertArrayEquals(
            byteArrayOf(16, 16, 235.toByte(), 235.toByte(), 128.toByte(), 128.toByte()),
            output,
        )
    }
}
