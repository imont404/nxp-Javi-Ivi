#include "avc_usb_debug_stream.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "avc_usb_debug_protocol.h"
#include "avc__master_config.h"
#include "board.h"
#include "clock_config.h"
#include "e.h"
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
#define AVC_USB_CONTROL_RX_BYTES (1024U)
#define AVC_USB_CONTROL_MAX_PAYLOAD_BYTES (128U)
#define AVC_USB_CONTROL_RESPONSE_PAYLOAD_BYTES (AVC_DBG_CONTROL_HELLO_RESPONSE_BYTES)
#define AVC_USB_CONTROL_RESPONSE_QUEUE_DEPTH (4U)
#define AVC_USB_TX_CONTROL_BURST_MAX (4U)
#define AVC_USB_LOG_QUEUE_DEPTH (8U)
#define AVC_USB_LOG_CATEGORY_MAX_BYTES (15U)
#define AVC_USB_LOG_TEXT_MAX_BYTES (160U)
#define AVC_USB_TELEMETRY_QUEUE_DEPTH (16U)
#define AVC_USB_TELEMETRY_NAME_MAX_BYTES (31U)
#define AVC_USB_TELEMETRY_UNITS_MAX_BYTES (15U)
#define AVC_USB_TX_DIAGNOSTIC_BURST_MAX (2U)
#define AVC_USB_CONTROL_CAPABILITIES                                                                                \
    (AVC_DBG_CAPABILITY_FRAMED_CONTROL | AVC_DBG_CAPABILITY_CAMERA_FRAMES |                                        \
     AVC_DBG_CAPABILITY_SYNTHETIC_FRAMES | AVC_DBG_CAPABILITY_STREAM_STATS | AVC_DBG_CAPABILITY_LOG_TEXT |         \
     AVC_DBG_CAPABILITY_NAMED_TELEMETRY | AVC_DBG_CAPABILITY_ENTER_ISP)
#define AVC_USB_CONTROL_SUPPORTED_CHANNELS \
    (AVC_DBG_CHANNEL_FRAMES | AVC_DBG_CHANNEL_STATS | AVC_DBG_CHANNEL_LOGS | AVC_DBG_CHANNEL_TELEMETRY)

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

typedef struct avc_usb_control_response
{
    uint32_t msgId;
    uint32_t requestSequence;
    uint32_t status;
    uint32_t sessionId;
    uint32_t payloadLength;
    uint8_t payload[AVC_USB_CONTROL_RESPONSE_PAYLOAD_BYTES];
} avc_usb_control_response_t;

typedef struct avc_usb_log_record
{
    uint32_t timestampMs;
    uint32_t recordId;
    uint16_t textLength;
    uint8_t level;
    uint8_t categoryLength;
    char category[AVC_USB_LOG_CATEGORY_MAX_BYTES];
    char text[AVC_USB_LOG_TEXT_MAX_BYTES];
} avc_usb_log_record_t;

typedef struct avc_usb_telemetry_scalar
{
    uint32_t timestampMs;
    uint32_t sampleId;
    uint32_t valueBits;
    uint16_t nameLength;
    uint8_t valueType;
    uint8_t unitsLength;
    char name[AVC_USB_TELEMETRY_NAME_MAX_BYTES];
    char units[AVC_USB_TELEMETRY_UNITS_MAX_BYTES];
} avc_usb_telemetry_scalar_t;

extern usb_device_endpoint_struct_t g_UsbDeviceCdcVcomDicEndpoints[];
extern usb_device_class_struct_t g_UsbDeviceCdcVcomConfig;

static void avc_usb_debug_stream__schedule_tx(void);
static void avc_usb_debug_stream__handle_received_data(const uint8_t *data, uint32_t length);
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
static volatile uint8_t s_sessionActive;
static uint8_t s_statsEnabled;
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
static volatile uint8_t s_streamFrameResyncPending;
static volatile uint32_t s_cameraFrameGeneration;
static uint32_t s_streamFrameGeneration;
static uint32_t s_sessionId;
static uint32_t s_controlRxLength;
static uint8_t s_controlRxBuffer[AVC_USB_CONTROL_RX_BYTES];
static avc_usb_control_response_t s_controlResponseQueue[AVC_USB_CONTROL_RESPONSE_QUEUE_DEPTH];
static volatile uint8_t s_controlResponseHead;
static volatile uint8_t s_controlResponseCount;
static uint8_t s_txControlBurst;
static uint32_t s_controlResponseDropCount;
static uint32_t s_controlResponseQueueHighWater;
static avc_usb_log_record_t s_logQueue[AVC_USB_LOG_QUEUE_DEPTH];
static volatile uint8_t s_logHead;
static volatile uint8_t s_logCount;
static uint8_t s_logEnabled;
static uint8_t s_txDiagnosticBurst;
static uint32_t s_logNextRecordId;
static uint32_t s_logDropCount;
static uint32_t s_logQueueHighWater;
static avc_usb_telemetry_scalar_t s_telemetryQueue[AVC_USB_TELEMETRY_QUEUE_DEPTH];
static volatile uint8_t s_telemetryHead;
static volatile uint8_t s_telemetryCount;
static uint8_t s_telemetryEnabled;
static uint8_t s_txPreferTelemetry;
static uint32_t s_telemetryNextSampleId;
static uint32_t s_telemetryDropCount;
static uint32_t s_telemetryQueueHighWater;
static uint32_t s_telemetryCoalesceCount;
static volatile uint8_t s_enterIspRequested;

#if CONFIG__USB_DEBUG_PROFILE_ENABLE
static uint32_t s_profileReportTick;
static uint32_t s_profileCalls;
static uint32_t s_profileOpenCalls;
static uint32_t s_profileValidSamples;
static uint32_t s_profileInterruptedSamples;
static uint32_t s_profileTotalCycles;
static uint32_t s_profileMaxCycles;
#endif

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

