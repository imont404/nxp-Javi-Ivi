#include "avc_usb_debug_stream.h"

#include <stdint.h>
#include <string.h>

#include "avc_usb_debug_protocol.h"
#include "avc__master_config.h"
#include "board.h"
#include "clock_config.h"
#include "fsl_common.h"
#include "fsl_debug_console.h"
#include "fsl_device_registers.h"
#include "usb_device_config.h"
#include "usb.h"
#include "usb_device.h"
#include "usb_device_class.h"
#include "usb_device_cdc_acm.h"
#include "usb_device_ch9.h"
#include "usb_device_descriptor.h"
#include "usb_phy.h"

#if !(defined(USB_DEVICE_CONFIG_EHCI) && (USB_DEVICE_CONFIG_EHCI > 0U))
#error AVC USB debug stream expects the MCXN947 HS EHCI device controller.
#endif

#define AVC_USB_CONTROLLER_ID kUSB_ControllerEhci0
#define AVC_USB_INTERRUPT_PRIORITY (3U)
#define AVC_USB_DATA_BUFF_SIZE HS_CDC_VCOM_BULK_OUT_PACKET_SIZE

#define AVC_USB_STREAM_FRAME_WIDTH ((uint32_t)CONFIG__CAMERA_RESOLUTION_X)
#define AVC_USB_STREAM_FRAME_HEIGHT ((uint32_t)CONFIG__CAMERA_RESOLUTION_Y)
#define AVC_USB_STREAM_FRAME_BYTES (AVC_USB_STREAM_FRAME_WIDTH * AVC_USB_STREAM_FRAME_HEIGHT * 2U)
#define AVC_USB_STREAM_TX_BYTES (16U * 1024U)
#define AVC_USB_STREAM_DATA_OFFSET \
    (AVC_DBG_PACKET_HEADER_BYTES + AVC_DBG_RUI_WRITE_FRAME_BUFFER_RAW_HEADER_BYTES)
#define AVC_USB_STREAM_PAYLOAD_MAX_BYTES (AVC_USB_STREAM_TX_BYTES - AVC_USB_STREAM_DATA_OFFSET)
#define AVC_USB_STREAM_STATS_INTERVAL_CAMERA_FRAMES (24U)
#define AVC_USB_STREAM_STATS_INTERVAL_SYNTHETIC_FRAMES (0U)

#define AVC_USB_LINE_CODING_SIZE (0x07U)
#define AVC_USB_LINE_CODING_DTERATE (115200U)
#define AVC_USB_LINE_CODING_CHARFORMAT (0x00U)
#define AVC_USB_LINE_CODING_PARITYTYPE (0x00U)
#define AVC_USB_LINE_CODING_DATABITS (0x08U)

#define AVC_USB_COMM_FEATURE_DATA_SIZE (0x02U)
#define AVC_USB_STATUS_ABSTRACT_STATE (0x0000U)
#define AVC_USB_COUNTRY_SETTING (0x0000U)

#define AVC_USB_NOTIF_PACKET_SIZE (0x08U)
#define AVC_USB_UART_BITMAP_SIZE (0x02U)
#define AVC_USB_NOTIF_REQUEST_TYPE (0xA1U)

typedef struct avc_usb_cdc_vcom
{
    usb_device_handle deviceHandle;
    class_handle_t cdcAcmHandle;
    volatile uint8_t attach;
    uint8_t speed;
    volatile uint8_t startTransactions;
    uint8_t currentConfiguration;
    uint8_t currentInterfaceAlternateSetting[USB_CDC_VCOM_INTERFACE_COUNT];
} avc_usb_cdc_vcom_t;

typedef struct avc_usb_cdc_acm_info
{
    uint8_t serialStateBuf[AVC_USB_NOTIF_PACKET_SIZE + AVC_USB_UART_BITMAP_SIZE];
    uint8_t dtePresent;
    uint16_t breakDuration;
    uint8_t dteStatus;
    uint8_t currentInterface;
    uint16_t uartState;
} avc_usb_cdc_acm_info_t;

typedef enum avc_usb_debug_stream_source
{
    AVC_USB_DEBUG_STREAM_SOURCE_CAMERA = AVC_DBG_STREAM_SOURCE_CAMERA,
    AVC_USB_DEBUG_STREAM_SOURCE_SYNTHETIC = AVC_DBG_STREAM_SOURCE_SYNTHETIC,
} avc_usb_debug_stream_source_t;

extern usb_device_endpoint_struct_t g_UsbDeviceCdcVcomDicEndpoints[];
extern usb_device_class_struct_t g_UsbDeviceCdcVcomConfig;

static void avc_usb_debug_stream__send_next_chunk(void);
static usb_status_t avc_usb_debug_stream__cdc_callback(class_handle_t handle, uint32_t event, void *param);
static usb_status_t avc_usb_debug_stream__device_callback(usb_device_handle handle, uint32_t event, void *param);

USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
static uint8_t s_lineCoding[AVC_USB_LINE_CODING_SIZE] = {
    (AVC_USB_LINE_CODING_DTERATE >> 0U) & 0xFFU,
    (AVC_USB_LINE_CODING_DTERATE >> 8U) & 0xFFU,
    (AVC_USB_LINE_CODING_DTERATE >> 16U) & 0xFFU,
    (AVC_USB_LINE_CODING_DTERATE >> 24U) & 0xFFU,
    AVC_USB_LINE_CODING_CHARFORMAT,
    AVC_USB_LINE_CODING_PARITYTYPE,
    AVC_USB_LINE_CODING_DATABITS,
};

USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
static uint8_t s_abstractState[AVC_USB_COMM_FEATURE_DATA_SIZE] = {
    (AVC_USB_STATUS_ABSTRACT_STATE >> 0U) & 0xFFU,
    (AVC_USB_STATUS_ABSTRACT_STATE >> 8U) & 0xFFU,
};

USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
static uint8_t s_countryCode[AVC_USB_COMM_FEATURE_DATA_SIZE] = {
    (AVC_USB_COUNTRY_SETTING >> 0U) & 0xFFU,
    (AVC_USB_COUNTRY_SETTING >> 8U) & 0xFFU,
};

USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) static avc_usb_cdc_acm_info_t s_usbCdcAcmInfo;
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) static uint8_t s_currRecvBuf[AVC_USB_DATA_BUFF_SIZE];
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) static uint8_t s_streamTxBuf[AVC_USB_STREAM_TX_BYTES];

static avc_usb_cdc_vcom_t s_cdcVcom;
static volatile uint32_t s_recvSize;
static volatile uint8_t s_streamEnabled;
static volatile uint8_t s_streamTxBusy;
static volatile uint8_t s_streamFrameActive;
static uint8_t s_usbInitialized;
static avc_usb_debug_stream_source_t s_streamSource;
static const uint8_t *s_streamFrameData;
static uint32_t s_streamFrameId;
static uint32_t s_streamNextFrameId;
static uint32_t s_streamSequence;
static uint32_t s_streamOffset;
static uint32_t s_streamBusyCount;
static uint32_t s_streamSendErrorCount;
static uint32_t s_streamDroppedFrameCount;
static uint32_t s_streamStartedFrameCount;
static uint32_t s_streamCompletedFrameCount;
static uint32_t s_streamPacketsSent;
static uint32_t s_streamBytesSent;
static uint32_t s_streamRxCommandCount;
static volatile uint8_t s_streamStatsDue;
static volatile uint8_t s_streamDroppedBeforePacket;

static usb_device_class_config_struct_t s_cdcAcmConfig[1] = {{
    avc_usb_debug_stream__cdc_callback,
    0,
    &g_UsbDeviceCdcVcomConfig,
}};

static usb_device_class_config_list_struct_t s_cdcAcmConfigList = {
    s_cdcAcmConfig,
    avc_usb_debug_stream__device_callback,
    1,
};

static uint8_t avc_usb_debug_stream__upper(uint8_t value)
{
    if ((value >= (uint8_t)'a') && (value <= (uint8_t)'z'))
    {
        return (uint8_t)(value - ((uint8_t)'a' - (uint8_t)'A'));
    }
    return value;
}

static uint8_t avc_usb_debug_stream__command_matches(const uint8_t *data, uint32_t length, const char *command)
{
    uint32_t index = 0U;
    uint32_t commandIndex = 0U;

    while ((index < length) && ((data[index] == (uint8_t)' ') || (data[index] == (uint8_t)'\r') ||
                                (data[index] == (uint8_t)'\n') || (data[index] == (uint8_t)'\t')))
    {
        index++;
    }

    while (command[commandIndex] != '\0')
    {
        if (index >= length)
        {
            return 0U;
        }

        if (avc_usb_debug_stream__upper(data[index]) != (uint8_t)command[commandIndex])
        {
            return 0U;
        }

        index++;
        commandIndex++;
    }

    return 1U;
}

static void avc_usb_debug_stream__fill_static_payload(void)
{
    for (uint32_t i = 0U; i < AVC_USB_STREAM_PAYLOAD_MAX_BYTES; i += 2U)
    {
        uint32_t pixel = i >> 1U;
        uint16_t r = (uint16_t)((pixel >> 5U) & 0x1FU);
        uint16_t g = (uint16_t)((pixel >> 2U) & 0x3FU);
        uint16_t b = (uint16_t)((pixel >> 9U) & 0x1FU);
        uint16_t rgb = (uint16_t)((r << 11U) | (g << 5U) | b);

        s_streamTxBuf[AVC_USB_STREAM_DATA_OFFSET + i] = (uint8_t)(rgb & 0xFFU);
        s_streamTxBuf[AVC_USB_STREAM_DATA_OFFSET + i + 1U] = (uint8_t)((rgb >> 8U) & 0xFFU);
    }
}

static uint32_t avc_usb_debug_stream__stats_interval_frames(void)
{
    if (s_streamSource == AVC_USB_DEBUG_STREAM_SOURCE_SYNTHETIC)
    {
        return AVC_USB_STREAM_STATS_INTERVAL_SYNTHETIC_FRAMES;
    }

    return AVC_USB_STREAM_STATS_INTERVAL_CAMERA_FRAMES;
}

static void avc_usb_debug_stream__mark_frame_completed(void)
{
    uint32_t interval;

    s_streamCompletedFrameCount++;
    interval = avc_usb_debug_stream__stats_interval_frames();
    if ((interval != 0U) && ((s_streamCompletedFrameCount % interval) == 0U))
    {
        s_streamStatsDue = 1U;
    }
}

