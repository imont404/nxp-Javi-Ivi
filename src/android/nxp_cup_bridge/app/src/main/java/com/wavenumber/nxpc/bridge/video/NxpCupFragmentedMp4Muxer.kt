package com.wavenumber.nxpc.bridge.video

import java.io.ByteArrayOutputStream
import java.io.DataOutputStream

data class NxpCupMp4Initialization(
    val bytes: ByteArray,
    val codecConfig: Int,
)

data class NxpCupMp4Fragment(
    val frameId: Long,
    val capturedNs: Long,
    val keyFrame: Boolean,
    val bytes: ByteArray,
)

/** Minimal single-track fragmented MP4 writer for Media Source Extensions. */
class NxpCupFragmentedMp4Muxer(
    private val width: Int = 320,
    private val height: Int = 200,
    private val frameRate: Int = 24,
) {
    companion object {
        private const val TIMESCALE = 90_000
    }

    private var sequence = 1
    private var decodeTime = 0L

    fun initialization(format: NxpCupH264FormatView): NxpCupMp4Initialization {
        val sps = annexBNalUnits(format.csd0).firstOrNull { (it[0].toInt() and 0x1f) == 7 }
            ?: error("H.264 csd-0 has no SPS")
        val pps = annexBNalUnits(format.csd1).firstOrNull { (it[0].toInt() and 0x1f) == 8 }
            ?: error("H.264 csd-1 has no PPS")
        require(sps.size >= 4) { "H.264 SPS is too short" }
        sequence = 1
        decodeTime = 0
        val codecConfig =
            ((sps[1].toInt() and 0xff) shl 16) or
                ((sps[2].toInt() and 0xff) shl 8) or
                (sps[3].toInt() and 0xff)
        return NxpCupMp4Initialization(
            bytes = concat(
                box("ftyp") {
                    ascii("isom")
                    u32(0x200)
                    ascii("isom")
                    ascii("iso6")
                    ascii("avc1")
                    ascii("mp41")
                },
                movieBox(sps, pps),
            ),
            codecConfig = codecConfig,
        )
    }

    fun fragment(accessUnit: NxpCupH264AccessUnitView): NxpCupMp4Fragment {
        val sample = annexBToNxpCupc(accessUnit.bytes)
        val sampleDuration = TIMESCALE / frameRate
        val moof = box("moof") {
            box("mfhd") {
                fullBox(0, 0)
                u32(sequence++)
            }
            box("traf") {
                box("tfhd") {
                    fullBox(0, 0x020000)
                    u32(1)
                }
                box("tfdt") {
                    fullBox(1, 0)
                    u64(decodeTime)
                }
                box("trun") {
                    fullBox(0, 0x000701)
                    u32(1)
                    mark("dOff")
                    u32(sampleDuration)
                    u32(sample.size)
                    u32(if (accessUnit.keyFrame) 0x02000000 else 0x01010000)
                }
            }
        }.also { bytes ->
            val marker = findMarker(bytes, "dOff")
            putU32(bytes, marker, bytes.size + 8)
        }
        decodeTime += sampleDuration
        return NxpCupMp4Fragment(
            accessUnit.frameId,
            accessUnit.capturedNs,
            accessUnit.keyFrame,
            concat(moof, box("mdat") { bytes(sample) }),
        )
    }

    private fun movieBox(sps: ByteArray, pps: ByteArray): ByteArray = box("moov") {
        box("mvhd") {
            fullBox(0, 0)
            u32(0); u32(0); u32(TIMESCALE); u32(0)
            u32(0x00010000); u16(0x0100); u16(0)
            zeros(8); matrix(); zeros(24); u32(2)
        }
        box("trak") {
            box("tkhd") {
                fullBox(0, 7)
                u32(0); u32(0); u32(1); u32(0); u32(0)
                zeros(8); u16(0); u16(0); u16(0); u16(0)
                matrix(); u32(width shl 16); u32(height shl 16)
            }
            box("mdia") {
                box("mdhd") {
                    fullBox(0, 0)
                    u32(0); u32(0); u32(TIMESCALE); u32(0); u16(0x55c4); u16(0)
                }
                box("hdlr") {
                    fullBox(0, 0)
                    u32(0); ascii("vide"); zeros(12); ascii("VideoHandler"); u8(0)
                }
                box("minf") {
                    box("vmhd") {
                        fullBox(0, 1)
                        u16(0); u16(0); u16(0); u16(0)
                    }
                    box("dinf") {
                        box("dref") {
                            fullBox(0, 0); u32(1)
                            box("url ") { fullBox(0, 1) }
                        }
                    }
                    box("stbl") {
                        box("stsd") {
                            fullBox(0, 0); u32(1)
                            box("avc1") {
                                zeros(6); u16(1); u16(0); u16(0); zeros(12)
                                u16(width); u16(height)
                                u32(0x00480000); u32(0x00480000); u32(0); u16(1)
                                zeros(32); u16(0x18); u16(0xffff)
                                box("avcC") {
                                    u8(1); u8(sps[1].toInt()); u8(sps[2].toInt()); u8(sps[3].toInt())
                                    u8(0xff); u8(0xe1); u16(sps.size); bytes(sps)
                                    u8(1); u16(pps.size); bytes(pps)
                                }
                            }
                        }
                        emptyTable("stts"); emptyTable("stsc")
                        box("stsz") { fullBox(0, 0); u32(0); u32(0) }
                        emptyTable("stco")
                    }
                }
            }
        }
        box("mvex") {
            box("trex") {
                fullBox(0, 0)
                u32(1); u32(1); u32(TIMESCALE / frameRate); u32(0); u32(0x01010000)
            }
        }
    }

    private fun Mp4Writer.emptyTable(type: String) = box(type) { fullBox(0, 0); u32(0) }

    private fun annexBToNxpCupc(source: ByteArray): ByteArray {
        val units = annexBNalUnits(source)
        require(units.isNotEmpty()) { "H.264 access unit has no Annex-B NAL units" }
        return Mp4Writer().apply {
            units.forEach { unit -> u32(unit.size); bytes(unit) }
        }.toByteArray()
    }

    internal fun annexBNalUnits(source: ByteArray): List<ByteArray> {
        val starts = ArrayList<Pair<Int, Int>>()
        var index = 0
        while (index + 3 <= source.size) {
            val startSize = when {
                index + 4 <= source.size && source[index] == 0.toByte() && source[index + 1] == 0.toByte() &&
                    source[index + 2] == 0.toByte() && source[index + 3] == 1.toByte() -> 4
                source[index] == 0.toByte() && source[index + 1] == 0.toByte() && source[index + 2] == 1.toByte() -> 3
                else -> 0
            }
            if (startSize > 0) {
                starts.add(index to startSize)
                index += startSize
            } else {
                index++
            }
        }
        return starts.mapIndexedNotNull { unitIndex, (start, startSize) ->
            val end = if (unitIndex + 1 < starts.size) starts[unitIndex + 1].first else source.size
            if (end > start + startSize) source.copyOfRange(start + startSize, end) else null
        }
    }

    private fun box(type: String, body: Mp4Writer.() -> Unit): ByteArray = Mp4Writer().apply {
        u32(0); ascii(type); body()
    }.toByteArray().also { putU32(it, 0, it.size) }

    private fun Mp4Writer.box(type: String, body: Mp4Writer.() -> Unit) = bytes(this@NxpCupFragmentedMp4Muxer.box(type, body))

    private fun findMarker(bytes: ByteArray, name: String): Int {
        val marker = name.toByteArray(Charsets.US_ASCII)
        outer@ for (index in 0..bytes.size - marker.size) {
            for (offset in marker.indices) if (bytes[index + offset] != marker[offset]) continue@outer
            bytes.fill(0, index, index + marker.size)
            return index
        }
        error("MP4 patch marker not found: $name")
    }

    private fun putU32(destination: ByteArray, offset: Int, value: Int) {
        destination[offset] = (value ushr 24).toByte()
        destination[offset + 1] = (value ushr 16).toByte()
        destination[offset + 2] = (value ushr 8).toByte()
        destination[offset + 3] = value.toByte()
    }

    private fun concat(vararg arrays: ByteArray): ByteArray {
        val result = ByteArray(arrays.sumOf(ByteArray::size))
        var offset = 0
        arrays.forEach { array -> array.copyInto(result, offset); offset += array.size }
        return result
    }

    private class Mp4Writer {
        private val buffer = ByteArrayOutputStream()
        private val output = DataOutputStream(buffer)

        fun u8(value: Int) = output.writeByte(value)
        fun u16(value: Int) = output.writeShort(value)
        fun u32(value: Int) = output.writeInt(value)
        fun u64(value: Long) = output.writeLong(value)
        fun ascii(value: String) = output.write(value.toByteArray(Charsets.US_ASCII))
        fun bytes(value: ByteArray) = output.write(value)
        fun zeros(count: Int) = repeat(count) { output.writeByte(0) }
        fun fullBox(version: Int, flags: Int) = u32((version shl 24) or flags)
        fun matrix() {
            u32(0x00010000); u32(0); u32(0)
            u32(0); u32(0x00010000); u32(0)
            u32(0); u32(0); u32(0x40000000)
        }
        fun mark(value: String) = ascii(value)
        fun toByteArray(): ByteArray = buffer.toByteArray()
    }
}
