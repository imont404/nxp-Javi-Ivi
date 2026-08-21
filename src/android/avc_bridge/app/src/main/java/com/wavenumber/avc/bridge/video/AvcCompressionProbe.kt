package com.wavenumber.avc.bridge.video

import android.graphics.Bitmap
import android.media.MediaCodec
import android.media.MediaCodecInfo
import android.media.MediaFormat
import java.io.OutputStream
import java.nio.ByteBuffer
import java.util.ArrayDeque
import java.util.Locale
import java.util.concurrent.ArrayBlockingQueue
import java.util.concurrent.atomic.AtomicLong
import kotlin.math.roundToInt

enum class AvcCompressionMode(val wireName: String) {
    JPEG("jpeg"),
    H264("h264");

    companion object {
        fun parse(value: String?): AvcCompressionMode? =
            entries.firstOrNull { it.wireName.equals(value, ignoreCase = true) }
    }
}

data class AvcCompressionSnapshot(
    val mode: AvcCompressionMode,
    val state: String,
    val encoder: String,
    val offeredFrames: Long,
    val submittedFrames: Long,
    val encodedFrames: Long,
    val droppedFrames: Long,
    val encodedBytes: Long,
    val framesPerSecond: Double,
    val megabitsPerSecond: Double,
    val meanWorkMs: Double,
    val p95WorkMs: Double,
    val meanLatencyMs: Double,
    val p95LatencyMs: Double,
    val error: String = "",
) {
    fun logLine(): String =
        "mode=${mode.wireName} state=$state encoder=${encoder.replace(' ', '_')} " +
            "offered=$offeredFrames submitted=$submittedFrames encoded=$encodedFrames " +
            "dropped=$droppedFrames bytes=$encodedBytes fps=%.2f mbit_s=%.3f ".format(
                Locale.US,
                framesPerSecond,
                megabitsPerSecond,
            ) +
            "work_mean_ms=%.3f work_p95_ms=%.3f latency_mean_ms=%.3f latency_p95_ms=%.3f".format(
                Locale.US,
                meanWorkMs,
                p95WorkMs,
                meanLatencyMs,
                p95LatencyMs,
            ) + if (error.isEmpty()) "" else " error=${error.replace(' ', '_')}"
}