static uint8_t avc_usb_debug_stream__send_stats_report(void)
{
    avc_dbg_packet_header_t *packetHeader;
    avc_dbg_stats_report_t *stats;
    uint32_t streamFlags = 0U;
    uint32_t packetBytes = AVC_DBG_PACKET_HEADER_BYTES + AVC_DBG_STATS_REPORT_BYTES;
    usb_status_t error;

    if (s_streamStatsDue == 0U)
    {
        return 0U;
    }

    if ((s_streamSource == AVC_USB_DEBUG_STREAM_SOURCE_CAMERA) && (s_streamFrameActive != 0U))
    {
        return 0U;
    }

    if (s_streamEnabled != 0U)
    {
        streamFlags |= AVC_DBG_STATS_FLAG_STREAM_ENABLED;
    }
    if (s_streamTxBusy != 0U)
    {
        streamFlags |= AVC_DBG_STATS_FLAG_TX_BUSY;
    }
    if (s_streamFrameActive != 0U)
    {
        streamFlags |= AVC_DBG_STATS_FLAG_FRAME_ACTIVE;
    }

    packetHeader = (avc_dbg_packet_header_t *)s_streamTxBuf;
    packetHeader->magic = AVC_DBG_MAGIC;
    packetHeader->version = AVC_DBG_VERSION;
    packetHeader->header_bytes = AVC_DBG_PACKET_HEADER_BYTES;
    packetHeader->flags = (s_streamDroppedBeforePacket != 0U) ? AVC_DBG_PACKET_FLAG_DROPPED_BEFORE : 0U;
    packetHeader->msg_id = AVC_DBG_STATS_REPORT;
    packetHeader->sequence = s_streamSequence;
    packetHeader->payload_length = AVC_DBG_STATS_REPORT_BYTES;
    packetHeader->arg0 = s_streamDroppedFrameCount;
    packetHeader->arg1 = s_streamBusyCount;
    packetHeader->arg2 = s_streamSendErrorCount;

    stats = (avc_dbg_stats_report_t *)&s_streamTxBuf[AVC_DBG_PACKET_HEADER_BYTES];
    stats->stream_source = (uint32_t)s_streamSource;
    stats->stream_flags = streamFlags;
    stats->next_sequence = s_streamSequence;
    stats->current_frame_id = s_streamFrameId;
    stats->frames_started = s_streamStartedFrameCount;
    stats->frames_completed = s_streamCompletedFrameCount;
    stats->frames_dropped = s_streamDroppedFrameCount;
    stats->packets_sent = s_streamPacketsSent;
    stats->bytes_sent = s_streamBytesSent;
    stats->endpoint_busy_count = s_streamBusyCount;
    stats->send_error_count = s_streamSendErrorCount;
    stats->rx_command_count = s_streamRxCommandCount;

    error = USB_DeviceCdcAcmSend(s_cdcVcom.cdcAcmHandle,
                                 USB_CDC_VCOM_BULK_IN_ENDPOINT,
                                 s_streamTxBuf,
                                 packetBytes);
    if (error == kStatus_USB_Success)
    {
        s_streamTxBusy = 1U;
        s_streamSequence++;
        s_streamPacketsSent++;
        s_streamBytesSent += packetBytes;
        s_streamStatsDue = 0U;
        s_streamDroppedBeforePacket = 0U;
        return 1U;
    }
    else if (error == kStatus_USB_Busy)
    {
        s_streamBusyCount++;
    }
    else
    {
        s_streamSendErrorCount++;
    }

    return 0U;
}

static void avc_usb_debug_stream__start_source(avc_usb_debug_stream_source_t source)
{
    uint32_t usbOsaCurrentSr = DisableGlobalIRQ();

    s_streamSource = source;
    s_streamFrameData = NULL;
    s_streamFrameActive = 0U;
    s_streamFrameId = 0U;
    s_streamNextFrameId = 0U;
    s_streamSequence = 0U;
    s_streamOffset = 0U;
    s_streamDroppedFrameCount = 0U;
    s_streamStartedFrameCount = 0U;
    s_streamCompletedFrameCount = 0U;
    s_streamPacketsSent = 0U;
    s_streamBytesSent = 0U;
    s_streamBusyCount = 0U;
    s_streamSendErrorCount = 0U;
    s_streamStatsDue = (source == AVC_USB_DEBUG_STREAM_SOURCE_CAMERA) ? 1U : 0U;
    s_streamDroppedBeforePacket = 0U;
    s_streamEnabled = 1U;

    EnableGlobalIRQ(usbOsaCurrentSr);
}

void avc_usb_debug_stream__start(void)
{
    avc_usb_debug_stream__start_source(AVC_USB_DEBUG_STREAM_SOURCE_CAMERA);
}

static void avc_usb_debug_stream__start_synthetic(void)
{
    avc_usb_debug_stream__start_source(AVC_USB_DEBUG_STREAM_SOURCE_SYNTHETIC);
}

