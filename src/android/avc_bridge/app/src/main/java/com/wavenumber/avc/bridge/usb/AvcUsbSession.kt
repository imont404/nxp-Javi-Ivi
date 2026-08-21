package com.wavenumber.avc.bridge.usb

import android.hardware.usb.UsbConstants
import android.hardware.usb.UsbDevice
import android.hardware.usb.UsbDeviceConnection
import android.hardware.usb.UsbEndpoint
import android.hardware.usb.UsbInterface
import android.hardware.usb.UsbManager
import com.wavenumber.avc.bridge.protocol.AvcControlPacketBuilder
import com.wavenumber.avc.bridge.protocol.AvcPacket
import com.wavenumber.avc.bridge.protocol.AvcPayloadDecoder
import com.wavenumber.avc.bridge.protocol.AvcProtocol
import com.wavenumber.avc.bridge.protocol.AvcStreamParser
import com.wavenumber.avc.bridge.video.AvcFrameAssembler
import com.wavenumber.avc.bridge.video.AvcVideoFrame
import com.wavenumber.avc.bridge.video.LatestFrameMailbox
import java.util.ArrayDeque

enum class AvcUsbState(val wireName: String) {
    IDLE("idle"),
    OPENING("opening"),
    HELLO("hello"),
    CHANNELS("channels"),
    PING("ping"),
    STREAMING("streaming"),
    CLOSING("closing"),
    COMPLETE("complete"),
    DISCONNECTED("disconnected"),
    ERROR("error"),
}

data class AvcUsbHealth(
    val state: AvcUsbState,
    val detail: String,
    val packets: Long = 0,
    val bytes: Long = 0,
    val sessionId: Long = 0,
    val frames: Long = 0,
    val framesPerSecond: Double = 0.0,
    val mebibytesPerSecond: Double = 0.0,
    val sequenceErrors: Long = 0,
    val malformedChunks: Long = 0,
    val previewDrops: Long = 0,
    val statsReports: Long = 0,
    val logRecords: Long = 0,
    val telemetryRecords: Long = 0,
)

