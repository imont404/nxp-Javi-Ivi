package com.wavenumber.avc.bridge

import android.app.Activity
import android.app.PendingIntent
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.hardware.usb.UsbDevice
import android.hardware.usb.UsbManager
import android.os.Build
import android.os.Bundle
import android.util.Log
import android.view.Gravity
import android.view.WindowManager
import android.widget.TextView
import androidx.core.content.ContextCompat
import com.wavenumber.avc.bridge.usb.AvcUsbHealth
import com.wavenumber.avc.bridge.usb.AvcUsbSession
import com.wavenumber.avc.bridge.usb.AvcUsbState

class MainActivity : Activity() {
    companion object {
        private const val ACTION_USB_PERMISSION = "com.wavenumber.avc.bridge.USB_PERMISSION"
        private const val HEALTH_TAG = "AVC_BRIDGE_HEALTH"
    }

    private lateinit var usbManager: UsbManager
    private lateinit var session: AvcUsbSession
    private lateinit var statusView: TextView
    private var permissionPending = false

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
        statusView = TextView(this).apply {
            textSize = 18f
            gravity = Gravity.START
            setPadding(36, 48, 36, 36)
            setTextIsSelectable(true)
        }
        setContentView(statusView)

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
                appendLine("Packets: ${health.packets}")
                appendLine("Bytes: ${health.bytes}")
                if (health.sessionId != 0L) appendLine("Session ID: ${health.sessionId}")
            }
            Log.i(
                HEALTH_TAG,
                "state=${health.state.wireName} usb_devices=${devices.size} packets=${health.packets} " +
                    "bytes=${health.bytes} session_id=${health.sessionId} detail=${health.detail.replace(' ', '_')}",
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
