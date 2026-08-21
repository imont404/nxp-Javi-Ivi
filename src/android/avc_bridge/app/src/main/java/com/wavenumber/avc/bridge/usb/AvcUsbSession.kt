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
import java.util.ArrayDeque

enum class AvcUsbState(val wireName: String) {
    IDLE("idle"),
    OPENING("opening"),
    HELLO("hello"),
    CHANNELS("channels"),
    PING("ping"),
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
)

class AvcUsbSession(
    private val usbManager: UsbManager,
    private val onHealth: (AvcUsbHealth) -> Unit,
) {
    companion object {
        const val AVC_VENDOR_ID = 0x1FC9
        const val AVC_PRODUCT_ID = 0x0094
        private const val READ_TIMEOUT_MS = 100
        private const val RESPONSE_TIMEOUT_MS = 3_000L
    }

    @Volatile
    private var stopRequested = false
    private var worker: Thread? = null

    fun start(device: UsbDevice) {
        if (worker?.isAlive == true) return
        stopRequested = false
        worker = Thread({ runProof(device) }, "avc-usb-session").also { it.start() }
    }

    fun stop() {
        stopRequested = true
        worker?.interrupt()
        worker = null
    }

    private fun runProof(device: UsbDevice) {
        var connection: UsbDeviceConnection? = null
        var dataInterface: UsbInterface? = null
        try {
            report(AvcUsbState.OPENING, "opening %04X:%04X".format(device.vendorId, device.productId))
            val endpoints = findCdcEndpoints(device)
            dataInterface = endpoints.dataInterface
            connection = usbManager.openDevice(device) ?: error("UsbManager.openDevice returned null")
            check(connection.claimInterface(dataInterface, true)) { "could not claim CDC data interface" }
            endpoints.controlInterface?.let { configureCdc(connection, it) }

            val responses = ArrayDeque<AvcPacket>()
            var receivedBytes = 0L
            val parser = AvcStreamParser { packet -> responses.addLast(packet) }

            fun exchange(
                state: AvcUsbState,
                sequence: Int,
                messageId: Int,
                arg0: Int = 0,
                arg1: Int = 0,
            ): AvcPacket {
                report(state, "request=0x%08X sequence=$sequence".format(messageId), parser.parsedPackets, receivedBytes)
                val request = AvcControlPacketBuilder.build(sequence, messageId, arg0, arg1)
                val written = connection.bulkTransfer(endpoints.bulkOut, request, request.size, 1_000)
                check(written == request.size) { "short USB write: $written/${request.size}" }
                val deadline = System.currentTimeMillis() + RESPONSE_TIMEOUT_MS
                val readBuffer = ByteArray(16 * 1024)
                while (!stopRequested && System.currentTimeMillis() < deadline) {
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
                }
                error("timed out waiting for 0x%08X sequence=$sequence".format(messageId))
            }

            val helloPacket = exchange(AvcUsbState.HELLO, 0, AvcProtocol.MSG_CONTROL_HELLO)
            val hello = AvcPayloadDecoder.hello(helloPacket.payload)
            check(hello.sessionId == (helloPacket.header.arg2.toLong() and 0xFFFF_FFFFL)) {
                "HELLO session IDs disagree"
            }
            exchange(AvcUsbState.CHANNELS, 1, AvcProtocol.MSG_CONTROL_SET_CHANNELS, arg0 = 0)
            exchange(AvcUsbState.PING, 2, AvcProtocol.MSG_CONTROL_PING)
            exchange(AvcUsbState.CLOSING, 3, AvcProtocol.MSG_CONTROL_CLOSE)
            report(
                AvcUsbState.COMPLETE,
                "HELLO, SET_CHANNELS(0), PING, CLOSE passed",
                parser.parsedPackets,
                receivedBytes,
                hello.sessionId,
            )
        } catch (error: Throwable) {
            if (stopRequested) {
                report(AvcUsbState.DISCONNECTED, "session stopped")
            } else {
                report(AvcUsbState.ERROR, error.message ?: error.javaClass.simpleName)
            }
        } finally {
            dataInterface?.let { connection?.releaseInterface(it) }
            connection?.close()
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

    private fun report(
        state: AvcUsbState,
        detail: String,
        packets: Long = 0,
        bytes: Long = 0,
        sessionId: Long = 0,
    ) = onHealth(AvcUsbHealth(state, detail, packets, bytes, sessionId))

    private data class CdcEndpoints(
        val controlInterface: UsbInterface?,
        val dataInterface: UsbInterface,
        val bulkIn: UsbEndpoint,
        val bulkOut: UsbEndpoint,
    )
}