#if CONFIG__USB_DEBUG_PROFILE_ENABLE
static void avc_usb_debug_stream__profile_service(uint32_t startCycles, uint8_t open)
{
    uint32_t endCycles = DWT->CYCCNT;
    uint32_t elapsedCycles;
    uint32_t averageCycles;

    s_profileCalls++;
    if (open != 0U)
    {
        s_profileOpenCalls++;
    }

    if (endCycles >= startCycles)
    {
        elapsedCycles = endCycles - startCycles;
        s_profileValidSamples++;
        s_profileTotalCycles += elapsedCycles;
        if (elapsedCycles > s_profileMaxCycles)
        {
            s_profileMaxCycles = elapsedCycles;
        }
    }
    else
    {
        /* The camera callback resets DWT->CYCCNT; discard that crossing sample. */
        s_profileInterruptedSamples++;
    }

    if (e_tick__timeout(&s_profileReportTick, 1000U))
    {
        averageCycles = (s_profileValidSamples != 0U)
                            ? (s_profileTotalCycles / s_profileValidSamples)
                            : 0U;
        (void)DEBUG("avc_usb_profile: calls=%u open=%u avg_cycles=%u max_cycles=%u reset_samples=%u\r\n",
                    s_profileCalls,
                    s_profileOpenCalls,
                    averageCycles,
                    s_profileMaxCycles,
                    s_profileInterruptedSamples);
        s_profileCalls = 0U;
        s_profileOpenCalls = 0U;
        s_profileValidSamples = 0U;
        s_profileInterruptedSamples = 0U;
        s_profileTotalCycles = 0U;
        s_profileMaxCycles = 0U;
    }
}
#endif

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
    if (s_statsEnabled == 0U)
    {
        return;
    }

    interval = avc_usb_debug_stream__stats_interval_frames();
    if ((interval != 0U) && ((s_streamCompletedFrameCount % interval) == 0U))
    {
        s_streamStatsDue = 1U;
    }
}

static uint8_t avc_usb_debug_stream__submit_packet(uint32_t packetBytes)
{
    uint32_t packetSequence = s_streamSequence;
    usb_status_t error;

    /* Claim the one CDC IN transfer before calling the driver. This keeps the
     * main-loop service and the completion callback on the same ownership rule.
     * Reserve the sequence too: a short packet can complete before this call
     * returns, and the callback may immediately submit the next packet. */
    s_streamTxBusy = 1U;
    s_streamSequence = packetSequence + 1U;
    error = USB_DeviceCdcAcmSend(s_cdcVcom.cdcAcmHandle,
                                 USB_CDC_VCOM_BULK_IN_ENDPOINT,
                                 s_streamTxBuf,
                                 packetBytes);
    if (error == kStatus_USB_Success)
    {
        s_streamPacketsSent++;
        s_streamBytesSent += packetBytes;
        return 1U;
    }

    s_streamTxBusy = 0U;
    s_streamSequence = packetSequence;
    if (error == kStatus_USB_Busy)
    {
        s_streamBusyCount++;
    }
    else
    {
        s_streamSendErrorCount++;
    }

    return 0U;
}

