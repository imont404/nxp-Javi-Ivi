#include "nxpc_system.h"

#include "nxpc__io.h"

#if CONFIG__USB_DEBUG_STREAM_ENABLE
#include "nxpc_usb_debug_stream.h"
#include "fsl_runbootloader.h"
#endif

#define NXPC_SYSTEM_ISP_ACK_SETTLE_MS (100U)
#define NXPC_SYSTEM_ISP_MAX_WAIT_MS (500U)
#define NXPC_SYSTEM_ROM_ENTER_BOOT_TAG (0xEBU)
#define NXPC_SYSTEM_ROM_ISP_MODE (1U)
#define NXPC_SYSTEM_ROM_USB_HS_HID_INTERFACE (5U)

typedef struct
{
    volatile bool camera_frame_seen;
    bool camera_frame_reported;
    bool platform_ready;
    bool test_outputs_armed;
    bool test_arm_pending;
    nxpc_system_mode_t mode;
    nxpc_system_fault_t fault;
    uint32_t isp_entry_tick;
} nxpc_system_state_t;

static nxpc_system_state_t g_nxpc_system;

static bool nxpc_system__test_requested(void)
{
#if CONFIG__FORCE_TEST_MODE
    return true;
#elif CONFIG__FORCE_RACE_WAITING_MODE
    return false;
#else
    return GPIO_PinRead(GPIO3, TEST_SW_PIN) == 0U;
#endif
}

static void nxpc_system__set_mode(nxpc_system_mode_t mode)
{
    if (g_nxpc_system.mode == mode)
    {
        return;
    }

    /* Every post-init mode transition starts with safe actuator outputs. */
    if (g_nxpc_system.platform_ready)
    {
        nxpc__disable_motor_control();
        nxpc__set_servo(0.0f);
    }
    g_nxpc_system.test_outputs_armed = false;
    g_nxpc_system.test_arm_pending = false;
    g_nxpc_system.mode = mode;

    if (g_nxpc_system.platform_ready)
    {
        DEBUG("System mode: %s\r\n", nxpc_system__mode_label(mode));
    }
}

void nxpc_system__init(void)
{
    g_nxpc_system.camera_frame_seen = false;
    g_nxpc_system.camera_frame_reported = false;
    g_nxpc_system.platform_ready = false;
    g_nxpc_system.test_outputs_armed = false;
    g_nxpc_system.test_arm_pending = false;
    g_nxpc_system.mode = NXPC_SYSTEM_MODE_STARTUP;
    g_nxpc_system.fault = NXPC_SYSTEM_FAULT_NONE;
    g_nxpc_system.isp_entry_tick = 0U;

    /* Preserve the verified one-time camera/LCD/platform initialization order. */
    nxpc__init();
    g_nxpc_system.platform_ready = true;

    if (g_nxpc_system.fault != NXPC_SYSTEM_FAULT_NONE)
    {
        nxpc__disable_motor_control();
        DEBUG("System mode: %s\r\n", nxpc_system__mode_label(g_nxpc_system.mode));
        return;
    }

    if (nxpc_system__test_requested())
    {
        nxpc_system__set_mode(NXPC_SYSTEM_MODE_TEST);
    }
    else
    {
        nxpc_system__set_mode(NXPC_SYSTEM_MODE_RACE_WAITING);
    }
}

