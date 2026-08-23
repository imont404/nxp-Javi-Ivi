#include "avc_system.h"

#include "avc__io.h"

#if CONFIG__USB_DEBUG_STREAM_ENABLE
#include "avc_usb_debug_stream.h"
#include "fsl_runbootloader.h"
#endif

#define AVC_SYSTEM_ISP_ACK_SETTLE_MS (100U)
#define AVC_SYSTEM_ISP_MAX_WAIT_MS (500U)
#define AVC_SYSTEM_ROM_ENTER_BOOT_TAG (0xEBU)
#define AVC_SYSTEM_ROM_ISP_MODE (1U)
#define AVC_SYSTEM_ROM_USB_HS_HID_INTERFACE (5U)

typedef struct
{
    volatile bool camera_frame_seen;
    bool camera_frame_reported;
    bool platform_ready;
    avc_system_mode_t mode;
    avc_system_fault_t fault;
    uint32_t isp_entry_tick;
} avc_system_state_t;

static avc_system_state_t g_avc_system;

static bool avc_system__test_requested(void)
{
#if CONFIG__FORCE_TEST_MODE
    return true;
#elif CONFIG__FORCE_RACE_WAITING_MODE
    return false;
#else
    return GPIO_PinRead(GPIO3, TEST_SW_PIN) == 0U;
#endif
}

static void avc_system__set_mode(avc_system_mode_t mode)
{
    if (g_avc_system.mode == mode)
    {
        return;
    }

    /* Every post-init mode transition starts with disconnected, zero-duty motors. */
    if (g_avc_system.platform_ready)
    {
        avc__disable_motor_control();
    }
    g_avc_system.mode = mode;

    if (g_avc_system.platform_ready)
    {
        DEBUG("System mode: %s\r\n", avc_system__mode_label(mode));
    }
}

void avc_system__init(void)
{
    g_avc_system.camera_frame_seen = false;
    g_avc_system.camera_frame_reported = false;
    g_avc_system.platform_ready = false;
    g_avc_system.mode = AVC_SYSTEM_MODE_STARTUP;
    g_avc_system.fault = AVC_SYSTEM_FAULT_NONE;
    g_avc_system.isp_entry_tick = 0U;

    /* Preserve the verified one-time camera/LCD/platform initialization order. */
    avc__init();
    g_avc_system.platform_ready = true;

    if (g_avc_system.fault != AVC_SYSTEM_FAULT_NONE)
    {
        avc__disable_motor_control();
        DEBUG("System mode: %s\r\n", avc_system__mode_label(g_avc_system.mode));
        return;
    }

    if (avc_system__test_requested())
    {
        avc_system__set_mode(AVC_SYSTEM_MODE_TEST);
    }
    else
    {
        avc_system__set_mode(AVC_SYSTEM_MODE_RACE_WAITING);
    }
}

void avc_system__service(void)
{
    bool test_requested = avc_system__test_requested();

    if (g_avc_system.camera_frame_seen && !g_avc_system.camera_frame_reported)
    {
        g_avc_system.camera_frame_reported = true;
        DEBUG("Camera first frame received.\r\n");
    }

    if (g_avc_system.mode == AVC_SYSTEM_MODE_SAFE_FAULT)
    {
        avc__disable_motor_control();
        return;
    }

#if CONFIG__USB_DEBUG_STREAM_ENABLE
    if (avc_usb_debug_stream__take_enter_isp_request())
    {
        avc_system__set_mode(AVC_SYSTEM_MODE_ENTERING_ISP);
        avc__disable_motor_control();
        avc__set_servo(0.0f);
        g_avc_system.isp_entry_tick = e_tick__get_ms();
    }

    if (g_avc_system.mode == AVC_SYSTEM_MODE_ENTERING_ISP)
    {
        uint32_t elapsed = e_tick__delta(&g_avc_system.isp_entry_tick);

        avc__disable_motor_control();
        avc__set_servo(0.0f);
        if ((elapsed >= AVC_SYSTEM_ISP_ACK_SETTLE_MS) &&
            (avc_usb_debug_stream__tx_idle() || (elapsed >= AVC_SYSTEM_ISP_MAX_WAIT_MS)))
        {
            user_app_boot_invoke_option_t argument = {.option = {.B = {
                .tag = AVC_SYSTEM_ROM_ENTER_BOOT_TAG,
                .mode = AVC_SYSTEM_ROM_ISP_MODE,
                .boot_interface = AVC_SYSTEM_ROM_USB_HS_HID_INTERFACE,
            }}};

            bootloader_user_entry(&argument);
            avc_system__enter_fault(AVC_SYSTEM_FAULT_BOOTLOADER_RETURNED);
        }
        return;
    }
#endif

    if (test_requested)
    {
        if (g_avc_system.mode != AVC_SYSTEM_MODE_TEST)
        {
            avc_system__set_mode(AVC_SYSTEM_MODE_TEST);
        }
        return;
    }

    if (g_avc_system.mode == AVC_SYSTEM_MODE_TEST)
    {
        avc_system__set_mode(AVC_SYSTEM_MODE_RACE_WAITING);
    }

#if CONFIG__FORCE_RACE_WAITING_MODE
    return;
#endif

    if ((g_avc_system.mode == AVC_SYSTEM_MODE_RACE_WAITING) &&
        (button__up(&center_btn) != 0U))
    {
        if (g_avc_system.camera_frame_seen)
        {
            avc_system__set_mode(AVC_SYSTEM_MODE_STUDENT_RUNNING);
        }
        else
        {
            DEBUG("Student start rejected: no camera frame received.\r\n");
        }
    }
}

void avc_system__notify_camera_frame(void)
{
    g_avc_system.camera_frame_seen = true;
}

void avc_system__enter_fault(avc_system_fault_t fault)
{
    g_avc_system.fault = fault;
    avc_system__set_mode(AVC_SYSTEM_MODE_SAFE_FAULT);
}

avc_system_mode_t avc_system__mode(void)
{
    return g_avc_system.mode;
}

avc_system_fault_t avc_system__fault(void)
{
    return g_avc_system.fault;
}

bool avc_system__camera_frame_seen(void)
{
    return g_avc_system.camera_frame_seen;
}

const char *avc_system__mode_label(avc_system_mode_t mode)
{
    switch (mode)
    {
        case AVC_SYSTEM_MODE_STARTUP:
            return "STARTUP";

        case AVC_SYSTEM_MODE_TEST:
            return "TEST";

        case AVC_SYSTEM_MODE_RACE_WAITING:
            return "RACE / WAITING";

        case AVC_SYSTEM_MODE_STUDENT_RUNNING:
            return "STUDENT / RUNNING";

        case AVC_SYSTEM_MODE_ENTERING_ISP:
            return "ENTERING USB ISP";

        case AVC_SYSTEM_MODE_SAFE_FAULT:
            return "SAFE / FAULT";

        default:
            return "UNKNOWN";
    }
}
