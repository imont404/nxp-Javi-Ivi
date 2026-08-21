package com.wavenumber.avc.bridge

import android.app.Activity
import android.app.PendingIntent
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.graphics.Bitmap
import android.graphics.Color
import android.graphics.Typeface
import android.hardware.usb.UsbDevice
import android.hardware.usb.UsbManager
import android.os.Build
import android.os.Bundle
import android.util.Log
import android.view.Gravity
import android.view.ViewGroup
import android.view.WindowManager
import android.widget.ImageView
import android.widget.LinearLayout
import android.widget.TextView
import androidx.core.content.ContextCompat
import com.wavenumber.avc.bridge.usb.AvcUsbHealth
import com.wavenumber.avc.bridge.usb.AvcUsbSession
import com.wavenumber.avc.bridge.usb.AvcUsbState
import java.nio.ByteBuffer
import java.util.Locale

class MainActivity : Activity() {
    companion object {
        private const val ACTION_USB_PERMISSION = "com.wavenumber.avc.bridge.USB_PERMISSION"
        private const val HEALTH_TAG = "AVC_BRIDGE_HEALTH"
    }

    private lateinit var usbManager: UsbManager
    private lateinit var session: AvcUsbSession
    private lateinit var statusView: TextView
    private lateinit var previewView: ImageView
    private val previewBitmap = Bitmap.createBitmap(320, 200, Bitmap.Config.RGB_565)
    private var permissionPending = false
    private var renderedFrames = 0L
    private var lastRenderedFrameId = -1L

    private val renderLoop = object : Runnable {
        override fun run() {
            val frame = if (::session.isInitialized) session.takeLatestFrame() else null
            if (frame != null) {
                try {
                    previewBitmap.copyPixelsFromBuffer(ByteBuffer.wrap(frame.pixels))
                    renderedFrames++
                    lastRenderedFrameId = frame.frameId
                    previewView.invalidate()
                } finally {
                    session.releaseFrame(frame)
                }
            }
            if (::previewView.isInitialized) previewView.postDelayed(this, 33)
        }
    }

    private val usbReceiver = object : BroadcastReceiver() {
        override fun onReceive(context: Context?, intent: Intent?) {
            when (intent?.action) {
                ACTION_USB_PERMISSION -> {
                    permissionPending = false
                    val device = intent.usbDevice()
                    if (intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false) && device != null) {
                        session.start(device)
                    } else {
                        showHealth(AvcUsbHealth(AvcUsbState.ERROR, "USB permission denied"))
                    }
                }
                UsbManager.ACTION_USB_DEVICE_ATTACHED -> connectIfAvailable()
                UsbManager.ACTION_USB_DEVICE_DETACHED -> {
                    session.stop()
                    showHealth(AvcUsbHealth(AvcUsbState.DISCONNECTED, "AVC USB device detached"))
                }
            }
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON)

        usbManager = getSystemService(Context.USB_SERVICE) as UsbManager
        session = AvcUsbSession(usbManager, ::showHealth)
        previewView = ImageView(this).apply {
            setImageBitmap(previewBitmap)
            scaleType = ImageView.ScaleType.FIT_CENTER
            setBackgroundColor(Color.BLACK)
            contentDescription = "Live AVC camera preview"
        }
        statusView = TextView(this).apply {
            textSize = 14f
            gravity = Gravity.START
            setPadding(24, 18, 24, 24)
            setTextIsSelectable(true)
            setTextColor(Color.WHITE)
            setBackgroundColor(Color.rgb(18, 50, 74))
            typeface = Typeface.MONOSPACE
        }
        val root = LinearLayout(this).apply {
            orientation = LinearLayout.VERTICAL
            addView(
                previewView,
                LinearLayout.LayoutParams(
                    ViewGroup.LayoutParams.MATCH_PARENT,
                    0,
                    1f,
                ),
            )
            addView(
                statusView,
                LinearLayout.LayoutParams(
                    ViewGroup.LayoutParams.MATCH_PARENT,
                    ViewGroup.LayoutParams.WRAP_CONTENT,
                ),
            )
        }
        setContentView(root)
        previewView.post(renderLoop)