void nxpc_system__service(void)
{
    bool test_requested = nxpc_system__test_requested();

    if (g_nxpc_system.camera_frame_seen && !g_nxpc_system.camera_frame_reported)
    {
        g_nxpc_system.camera_frame_reported = true;
        DEBUG("Camera first frame received.\r\n");
    }

    if (g_nxpc_system.mode == NXPC_SYSTEM_MODE_SAFE_FAULT)
    {
        nxpc__disable_motor_control();
        return;
    }

#if CONFIG__USB_DEBUG_STREAM_ENABLE
    if (nxpc_usb_debug_stream__take_enter_isp_request())
    {
        nxpc_system__set_mode(NXPC_SYSTEM_MODE_ENTERING_ISP);
        nxpc__disable_motor_control();
        nxpc__set_servo(0.0f);
        g_nxpc_system.isp_entry_tick = e_tick__get_ms();
    }

    if (g_nxpc_system.mode == NXPC_SYSTEM_MODE_ENTERING_ISP)
    {
        uint32_t elapsed = e_tick__delta(&g_nxpc_system.isp_entry_tick);

        nxpc__disable_motor_control();
        nxpc__set_servo(0.0f);
        if ((elapsed >= NXPC_SYSTEM_ISP_ACK_SETTLE_MS) &&
            (nxpc_usb_debug_stream__tx_idle() || (elapsed >= NXPC_SYSTEM_ISP_MAX_WAIT_MS)))
        {
            user_app_boot_invoke_option_t argument = {.option = {.B = {
                .tag = NXPC_SYSTEM_ROM_ENTER_BOOT_TAG,
                .mode = NXPC_SYSTEM_ROM_ISP_MODE,
                .boot_interface = NXPC_SYSTEM_ROM_USB_HS_HID_INTERFACE,
            }}};

            bootloader_user_entry(&argument);
            nxpc_system__enter_fault(NXPC_SYSTEM_FAULT_BOOTLOADER_RETURNED);
        }
        return;
    }
#endif

    if (test_requested)
    {
        if (g_nxpc_system.mode != NXPC_SYSTEM_MODE_TEST)
        {
            nxpc_system__set_mode(NXPC_SYSTEM_MODE_TEST);
        }

        if (button__up(&center_btn) != 0U)
        {
            if (g_nxpc_system.test_outputs_armed || g_nxpc_system.test_arm_pending)
            {
                g_nxpc_system.test_outputs_armed = false;
                g_nxpc_system.test_arm_pending = false;
                nxpc__disable_motor_control();
            }
            else
            {
                g_nxpc_system.test_arm_pending = true;
            }
        }

        if (g_nxpc_system.test_arm_pending &&
            (nxpc__read_alpha() > 0.45f) && (nxpc__read_alpha() < 0.55f) &&
            (nxpc__read_beta() > 0.45f) && (nxpc__read_beta() < 0.55f) &&
            (nxpc__read_gamma() > 0.45f) && (nxpc__read_gamma() < 0.55f))
        {
            g_nxpc_system.test_arm_pending = false;
            g_nxpc_system.test_outputs_armed = true;
            DEBUG("TEST outputs armed.\r\n");
        }
        return;
    }

    if (g_nxpc_system.mode == NXPC_SYSTEM_MODE_TEST)
    {
        nxpc_system__set_mode(NXPC_SYSTEM_MODE_RACE_WAITING);
    }

#if CONFIG__FORCE_RACE_WAITING_MODE
    return;
#endif

    if ((g_nxpc_system.mode == NXPC_SYSTEM_MODE_RACE_WAITING) &&
        (button__up(&center_btn) != 0U))
    {
        if (g_nxpc_system.camera_frame_seen)
        {
            nxpc_system__set_mode(NXPC_SYSTEM_MODE_RACE_RUNNING);
        }
        else
        {
            DEBUG("Race start rejected: no camera frame received.\r\n");
        }
    }
}

void nxpc_system__notify_camera_frame(void)
{
    g_nxpc_system.camera_frame_seen = true;
}

void nxpc_system__enter_fault(nxpc_system_fault_t fault)
{
    g_nxpc_system.fault = fault;
    nxpc_system__set_mode(NXPC_SYSTEM_MODE_SAFE_FAULT);
}

nxpc_system_mode_t nxpc_system__mode(void)
{
    return g_nxpc_system.mode;
}

nxpc_system_fault_t nxpc_system__fault(void)
{
    return g_nxpc_system.fault;
}

bool nxpc_system__camera_frame_seen(void)
{
    return g_nxpc_system.camera_frame_seen;
}

bool nxpc_system__outputs_allowed(void)
{
    return (g_nxpc_system.mode == NXPC_SYSTEM_MODE_RACE_RUNNING) ||
           ((g_nxpc_system.mode == NXPC_SYSTEM_MODE_TEST) && g_nxpc_system.test_outputs_armed);
}

bool nxpc_system__test_outputs_armed(void)
{
    return g_nxpc_system.test_outputs_armed;
}

bool nxpc_system__test_arm_pending(void)
{
    return g_nxpc_system.test_arm_pending;
}

const char *nxpc_system__mode_label(nxpc_system_mode_t mode)
{
    switch (mode)
    {
        case NXPC_SYSTEM_MODE_STARTUP:
            return "STARTUP";

        case NXPC_SYSTEM_MODE_TEST:
            return "TEST";

        case NXPC_SYSTEM_MODE_RACE_WAITING:
            return "RACE / WAITING";

        case NXPC_SYSTEM_MODE_RACE_RUNNING:
            return "RACE RUNNING";

        case NXPC_SYSTEM_MODE_ENTERING_ISP:
            return "ENTERING USB ISP";

        case NXPC_SYSTEM_MODE_SAFE_FAULT:
            return "SAFE / FAULT";

        default:
            return "UNKNOWN";
    }
}
