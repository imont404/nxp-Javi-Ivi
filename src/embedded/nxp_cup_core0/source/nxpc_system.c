#include "nxpc_system.h"

#include <stddef.h>

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
#define NXPC_SYSTEM_TEST_ARM_WINDOW_MS (5000U)
#define NXPC_SYSTEM_TEST_CENTER_DWELL_MS (250U)
#define NXPC_SYSTEM_TEST_CENTER_SAMPLE_MAX_GAP_MS (50U)
#define NXPC_SYSTEM_TEST_CENTER_MIN (0.45f)
#define NXPC_SYSTEM_TEST_CENTER_MAX (0.55f)

typedef struct
{
    volatile bool camera_frame_seen;
    volatile uint32_t camera_last_frame_ms;
    bool camera_frame_reported;
    bool platform_ready;
    bool test_outputs_armed;
    bool test_arm_pending;
    bool test_center_dwell_active;
    nxpc_system_mode_t mode;
    nxpc_test_page_t test_page;
    nxpc_system_fault_t fault;
    uint32_t isp_entry_tick;
    uint32_t test_arm_request_tick;
    uint32_t test_center_dwell_tick;
    uint32_t test_center_last_sample_tick;
    uint32_t center_release_sequence;
    bool suppress_center_release;
    bool center_rebaseline_pending;
} nxpc_system_runtime_t;

typedef enum
{
    NXPC_CENTER_RELEASE_NONE = 0,
    NXPC_CENTER_RELEASE_SINGLE,
    NXPC_CENTER_RELEASE_AMBIGUOUS
} nxpc_center_release_result_t;

static nxpc_system_runtime_t g_nxpc_system;

static void nxpc_system__safe_outputs(void)
{
    if (g_nxpc_system.platform_ready)
    {
        nxpc__disable_motor_control();
        nxpc__set_servo(0.0f);
    }
}

static void nxpc_system__cancel_test_arming(void)
{
    g_nxpc_system.test_outputs_armed = false;
    g_nxpc_system.test_arm_pending = false;
    g_nxpc_system.test_center_dwell_active = false;
    g_nxpc_system.test_arm_request_tick = 0U;
    g_nxpc_system.test_center_dwell_tick = 0U;
    g_nxpc_system.test_center_last_sample_tick = 0U;
}

static void nxpc_system__baseline_center_button(const button_snapshot_t *buttons)
{
    if (buttons == NULL)
    {
        return;
    }

    g_nxpc_system.center_release_sequence =
        buttons->button[BUTTON_ID_CENTER].release_sequence;
    g_nxpc_system.suppress_center_release =
        buttons->button[BUTTON_ID_CENTER].release_pending;
}

static nxpc_center_release_result_t nxpc_system__take_center_release(
    const button_snapshot_t *buttons)
{
    const button_state_snapshot_t *center;
    uint32_t release_delta;

    if (buttons == NULL)
    {
        return NXPC_CENTER_RELEASE_NONE;
    }

    center = &buttons->button[BUTTON_ID_CENTER];
    if (g_nxpc_system.suppress_center_release)
    {
        if (center->release_pending)
        {
            return NXPC_CENTER_RELEASE_NONE;
        }

        g_nxpc_system.center_release_sequence = center->release_sequence;
        g_nxpc_system.suppress_center_release = false;
        return NXPC_CENTER_RELEASE_NONE;
    }

    release_delta = center->release_sequence - g_nxpc_system.center_release_sequence;
    if (release_delta == 0U)
    {
        return NXPC_CENTER_RELEASE_NONE;
    }

    g_nxpc_system.center_release_sequence = center->release_sequence;
    return (release_delta == 1U) ? NXPC_CENTER_RELEASE_SINGLE :
                                   NXPC_CENTER_RELEASE_AMBIGUOUS;
}

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
    nxpc_system__safe_outputs();
    nxpc_system__cancel_test_arming();
    if (mode == NXPC_SYSTEM_MODE_TEST)
    {
        g_nxpc_system.test_page = NXPC_TEST_PAGE_CAMERA_IO;
    }
    g_nxpc_system.mode = mode;
    g_nxpc_system.center_rebaseline_pending = true;

    if (g_nxpc_system.platform_ready)
    {
        DEBUG("System mode: %s\r\n", nxpc_system__mode_label(mode));
    }
}