static uint8_t avc_usb_debug_stream__send_stats_report(void)
{
    avc_dbg_packet_header_t *packetHeader;
    avc_dbg_stats_report_t *stats;
    uint32_t streamFlags = 0U;
    uint32_t packetBytes = AVC_DBG_PACKET_HEADER_BYTES + AVC_DBG_STATS_REPORT_BYTES;

    if (s_streamStatsDue == 0U)
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
    stats->control_response_drop_count = s_controlResponseDropCount;
    stats->control_response_queue_high_water = s_controlResponseQueueHighWater;
    stats->log_drop_count = s_logDropCount;
    stats->log_queue_high_water = s_logQueueHighWater;
    stats->telemetry_drop_count = s_telemetryDropCount;
    stats->telemetry_queue_high_water = s_telemetryQueueHighWater;
    stats->telemetry_coalesce_count = s_telemetryCoalesceCount;

    if (avc_usb_debug_stream__submit_packet(packetBytes) != 0U)
    {
        s_streamStatsDue = 0U;
        s_streamDroppedBeforePacket = 0U;
        return 1U;
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
    s_streamOffset = 0U;
    s_streamDroppedFrameCount = 0U;
    s_streamStartedFrameCount = 0U;
    s_streamCompletedFrameCount = 0U;
    s_streamPacketsSent = 0U;
    s_streamBytesSent = 0U;
    s_streamBusyCount = 0U;
    s_streamSendErrorCount = 0U;
    s_streamStatsDue = ((s_statsEnabled != 0U) && (source == AVC_USB_DEBUG_STREAM_SOURCE_CAMERA)) ? 1U : 0U;
    s_streamDroppedBeforePacket = 0U;
    s_streamFrameResyncPending = 0U;
    s_streamEnabled = 1U;

    EnableGlobalIRQ(usbOsaCurrentSr);
}

void avc_usb_debug_stream__start(void)
{
    s_streamSequence = 0U;
    s_statsEnabled = 1U;
    avc_usb_debug_stream__start_source(AVC_USB_DEBUG_STREAM_SOURCE_CAMERA);
}

static void avc_usb_debug_stream__start_synthetic(void)
{
    s_streamSequence = 0U;
    s_statsEnabled = 0U;
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
    s_streamFrameResyncPending = 0U;
    s_statsEnabled = 0U;

    EnableGlobalIRQ(usbOsaCurrentSr);
}

bool avc_usb_debug_stream__is_open(void)
{
    return (s_cdcVcom.attach != 0U) && (s_cdcVcom.startTransactions != 0U);
}

bool avc_usb_debug_stream__session_active(void)
{
    return s_sessionActive != 0U;
}

bool avc_usb_debug_stream__camera_frames_active(void)
{
    return (s_sessionActive != 0U) && (s_streamEnabled != 0U) &&
           (s_streamSource == AVC_USB_DEBUG_STREAM_SOURCE_CAMERA) && avc_usb_debug_stream__is_open();
}

bool avc_usb_debug_stream__take_enter_isp_request(void)
{
    uint32_t usbOsaCurrentSr = DisableGlobalIRQ();
    bool requested = s_enterIspRequested != 0U;

    s_enterIspRequested = 0U;
    EnableGlobalIRQ(usbOsaCurrentSr);
    return requested;
}

bool avc_usb_debug_stream__tx_idle(void)
{
    return (s_streamTxBusy == 0U) && (s_controlResponseCount == 0U);
}

static uint32_t avc_usb_debug_stream__bounded_text_length(const char *text, uint32_t maxLength)
{
    uint32_t length = 0U;

    while ((length < maxLength) && (text[length] != '\0'))
    {
        length++;
    }

    return length;
}

bool avc_usb_debug_stream__log_text(uint8_t level, const char *category, const char *text)
{
    avc_usb_log_record_t record;
    avc_usb_log_record_t *queuedRecord;
    uint32_t usbOsaCurrentSr;
    uint8_t queued = 0U;
    uint8_t tail;

    if ((s_logEnabled == 0U) || (s_sessionActive == 0U) || !avc_usb_debug_stream__is_open() ||
        (level > AVC_DBG_LOG_LEVEL_ERROR) || (category == NULL) || (text == NULL))
    {
        return false;
    }

    record.timestampMs = e_tick__get_ms();
    record.recordId = 0U;
    record.level = level;
    record.categoryLength = (uint8_t)avc_usb_debug_stream__bounded_text_length(
        category,
        AVC_USB_LOG_CATEGORY_MAX_BYTES);
    record.textLength = (uint16_t)avc_usb_debug_stream__bounded_text_length(text, AVC_USB_LOG_TEXT_MAX_BYTES);
    (void)memcpy(record.category, category, record.categoryLength);
    (void)memcpy(record.text, text, record.textLength);

    usbOsaCurrentSr = DisableGlobalIRQ();
    if ((s_logEnabled != 0U) && (s_logCount < AVC_USB_LOG_QUEUE_DEPTH))
    {
        tail = (uint8_t)(s_logHead + s_logCount);
        if (tail >= AVC_USB_LOG_QUEUE_DEPTH)
        {
            tail = (uint8_t)(tail - AVC_USB_LOG_QUEUE_DEPTH);
        }

        queuedRecord = &s_logQueue[tail];
        (void)memcpy(queuedRecord, &record, sizeof(record));
        queuedRecord->recordId = s_logNextRecordId;
        s_logNextRecordId++;
        s_logCount++;
        if (s_logCount > s_logQueueHighWater)
        {
            s_logQueueHighWater = s_logCount;
        }
        queued = 1U;
    }
    else if (s_logEnabled != 0U)
    {
        s_logDropCount++;
    }
    EnableGlobalIRQ(usbOsaCurrentSr);

    if (queued != 0U)
    {
        avc_usb_debug_stream__schedule_tx();
    }

    return queued != 0U;
}

bool avc_usb_debug_stream__logf(uint8_t level, const char *category, const char *format, ...)
{
    char text[AVC_USB_LOG_TEXT_MAX_BYTES + 1U];
    va_list arguments;
    int result;

    if ((s_logEnabled == 0U) || (s_sessionActive == 0U) || !avc_usb_debug_stream__is_open() ||
        (format == NULL))
    {
        return false;
    }

    va_start(arguments, format);
    result = vsnprintf(text, sizeof(text), format, arguments);
    va_end(arguments);
    if (result < 0)
    {
        return false;
    }

    return avc_usb_debug_stream__log_text(level, category, text);
}

static bool avc_usb_debug_stream__telemetry_scalar(const char *name,
                                                   uint8_t valueType,
                                                   uint32_t valueBits,
                                                   const char *units)
{
    avc_usb_telemetry_scalar_t record;
    avc_usb_telemetry_scalar_t *queuedRecord = NULL;
    uint32_t usbOsaCurrentSr;
    uint8_t index;
    uint8_t queued = 0U;

    if ((s_telemetryEnabled == 0U) || (s_sessionActive == 0U) || !avc_usb_debug_stream__is_open() ||
        (name == NULL) || (name[0] == '\0') || (units == NULL) ||
        (valueType < AVC_DBG_TELEMETRY_TYPE_I32) || (valueType > AVC_DBG_TELEMETRY_TYPE_BOOL))
    {
        return false;
    }

    record.timestampMs = e_tick__get_ms();
    record.sampleId = 0U;
    record.valueBits = valueBits;
    record.valueType = valueType;
    record.nameLength = (uint16_t)avc_usb_debug_stream__bounded_text_length(
        name,
        AVC_USB_TELEMETRY_NAME_MAX_BYTES);
    record.unitsLength = (uint8_t)avc_usb_debug_stream__bounded_text_length(
        units,
        AVC_USB_TELEMETRY_UNITS_MAX_BYTES);
    (void)memcpy(record.name, name, record.nameLength);
    (void)memcpy(record.units, units, record.unitsLength);

    usbOsaCurrentSr = DisableGlobalIRQ();
    if (s_telemetryEnabled != 0U)
    {
        for (uint8_t offset = 0U; offset < s_telemetryCount; offset++)
        {
            index = (uint8_t)(s_telemetryHead + offset);
            if (index >= AVC_USB_TELEMETRY_QUEUE_DEPTH)
            {
                index = (uint8_t)(index - AVC_USB_TELEMETRY_QUEUE_DEPTH);
            }

            if ((s_telemetryQueue[index].nameLength == record.nameLength) &&
                (memcmp(s_telemetryQueue[index].name, record.name, record.nameLength) == 0))
            {
                queuedRecord = &s_telemetryQueue[index];
                s_telemetryCoalesceCount++;
                break;
            }
        }

        if ((queuedRecord == NULL) && (s_telemetryCount < AVC_USB_TELEMETRY_QUEUE_DEPTH))
        {
            index = (uint8_t)(s_telemetryHead + s_telemetryCount);
            if (index >= AVC_USB_TELEMETRY_QUEUE_DEPTH)
            {
                index = (uint8_t)(index - AVC_USB_TELEMETRY_QUEUE_DEPTH);
            }
            queuedRecord = &s_telemetryQueue[index];
            s_telemetryCount++;
            if (s_telemetryCount > s_telemetryQueueHighWater)
            {
                s_telemetryQueueHighWater = s_telemetryCount;
            }
        }

        if (queuedRecord != NULL)
        {
            (void)memcpy(queuedRecord, &record, sizeof(record));
            queuedRecord->sampleId = s_telemetryNextSampleId;
            s_telemetryNextSampleId++;
            queued = 1U;
        }
        else
        {
            s_telemetryDropCount++;
        }
    }
    EnableGlobalIRQ(usbOsaCurrentSr);

    if (queued != 0U)
    {
        avc_usb_debug_stream__schedule_tx();
    }

    return queued != 0U;
}

bool avc_usb_debug_stream__telemetry_i32(const char *name, int32_t value, const char *units)
{
    return avc_usb_debug_stream__telemetry_scalar(name,
                                                  AVC_DBG_TELEMETRY_TYPE_I32,
                                                  (uint32_t)value,
                                                  units);
}

bool avc_usb_debug_stream__telemetry_u32(const char *name, uint32_t value, const char *units)
{
    return avc_usb_debug_stream__telemetry_scalar(name, AVC_DBG_TELEMETRY_TYPE_U32, value, units);
}

bool avc_usb_debug_stream__telemetry_f32(const char *name, float value, const char *units)
{
    uint32_t valueBits;

    (void)memcpy(&valueBits, &value, sizeof(valueBits));
    return avc_usb_debug_stream__telemetry_scalar(name, AVC_DBG_TELEMETRY_TYPE_F32, valueBits, units);
}

bool avc_usb_debug_stream__telemetry_bool(const char *name, bool value)
{
    return avc_usb_debug_stream__telemetry_scalar(name,
                                                  AVC_DBG_TELEMETRY_TYPE_BOOL,
                                                  value ? 1U : 0U,
                                                  "");
}

void avc_usb_debug_stream__notify_camera_frame(void)
{
    s_cameraFrameGeneration++;
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
        s_streamFrameGeneration = s_cameraFrameGeneration;
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
        avc_usb_debug_stream__schedule_tx();
    }

    return accepted;
}

