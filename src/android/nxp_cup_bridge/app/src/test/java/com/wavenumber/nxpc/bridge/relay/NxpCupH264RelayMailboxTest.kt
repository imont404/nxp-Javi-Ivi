package com.wavenumber.nxpc.bridge.relay

import com.wavenumber.nxpc.bridge.video.NxpCupMp4Fragment
import com.wavenumber.nxpc.bridge.video.NxpCupMp4Initialization
import org.junit.Assert.assertEquals
import org.junit.Assert.assertNull
import org.junit.Assert.assertTrue
import org.junit.Test

class NxpCupH264RelayMailboxTest {
    @Test
    fun viewerStartsWithInitializationAndIdr() {
        val mailbox = NxpCupH264RelayMailbox(capacity = 4)
        mailbox.offerInitialization(NxpCupMp4Initialization(byteArrayOf(1), 0x42000d))
        assertEquals(1, mailbox.offerFragment(fragment(1, key = false)))
        assertNull(mailbox.takePacket())
        assertEquals(0, mailbox.offerFragment(fragment(2, key = true)))

        val initialization = requireNotNull(mailbox.takePacket())
        assertTrue(initialization.initialization)
        assertTrue(initialization.discontinuity)
        assertEquals(0x42000d, initialization.codecConfig)
        val idr = requireNotNull(mailbox.takePacket())
        assertTrue(idr.keyFrame)
        assertNull(mailbox.takePacket())
    }

    @Test
    fun reconnectDropsDependentFramesUntilNextIdr() {
        val mailbox = NxpCupH264RelayMailbox(capacity = 4)
        mailbox.offerInitialization(NxpCupMp4Initialization(byteArrayOf(1), 0x42000d))
        mailbox.offerFragment(fragment(1, key = true))
        mailbox.takePacket()
        mailbox.takePacket()
        mailbox.resetForViewer()
        assertEquals(1, mailbox.offerFragment(fragment(2, key = false)))
        assertNull(mailbox.takePacket())
        mailbox.offerFragment(fragment(3, key = true))
        assertTrue(requireNotNull(mailbox.takePacket()).initialization)
        assertEquals(3, requireNotNull(mailbox.takePacket()).frameId)
    }

    private fun fragment(id: Long, key: Boolean) =
        NxpCupMp4Fragment(id, id, key, byteArrayOf(id.toByte()))
}
