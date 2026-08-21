package com.wavenumber.avc.bridge.video

import android.media.MediaCodecInfo
import android.media.MediaCodecList

data class AvcEncoderCapability(
    val name: String,
    val canonicalName: String,
    val hardwareAccelerated: Boolean,
    val softwareOnly: Boolean,
    val vendor: Boolean,
    val colorFormats: List<Int>,
    val widthRange: String,
    val heightRange: String,
    val frameRateRange: String,
    val bitrateRange: String,
)

object AvcCodecInventory {
    const val MIME_TYPE = "video/avc"

    fun encoders(): List<AvcEncoderCapability> =
        MediaCodecList(MediaCodecList.ALL_CODECS).codecInfos
            .asSequence()
            .filter { it.isEncoder && it.supportedTypes.any { type -> type.equals(MIME_TYPE, true) } }
            .mapNotNull(::capability)
            .sortedWith(
                compareByDescending<AvcEncoderCapability> { it.hardwareAccelerated }
                    .thenBy { it.name },
            )
            .toList()

    fun logLine(codec: AvcEncoderCapability): String =
        "name=${codec.name} canonical=${codec.canonicalName} " +
            "hardware=${codec.hardwareAccelerated} software=${codec.softwareOnly} vendor=${codec.vendor} " +
            "colors=${codec.colorFormats.joinToString(",") { colorName(it) }} " +
            "width=${codec.widthRange} height=${codec.heightRange} " +
            "fps=${codec.frameRateRange} bitrate=${codec.bitrateRange}"

    fun colorName(format: Int): String = when (format) {
        MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Planar -> "YUV420Planar($format)"
        MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420SemiPlanar -> "YUV420SemiPlanar($format)"
        MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Flexible -> "YUV420Flexible($format)"
        MediaCodecInfo.CodecCapabilities.COLOR_FormatSurface -> "Surface($format)"
        else -> "0x${format.toUInt().toString(16)}"
    }

    private fun capability(info: MediaCodecInfo): AvcEncoderCapability? = try {
        val capabilities = info.getCapabilitiesForType(MIME_TYPE)
        val video = capabilities.videoCapabilities
        AvcEncoderCapability(
            name = info.name,
            canonicalName = info.canonicalName,
            hardwareAccelerated = info.isHardwareAccelerated,
            softwareOnly = info.isSoftwareOnly,
            vendor = info.isVendor,
            colorFormats = capabilities.colorFormats.toList(),
            widthRange = video.supportedWidths.toString(),
            heightRange = video.supportedHeights.toString(),
            frameRateRange = video.supportedFrameRates.toString(),
            bitrateRange = video.bitrateRange.toString(),
        )
    } catch (_: IllegalArgumentException) {
        null
    }
}