static uint8_t avc_usb_debug_stream__queue_control_response(uint32_t msgId,
                                                            uint32_t requestSequence,
                                                            uint32_t status,
                                                            const void *payload,
                                                            uint32_t payloadLength)
{
    avc_usb_control_response_t *response;
    uint8_t queued = 0U;
    uint8_t tail;
    uint32_t usbOsaCurrentSr;

    if (payloadLength > AVC_USB_CONTROL_RESPONSE_PAYLOAD_BYTES)
    {
        return 0U;
    }

    usbOsaCurrentSr = DisableGlobalIRQ();
    if (s_controlResponseCount < AVC_USB_CONTROL_RESPONSE_QUEUE_DEPTH)
    {
        tail = (uint8_t)(s_controlResponseHead + s_controlResponseCount);
        if (tail >= AVC_USB_CONTROL_RESPONSE_QUEUE_DEPTH)
        {
            tail = (uint8_t)(tail - AVC_USB_CONTROL_RESPONSE_QUEUE_DEPTH);
        }

        response = &s_controlResponseQueue[tail];
        response->msgId = msgId;
        response->requestSequence = requestSequence;
        response->status = status;
        response->sessionId = s_sessionId;
        response->payloadLength = payloadLength;
        if ((payload != NULL) && (payloadLength != 0U))
        {
            (void)memcpy(response->payload, payload, payloadLength);
        }
        s_controlResponseCount++;
        if (s_controlResponseCount > s_controlResponseQueueHighWater)
        {
            s_controlResponseQueueHighWater = s_controlResponseCount;
        }
        queued = 1U;
    }
    else
    {
        s_controlResponseDropCount++;
    }
    EnableGlobalIRQ(usbOsaCurrentSr);

    return queued;
}

static uint8_t avc_usb_debug_stream__send_control_response(void)
{
    avc_usb_control_response_t response;
    avc_dbg_packet_header_t *packetHeader;
    uint32_t packetBytes;
    uint32_t usbOsaCurrentSr;

    usbOsaCurrentSr = DisableGlobalIRQ();
    if (s_controlResponseCount == 0U)
    {
        EnableGlobalIRQ(usbOsaCurrentSr);
        return 0U;
    }
    (void)memcpy(&response, &s_controlResponseQueue[s_controlResponseHead], sizeof(response));
    EnableGlobalIRQ(usbOsaCurrentSr);

    packetHeader = (avc_dbg_packet_header_t *)s_streamTxBuf;
    packetHeader->magic = AVC_DBG_MAGIC;
    packetHeader->version = AVC_DBG_VERSION;
    packetHeader->header_bytes = AVC_DBG_PACKET_HEADER_BYTES;
    packetHeader->flags = AVC_DBG_PACKET_FLAG_RESPONSE;
    packetHeader->msg_id = response.msgId;
    packetHeader->sequence = s_streamSequence;
    packetHeader->payload_length = response.payloadLength;
    packetHeader->arg0 = response.requestSequence;
    packetHeader->arg1 = response.status;
    packetHeader->arg2 = response.sessionId;

    if (response.payloadLength != 0U)
    {
        (void)memcpy(&s_streamTxBuf[AVC_DBG_PACKET_HEADER_BYTES],
                     response.payload,
                     response.payloadLength);
    }

    packetBytes = AVC_DBG_PACKET_HEADER_BYTES + response.payloadLength;
    if (avc_usb_debug_stream__submit_packet(packetBytes) != 0U)
    {
        usbOsaCurrentSr = DisableGlobalIRQ();
        if (s_controlResponseCount != 0U)
        {
            s_controlResponseHead++;
            if (s_controlResponseHead >= AVC_USB_CONTROL_RESPONSE_QUEUE_DEPTH)
            {
                s_controlResponseHead = 0U;
            }
            s_controlResponseCount--;
        }
        EnableGlobalIRQ(usbOsaCurrentSr);

        if (s_txControlBurst < AVC_USB_TX_CONTROL_BURST_MAX)
        {
            s_txControlBurst++;
        }
        return 1U;
    }

    return 0U;
}

static uint8_t avc_usb_debug_stream__send_log_record(void)
{
    avc_usb_log_record_t record;
    avc_dbg_log_record_t *wireRecord;
    avc_dbg_packet_header_t *packetHeader;
    uint32_t packetBytes;
    uint32_t payloadBytes;
    uint32_t usbOsaCurrentSr;

    usbOsaCurrentSr = DisableGlobalIRQ();
    if (s_logCount == 0U)
    {
        EnableGlobalIRQ(usbOsaCurrentSr);
        return 0U;
    }
    (void)memcpy(&record, &s_logQueue[s_logHead], sizeof(record));
    EnableGlobalIRQ(usbOsaCurrentSr);

    payloadBytes = AVC_DBG_LOG_RECORD_HEADER_BYTES + record.categoryLength + record.textLength;
    packetHeader = (avc_dbg_packet_header_t *)s_streamTxBuf;
    packetHeader->magic = AVC_DBG_MAGIC;
    packetHeader->version = AVC_DBG_VERSION;
    packetHeader->header_bytes = AVC_DBG_PACKET_HEADER_BYTES;
    packetHeader->flags = 0U;
    packetHeader->msg_id = AVC_DBG_LOG_TEXT;
    packetHeader->sequence = s_streamSequence;
    packetHeader->payload_length = payloadBytes;
    packetHeader->arg0 = record.level;
    packetHeader->arg1 = record.recordId;
    packetHeader->arg2 = record.timestampMs;

    wireRecord = (avc_dbg_log_record_t *)&s_streamTxBuf[AVC_DBG_PACKET_HEADER_BYTES];
    wireRecord->timestamp_ms = record.timestampMs;
    wireRecord->record_id = record.recordId;
    wireRecord->text_bytes = record.textLength;
    wireRecord->level = record.level;
    wireRecord->category_bytes = record.categoryLength;
    (void)memcpy(&s_streamTxBuf[AVC_DBG_PACKET_HEADER_BYTES + AVC_DBG_LOG_RECORD_HEADER_BYTES],
                 record.category,
                 record.categoryLength);
    (void)memcpy(&s_streamTxBuf[AVC_DBG_PACKET_HEADER_BYTES + AVC_DBG_LOG_RECORD_HEADER_BYTES +
                               record.categoryLength],
                 record.text,
                 record.textLength);

    packetBytes = AVC_DBG_PACKET_HEADER_BYTES + payloadBytes;
    if (avc_usb_debug_stream__submit_packet(packetBytes) == 0U)
    {
        return 0U;
    }

    usbOsaCurrentSr = DisableGlobalIRQ();
    if (s_logCount != 0U)
    {
        s_logHead++;
        if (s_logHead >= AVC_USB_LOG_QUEUE_DEPTH)
        {
            s_logHead = 0U;
        }
        s_logCount--;
    }
    EnableGlobalIRQ(usbOsaCurrentSr);

    if (s_txDiagnosticBurst < AVC_USB_TX_DIAGNOSTIC_BURST_MAX)
    {
        s_txDiagnosticBurst++;
    }
    return 1U;
}