        val filter = IntentFilter().apply {
            addAction(ACTION_USB_PERMISSION)
            addAction(UsbManager.ACTION_USB_DEVICE_ATTACHED)
            addAction(UsbManager.ACTION_USB_DEVICE_DETACHED)
        }
        ContextCompat.registerReceiver(
            this,
            usbReceiver,
            filter,
            ContextCompat.RECEIVER_NOT_EXPORTED,
        )
        connectIfAvailable()
    }

    override fun onResume() {
        super.onResume()
        if (::session.isInitialized) connectIfAvailable()
    }

    override fun onDestroy() {
        previewView.removeCallbacks(renderLoop)
        session.stop()
        unregisterReceiver(usbReceiver)
        super.onDestroy()
    }

    private fun connectIfAvailable() {
        val device = usbManager.deviceList.values.firstOrNull {
            it.vendorId == AvcUsbSession.AVC_VENDOR_ID && it.productId == AvcUsbSession.AVC_PRODUCT_ID
        }
        if (device == null) {
            showHealth(AvcUsbHealth(AvcUsbState.IDLE, "waiting for AVC USB device"))
            return
        }
        if (usbManager.hasPermission(device)) {
            session.start(device)
            return
        }
        if (permissionPending) return
        permissionPending = true
        showHealth(AvcUsbHealth(AvcUsbState.IDLE, "USB permission required"))
        val permissionIntent = PendingIntent.getBroadcast(
            this,
            0,
            Intent(ACTION_USB_PERMISSION).setPackage(packageName),
            PendingIntent.FLAG_UPDATE_CURRENT or PendingIntent.FLAG_MUTABLE,
        )
        usbManager.requestPermission(device, permissionIntent)
    }

    private fun showHealth(health: AvcUsbHealth) {
        runOnUiThread {
            val devices = usbManager.deviceList.values.toList()
            statusView.text = buildString {
                appendLine("AVC Android Bridge")
                appendLine()
                appendLine("Device: ${Build.MANUFACTURER} ${Build.MODEL}")
                appendLine("Android: ${Build.VERSION.RELEASE} (API ${Build.VERSION.SDK_INT})")
                appendLine("Attached USB devices: ${devices.size}")
                appendLine("Session: ${health.state.wireName}")
                appendLine("Detail: ${health.detail}")
                appendLine(
                    "Frames: ${health.frames}  Displayed: $renderedFrames  " +
                        "Last: $lastRenderedFrameId",
                )
                appendLine(
                    "Rate: %.1f FPS  %.3f MiB/s".format(
                        Locale.US,
                        health.framesPerSecond,
                        health.mebibytesPerSecond,
                    ),
                )
                appendLine("Packets: ${health.packets}  Bytes: ${health.bytes}")
                appendLine(
                    "Errors: seq=${health.sequenceErrors} malformed=${health.malformedChunks} " +
                        "preview_drop=${health.previewDrops}",
                )
                appendLine(
                    "Diagnostics: stats=${health.statsReports} logs=${health.logRecords} " +
                        "telemetry=${health.telemetryRecords}",
                )
                if (health.sessionId != 0L) appendLine("Session ID: ${health.sessionId}")
            }
            Log.i(
                HEALTH_TAG,
                "state=${health.state.wireName} usb_devices=${devices.size} packets=${health.packets} " +
                    "bytes=${health.bytes} frames=${health.frames} fps=%.2f mib_s=%.3f ".format(
                        Locale.US,
                        health.framesPerSecond,
                        health.mebibytesPerSecond,
                    ) +
                    "seq_errors=${health.sequenceErrors} malformed=${health.malformedChunks} " +
                    "preview_drops=${health.previewDrops} session_id=${health.sessionId} " +
                    "detail=${health.detail.replace(' ', '_')}",
            )
        }
    }

    @Suppress("DEPRECATION")
    private fun Intent.usbDevice(): UsbDevice? =
        if (Build.VERSION.SDK_INT >= 33) {
            getParcelableExtra(UsbManager.EXTRA_DEVICE, UsbDevice::class.java)
        } else {
            getParcelableExtra(UsbManager.EXTRA_DEVICE)
        }
}