bool nxpc_system__select_test_page(nxpc_test_page_t page,
                                   const button_snapshot_t *buttons)
{
    if ((g_nxpc_system.mode != NXPC_SYSTEM_MODE_TEST) ||
        ((uint32_t)page >= (uint32_t)NXPC_TEST_PAGE_COUNT))
    {
        return false;
    }
    if (page == g_nxpc_system.test_page)
    {
        return true;
    }

    nxpc_system__safe_outputs();
    nxpc_system__cancel_test_arming();
    nxpc_system__baseline_center_button(buttons);
    g_nxpc_system.center_rebaseline_pending = false;
    g_nxpc_system.test_page = page;
    DEBUG("TEST page: %s\r\n", nxpc_system__test_page_label(page));
    return true;
}

nxpc_system_action_result_t nxpc_system__request_action(nxpc_system_action_t action)
{
    switch (action)
    {
        case NXPC_SYSTEM_ACTION_STOP:
            nxpc_system__safe_outputs();
            nxpc_system__cancel_test_arming();
            g_nxpc_system.center_rebaseline_pending = true;
            if (g_nxpc_system.mode == NXPC_SYSTEM_MODE_RACE_RUNNING)
            {
                nxpc_system__set_mode(NXPC_SYSTEM_MODE_RACE_WAITING);
            }
            return NXPC_SYSTEM_ACTION_ACCEPTED;

        case NXPC_SYSTEM_ACTION_RACE_START:
#if CONFIG__FORCE_RACE_WAITING_MODE
            return NXPC_SYSTEM_ACTION_DENIED;
#else
            if (nxpc_system__test_requested())
            {
                return NXPC_SYSTEM_ACTION_DENIED;
            }
            if (g_nxpc_system.mode != NXPC_SYSTEM_MODE_RACE_WAITING)
            {
                return NXPC_SYSTEM_ACTION_DENIED;
            }
            if (!nxpc_system__camera_live())
            {
                return NXPC_SYSTEM_ACTION_NOT_READY;
            }
            nxpc_system__set_mode(NXPC_SYSTEM_MODE_RACE_RUNNING);
            return NXPC_SYSTEM_ACTION_ACCEPTED;
#endif

        default:
            return NXPC_SYSTEM_ACTION_DENIED;
    }
}

#if CONFIG__USB_DEBUG_STREAM_ENABLE
static bool nxpc_system__service_usb_action(void)
{
    nxpc_usb_system_action_request_t request;
    nxpc_system_action_result_t result;
    uint32_t status;

    if (!nxpc_usb_debug_stream__take_system_action(&request))
    {
        return false;
    }

    result = nxpc_system__request_action((nxpc_system_action_t)request.action);
    switch (result)
    {
        case NXPC_SYSTEM_ACTION_ACCEPTED:
            status = NXPC_DBG_CONTROL_STATUS_OK;
            break;
        case NXPC_SYSTEM_ACTION_NOT_READY:
            status = NXPC_DBG_CONTROL_STATUS_NOT_READY;
            break;
        default:
            status = NXPC_DBG_CONTROL_STATUS_DENIED;
            break;
    }
    nxpc_usb_debug_stream__complete_system_action(request.requestSequence, status);
    return true;
}
#endif