static uint8_t avc_usb_debug_stream__send_telemetry_scalar(void)
{
    avc_usb_telemetry_scalar_t record;
    avc_dbg_telemetry_scalar_t *wireRecord;
    avc_dbg_packet_header_t *packetHeader;
    uint32_t packetBytes;
    uint32_t payloadBytes;
    uint32_t usbOsaCurrentSr;

    usbOsaCurrentSr = DisableGlobalIRQ();
    if (s_telemetryCount == 0U)
    {
        EnableGlobalIRQ(usbOsaCurrentSr);
        return 0U;
    }
    (void)memcpy(&record, &s_telemetryQueue[s_telemetryHead], sizeof(record));
    EnableGlobalIRQ(usbOsaCurrentSr);

    payloadBytes = AVC_DBG_TELEMETRY_SCALAR_HEADER_BYTES + record.nameLength + record.unitsLength;
    packetHeader = (avc_dbg_packet_header_t *)s_streamTxBuf;
    packetHeader->magic = AVC_DBG_MAGIC;
    packetHeader->version = AVC_DBG_VERSION;
    packetHeader->header_bytes = AVC_DBG_PACKET_HEADER_BYTES;
    packetHeader->flags = 0U;
    packetHeader->msg_id = AVC_DBG_TELEMETRY_SCALAR;
    packetHeader->sequence = s_streamSequence;
    packetHeader->payload_length = payloadBytes;
    packetHeader->arg0 = record.valueType;
    packetHeader->arg1 = record.sampleId;
    packetHeader->arg2 = record.timestampMs;

    wireRecord = (avc_dbg_telemetry_scalar_t *)&s_streamTxBuf[AVC_DBG_PACKET_HEADER_BYTES];
    wireRecord->timestamp_ms = record.timestampMs;
    wireRecord->sample_id = record.sampleId;
    wireRecord->value_bits = record.valueBits;
    wireRecord->name_bytes = record.nameLength;
    wireRecord->value_type = record.valueType;
    wireRecord->units_bytes = record.unitsLength;
    (void)memcpy(&s_streamTxBuf[AVC_DBG_PACKET_HEADER_BYTES + AVC_DBG_TELEMETRY_SCALAR_HEADER_BYTES],
                 record.name,
                 record.nameLength);
    (void)memcpy(&s_streamTxBuf[AVC_DBG_PACKET_HEADER_BYTES + AVC_DBG_TELEMETRY_SCALAR_HEADER_BYTES +
                               record.nameLength],
                 record.units,
                 record.unitsLength);

    packetBytes = AVC_DBG_PACKET_HEADER_BYTES + payloadBytes;
    if (avc_usb_debug_stream__submit_packet(packetBytes) == 0U)
    {
        return 0U;
    }

    usbOsaCurrentSr = DisableGlobalIRQ();
    if (s_telemetryCount != 0U)
    {
        s_telemetryHead++;
        if (s_telemetryHead >= AVC_USB_TELEMETRY_QUEUE_DEPTH)
        {
            s_telemetryHead = 0U;
        }
        s_telemetryCount--;
    }
    EnableGlobalIRQ(usbOsaCurrentSr);

    if (s_txDiagnosticBurst < AVC_USB_TX_DIAGNOSTIC_BURST_MAX)
    {
        s_txDiagnosticBurst++;
    }
    return 1U;
}