void avc_usb_debug_stream__stop(void)
{
    uint32_t usbOsaCurrentSr = DisableGlobalIRQ();

    s_streamEnabled = 0U;
    s_streamFrameData = NULL;
    s_streamFrameActive = 0U;
    s_streamOffset = 0U;
    s_streamStatsDue = 0U;
    s_streamDroppedBeforePacket = 0U;

    EnableGlobalIRQ(usbOsaCurrentSr);
}

bool avc_usb_debug_stream__is_open(void)
{
    return (s_cdcVcom.attach != 0U) && (s_cdcVcom.startTransactions != 0U);
}

bool avc_usb_debug_stream__publish_frame(const uint16_t *frame)
{
    bool accepted = false;
    uint32_t usbOsaCurrentSr;

    if ((frame == NULL) || (s_streamEnabled == 0U) || !avc_usb_debug_stream__is_open())
    {
        return false;
    }

    usbOsaCurrentSr = DisableGlobalIRQ();
    if ((s_streamEnabled != 0U) && (s_streamSource == AVC_USB_DEBUG_STREAM_SOURCE_CAMERA) &&
        (s_streamFrameActive == 0U))
    {
        s_streamFrameData = (const uint8_t *)frame;
        s_streamFrameId = s_streamNextFrameId;
        s_streamNextFrameId++;
        s_streamOffset = 0U;
        s_streamFrameActive = 1U;
        s_streamStartedFrameCount++;
        accepted = true;
    }
    else if ((s_streamEnabled != 0U) && (s_streamSource == AVC_USB_DEBUG_STREAM_SOURCE_CAMERA))
    {
        s_streamDroppedFrameCount++;
        s_streamDroppedBeforePacket = 1U;
    }
    EnableGlobalIRQ(usbOsaCurrentSr);

    if (accepted)
    {
        avc_usb_debug_stream__send_next_chunk();
    }

    return accepted;
}

static void avc_usb_debug_stream__handle_command(const uint8_t *data, uint32_t length)
{
    if ((data == NULL) || (length == 0U))
    {
        return;
    }

    s_streamRxCommandCount++;

    if ((data[0] == (uint8_t)'0') || avc_usb_debug_stream__command_matches(data, length, "STOP"))
    {
        avc_usb_debug_stream__stop();
        return;
    }

    if (avc_usb_debug_stream__command_matches(data, length, "START_SYNTH") ||
        avc_usb_debug_stream__command_matches(data, length, "SYNTH"))
    {
        avc_usb_debug_stream__start_synthetic();
        return;
    }

    if ((data[0] == (uint8_t)'1') || avc_usb_debug_stream__command_matches(data, length, "START") ||
        avc_usb_debug_stream__command_matches(data, length, "GO"))
    {
        avc_usb_debug_stream__start();
    }
}

static void avc_usb_debug_stream__send_next_chunk(void)
{
    uint32_t payloadBytes;
    uint32_t packetPayloadBytes;
    uint32_t remaining;
    uint32_t chunkFlags = 0U;
    const uint8_t *sourceFrameData = NULL;
    avc_dbg_packet_header_t *packetHeader;
    avc_dbg_rui_write_frame_buffer_raw_t *frameChunk;
    usb_status_t error;

    if ((s_streamEnabled == 0U) || !avc_usb_debug_stream__is_open())
    {
        return;
    }

    if (s_streamTxBusy != 0U)
    {
        return;
    }

    if (avc_usb_debug_stream__send_stats_report() != 0U)
    {
        return;
    }

    if (s_streamSource == AVC_USB_DEBUG_STREAM_SOURCE_CAMERA)
    {
        if ((s_streamFrameActive == 0U) || (s_streamFrameData == NULL))
        {
            return;
        }

        sourceFrameData = s_streamFrameData;
    }

    remaining = AVC_USB_STREAM_FRAME_BYTES - s_streamOffset;
    payloadBytes = (remaining > AVC_USB_STREAM_PAYLOAD_MAX_BYTES) ? AVC_USB_STREAM_PAYLOAD_MAX_BYTES : remaining;
    if (s_streamOffset == 0U)
    {
        chunkFlags |= AVC_DBG_RUI_CHUNK_FRAME_START;
    }
    if (payloadBytes == remaining)
    {
        chunkFlags |= AVC_DBG_RUI_CHUNK_FRAME_END;
    }

    if (sourceFrameData != NULL)
    {
        (void)memcpy(&s_streamTxBuf[AVC_USB_STREAM_DATA_OFFSET], &sourceFrameData[s_streamOffset], payloadBytes);
    }

    packetPayloadBytes = AVC_DBG_RUI_WRITE_FRAME_BUFFER_RAW_HEADER_BYTES + payloadBytes;

    packetHeader = (avc_dbg_packet_header_t *)s_streamTxBuf;
    packetHeader->magic = AVC_DBG_MAGIC;
    packetHeader->version = AVC_DBG_VERSION;
    packetHeader->header_bytes = AVC_DBG_PACKET_HEADER_BYTES;
    packetHeader->flags = (s_streamDroppedBeforePacket != 0U) ? AVC_DBG_PACKET_FLAG_DROPPED_BEFORE : 0U;
    packetHeader->msg_id = AVC_DBG_RUI_WRITE_FRAME_BUFFER_RAW;
    packetHeader->sequence = s_streamSequence;
    packetHeader->payload_length = packetPayloadBytes;
    packetHeader->arg0 = s_streamFrameId;
    packetHeader->arg1 = s_streamOffset;
    packetHeader->arg2 = payloadBytes;

    frameChunk = (avc_dbg_rui_write_frame_buffer_raw_t *)&s_streamTxBuf[AVC_DBG_PACKET_HEADER_BYTES];
    frameChunk->frame_id = s_streamFrameId;
    frameChunk->byte_offset = s_streamOffset;
    frameChunk->total_frame_bytes = AVC_USB_STREAM_FRAME_BYTES;
    frameChunk->width = (uint16_t)AVC_USB_STREAM_FRAME_WIDTH;
    frameChunk->height = (uint16_t)AVC_USB_STREAM_FRAME_HEIGHT;
    frameChunk->pixel_format = AVC_DBG_PIXEL_FORMAT_RGB565_LE;
    frameChunk->buffer_id = 0U;
    frameChunk->chunk_flags = chunkFlags;

    error = USB_DeviceCdcAcmSend(s_cdcVcom.cdcAcmHandle,
                                 USB_CDC_VCOM_BULK_IN_ENDPOINT,
                                 s_streamTxBuf,
                                 AVC_DBG_PACKET_HEADER_BYTES + packetPayloadBytes);
    if (error == kStatus_USB_Success)
    {
        s_streamTxBusy = 1U;
        s_streamSequence++;
        s_streamPacketsSent++;
        s_streamBytesSent += AVC_DBG_PACKET_HEADER_BYTES + packetPayloadBytes;
        s_streamDroppedBeforePacket = 0U;
        if ((chunkFlags & AVC_DBG_RUI_CHUNK_FRAME_END) != 0U)
        {
            s_streamOffset = 0U;
            if (s_streamSource == AVC_USB_DEBUG_STREAM_SOURCE_CAMERA)
            {
                s_streamFrameData = NULL;
                s_streamFrameActive = 0U;
            }
            else
            {
                s_streamFrameId++;
                s_streamStartedFrameCount++;
            }
            avc_usb_debug_stream__mark_frame_completed();
        }
        else
        {
            s_streamOffset += payloadBytes;
        }
    }
    else if (error == kStatus_USB_Busy)
    {
        s_streamBusyCount++;
    }
    else
    {
        s_streamSendErrorCount++;
    }
}