class AvcCompressionProbe(
    private val mode: AvcCompressionMode,
    private val jpegQuality: Int = 70,
    private val h264Bitrate: Int = 750_000,
    private val onSnapshot: (AvcCompressionSnapshot) -> Unit,
) : AutoCloseable {
    companion object {
        private const val WIDTH = 320
        private const val HEIGHT = 200
        private const val RGB565_BYTES = WIDTH * HEIGHT * 2
        private const val JPEG_CAPACITY = RGB565_BYTES * 2
        private const val SNAPSHOT_PERIOD_NS = 1_000_000_000L
    }

    private data class PendingFrame(
        val frameId: Long,
        val capturedNs: Long,
        val pixels: ByteArray,
    )

    private val freeBuffers = ArrayBlockingQueue<ByteArray>(3).apply {
        repeat(3) { add(ByteArray(RGB565_BYTES)) }
    }
    private val latestFrame = ArrayBlockingQueue<PendingFrame>(1)
    private val offeredFrames = AtomicLong()
    private val droppedFrames = AtomicLong()
    @Volatile
    private var stopping = false
    private val worker = Thread(::run, "avc-${mode.wireName}-probe").also { it.start() }

    init {
        require(jpegQuality in 1..100)
        require(h264Bitrate > 0)
    }

    fun offerFrame(frame: AvcVideoFrame) {
        if (stopping || frame.width != WIDTH || frame.height != HEIGHT || frame.pixels.size != RGB565_BYTES) {
            return
        }
        offeredFrames.incrementAndGet()
        var target = freeBuffers.poll()
        if (target == null) {
            val stale = latestFrame.poll()
            if (stale != null) {
                droppedFrames.incrementAndGet()
                target = stale.pixels
            }
        }
        if (target == null) {
            droppedFrames.incrementAndGet()
            return
        }
        frame.pixels.copyInto(target)
        val pending = PendingFrame(frame.frameId, System.nanoTime(), target)
        val stale = latestFrame.poll()
        if (stale != null) {
            droppedFrames.incrementAndGet()
            freeBuffers.offer(stale.pixels)
        }
        if (!latestFrame.offer(pending)) {
            droppedFrames.incrementAndGet()
            freeBuffers.offer(target)
        }
    }

    override fun close() {
        stopping = true
        worker.interrupt()
        worker.join(2_000)
        latestFrame.poll()?.let { freeBuffers.offer(it.pixels) }
    }

    private fun run() {
        val measurements = Measurements(mode, offeredFrames, droppedFrames, onSnapshot)
        try {
            when (mode) {
                AvcCompressionMode.JPEG -> runJpeg(measurements)
                AvcCompressionMode.H264 -> runH264(measurements)
            }
        } catch (error: Throwable) {
            if (!stopping) measurements.fail(error)
        } finally {
            measurements.publish(if (stopping) "stopped" else measurements.state)
        }
    }

    private fun runJpeg(measurements: Measurements) {
        measurements.encoder = "Bitmap.JPEG quality=$jpegQuality"
        measurements.state = "running"
        val bitmap = Bitmap.createBitmap(WIDTH, HEIGHT, Bitmap.Config.RGB_565)
        val output = FixedByteArrayOutputStream(JPEG_CAPACITY)
        try {
            while (!stopping) {
                val frame = takeFrame() ?: continue
                val startNs = System.nanoTime()
                try {
                    bitmap.copyPixelsFromBuffer(ByteBuffer.wrap(frame.pixels))
                    output.reset()
                    check(bitmap.compress(Bitmap.CompressFormat.JPEG, jpegQuality, output)) {
                        "Bitmap JPEG encoder returned false"
                    }
                    val doneNs = System.nanoTime()
                    measurements.encoded(output.size.toLong(), startNs, doneNs, frame.capturedNs)
                } finally {
                    freeBuffers.offer(frame.pixels)
                }
            }
        } finally {
            bitmap.recycle()
        }
    }

    private fun runH264(measurements: Measurements) {
        val capability = AvcCodecInventory.encoders().firstOrNull {
            it.hardwareAccelerated && !it.name.endsWith(".secure") && it.name.startsWith("c2.")
        } ?: AvcCodecInventory.encoders().firstOrNull {
            it.hardwareAccelerated && !it.name.endsWith(".secure")
        } ?: error("No non-secure hardware AVC encoder")
        val colorFormat = when {
            MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Planar in capability.colorFormats ->
                MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Planar
            MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Flexible in capability.colorFormats ->
                MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Flexible
            else -> error("Hardware AVC encoder has no supported planar YUV420 byte-buffer input")
        }
        val converter = Rgb565ToI420(WIDTH, HEIGHT)
        val yuv = ByteArray(WIDTH * HEIGHT * 3 / 2)
        val codec = MediaCodec.createByCodecName(capability.name)
        val format = MediaFormat.createVideoFormat(AvcCodecInventory.MIME_TYPE, WIDTH, HEIGHT).apply {
            setInteger(MediaFormat.KEY_COLOR_FORMAT, colorFormat)
            setInteger(MediaFormat.KEY_BIT_RATE, h264Bitrate)
            setInteger(MediaFormat.KEY_FRAME_RATE, 24)
            setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, 1)
            setInteger(MediaFormat.KEY_PROFILE, MediaCodecInfo.CodecProfileLevel.AVCProfileBaseline)
            setInteger(MediaFormat.KEY_BITRATE_MODE, MediaCodecInfo.EncoderCapabilities.BITRATE_MODE_CBR)
        }
        measurements.encoder = "${capability.name} ${AvcCodecInventory.colorName(colorFormat)} bitrate=$h264Bitrate"
        val pendingTimestamps = HashMap<Long, Long>()
        val bufferInfo = MediaCodec.BufferInfo()
        fun drainOutput() {
            while (true) {
                val outputIndex = codec.dequeueOutputBuffer(bufferInfo, 0)
                if (outputIndex < 0) return
                try {
                    if (bufferInfo.size > 0 && bufferInfo.flags and MediaCodec.BUFFER_FLAG_CODEC_CONFIG == 0) {
                        val doneNs = System.nanoTime()
                        val capturedNs = pendingTimestamps.remove(bufferInfo.presentationTimeUs) ?: doneNs
                        measurements.output(bufferInfo.size.toLong(), doneNs, capturedNs)
                    }
                } finally {
                    codec.releaseOutputBuffer(outputIndex, false)
                }
            }
        }
        try {
            codec.configure(format, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE)
            codec.start()
            measurements.state = "running"
            val epochNs = System.nanoTime()
            while (!stopping) {
                drainOutput()
                val frame = takeFrame() ?: continue
                val startNs = System.nanoTime()
                try {
                    val inputIndex = codec.dequeueInputBuffer(0)
                    if (inputIndex < 0) {
                        droppedFrames.incrementAndGet()
                        continue
                    }
                    converter.convert(frame.pixels, yuv)
                    val input = checkNotNull(codec.getInputBuffer(inputIndex))
                    input.clear()
                    check(input.remaining() >= yuv.size) { "AVC input buffer is too small: ${input.remaining()}" }
                    input.put(yuv)
                    val presentationTimeUs = (frame.capturedNs - epochNs).coerceAtLeast(0) / 1_000
                    pendingTimestamps[presentationTimeUs] = frame.capturedNs
                    codec.queueInputBuffer(inputIndex, 0, yuv.size, presentationTimeUs, 0)
                    measurements.submitted(System.nanoTime() - startNs)
                    drainOutput()
                } finally {
                    freeBuffers.offer(frame.pixels)
                }
            }
            repeat(10) {
                drainOutput()
                if (pendingTimestamps.isEmpty()) return@repeat
                Thread.sleep(2)
            }
        } finally {
            try {
                codec.stop()
            } finally {
                codec.release()
            }
        }
    }

    private fun takeFrame(): PendingFrame? = try {
        latestFrame.take()
    } catch (_: InterruptedException) {
        null
    }

    private class Measurements(
        private val mode: AvcCompressionMode,
        private val offeredFrames: AtomicLong,
        private val droppedFrames: AtomicLong,
        private val onSnapshot: (AvcCompressionSnapshot) -> Unit,
    ) {
        var state = "starting"
        var encoder = "unknown"
        private var submittedFrames = 0L
        private var encodedFrames = 0L
        private var encodedBytes = 0L
        private var startNs = 0L
        private var lastSnapshotNs = 0L
        private val workTimesMs = SampleWindow()
        private val latenciesMs = SampleWindow()

        fun submitted(workNs: Long) {
            markStarted()
            submittedFrames++
            workTimesMs.add(workNs / 1_000_000.0)
            publishIfDue()
        }

        fun encoded(bytes: Long, startNs: Long, doneNs: Long, capturedNs: Long) {
            submitted(doneNs - startNs)
            output(bytes, doneNs, capturedNs)
        }

        fun output(bytes: Long, doneNs: Long, capturedNs: Long) {
            markStarted()
            encodedFrames++
            encodedBytes += bytes
            latenciesMs.add((doneNs - capturedNs).coerceAtLeast(0) / 1_000_000.0)
            publishIfDue()
        }

        fun fail(error: Throwable) {
            state = "error"
            publish("error", error.message ?: error.javaClass.simpleName)
        }

        fun publish(stateOverride: String = state, error: String = "") {
            val nowNs = System.nanoTime()
            val elapsedSeconds = if (startNs == 0L) 0.0 else (nowNs - startNs) / 1_000_000_000.0
            onSnapshot(
                AvcCompressionSnapshot(
                    mode = mode,
                    state = stateOverride,
                    encoder = encoder,
                    offeredFrames = offeredFrames.get(),
                    submittedFrames = submittedFrames,
                    encodedFrames = encodedFrames,
                    droppedFrames = droppedFrames.get(),
                    encodedBytes = encodedBytes,
                    framesPerSecond = if (elapsedSeconds > 0) encodedFrames / elapsedSeconds else 0.0,
                    megabitsPerSecond = if (elapsedSeconds > 0) encodedBytes * 8.0 / elapsedSeconds / 1_000_000.0 else 0.0,
                    meanWorkMs = workTimesMs.mean(),
                    p95WorkMs = workTimesMs.percentile(0.95),
                    meanLatencyMs = latenciesMs.mean(),
                    p95LatencyMs = latenciesMs.percentile(0.95),
                    error = error,
                ),
            )
            lastSnapshotNs = nowNs
        }

        private fun markStarted() {
            if (startNs == 0L) {
                startNs = System.nanoTime()
                lastSnapshotNs = startNs
            }
        }

        private fun publishIfDue() {
            if (System.nanoTime() - lastSnapshotNs >= SNAPSHOT_PERIOD_NS) publish()
        }
    }

    private class SampleWindow(private val capacity: Int = 512) {
        private val values = ArrayDeque<Double>(capacity)

        fun add(value: Double) {
            if (values.size == capacity) values.removeFirst()
            values.addLast(value)
        }

        fun mean(): Double = if (values.isEmpty()) 0.0 else values.sum() / values.size

        fun percentile(fraction: Double): Double {
            if (values.isEmpty()) return 0.0
            val sorted = values.sorted()
            return sorted[((sorted.lastIndex * fraction).roundToInt()).coerceIn(sorted.indices)]
        }
    }

    private class FixedByteArrayOutputStream(capacity: Int) : OutputStream() {
        private val buffer = ByteArray(capacity)
        var size = 0
            private set

        fun reset() {
            size = 0
        }

        override fun write(value: Int) {
            check(size < buffer.size) { "compressed JPEG exceeds ${buffer.size} bytes" }
            buffer[size++] = value.toByte()
        }

        override fun write(source: ByteArray, offset: Int, count: Int) {
            require(offset >= 0 && count >= 0 && offset + count <= source.size)
            check(size + count <= buffer.size) { "compressed JPEG exceeds ${buffer.size} bytes" }
            source.copyInto(buffer, size, offset, offset + count)
            size += count
        }
    }
}