static void avc_usb_debug_stream__handle_control_packet(const avc_dbg_packet_header_t *request)
{
    uint8_t logWasEnabled;
    uint8_t telemetryWasEnabled;
    uint32_t requestedChannels;
    uint32_t requestedSource;

    if ((request->flags & AVC_DBG_PACKET_FLAG_RESPONSE) != 0U)
    {
        return;
    }

    switch (request->msg_id)
    {
        case AVC_DBG_CONTROL_HELLO:
        {
            avc_dbg_control_hello_response_t hello;

            avc_usb_debug_stream__stop();
            s_logEnabled = 0U;
            s_logHead = 0U;
            s_logCount = 0U;
            s_telemetryEnabled = 0U;
            s_telemetryHead = 0U;
            s_telemetryCount = 0U;
            s_txDiagnosticBurst = 0U;
            s_streamSequence = 0U;
            s_sessionId++;
            if (s_sessionId == 0U)
            {
                s_sessionId = 1U;
            }
            s_sessionActive = 1U;

            hello.capability_flags = AVC_USB_CONTROL_CAPABILITIES;
            hello.active_channel_flags = 0U;
            hello.max_packet_bytes = AVC_USB_STREAM_TX_BYTES;
            hello.frame_width = (uint16_t)AVC_USB_STREAM_FRAME_WIDTH;
            hello.frame_height = (uint16_t)AVC_USB_STREAM_FRAME_HEIGHT;
            hello.pixel_format = AVC_DBG_PIXEL_FORMAT_RGB565_LE;
            hello.reserved = 0U;
            hello.session_id = s_sessionId;

            (void)avc_usb_debug_stream__queue_control_response(AVC_DBG_CONTROL_HELLO,
                                                               request->sequence,
                                                               AVC_DBG_CONTROL_STATUS_OK,
                                                               &hello,
                                                               sizeof(hello));
            break;
        }

        case AVC_DBG_CONTROL_SET_CHANNELS:
            if (s_sessionActive == 0U)
            {
                (void)avc_usb_debug_stream__queue_control_response(AVC_DBG_CONTROL_SET_CHANNELS,
                                                                   request->sequence,
                                                                   AVC_DBG_CONTROL_STATUS_SESSION_REQUIRED,
                                                                   NULL,
                                                                   0U);
                break;
            }

            requestedChannels = request->arg0;
            requestedSource = request->arg1;
            if (((requestedChannels & ~AVC_USB_CONTROL_SUPPORTED_CHANNELS) != 0U) ||
                (((requestedChannels & AVC_DBG_CHANNEL_STATS) != 0U) &&
                 ((requestedChannels & AVC_DBG_CHANNEL_FRAMES) == 0U)) ||
                (((requestedChannels & AVC_DBG_CHANNEL_STATS) != 0U) &&
                 (requestedSource == AVC_DBG_STREAM_SOURCE_SYNTHETIC)) ||
                (requestedSource > AVC_DBG_STREAM_SOURCE_SYNTHETIC))
            {
                (void)avc_usb_debug_stream__queue_control_response(AVC_DBG_CONTROL_SET_CHANNELS,
                                                                   request->sequence,
                                                                   AVC_DBG_CONTROL_STATUS_BAD_ARGUMENT,
                                                                   NULL,
                                                                   0U);
                break;
            }

            logWasEnabled = s_logEnabled;
            telemetryWasEnabled = s_telemetryEnabled;
            s_logEnabled = ((requestedChannels & AVC_DBG_CHANNEL_LOGS) != 0U) ? 1U : 0U;
            s_telemetryEnabled = ((requestedChannels & AVC_DBG_CHANNEL_TELEMETRY) != 0U) ? 1U : 0U;
            if (s_logEnabled == 0U)
            {
                s_logHead = 0U;
                s_logCount = 0U;
                s_txDiagnosticBurst = 0U;
            }
            if (s_telemetryEnabled == 0U)
            {
                s_telemetryHead = 0U;
                s_telemetryCount = 0U;
                s_txDiagnosticBurst = 0U;
            }

            if ((requestedChannels & AVC_DBG_CHANNEL_FRAMES) == 0U)
            {
                avc_usb_debug_stream__stop();
            }
            else
            {
                s_statsEnabled = ((requestedChannels & AVC_DBG_CHANNEL_STATS) != 0U) ? 1U : 0U;
                avc_usb_debug_stream__start_source((avc_usb_debug_stream_source_t)requestedSource);
            }

            (void)avc_usb_debug_stream__queue_control_response(AVC_DBG_CONTROL_SET_CHANNELS,
                                                               request->sequence,
                                                               AVC_DBG_CONTROL_STATUS_OK,
                                                               NULL,
                                                               0U);
            if ((s_logEnabled != 0U) && (logWasEnabled == 0U))
            {
                (void)avc_usb_debug_stream__log_text(AVC_DBG_LOG_LEVEL_INFO,
                                                     "system",
                                                     "USB diagnostic log channel active");
            }
            if ((s_telemetryEnabled != 0U) && (telemetryWasEnabled == 0U))
            {
                (void)avc_usb_debug_stream__telemetry_u32("system.uptime", e_tick__get_ms(), "ms");
            }
            break;

        case AVC_DBG_CONTROL_PING:
            (void)avc_usb_debug_stream__queue_control_response(AVC_DBG_CONTROL_PING,
                                                               request->sequence,
                                                               (s_sessionActive != 0U)
                                                                   ? AVC_DBG_CONTROL_STATUS_OK
                                                                   : AVC_DBG_CONTROL_STATUS_SESSION_REQUIRED,
                                                               NULL,
                                                               0U);
            break;

        case AVC_DBG_CONTROL_CLOSE:
            avc_usb_debug_stream__stop();
            (void)avc_usb_debug_stream__queue_control_response(AVC_DBG_CONTROL_CLOSE,
                                                               request->sequence,
                                                               AVC_DBG_CONTROL_STATUS_OK,
                                                               NULL,
                                                               0U);
            s_logEnabled = 0U;
            s_logHead = 0U;
            s_logCount = 0U;
            s_telemetryEnabled = 0U;
            s_telemetryHead = 0U;
            s_telemetryCount = 0U;
            s_txDiagnosticBurst = 0U;
            s_sessionActive = 0U;
            break;

        case AVC_DBG_CONTROL_ENTER_ISP:
            if (s_sessionActive == 0U)
            {
                (void)avc_usb_debug_stream__queue_control_response(
                    AVC_DBG_CONTROL_ENTER_ISP,
                    request->sequence,
                    AVC_DBG_CONTROL_STATUS_SESSION_REQUIRED,
                    NULL,
                    0U);
                break;
            }
            if ((request->arg0 != AVC_DBG_ENTER_ISP_CONFIRMATION) ||
                (request->arg1 != 0U) || (request->arg2 != 0U))
            {
                (void)avc_usb_debug_stream__queue_control_response(
                    AVC_DBG_CONTROL_ENTER_ISP,
                    request->sequence,
                    AVC_DBG_CONTROL_STATUS_BAD_ARGUMENT,
                    NULL,
                    0U);
                break;
            }
            if (s_enterIspRequested != 0U)
            {
                (void)avc_usb_debug_stream__queue_control_response(
                    AVC_DBG_CONTROL_ENTER_ISP,
                    request->sequence,
                    AVC_DBG_CONTROL_STATUS_BUSY,
                    NULL,
                    0U);
                break;
            }
            if (avc_usb_debug_stream__queue_control_response(AVC_DBG_CONTROL_ENTER_ISP,
                                                              request->sequence,
                                                              AVC_DBG_CONTROL_STATUS_OK,
                                                              NULL,
                                                              0U) != 0U)
            {
                avc_usb_debug_stream__stop();
                s_logEnabled = 0U;
                s_logHead = 0U;
                s_logCount = 0U;
                s_telemetryEnabled = 0U;
                s_telemetryHead = 0U;
                s_telemetryCount = 0U;
                s_txDiagnosticBurst = 0U;
                s_enterIspRequested = 1U;
            }
            break;

        default:
            (void)avc_usb_debug_stream__queue_control_response(AVC_DBG_CONTROL_ERROR,
                                                               request->sequence,
                                                               AVC_DBG_CONTROL_STATUS_UNSUPPORTED,
                                                               NULL,
                                                               0U);
            break;
    }
}