static void avc_usb_debug_stream__clock_init(void)
{
    usb_phy_config_struct_t phyConfig = {
        BOARD_USB_PHY_D_CAL,
        BOARD_USB_PHY_TXCAL45DP,
        BOARD_USB_PHY_TXCAL45DM,
    };

    SPC0->ACTIVE_VDELAY = 0x0500U;
    SPC0->ACTIVE_CFG &= ~SPC_ACTIVE_CFG_CORELDO_VDD_DS_MASK;
    SPC0->ACTIVE_CFG |= SPC_ACTIVE_CFG_DCDC_VDD_LVL(0x3U) | SPC_ACTIVE_CFG_CORELDO_VDD_LVL(0x3U) |
                        SPC_ACTIVE_CFG_SYSLDO_VDD_DS_MASK | SPC_ACTIVE_CFG_DCDC_VDD_DS(0x2U);
    while ((SPC0->SC & SPC_SC_BUSY_MASK) != 0U)
    {
    }

    if ((SCG0->LDOCSR & SCG_LDOCSR_LDOEN_MASK) == 0U)
    {
        SCG0->TRIM_LOCK = 0x5a5a0001U;
        SCG0->LDOCSR |= SCG_LDOCSR_LDOEN_MASK;
        while ((SCG0->LDOCSR & SCG_LDOCSR_VOUT_OK_MASK) == 0U)
        {
        }
    }

    SYSCON->AHBCLKCTRLSET[2] |= SYSCON_AHBCLKCTRL2_USB_HS_MASK | SYSCON_AHBCLKCTRL2_USB_HS_PHY_MASK;

    if ((SCG0->SOSCCSR & SCG_SOSCCSR_SOSCVLD_MASK) == 0U)
    {
        (void)CLOCK_SetupExtClocking(BOARD_XTAL0_CLK_HZ);
    }

    SYSCON->CLOCK_CTRL |= SYSCON_CLOCK_CTRL_CLKIN_ENA_MASK | SYSCON_CLOCK_CTRL_CLKIN_ENA_FM_USBH_LPT_MASK;
    CLOCK_EnableClock(kCLOCK_UsbHs);
    CLOCK_EnableClock(kCLOCK_UsbHsPhy);
    (void)CLOCK_EnableUsbhsPhyPllClock(kCLOCK_Usbphy480M, BOARD_XTAL0_CLK_HZ);
    (void)CLOCK_EnableUsbhsClock();
    USB_EhciPhyInit(AVC_USB_CONTROLLER_ID, BOARD_XTAL0_CLK_HZ, &phyConfig);
}

static void avc_usb_debug_stream__isr_enable(void)
{
    uint8_t irqNumber;
    uint8_t usbDeviceEhciIrq[] = USBHS_IRQS;

    irqNumber = usbDeviceEhciIrq[AVC_USB_CONTROLLER_ID - kUSB_ControllerEhci0];
    NVIC_SetPriority((IRQn_Type)irqNumber, AVC_USB_INTERRUPT_PRIORITY);
    EnableIRQ((IRQn_Type)irqNumber);
}