class AvcUsbSession(
    private val usbManager: UsbManager,
    private val onHealth: (AvcUsbHealth) -> Unit,
    private val onCompletedFrame: ((AvcVideoFrame) -> Unit)? = null,
    private val onDiagnosticPacket: ((AvcPacket) -> Unit)? = null,
) {
    companion object {
        const val AVC_VENDOR_ID = 0x1FC9
        const val AVC_PRODUCT_ID = 0x0094
        private const val FRAME_WIDTH = 320
        private const val FRAME_HEIGHT = 200
        private const val FRAME_BYTES = FRAME_WIDTH * FRAME_HEIGHT * 2
        private const val READ_TIMEOUT_MS = 100
        private const val RESPONSE_TIMEOUT_MS = 3_000L
        private const val HEALTH_PERIOD_NS = 1_000_000_000L
    }

    private val mailbox = LatestFrameMailbox(FRAME_BYTES)

    @Volatile
    private var stopRequested = false
    @Volatile
    private var worker: Thread? = null

    fun start(device: UsbDevice) {
        if (worker?.isAlive == true) return
        mailbox.reset()
        stopRequested = false
        worker = Thread({ runStream(device) }, "avc-usb-session").also { it.start() }
    }

    fun stop() {
        stopRequested = true
        worker?.interrupt()
    }

    fun takeLatestFrame(): AvcVideoFrame? = mailbox.takeLatest()

    fun releaseFrame(frame: AvcVideoFrame) = mailbox.release(frame)

    private fun runStream(device: UsbDevice) {
        var connection: UsbDeviceConnection? = null
        var dataInterface: UsbInterface? = null
        val assembler = AvcFrameAssembler(mailbox, FRAME_WIDTH, FRAME_HEIGHT, onCompletedFrame)
        try {
            report(AvcUsbHealth(AvcUsbState.OPENING, "opening %04X:%04X".format(device.vendorId, device.productId)))
            val endpoints = findCdcEndpoints(device)
            dataInterface = endpoints.dataInterface
            connection = usbManager.openDevice(device) ?: error("UsbManager.openDevice returned null")
            check(connection.claimInterface(endpoints.dataInterface, true)) { "could not claim CDC data interface" }
            endpoints.controlInterface?.let { configureCdc(connection, it) }

            val responses = ArrayDeque<AvcPacket>()
            var receivedBytes = 0L
            var statsReports = 0L
            var logRecords = 0L
            var telemetryRecords = 0L
            var malformedPayloads = 0L
            val parser = AvcStreamParser { packet ->
                when (packet.header.messageId) {
                    AvcProtocol.MSG_RUI_WRITE_FRAME_BUFFER_RAW -> assembler.accept(packet)
                    AvcProtocol.MSG_STATS_REPORT -> try {
                        AvcPayloadDecoder.stats(packet.payload)
                        statsReports++
                    } catch (_: IllegalArgumentException) {
                        malformedPayloads++
                    }
                    AvcProtocol.MSG_LOG_TEXT -> try {
                        AvcPayloadDecoder.log(packet.payload)
                        logRecords++
                    } catch (_: IllegalArgumentException) {
                        malformedPayloads++
                    }
                    AvcProtocol.MSG_TELEMETRY_SCALAR -> try {
                        AvcPayloadDecoder.telemetry(packet.payload)
                        telemetryRecords++
                        onDiagnosticPacket?.invoke(packet)
                    } catch (_: IllegalArgumentException) {
                        malformedPayloads++
                    }
                }
                if (packet.header.flags and AvcProtocol.FLAG_RESPONSE != 0) {
                    responses.addLast(packet)
                }
            }
            val readBuffer = ByteArray(16 * 1024)

            fun readOnce(): Int {
                val count = connection.bulkTransfer(
                    endpoints.bulkIn,
                    readBuffer,
                    readBuffer.size,
                    READ_TIMEOUT_MS,
                )
                if (count > 0) {
                    receivedBytes += count
                    parser.push(readBuffer, 0, count)
                }
                return count
            }

            fun exchange(
                state: AvcUsbState,
                sequence: Int,
                messageId: Int,
                arg0: Int = 0,
                arg1: Int = 0,
                allowWhileStopping: Boolean = false,
            ): AvcPacket {
                report(
                    AvcUsbHealth(
                        state,
                        "request=0x%08X sequence=$sequence".format(messageId),
                        parser.parsedPackets,
                        receivedBytes,
                    ),
                )
                val request = AvcControlPacketBuilder.build(sequence, messageId, arg0, arg1)
                val written = connection.bulkTransfer(endpoints.bulkOut, request, request.size, 1_000)
                check(written == request.size) { "short USB write: $written/${request.size}" }
                val deadline = System.currentTimeMillis() + RESPONSE_TIMEOUT_MS
                while ((allowWhileStopping || !stopRequested) && System.currentTimeMillis() < deadline) {
                    val iterator = responses.iterator()
                    while (iterator.hasNext()) {
                        val packet = iterator.next()
                        if (
                            packet.header.messageId == messageId &&
                            packet.header.arg0 == sequence &&
                            packet.header.flags and AvcProtocol.FLAG_RESPONSE != 0
                        ) {
                            iterator.remove()
                            check(packet.header.arg1 == 0) {
                                "control status ${packet.header.arg1} for 0x%08X".format(messageId)
                            }
                            return packet
                        }
                    }
                    readOnce()
                }
                error("timed out waiting for 0x%08X sequence=$sequence".format(messageId))
            }

            val helloPacket = exchange(AvcUsbState.HELLO, 0, AvcProtocol.MSG_CONTROL_HELLO)
            val hello = AvcPayloadDecoder.hello(helloPacket.payload)
            check(hello.frameWidth == FRAME_WIDTH && hello.frameHeight == FRAME_HEIGHT) {
                "unsupported frame geometry ${hello.frameWidth}x${hello.frameHeight}"
            }
            check(hello.pixelFormat == AvcProtocol.PIXEL_FORMAT_RGB565_LE) {
                "unsupported pixel format ${hello.pixelFormat}"
            }
            check(hello.sessionId == (helloPacket.header.arg2.toLong() and 0xFFFF_FFFFL)) {
                "HELLO session IDs disagree"
            }

            val channels = AvcProtocol.CHANNEL_FRAMES or AvcProtocol.CHANNEL_STATS or
                AvcProtocol.CHANNEL_LOGS or AvcProtocol.CHANNEL_TELEMETRY
            exchange(
                AvcUsbState.CHANNELS,
                1,
                AvcProtocol.MSG_CONTROL_SET_CHANNELS,
                arg0 = channels,
                arg1 = 0,
            )
            exchange(AvcUsbState.PING, 2, AvcProtocol.MSG_CONTROL_PING)
            parser.beginSequenceWindow()

            var lastReportNs = System.nanoTime()
            var lastReportBytes = receivedBytes
            var lastReportFrames = assembler.completedFrames
            report(AvcUsbHealth(AvcUsbState.STREAMING, "camera subscription active", sessionId = hello.sessionId))
            while (!stopRequested) {
                readOnce()
                val nowNs = System.nanoTime()
                if (nowNs - lastReportNs < HEALTH_PERIOD_NS) continue
                val elapsedSeconds = (nowNs - lastReportNs).toDouble() / 1_000_000_000.0
                val frameStats = assembler.stats()
                val fps = (frameStats.completedFrames - lastReportFrames) / elapsedSeconds
                val mibPerSecond = (receivedBytes - lastReportBytes) / elapsedSeconds / (1024.0 * 1024.0)
                report(
                    AvcUsbHealth(
                        state = AvcUsbState.STREAMING,
                        detail = "camera subscription active",
                        packets = parser.parsedPackets,
                        bytes = receivedBytes,
                        sessionId = hello.sessionId,
                        frames = frameStats.completedFrames,
                        framesPerSecond = fps,
                        mebibytesPerSecond = mibPerSecond,
                        sequenceErrors = parser.sequenceErrors,
                        malformedChunks = frameStats.malformedChunks + malformedPayloads,
                        previewDrops = mailbox.supersededFrames + frameStats.noBufferDrops,
                        statsReports = statsReports,
                        logRecords = logRecords,
                        telemetryRecords = telemetryRecords,
                    ),
                )
                lastReportNs = nowNs
                lastReportBytes = receivedBytes
                lastReportFrames = frameStats.completedFrames

                if (usbManager.deviceList.values.none { it.deviceName == device.deviceName }) {
                    error("AVC USB device disconnected")
                }
            }

            exchange(
                AvcUsbState.CLOSING,
                3,
                AvcProtocol.MSG_CONTROL_SET_CHANNELS,
                allowWhileStopping = true,
            )
            exchange(
                AvcUsbState.CLOSING,
                4,
                AvcProtocol.MSG_CONTROL_CLOSE,
                allowWhileStopping = true,
            )
            report(
                AvcUsbHealth(
                    AvcUsbState.COMPLETE,
                    "stream stopped cleanly",
                    parser.parsedPackets,
                    receivedBytes,
                    hello.sessionId,
                    assembler.completedFrames,
                ),
            )
        } catch (error: Throwable) {
            if (stopRequested) {
                report(AvcUsbHealth(AvcUsbState.DISCONNECTED, "session stopped"))
            } else {
                report(AvcUsbHealth(AvcUsbState.ERROR, error.message ?: error.javaClass.simpleName))
            }
        } finally {
            assembler.abortActive()
            dataInterface?.let { connection?.releaseInterface(it) }
            connection?.close()
            if (Thread.currentThread() === worker) worker = null
        }
    }

    private fun findCdcEndpoints(device: UsbDevice): CdcEndpoints {
        var controlInterface: UsbInterface? = null
        var dataInterface: UsbInterface? = null
        var bulkIn: UsbEndpoint? = null
        var bulkOut: UsbEndpoint? = null
        for (interfaceIndex in 0 until device.interfaceCount) {
            val candidate = device.getInterface(interfaceIndex)
            if (candidate.interfaceClass == UsbConstants.USB_CLASS_COMM) controlInterface = candidate
            var candidateIn: UsbEndpoint? = null
            var candidateOut: UsbEndpoint? = null
            for (endpointIndex in 0 until candidate.endpointCount) {
                val endpoint = candidate.getEndpoint(endpointIndex)
                if (endpoint.type != UsbConstants.USB_ENDPOINT_XFER_BULK) continue
                if (endpoint.direction == UsbConstants.USB_DIR_IN) candidateIn = endpoint
                if (endpoint.direction == UsbConstants.USB_DIR_OUT) candidateOut = endpoint
            }
            if (candidateIn != null && candidateOut != null) {
                dataInterface = candidate
                bulkIn = candidateIn
                bulkOut = candidateOut
            }
        }
        return CdcEndpoints(
            controlInterface,
            dataInterface ?: error("CDC bulk interface not found"),
            bulkIn ?: error("CDC bulk IN endpoint not found"),
            bulkOut ?: error("CDC bulk OUT endpoint not found"),
        )
    }

    private fun configureCdc(connection: UsbDeviceConnection, controlInterface: UsbInterface) {
        val lineCoding1152008N1 = byteArrayOf(0x00, 0xC2.toByte(), 0x01, 0x00, 0x00, 0x00, 0x08)
        connection.controlTransfer(0x21, 0x20, 0, controlInterface.id, lineCoding1152008N1, 7, 1_000)
        connection.controlTransfer(0x21, 0x22, 3, controlInterface.id, null, 0, 1_000)
    }

    private fun report(health: AvcUsbHealth) = onHealth(health)

    private data class CdcEndpoints(
        val controlInterface: UsbInterface?,
        val dataInterface: UsbInterface,
        val bulkIn: UsbEndpoint,
        val bulkOut: UsbEndpoint,
    )
}