static void avc_usb_debug_stream__consume_control_rx(uint32_t bytes)
{
    if (bytes >= s_controlRxLength)
    {
        s_controlRxLength = 0U;
        return;
    }

    (void)memmove(s_controlRxBuffer, &s_controlRxBuffer[bytes], s_controlRxLength - bytes);
    s_controlRxLength -= bytes;
}

static void avc_usb_debug_stream__handle_received_data(const uint8_t *data, uint32_t length)
{
    avc_dbg_packet_header_t request;
    uint32_t packetBytes;

    if ((data == NULL) || (length == 0U))
    {
        return;
    }

    if (length > (AVC_USB_CONTROL_RX_BYTES - s_controlRxLength))
    {
        s_controlRxLength = 0U;
        return;
    }

    (void)memcpy(&s_controlRxBuffer[s_controlRxLength], data, length);
    s_controlRxLength += length;

    while (s_controlRxLength >= AVC_DBG_PACKET_HEADER_BYTES)
    {
        (void)memcpy(&request, s_controlRxBuffer, sizeof(request));
        if (request.magic != AVC_DBG_MAGIC)
        {
            avc_usb_debug_stream__consume_control_rx(1U);
            continue;
        }

        if ((request.version != AVC_DBG_VERSION) ||
            (request.header_bytes != AVC_DBG_PACKET_HEADER_BYTES) ||
            (request.payload_length > AVC_USB_CONTROL_MAX_PAYLOAD_BYTES))
        {
            avc_usb_debug_stream__consume_control_rx(1U);
            continue;
        }

        packetBytes = AVC_DBG_PACKET_HEADER_BYTES + request.payload_length;
        if (s_controlRxLength < packetBytes)
        {
            return;
        }

        s_streamRxCommandCount++;
        avc_usb_debug_stream__handle_control_packet(&request);
        avc_usb_debug_stream__consume_control_rx(packetBytes);
    }
}

static uint8_t avc_usb_debug_stream__send_next_diagnostic(void)
{
    if ((s_txPreferTelemetry != 0U) && (s_telemetryCount != 0U) &&
        (avc_usb_debug_stream__send_telemetry_scalar() != 0U))
    {
        s_txPreferTelemetry = 0U;
        return 1U;
    }

    if ((s_logCount != 0U) && (avc_usb_debug_stream__send_log_record() != 0U))
    {
        s_txPreferTelemetry = 1U;
        return 1U;
    }

    if ((s_telemetryCount != 0U) && (avc_usb_debug_stream__send_telemetry_scalar() != 0U))
    {
        s_txPreferTelemetry = 0U;
        return 1U;
    }

    return 0U;
}