void USB1_HS_IRQHandler(void)
{
    if (s_cdcVcom.deviceHandle != NULL)
    {
        USB_DeviceEhciIsrFunction(s_cdcVcom.deviceHandle);
    }

    SDK_ISR_EXIT_BARRIER;
}

void avc_usb_debug_stream__init(void)
{
    if (s_usbInitialized != 0U)
    {
        return;
    }

    avc_usb_debug_stream__clock_init();

    s_cdcVcom.speed = USB_SPEED_FULL;
    s_cdcVcom.attach = 0U;
    s_cdcVcom.startTransactions = 0U;
    s_cdcVcom.cdcAcmHandle = (class_handle_t)NULL;
    s_cdcVcom.deviceHandle = NULL;
    s_recvSize = 0U;
    s_streamEnabled = 0U;
    s_streamTxBusy = 0U;
    s_streamFrameActive = 0U;
    s_streamSource = AVC_USB_DEBUG_STREAM_SOURCE_CAMERA;
    s_streamFrameData = NULL;
    s_streamFrameId = 0U;
    s_streamNextFrameId = 0U;
    s_streamSequence = 0U;
    s_streamOffset = 0U;
    s_streamBusyCount = 0U;
    s_streamSendErrorCount = 0U;
    s_streamDroppedFrameCount = 0U;
    s_streamStartedFrameCount = 0U;
    s_streamCompletedFrameCount = 0U;
    s_streamPacketsSent = 0U;
    s_streamBytesSent = 0U;
    s_streamRxCommandCount = 0U;
    s_streamStatsDue = 0U;
    s_streamDroppedBeforePacket = 0U;
    avc_usb_debug_stream__fill_static_payload();

    if (USB_DeviceClassInit(AVC_USB_CONTROLLER_ID, &s_cdcAcmConfigList, &s_cdcVcom.deviceHandle) !=
        kStatus_USB_Success)
    {
        (void)DbgConsole_Printf("avc_usb: USB_DeviceClassInit failed\r\n");
        return;
    }

    s_cdcVcom.cdcAcmHandle = s_cdcAcmConfigList.config->classHandle;
    avc_usb_debug_stream__isr_enable();

    SDK_DelayAtLeastUs(5000U, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
    USB_DeviceRun(s_cdcVcom.deviceHandle);
    s_usbInitialized = 1U;

    (void)DbgConsole_Printf("avc_usb: CDC debug stream initialized\r\n");
}

void avc_usb_debug_stream__service(void)
{
    uint32_t usbOsaCurrentSr;
    uint32_t recvSize = 0U;
    usb_status_t error;

    if (!avc_usb_debug_stream__is_open())
    {
        return;
    }

    if ((s_recvSize != 0U) && (s_recvSize != USB_CANCELLED_TRANSFER_LENGTH))
    {
        usbOsaCurrentSr = DisableGlobalIRQ();
        if ((s_recvSize != 0U) && (s_recvSize != USB_CANCELLED_TRANSFER_LENGTH))
        {
            recvSize = s_recvSize;
            s_recvSize = 0U;
        }
        EnableGlobalIRQ(usbOsaCurrentSr);

        avc_usb_debug_stream__handle_command(s_currRecvBuf, recvSize);

        error = USB_DeviceCdcAcmRecv(s_cdcVcom.cdcAcmHandle,
                                     USB_CDC_VCOM_BULK_OUT_ENDPOINT,
                                     s_currRecvBuf,
                                     g_UsbDeviceCdcVcomDicEndpoints[1].maxPacketSize);
        if (error != kStatus_USB_Success)
        {
            (void)error;
        }
    }

    avc_usb_debug_stream__send_next_chunk();
}

static usb_status_t avc_usb_debug_stream__cdc_callback(class_handle_t handle, uint32_t event, void *param)
{
#if ((defined USB_DEVICE_CONFIG_CDC_CIC_EP_DISABLE) && (USB_DEVICE_CONFIG_CDC_CIC_EP_DISABLE > 0U))
#else
    uint32_t len;
#endif
    uint8_t *uartBitmap;
    usb_device_cdc_acm_request_param_struct_t *acmReqParam;
    usb_device_endpoint_callback_message_struct_t *epCbParam;
    usb_status_t error = kStatus_USB_InvalidRequest;
    avc_usb_cdc_acm_info_t *acmInfo = &s_usbCdcAcmInfo;

    acmReqParam = (usb_device_cdc_acm_request_param_struct_t *)param;
    epCbParam = (usb_device_endpoint_callback_message_struct_t *)param;

    switch (event)
    {
        case kUSB_DeviceCdcEventSendResponse:
            if (epCbParam->buffer == s_streamTxBuf)
            {
                s_streamTxBusy = 0U;
                avc_usb_debug_stream__send_next_chunk();
            }
            error = kStatus_USB_Success;
            break;

        case kUSB_DeviceCdcEventRecvResponse:
            if (avc_usb_debug_stream__is_open())
            {
                s_recvSize = epCbParam->length;
                error = kStatus_USB_Success;

                if (s_recvSize == 0U)
                {
                    error = USB_DeviceCdcAcmRecv(handle,
                                                 USB_CDC_VCOM_BULK_OUT_ENDPOINT,
                                                 s_currRecvBuf,
                                                 g_UsbDeviceCdcVcomDicEndpoints[1].maxPacketSize);
                }
            }
            break;

        case kUSB_DeviceCdcEventSerialStateNotif:
            ((usb_device_cdc_acm_struct_t *)handle)->hasSentState = 0U;
            error = kStatus_USB_Success;
            break;

        case kUSB_DeviceCdcEventSetCommFeature:
            if (acmReqParam->setupValue == USB_DEVICE_CDC_FEATURE_ABSTRACT_STATE)
            {
                if (acmReqParam->isSetup == 1U)
                {
                    *(acmReqParam->buffer) = s_abstractState;
                    *(acmReqParam->length) = sizeof(s_abstractState);
                }
                error = kStatus_USB_Success;
            }
            else if (acmReqParam->setupValue == USB_DEVICE_CDC_FEATURE_COUNTRY_SETTING)
            {
                if (acmReqParam->isSetup == 1U)
                {
                    *(acmReqParam->buffer) = s_countryCode;
                    *(acmReqParam->length) = sizeof(s_countryCode);
                }
                error = kStatus_USB_Success;
            }
            break;

        case kUSB_DeviceCdcEventGetCommFeature:
            if (acmReqParam->setupValue == USB_DEVICE_CDC_FEATURE_ABSTRACT_STATE)
            {
                *(acmReqParam->buffer) = s_abstractState;
                *(acmReqParam->length) = AVC_USB_COMM_FEATURE_DATA_SIZE;
                error = kStatus_USB_Success;
            }
            else if (acmReqParam->setupValue == USB_DEVICE_CDC_FEATURE_COUNTRY_SETTING)
            {
                *(acmReqParam->buffer) = s_countryCode;
                *(acmReqParam->length) = AVC_USB_COMM_FEATURE_DATA_SIZE;
                error = kStatus_USB_Success;
            }
            break;

        case kUSB_DeviceCdcEventGetLineCoding:
            *(acmReqParam->buffer) = s_lineCoding;
            *(acmReqParam->length) = AVC_USB_LINE_CODING_SIZE;
            error = kStatus_USB_Success;
            break;

        case kUSB_DeviceCdcEventSetLineCoding:
            if (acmReqParam->isSetup == 1U)
            {
                *(acmReqParam->buffer) = s_lineCoding;
                *(acmReqParam->length) = sizeof(s_lineCoding);
            }
            error = kStatus_USB_Success;
            break;

        case kUSB_DeviceCdcEventSetControlLineState:
            acmInfo->dteStatus = (uint8_t)acmReqParam->setupValue;
            if ((acmInfo->dteStatus & USB_DEVICE_CDC_CONTROL_SIG_BITMAP_CARRIER_ACTIVATION) != 0U)
            {
                acmInfo->uartState |= USB_DEVICE_CDC_UART_STATE_TX_CARRIER;
            }
            else
            {
                acmInfo->uartState &= (uint16_t)~USB_DEVICE_CDC_UART_STATE_TX_CARRIER;
            }

            if ((acmInfo->dteStatus & USB_DEVICE_CDC_CONTROL_SIG_BITMAP_DTE_PRESENCE) != 0U)
            {
                acmInfo->uartState |= USB_DEVICE_CDC_UART_STATE_RX_CARRIER;
            }
            else
            {
                acmInfo->uartState &= (uint16_t)~USB_DEVICE_CDC_UART_STATE_RX_CARRIER;
            }

            acmInfo->dtePresent =
                ((acmInfo->dteStatus & USB_DEVICE_CDC_CONTROL_SIG_BITMAP_DTE_PRESENCE) != 0U) ? 1U : 0U;

            acmInfo->serialStateBuf[0] = AVC_USB_NOTIF_REQUEST_TYPE;
            acmInfo->serialStateBuf[1] = USB_DEVICE_CDC_NOTIF_SERIAL_STATE;
            acmInfo->serialStateBuf[2] = 0x00U;
            acmInfo->serialStateBuf[3] = 0x00U;
            acmInfo->serialStateBuf[4] = acmReqParam->interfaceIndex;
            acmInfo->serialStateBuf[5] = 0x00U;
            acmInfo->serialStateBuf[6] = AVC_USB_UART_BITMAP_SIZE;
            acmInfo->serialStateBuf[7] = 0x00U;
            uartBitmap = (uint8_t *)&acmInfo->serialStateBuf[AVC_USB_NOTIF_PACKET_SIZE + AVC_USB_UART_BITMAP_SIZE - 2U];
            uartBitmap[0] = (uint8_t)(acmInfo->uartState & 0xFFU);
            uartBitmap[1] = (uint8_t)((acmInfo->uartState >> 8U) & 0xFFU);

#if ((defined USB_DEVICE_CONFIG_CDC_CIC_EP_DISABLE) && (USB_DEVICE_CONFIG_CDC_CIC_EP_DISABLE > 0U))
#else
            len = (uint32_t)(AVC_USB_NOTIF_PACKET_SIZE + AVC_USB_UART_BITMAP_SIZE);
            if (((usb_device_cdc_acm_struct_t *)handle)->hasSentState == 0U)
            {
                error = USB_DeviceCdcAcmSend(handle,
                                             USB_CDC_VCOM_INTERRUPT_IN_ENDPOINT,
                                             acmInfo->serialStateBuf,
                                             len);
                ((usb_device_cdc_acm_struct_t *)handle)->hasSentState = 1U;
            }
#endif

            if (s_cdcVcom.attach == 1U)
            {
                if (acmInfo->dtePresent != 0U)
                {
                    s_cdcVcom.startTransactions = 1U;
                }
                else
                {
                    s_cdcVcom.startTransactions = 0U;
                    avc_usb_debug_stream__stop();
                }
            }
            error = kStatus_USB_Success;
            break;

        default:
            break;
    }

    return error;
}

static usb_status_t avc_usb_debug_stream__device_callback(usb_device_handle handle, uint32_t event, void *param)
{
    usb_status_t error = kStatus_USB_InvalidRequest;
    uint16_t *temp16 = (uint16_t *)param;
    uint8_t *temp8 = (uint8_t *)param;

    switch (event)
    {
        case kUSB_DeviceEventBusReset:
            s_cdcVcom.attach = 0U;
            s_cdcVcom.startTransactions = 0U;
            s_cdcVcom.currentConfiguration = 0U;
            s_streamTxBusy = 0U;
            avc_usb_debug_stream__stop();
            error = kStatus_USB_Success;

            if (USB_DeviceClassGetSpeed(AVC_USB_CONTROLLER_ID, &s_cdcVcom.speed) == kStatus_USB_Success)
            {
                USB_DeviceSetSpeed(handle, s_cdcVcom.speed);
            }
            break;

        case kUSB_DeviceEventSetConfiguration:
            if (*temp8 == 0U)
            {
                s_cdcVcom.attach = 0U;
                s_cdcVcom.startTransactions = 0U;
                s_cdcVcom.currentConfiguration = 0U;
                s_streamTxBusy = 0U;
                avc_usb_debug_stream__stop();
                error = kStatus_USB_Success;
            }
            else if (*temp8 == USB_CDC_VCOM_CONFIGURE_INDEX)
            {
                s_cdcVcom.attach = 1U;
                s_cdcVcom.currentConfiguration = *temp8;
                error = kStatus_USB_Success;
                (void)USB_DeviceCdcAcmRecv(s_cdcVcom.cdcAcmHandle,
                                           USB_CDC_VCOM_BULK_OUT_ENDPOINT,
                                           s_currRecvBuf,
                                           g_UsbDeviceCdcVcomDicEndpoints[1].maxPacketSize);
            }
            break;

        case kUSB_DeviceEventSetInterface:
            if (s_cdcVcom.attach != 0U)
            {
                uint8_t interface = (uint8_t)((*temp16 & 0xFF00U) >> 8U);
                uint8_t alternateSetting = (uint8_t)(*temp16 & 0x00FFU);

                if ((interface == USB_CDC_VCOM_COMM_INTERFACE_INDEX) &&
                    (alternateSetting < USB_CDC_VCOM_COMM_INTERFACE_ALTERNATE_COUNT))
                {
                    s_cdcVcom.currentInterfaceAlternateSetting[interface] = alternateSetting;
                    error = kStatus_USB_Success;
                }
                else if ((interface == USB_CDC_VCOM_DATA_INTERFACE_INDEX) &&
                         (alternateSetting < USB_CDC_VCOM_DATA_INTERFACE_ALTERNATE_COUNT))
                {
                    s_cdcVcom.currentInterfaceAlternateSetting[interface] = alternateSetting;
                    error = kStatus_USB_Success;
                }
            }
            break;

        case kUSB_DeviceEventGetConfiguration:
            if (param != NULL)
            {
                *temp8 = s_cdcVcom.currentConfiguration;
                error = kStatus_USB_Success;
            }
            break;

        case kUSB_DeviceEventGetInterface:
            if (param != NULL)
            {
                uint8_t interface = (uint8_t)((*temp16 & 0xFF00U) >> 8U);
                if (interface < USB_CDC_VCOM_INTERFACE_COUNT)
                {
                    *temp16 = (*temp16 & 0xFF00U) | s_cdcVcom.currentInterfaceAlternateSetting[interface];
                    error = kStatus_USB_Success;
                }
            }
            break;

        case kUSB_DeviceEventGetDeviceDescriptor:
            if (param != NULL)
            {
                error = USB_DeviceGetDeviceDescriptor(handle, (usb_device_get_device_descriptor_struct_t *)param);
            }
            break;

        case kUSB_DeviceEventGetConfigurationDescriptor:
            if (param != NULL)
            {
                error = USB_DeviceGetConfigurationDescriptor(
                    handle,
                    (usb_device_get_configuration_descriptor_struct_t *)param);
            }
            break;

        case kUSB_DeviceEventGetStringDescriptor:
            if (param != NULL)
            {
                error = USB_DeviceGetStringDescriptor(handle, (usb_device_get_string_descriptor_struct_t *)param);
            }
            break;

        default:
            break;
    }

    return error;
}