void nxpc_system__init(void)
{
    g_nxpc_system.camera_frame_seen = false;
    g_nxpc_system.camera_last_frame_ms = 0U;
    g_nxpc_system.camera_frame_reported = false;
    g_nxpc_system.platform_ready = false;
    g_nxpc_system.test_outputs_armed = false;
    g_nxpc_system.test_arm_pending = false;
    g_nxpc_system.test_center_dwell_active = false;
    g_nxpc_system.mode = NXPC_SYSTEM_MODE_STARTUP;
    g_nxpc_system.test_page = NXPC_TEST_PAGE_CAMERA_IO;
    g_nxpc_system.fault = NXPC_SYSTEM_FAULT_NONE;
    g_nxpc_system.isp_entry_tick = 0U;
    g_nxpc_system.test_arm_request_tick = 0U;
    g_nxpc_system.test_center_dwell_tick = 0U;
    g_nxpc_system.test_center_last_sample_tick = 0U;
    g_nxpc_system.center_release_sequence = 0U;
    g_nxpc_system.suppress_center_release = false;
    g_nxpc_system.center_rebaseline_pending = true;

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

void nxpc_system__service(const button_snapshot_t *buttons)
{
    bool test_requested = nxpc_system__test_requested();

    if (g_nxpc_system.center_rebaseline_pending)
    {
        nxpc_system__baseline_center_button(buttons);
        g_nxpc_system.center_rebaseline_pending = false;
    }

    if (g_nxpc_system.camera_frame_seen && !g_nxpc_system.camera_frame_reported)
    {
        g_nxpc_system.camera_frame_reported = true;
        DEBUG("Camera first frame received.\r\n");
    }

#if CONFIG__USB_DEBUG_STREAM_ENABLE
    /* Defer command completion until the system state machine validates it. */
    if (nxpc_system__service_usb_action())
    {
        /* A host action has priority; discard any coincident physical EXE. */
        nxpc_system__baseline_center_button(buttons);
        g_nxpc_system.center_rebaseline_pending = false;
        return;
    }
#endif

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
        nxpc_center_release_result_t center_release;
        uint32_t now;
        float alpha;
        float beta;
        float gamma;

        if (g_nxpc_system.mode != NXPC_SYSTEM_MODE_TEST)
        {
            nxpc_system__set_mode(NXPC_SYSTEM_MODE_TEST);
            return;
        }

        center_release = nxpc_system__take_center_release(buttons);
        if (center_release == NXPC_CENTER_RELEASE_AMBIGUOUS)
        {
            nxpc_system__cancel_test_arming();
            nxpc_system__safe_outputs();
            DEBUG("Ambiguous EXE release sequence in TEST; outputs disarmed.\r\n");
            return;
        }

        if (g_nxpc_system.test_page != NXPC_TEST_PAGE_MOTORS)
        {
            if ((center_release != NXPC_CENTER_RELEASE_NONE) ||
                g_nxpc_system.test_outputs_armed || g_nxpc_system.test_arm_pending)
            {
                nxpc_system__cancel_test_arming();
                nxpc_system__safe_outputs();
            }
            return;
        }

        now = e_tick__get_ms();
        if (center_release == NXPC_CENTER_RELEASE_SINGLE)
        {
            if (g_nxpc_system.test_outputs_armed || g_nxpc_system.test_arm_pending)
            {
                nxpc_system__cancel_test_arming();
                nxpc_system__safe_outputs();
                DEBUG("TEST outputs disarmed.\r\n");
            }
            else
            {
                g_nxpc_system.test_arm_pending = true;
                g_nxpc_system.test_center_dwell_active = false;
                g_nxpc_system.test_arm_request_tick = now;
                DEBUG("TEST arm requested; center all pots.\r\n");
            }
        }

        if (!g_nxpc_system.test_arm_pending)
        {
            return;
        }

        if ((uint32_t)(now - g_nxpc_system.test_arm_request_tick) >=
            NXPC_SYSTEM_TEST_ARM_WINDOW_MS)
        {
            nxpc_system__cancel_test_arming();
            nxpc_system__safe_outputs();
            DEBUG("TEST arm request timed out.\r\n");
            return;
        }

        alpha = nxpc__read_alpha();
        beta = nxpc__read_beta();
        gamma = nxpc__read_gamma();
        if ((alpha >= NXPC_SYSTEM_TEST_CENTER_MIN) &&
            (alpha <= NXPC_SYSTEM_TEST_CENTER_MAX) &&
            (beta >= NXPC_SYSTEM_TEST_CENTER_MIN) &&
            (beta <= NXPC_SYSTEM_TEST_CENTER_MAX) &&
            (gamma >= NXPC_SYSTEM_TEST_CENTER_MIN) &&
            (gamma <= NXPC_SYSTEM_TEST_CENTER_MAX))
        {
            if (!g_nxpc_system.test_center_dwell_active)
            {
                g_nxpc_system.test_center_dwell_active = true;
                g_nxpc_system.test_center_dwell_tick = now;
                g_nxpc_system.test_center_last_sample_tick = now;
            }
            else
            {
                if ((uint32_t)(now - g_nxpc_system.test_center_last_sample_tick) >
                    NXPC_SYSTEM_TEST_CENTER_SAMPLE_MAX_GAP_MS)
                {
                    g_nxpc_system.test_center_dwell_tick = now;
                }
                g_nxpc_system.test_center_last_sample_tick = now;

                if ((uint32_t)(now - g_nxpc_system.test_center_dwell_tick) >=
                    NXPC_SYSTEM_TEST_CENTER_DWELL_MS)
                {
                    g_nxpc_system.test_arm_pending = false;
                    g_nxpc_system.test_center_dwell_active = false;
                    g_nxpc_system.test_outputs_armed = true;
                    DEBUG("TEST outputs armed.\r\n");
                }
            }
        }
        else
        {
            g_nxpc_system.test_center_dwell_active = false;
        }
        return;
    }

    if (g_nxpc_system.mode == NXPC_SYSTEM_MODE_TEST)
    {
        nxpc_system__set_mode(NXPC_SYSTEM_MODE_RACE_WAITING);
        return;
    }

#if CONFIG__FORCE_RACE_WAITING_MODE
    return;
#endif

    if (g_nxpc_system.mode == NXPC_SYSTEM_MODE_RACE_RUNNING)
    {
        nxpc_center_release_result_t center_release =
            nxpc_system__take_center_release(buttons);

        if (center_release != NXPC_CENTER_RELEASE_NONE)
        {
            nxpc_system__set_mode(NXPC_SYSTEM_MODE_RACE_WAITING);
            nxpc_system__baseline_center_button(buttons);
            g_nxpc_system.center_rebaseline_pending = false;
            if (center_release == NXPC_CENTER_RELEASE_SINGLE)
            {
                DEBUG("Race stopped by EXE; waiting for start.\r\n");
            }
            else
            {
                DEBUG("Ambiguous EXE release sequence; race stopped.\r\n");
            }
        }
        return;
    }

    if (g_nxpc_system.mode == NXPC_SYSTEM_MODE_RACE_WAITING)
    {
        nxpc_center_release_result_t center_release =
            nxpc_system__take_center_release(buttons);

        if (center_release == NXPC_CENTER_RELEASE_AMBIGUOUS)
        {
            nxpc__disable_motor_control();
            nxpc__set_servo(0.0f);
            DEBUG("Ambiguous EXE release sequence; remaining in RACE WAITING.\r\n");
        }
        else if ((center_release == NXPC_CENTER_RELEASE_SINGLE) &&
                 nxpc_system__camera_live())
        {
            nxpc_system__set_mode(NXPC_SYSTEM_MODE_RACE_RUNNING);
            nxpc_system__baseline_center_button(buttons);
            g_nxpc_system.center_rebaseline_pending = false;
        }
        else if (center_release == NXPC_CENTER_RELEASE_SINGLE)
        {
            DEBUG("Race start rejected: camera is not live.\r\n");
        }
    }
}