static void avc_usb_debug_stream__schedule_tx(void)
{
    uint32_t payloadBytes;
    uint32_t packetPayloadBytes;
    uint32_t remaining;
    uint32_t chunkFlags = 0U;
    const uint8_t *sourceFrameData = NULL;
    avc_dbg_packet_header_t *packetHeader;
    avc_dbg_rui_write_frame_buffer_raw_t *frameChunk;
    uint8_t framePending;
    uint8_t nonControlPending;

    if (!avc_usb_debug_stream__is_open())
    {
        return;
    }

    if (s_streamTxBusy != 0U)
    {
        return;
    }

    framePending = ((s_streamEnabled != 0U) &&
                    ((s_streamSource == AVC_USB_DEBUG_STREAM_SOURCE_SYNTHETIC) ||
                     (s_streamFrameActive != 0U)))
                       ? 1U
                       : 0U;
    nonControlPending = ((s_streamStatsDue != 0U) || (s_logCount != 0U) || (s_telemetryCount != 0U) ||
                         (framePending != 0U))
                            ? 1U
                            : 0U;

    /* Control replies preempt bulk data between packets, but a command flood
     * cannot indefinitely starve an already-pending diagnostic packet. */
    if ((s_controlResponseCount != 0U) &&
        ((s_txControlBurst < AVC_USB_TX_CONTROL_BURST_MAX) || (nonControlPending == 0U)) &&
        (avc_usb_debug_stream__send_control_response() != 0U))
    {
        return;
    }

    if (avc_usb_debug_stream__send_stats_report() != 0U)
    {
        s_txControlBurst = 0U;
        if (s_txDiagnosticBurst < AVC_USB_TX_DIAGNOSTIC_BURST_MAX)
        {
            s_txDiagnosticBurst++;
        }
        return;
    }

    if (((s_logCount != 0U) || (s_telemetryCount != 0U)) &&
        ((s_txDiagnosticBurst < AVC_USB_TX_DIAGNOSTIC_BURST_MAX) || (framePending == 0U)) &&
        (avc_usb_debug_stream__send_next_diagnostic() != 0U))
    {
        s_txControlBurst = 0U;
        return;
    }

    if (s_streamEnabled == 0U)
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
        if ((s_cameraFrameGeneration - s_streamFrameGeneration) >= CONFIG__CAMERA_FRAME_BUFFER_COUNT)
        {
            s_streamFrameData = NULL;
            s_streamFrameActive = 0U;
            s_streamOffset = 0U;
            s_streamDroppedFrameCount++;
            s_streamDroppedBeforePacket = 1U;
            s_streamFrameResyncPending = 1U;
            return;
        }
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
        if ((s_cameraFrameGeneration - s_streamFrameGeneration) >= CONFIG__CAMERA_FRAME_BUFFER_COUNT)
        {
            s_streamFrameData = NULL;
            s_streamFrameActive = 0U;
            s_streamOffset = 0U;
            s_streamDroppedFrameCount++;
            s_streamDroppedBeforePacket = 1U;
            s_streamFrameResyncPending = 1U;
            return;
        }
    }

    packetPayloadBytes = AVC_DBG_RUI_WRITE_FRAME_BUFFER_RAW_HEADER_BYTES + payloadBytes;

    packetHeader = (avc_dbg_packet_header_t *)s_streamTxBuf;
    packetHeader->magic = AVC_DBG_MAGIC;
    packetHeader->version = AVC_DBG_VERSION;
    packetHeader->header_bytes = AVC_DBG_PACKET_HEADER_BYTES;
    packetHeader->flags = ((s_streamDroppedBeforePacket != 0U) || (s_streamFrameResyncPending != 0U))
                              ? AVC_DBG_PACKET_FLAG_DROPPED_BEFORE
                              : 0U;
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

    if (avc_usb_debug_stream__submit_packet(AVC_DBG_PACKET_HEADER_BYTES + packetPayloadBytes) != 0U)
    {
        s_txControlBurst = 0U;
        s_txDiagnosticBurst = 0U;
        s_streamDroppedBeforePacket = 0U;
        if ((chunkFlags & AVC_DBG_RUI_CHUNK_FRAME_START) != 0U)
        {
            s_streamFrameResyncPending = 0U;
        }
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
    s_sessionActive = 0U;
    s_statsEnabled = 0U;
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
    s_streamFrameResyncPending = 0U;
    s_cameraFrameGeneration = 0U;
    s_streamFrameGeneration = 0U;
    s_sessionId = 0U;
    s_controlRxLength = 0U;
    (void)memset(s_controlResponseQueue, 0, sizeof(s_controlResponseQueue));
    s_controlResponseHead = 0U;
    s_controlResponseCount = 0U;
    s_txControlBurst = 0U;
    s_controlResponseDropCount = 0U;
    s_controlResponseQueueHighWater = 0U;
    (void)memset(s_logQueue, 0, sizeof(s_logQueue));
    s_logHead = 0U;
    s_logCount = 0U;
    s_logEnabled = 0U;
    s_txDiagnosticBurst = 0U;
    s_logNextRecordId = 0U;
    s_logDropCount = 0U;
    s_logQueueHighWater = 0U;
    (void)memset(s_telemetryQueue, 0, sizeof(s_telemetryQueue));
    s_telemetryHead = 0U;
    s_telemetryCount = 0U;
    s_telemetryEnabled = 0U;
    s_txPreferTelemetry = 0U;
    s_telemetryNextSampleId = 0U;
    s_telemetryDropCount = 0U;
    s_telemetryQueueHighWater = 0U;
    s_telemetryCoalesceCount = 0U;
    s_enterIspRequested = 0U;
#if CONFIG__USB_DEBUG_PROFILE_ENABLE
    s_profileReportTick = e_tick__get_ms();
    s_profileCalls = 0U;
    s_profileOpenCalls = 0U;
    s_profileValidSamples = 0U;
    s_profileInterruptedSamples = 0U;
    s_profileTotalCycles = 0U;
    s_profileMaxCycles = 0U;
#endif
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
#if CONFIG__USB_DEBUG_PROFILE_ENABLE
    uint32_t profileStartCycles = DWT->CYCCNT;
#endif

    if (!avc_usb_debug_stream__is_open())
    {
#if CONFIG__USB_DEBUG_PROFILE_ENABLE
        avc_usb_debug_stream__profile_service(profileStartCycles, 0U);
#endif
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

        avc_usb_debug_stream__handle_received_data(s_currRecvBuf, recvSize);

        error = USB_DeviceCdcAcmRecv(s_cdcVcom.cdcAcmHandle,
                                     USB_CDC_VCOM_BULK_OUT_ENDPOINT,
                                     s_currRecvBuf,
                                     g_UsbDeviceCdcVcomDicEndpoints[1].maxPacketSize);
        if (error != kStatus_USB_Success)
        {
            (void)error;
        }
    }

    avc_usb_debug_stream__schedule_tx();
#if CONFIG__USB_DEBUG_PROFILE_ENABLE
    avc_usb_debug_stream__profile_service(profileStartCycles, 1U);
#endif
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
                avc_usb_debug_stream__schedule_tx();
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
            else
            {
                /* A generic host may write with DTR low. Discard that data and
                 * keep bulk OUT armed so the next recognized session can send
                 * HELLO without requiring a USB bus reset. */
                s_recvSize = 0U;
                error = USB_DeviceCdcAcmRecv(handle,
                                             USB_CDC_VCOM_BULK_OUT_ENDPOINT,
                                             s_currRecvBuf,
                                             g_UsbDeviceCdcVcomDicEndpoints[1].maxPacketSize);
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
                    s_recvSize = 0U;
                    avc_usb_debug_stream__stop();
                    s_sessionActive = 0U;
                    s_controlRxLength = 0U;
                    s_controlResponseHead = 0U;
                    s_controlResponseCount = 0U;
                    s_txControlBurst = 0U;
                    s_controlResponseDropCount = 0U;
                    s_controlResponseQueueHighWater = 0U;
                    s_logHead = 0U;
                    s_logCount = 0U;
                    s_logEnabled = 0U;
                    s_txDiagnosticBurst = 0U;
                    s_logNextRecordId = 0U;
                    s_logDropCount = 0U;
                    s_logQueueHighWater = 0U;
                    s_telemetryHead = 0U;
                    s_telemetryCount = 0U;
                    s_telemetryEnabled = 0U;
                    s_txPreferTelemetry = 0U;
                    s_telemetryNextSampleId = 0U;
                    s_telemetryDropCount = 0U;
                    s_telemetryQueueHighWater = 0U;
                    s_telemetryCoalesceCount = 0U;
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
            s_sessionActive = 0U;
            s_controlRxLength = 0U;
            s_controlResponseHead = 0U;
            s_controlResponseCount = 0U;
            s_txControlBurst = 0U;
            s_controlResponseDropCount = 0U;
            s_controlResponseQueueHighWater = 0U;
            s_logHead = 0U;
            s_logCount = 0U;
            s_logEnabled = 0U;
            s_txDiagnosticBurst = 0U;
            s_logNextRecordId = 0U;
            s_logDropCount = 0U;
            s_logQueueHighWater = 0U;
            s_telemetryHead = 0U;
            s_telemetryCount = 0U;
            s_telemetryEnabled = 0U;
            s_txPreferTelemetry = 0U;
            s_telemetryNextSampleId = 0U;
            s_telemetryDropCount = 0U;
            s_telemetryQueueHighWater = 0U;
            s_telemetryCoalesceCount = 0U;
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
                s_sessionActive = 0U;
                s_controlRxLength = 0U;
                s_controlResponseHead = 0U;
                s_controlResponseCount = 0U;
                s_txControlBurst = 0U;
                s_controlResponseDropCount = 0U;
                s_controlResponseQueueHighWater = 0U;
                s_logHead = 0U;
                s_logCount = 0U;
                s_logEnabled = 0U;
                s_txDiagnosticBurst = 0U;
                s_logNextRecordId = 0U;
                s_logDropCount = 0U;
                s_logQueueHighWater = 0U;
                s_telemetryHead = 0U;
                s_telemetryCount = 0U;
                s_telemetryEnabled = 0U;
                s_txPreferTelemetry = 0U;
                s_telemetryNextSampleId = 0U;
                s_telemetryDropCount = 0U;
                s_telemetryQueueHighWater = 0U;
                s_telemetryCoalesceCount = 0U;
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