internal class Rgb565ToI420(
    private val width: Int,
    private val height: Int,
) {
    init {
        require(width > 0 && height > 0 && width % 2 == 0 && height % 2 == 0)
    }

    private val yLut = ByteArray(65_536)
    private val uLut = ByteArray(65_536)
    private val vLut = ByteArray(65_536)

    init {
        for (pixel in 0..0xFFFF) {
            val red = ((pixel shr 11) and 0x1F) * 255 / 31
            val green = ((pixel shr 5) and 0x3F) * 255 / 63
            val blue = (pixel and 0x1F) * 255 / 31
            yLut[pixel] = clip(((66 * red + 129 * green + 25 * blue + 128) shr 8) + 16).toByte()
            uLut[pixel] = clip(((-38 * red - 74 * green + 112 * blue + 128) shr 8) + 128).toByte()
            vLut[pixel] = clip(((112 * red - 94 * green - 18 * blue + 128) shr 8) + 128).toByte()
        }
    }

    fun convert(rgb565LittleEndian: ByteArray, i420: ByteArray) {
        require(rgb565LittleEndian.size == width * height * 2)
        require(i420.size >= width * height * 3 / 2)
        val yPlaneSize = width * height
        val uOffset = yPlaneSize
        val vOffset = yPlaneSize + yPlaneSize / 4
        var uvIndex = 0
        for (row in 0 until height step 2) {
            for (column in 0 until width step 2) {
                var uSum = 0
                var vSum = 0
                for (dy in 0..1) {
                    for (dx in 0..1) {
                        val pixelIndex = (row + dy) * width + column + dx
                        val byteIndex = pixelIndex * 2
                        val pixel = (rgb565LittleEndian[byteIndex].toInt() and 0xFF) or
                            ((rgb565LittleEndian[byteIndex + 1].toInt() and 0xFF) shl 8)
                        i420[pixelIndex] = yLut[pixel]
                        uSum += uLut[pixel].toInt() and 0xFF
                        vSum += vLut[pixel].toInt() and 0xFF
                    }
                }
                i420[uOffset + uvIndex] = ((uSum + 2) / 4).toByte()
                i420[vOffset + uvIndex] = ((vSum + 2) / 4).toByte()
                uvIndex++
            }
        }
    }

    private fun clip(value: Int): Int = value.coerceIn(0, 255)
}