void nxpc_system__notify_camera_frame(void)
{
    g_nxpc_system.camera_last_frame_ms = e_tick__get_ms();
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

nxpc_system_state_t nxpc_system__state(void)
{
    switch (g_nxpc_system.mode)
    {
        case NXPC_SYSTEM_MODE_TEST:
            if (g_nxpc_system.test_outputs_armed)
            {
                return NXPC_SYSTEM_STATE_TEST_ARMED;
            }
            if (g_nxpc_system.test_arm_pending)
            {
                return NXPC_SYSTEM_STATE_TEST_CENTER_POTS;
            }
            return NXPC_SYSTEM_STATE_TEST_DISARMED;

        case NXPC_SYSTEM_MODE_RACE_WAITING:
            return nxpc_system__camera_live() ? NXPC_SYSTEM_STATE_RACE_READY :
                                               NXPC_SYSTEM_STATE_RACE_WAITING_CAMERA;

        case NXPC_SYSTEM_MODE_RACE_RUNNING:
            return NXPC_SYSTEM_STATE_RACE_RUNNING;

        case NXPC_SYSTEM_MODE_ENTERING_ISP:
            return NXPC_SYSTEM_STATE_ENTERING_ISP;

        case NXPC_SYSTEM_MODE_SAFE_FAULT:
            switch (g_nxpc_system.fault)
            {
                case NXPC_SYSTEM_FAULT_CAMERA_STARTUP:
                    return NXPC_SYSTEM_STATE_FAULT_CAMERA_STARTUP;
                case NXPC_SYSTEM_FAULT_CAMERA_LOST:
                    return NXPC_SYSTEM_STATE_FAULT_CAMERA_LOST;
                case NXPC_SYSTEM_FAULT_CALLBACK_OVERRUN:
                    return NXPC_SYSTEM_STATE_FAULT_CALLBACK_OVERRUN;
                case NXPC_SYSTEM_FAULT_BOOTLOADER_RETURNED:
                    return NXPC_SYSTEM_STATE_FAULT_BOOTLOADER_RETURNED;
                case NXPC_SYSTEM_FAULT_NONE:
                default:
                    return NXPC_SYSTEM_STATE_SAFE_FAULT;
            }

        case NXPC_SYSTEM_MODE_STARTUP:
        default:
            return NXPC_SYSTEM_STATE_INITIALIZING;
    }
}

bool nxpc_system__camera_frame_seen(void)
{
    return g_nxpc_system.camera_frame_seen;
}

bool nxpc_system__camera_live(void)
{
    uint32_t interrupt_state = DisableGlobalIRQ();
    bool frame_seen = g_nxpc_system.camera_frame_seen;
    uint32_t last_frame_ms = g_nxpc_system.camera_last_frame_ms;

    EnableGlobalIRQ(interrupt_state);
    return frame_seen &&
           ((uint32_t)(e_tick__get_ms() - last_frame_ms) <=
            NXPC_SYSTEM_CAMERA_FRESH_MS);
}

bool nxpc_system__outputs_allowed(void)
{
    return (g_nxpc_system.mode == NXPC_SYSTEM_MODE_RACE_RUNNING) ||
           ((g_nxpc_system.mode == NXPC_SYSTEM_MODE_TEST) &&
            (g_nxpc_system.test_page == NXPC_TEST_PAGE_MOTORS) &&
            g_nxpc_system.test_outputs_armed);
}

bool nxpc_system__test_outputs_armed(void)
{
    return g_nxpc_system.test_outputs_armed;
}

bool nxpc_system__test_arm_pending(void)
{
    return g_nxpc_system.test_arm_pending;
}

nxpc_test_page_t nxpc_system__test_page(void)
{
    return g_nxpc_system.test_page;
}

void nxpc_system__notify_test_motor_lease_expired(void)
{
    if ((g_nxpc_system.mode == NXPC_SYSTEM_MODE_TEST) &&
        (g_nxpc_system.test_outputs_armed || g_nxpc_system.test_arm_pending))
    {
        nxpc_system__cancel_test_arming();
        nxpc_system__safe_outputs();
        g_nxpc_system.center_rebaseline_pending = true;
        DEBUG("TEST motor lease expired; outputs require a new arm request.\r\n");
    }
}

const char *nxpc_system__test_page_label(nxpc_test_page_t page)
{
    switch (page)
    {
        case NXPC_TEST_PAGE_CAMERA_IO:
            return "CAMERA / IO";
        case NXPC_TEST_PAGE_VISION:
            return "VISION";
        case NXPC_TEST_PAGE_MOTORS:
            return "MOTORS";
        default:
            return "UNKNOWN";
    }
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

const char *nxpc_system__state_label(nxpc_system_state_t state)
{
    switch (state)
    {
        case NXPC_SYSTEM_STATE_INITIALIZING:
            return "INITIALIZING";
        case NXPC_SYSTEM_STATE_TEST_DISARMED:
            return "DISARMED";
        case NXPC_SYSTEM_STATE_TEST_CENTER_POTS:
            return "CENTER POTS";
        case NXPC_SYSTEM_STATE_TEST_ARMED:
            return "MOTORS ARMED";
        case NXPC_SYSTEM_STATE_RACE_WAITING_CAMERA:
            return "WAITING FOR CAMERA";
        case NXPC_SYSTEM_STATE_RACE_READY:
            return "READY TO START";
        case NXPC_SYSTEM_STATE_RACE_RUNNING:
            return "RUNNING";
        case NXPC_SYSTEM_STATE_ENTERING_ISP:
            return "PREPARING USB ISP";
        case NXPC_SYSTEM_STATE_FAULT_CAMERA_STARTUP:
            return "CAMERA STARTUP FAULT";
        case NXPC_SYSTEM_STATE_FAULT_CAMERA_LOST:
            return "CAMERA LOST";
        case NXPC_SYSTEM_STATE_FAULT_CALLBACK_OVERRUN:
            return "CALLBACK OVERRUN";
        case NXPC_SYSTEM_STATE_FAULT_BOOTLOADER_RETURNED:
            return "BOOTLOADER RETURNED";
        case NXPC_SYSTEM_STATE_SAFE_FAULT:
            return "SAFE FAULT";
        default:
            return "UNKNOWN";
    }
}
