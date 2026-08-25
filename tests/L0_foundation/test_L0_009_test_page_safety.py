"""L0_009 - execute the TEST page and arming safety state machine natively."""

import shutil
import subprocess
from pathlib import Path

import pytest


REPO = Path(__file__).resolve().parents[2]
SOURCE = REPO / "src/embedded/nxp_cup_core0/source"
SYSTEM = SOURCE / "nxpc_system.c"
BUTTON_INCLUDE = SOURCE / "nxpc_io"


STUB_HEADER = r"""
#ifndef NXPC_TEST_SYSTEM_STUBS_H_
#define NXPC_TEST_SYSTEM_STUBS_H_

#include <stdbool.h>
#include <stdint.h>

#define NXPC__IO_H_
#define CONFIG__USB_DEBUG_STREAM_ENABLE 0
#define CONFIG__FORCE_TEST_MODE 0
#define CONFIG__FORCE_RACE_WAITING_MODE 0
#define TEST_SW_PIN 18U
#define GPIO3 ((void *)0)
#define DEBUG(...) do { } while (0)

void nxpc__init(void);
void nxpc__disable_motor_control(void);
void nxpc__set_servo(float position);
float nxpc__read_alpha(void);
float nxpc__read_beta(void);
float nxpc__read_gamma(void);
uint32_t e_tick__get_ms(void);
uint32_t GPIO_PinRead(void *base, uint32_t pin);
uint32_t DisableGlobalIRQ(void);
void EnableGlobalIRQ(uint32_t state);

#endif
"""


HARNESS = r"""
#include <stdint.h>
#include <stdio.h>

#include "nxpc_system.h"

static uint32_t now_ms;
static bool test_jumper = true;
static float alpha = 0.5f;
static float beta = 0.5f;
static float gamma = 0.5f;
static unsigned disable_count;
static unsigned center_count;

void nxpc__init(void) { }
void nxpc__disable_motor_control(void) { disable_count++; }
void nxpc__set_servo(float position)
{
    if (position == 0.0f) center_count++;
}
float nxpc__read_alpha(void) { return alpha; }
float nxpc__read_beta(void) { return beta; }
float nxpc__read_gamma(void) { return gamma; }
uint32_t e_tick__get_ms(void) { return now_ms; }
uint32_t DisableGlobalIRQ(void) { return 0U; }
void EnableGlobalIRQ(uint32_t state) { (void)state; }
uint32_t GPIO_PinRead(void *base, uint32_t pin)
{
    (void)base;
    (void)pin;
    return test_jumper ? 0U : 1U;
}

static int fail(const char *message)
{
    (void)fprintf(stderr, "%s\n", message);
    return 1;
}

static void release_exe(button_snapshot_t *buttons)
{
    buttons->button[BUTTON_ID_CENTER].release_sequence++;
}

int main(void)
{
    button_snapshot_t buttons = {0};
    unsigned safe_stop_count;

    nxpc_system__init();
    nxpc_system__service(&buttons);
    if (nxpc_system__mode() != NXPC_SYSTEM_MODE_TEST)
        return fail("TEST jumper did not select TEST mode");
    if (nxpc_system__test_page() != NXPC_TEST_PAGE_CAMERA_IO)
        return fail("TEST did not enter on CAMERA / IO");
    if (nxpc_system__outputs_allowed())
        return fail("CAMERA / IO allowed outputs");
    if (nxpc_system__select_test_page((nxpc_test_page_t)-1, &buttons))
        return fail("negative TEST page was accepted");

    safe_stop_count = disable_count;
    if (!nxpc_system__select_test_page(NXPC_TEST_PAGE_ACTUATORS, &buttons))
        return fail("ACTUATORS page selection failed");
    if ((disable_count <= safe_stop_count) || (center_count == 0U))
        return fail("page transition did not safe-stop hardware first");

    now_ms = 100U;
    release_exe(&buttons);
    nxpc_system__service(&buttons);
    if (!nxpc_system__test_arm_pending() || nxpc_system__outputs_allowed())
        return fail("EXE did not open a disarmed midpoint window");
    now_ms = 349U;
    nxpc_system__service(&buttons);
    if (nxpc_system__outputs_allowed())
        return fail("a long sample gap falsely satisfied midpoint dwell");
    now_ms = 399U;
    nxpc_system__service(&buttons);
    now_ms = 449U;
    nxpc_system__service(&buttons);
    now_ms = 499U;
    nxpc_system__service(&buttons);
    now_ms = 549U;
    nxpc_system__service(&buttons);
    now_ms = 599U;
    nxpc_system__service(&buttons);
    if (!nxpc_system__outputs_allowed())
        return fail("outputs did not arm after midpoint dwell");

    if (!nxpc_system__select_test_page(NXPC_TEST_PAGE_VISION, &buttons))
        return fail("VISION page selection failed");
    if (nxpc_system__outputs_allowed() || nxpc_system__test_arm_pending())
        return fail("page change inherited actuator arming");
    release_exe(&buttons);
    nxpc_system__service(&buttons);
    if (nxpc_system__test_arm_pending())
        return fail("EXE armed outside ACTUATORS");

    if (!nxpc_system__select_test_page(NXPC_TEST_PAGE_ACTUATORS, &buttons))
        return fail("ACTUATORS re-entry failed");
    alpha = 0.8f;
    now_ms = 1000U;
    release_exe(&buttons);
    nxpc_system__service(&buttons);
    now_ms = 1100U;
    nxpc_system__service(&buttons);
    alpha = 0.5f;
    now_ms = 1200U;
    nxpc_system__service(&buttons);
    now_ms = 1250U;
    nxpc_system__service(&buttons);
    now_ms = 1300U;
    nxpc_system__service(&buttons);
    now_ms = 1350U;
    nxpc_system__service(&buttons);
    now_ms = 1400U;
    nxpc_system__service(&buttons);
    if (nxpc_system__outputs_allowed())
        return fail("off-center time counted toward midpoint dwell");
    now_ms = 1450U;
    nxpc_system__service(&buttons);
    if (!nxpc_system__outputs_allowed())
        return fail("restarted midpoint dwell did not arm");

    nxpc_system__notify_test_motor_lease_expired();
    if (nxpc_system__outputs_allowed() || nxpc_system__test_arm_pending())
        return fail("lease expiry did not require a fresh arm request");

    alpha = 0.8f;
    now_ms = 2000U;
    release_exe(&buttons);
    nxpc_system__service(&buttons);
    now_ms = 7000U;
    nxpc_system__service(&buttons);
    if (nxpc_system__test_arm_pending() || nxpc_system__outputs_allowed())
        return fail("five-second arm window did not time out safely");

    alpha = 0.5f;
    now_ms = UINT32_MAX - 100U;
    release_exe(&buttons);
    nxpc_system__service(&buttons);
    now_ms = UINT32_MAX - 50U;
    nxpc_system__service(&buttons);
    now_ms = UINT32_MAX;
    nxpc_system__service(&buttons);
    now_ms = 49U;
    nxpc_system__service(&buttons);
    now_ms = 99U;
    nxpc_system__service(&buttons);
    now_ms = 149U;
    nxpc_system__service(&buttons);
    if (!nxpc_system__outputs_allowed())
        return fail("midpoint dwell was not wrap-safe");

    buttons.button[BUTTON_ID_CENTER].release_sequence += 2U;
    nxpc_system__service(&buttons);
    if (nxpc_system__outputs_allowed())
        return fail("ambiguous EXE epoch did not fail safe");

    test_jumper = false;
    nxpc_system__service(&buttons);
    if (nxpc_system__mode() != NXPC_SYSTEM_MODE_RACE_WAITING)
        return fail("TEST exit did not enter RACE WAITING");
    nxpc_system__service(&buttons);
    nxpc_system__notify_camera_frame();
    if (nxpc_system__state() != NXPC_SYSTEM_STATE_RACE_READY)
        return fail("fresh camera did not make RACE ready");
    now_ms += NXPC_SYSTEM_CAMERA_FRESH_MS + 1U;
    if (nxpc_system__state() != NXPC_SYSTEM_STATE_RACE_WAITING_CAMERA)
        return fail("stale camera left RACE ready");
    release_exe(&buttons);
    nxpc_system__service(&buttons);
    if (nxpc_system__mode() != NXPC_SYSTEM_MODE_RACE_WAITING)
        return fail("stale camera was allowed to start RACE");

    now_ms = UINT32_MAX - 100U;
    nxpc_system__notify_camera_frame();
    now_ms = 100U;
    if (!nxpc_system__camera_live())
        return fail("camera freshness was not tick-wrap safe");
    now_ms = 200U;
    if (nxpc_system__camera_live())
        return fail("wrapped camera timestamp never became stale");

    nxpc_system__notify_camera_frame();
    release_exe(&buttons);
    nxpc_system__service(&buttons);
    if (nxpc_system__mode() != NXPC_SYSTEM_MODE_RACE_RUNNING)
        return fail("fresh camera and EXE did not start RACE");
    release_exe(&buttons);
    nxpc_system__service(&buttons);
    if (nxpc_system__mode() != NXPC_SYSTEM_MODE_RACE_WAITING)
        return fail("EXE did not stop RACE");
    if (nxpc_system__request_action(NXPC_SYSTEM_ACTION_RACE_START) !=
        NXPC_SYSTEM_ACTION_ACCEPTED)
        return fail("validated remote action did not start ready RACE");
    if (nxpc_system__request_action(NXPC_SYSTEM_ACTION_STOP) !=
        NXPC_SYSTEM_ACTION_ACCEPTED)
        return fail("validated remote action did not stop RACE");
    if (nxpc_system__mode() != NXPC_SYSTEM_MODE_RACE_WAITING)
        return fail("remote STOP did not return to RACE WAITING");
    test_jumper = true;
    nxpc_system__service(&buttons);
    if ((nxpc_system__mode() != NXPC_SYSTEM_MODE_TEST) ||
        (nxpc_system__test_page() != NXPC_TEST_PAGE_CAMERA_IO))
        return fail("TEST re-entry did not reset CAMERA / IO");

    return 0;
}
"""


def test_test_page_safety_state_machine(tmp_path: Path):
    compiler = shutil.which("clang") or shutil.which("gcc") or shutil.which("cc")
    if compiler is None:
        pytest.skip("a native C compiler is required for the TEST safety trace")

    stub = tmp_path / "nxpc_test_system_stubs.h"
    harness = tmp_path / "nxpc_test_system_harness.c"
    executable = tmp_path / (
        "nxpc_test_system_harness.exe"
        if Path(compiler).suffix.lower() == ".exe"
        else "nxpc_test_system_harness"
    )
    stub.write_text(STUB_HEADER, encoding="utf-8")
    harness.write_text(HARNESS, encoding="utf-8")

    compile_result = subprocess.run(
        [
            compiler,
            "-std=c99",
            "-Wall",
            "-Wextra",
            "-Werror",
            "-include",
            str(stub),
            str(harness),
            str(SYSTEM),
            f"-I{SOURCE}",
            f"-I{BUTTON_INCLUDE}",
            "-o",
            str(executable),
        ],
        cwd=REPO,
        capture_output=True,
        text=True,
        check=False,
    )
    assert compile_result.returncode == 0, compile_result.stdout + compile_result.stderr

    run_result = subprocess.run(
        [str(executable)],
        cwd=REPO,
        capture_output=True,
        text=True,
        check=False,
    )
    assert run_result.returncode == 0, run_result.stdout + run_result.stderr


def test_framework_caps_test_motors_and_clears_the_lease_before_page_change():
    framework = (SOURCE / "nxpc_framework.c").read_text(encoding="utf-8")

    assert "NXPC_TEST_MOTOR_DUTY_LIMIT (0.25f)" in framework
    transition = framework.split("bool nxpc_framework__select_test_page", 1)[1].split(
        "uint16_t *nxpc_framework__take_latest_frame", 1
    )[0]
    assert transition.index("g_motor_lease_remaining_ms = 0U") < transition.index(
        "nxpc_system__select_test_page"
    )
    assert transition.index("nxpc__disable_motor_control()") < transition.index(
        "nxpc_system__select_test_page"
    )
    assert transition.index("nxpc__set_servo(0.0f)") < transition.index(
        "nxpc_system__select_test_page"
    )


def test_navigation_is_frame_independent_and_vision_owns_the_participant_callback():
    framework = (SOURCE / "nxpc_framework.c").read_text(encoding="utf-8")
    main = (SOURCE / "main.c").read_text(encoding="utf-8")
    service = framework.split("void nxpc_framework__service(void)", 1)[1].split(
        "bool nxpc_framework__select_test_page", 1
    )[0]

    assert "nxpc__test_navigation_service();" in service
    assert service.index("nxpc__test_navigation_service();") < service.index(
        "nxpc__render_banner_if_changed();"
    )
    assert '"TEST MODE"' in framework
    for label in ('"< CAMERA / IO >"', '"< VISION >"', '"< MOTORS >"'):
        assert label in framework
    assert "nxpc__draw_large_centered" in framework
    assert "(left_release != NXPC_RELEASE_NONE)" in framework
    assert "(right_release != NXPC_RELEASE_NONE)" in framework
    assert "nxpc__baseline_test_navigation();" in framework
    assert "nxpc_system__test_page() == NXPC_TEST_PAGE_VISION" in main


def test_camera_io_samples_without_frames_and_overlays_before_publication():
    framework = (SOURCE / "nxpc_framework.c").read_text(encoding="utf-8")
    service = framework.split("void nxpc_framework__service(void)", 1)[1].split(
        "bool nxpc_framework__select_test_page", 1
    )[0]
    finish = framework.split("void nxpc_framework__finish_frame", 1)[1].split(
        "color_features_t color_rgb565_to_yhsv", 1
    )[0]

    assert "NXPC_TEST_IO_SAMPLE_MS (100U)" in framework
    assert "nxpc__test_io_service();" in service
    assert "nxpc__camera_live()" in framework
    for label, x in (("A", 2), ("B", 78), ("G", 154), ("BAT", 228)):
        assert f'&g_status_surface, {x}, 22, "{label}"' in framework
    assert "nxpc__refresh_camera_io_status();" in finish
    assert "nxpc__overlay_camera_io(frame);" not in finish
    assert "alpha_thousandths" in framework
    assert "BTN L:" not in framework
    assert "FRAME:%lums WORK:" not in framework
    assert finish.index("nxpc__refresh_camera_io_status();") < finish.index(
        "nxpc_usb_debug_stream__publish_frame(frame)"
    )


def test_vision_sandbox_has_no_actuator_commands_or_race_solution():
    vision = (SOURCE / "app/test_mode.c").read_text(encoding="utf-8")

    assert "motors_set_duty(" not in vision
    assert "motors_stop(" not in vision
    assert "steering_set(" not in vision
    assert "frame_draw_vertical_line(" in vision
    assert "frame_fill_rectangle(" not in vision
    assert "frame_draw_text(" not in vision
    assert "not a lane" in vision.lower()


def test_actuator_page_control_is_bounded_and_frame_independent():
    framework = (SOURCE / "nxpc_framework.c").read_text(encoding="utf-8")
    service = framework.split("void nxpc_framework__service(void)", 1)[1].split(
        "bool nxpc_framework__select_test_page", 1
    )[0]
    finish = framework.split("void nxpc_framework__finish_frame", 1)[1].split(
        "color_features_t color_rgb565_to_yhsv", 1
    )[0]
    actuator_service = framework.split("static void nxpc__test_actuator_service", 1)[1].split(
        "static void nxpc__motor_lease_tick", 1
    )[0]

    assert "NXPC_TEST_ACTUATOR_CONTROL_MS (20U)" in framework
    assert "nxpc_system__test_page() != NXPC_TEST_PAGE_ACTUATORS" in actuator_service
    assert "motors_set_duty((alpha * 2.0f) - 1.0f" in actuator_service
    assert "(gamma * 2.0f) - 1.0f" in actuator_service
    assert "steering_set((beta * 2.0f) - 1.0f)" in actuator_service
    assert service.index("nxpc_system__notify_test_motor_lease_expired();") < service.index(
        "nxpc__test_actuator_service();"
    )
    assert '"PRESS EXE TO TEST"' in framework
    assert '"L%+ld%%  S%+ld%%  R%+ld%%"' in framework
    assert "OUTPUTS SAFE" not in framework
    assert "OUTPUTS ARMED" not in framework
    assert finish.index("nxpc__refresh_motors_status();") < finish.index(
        "nxpc_usb_debug_stream__publish_frame(frame)"
    )


def test_test_lease_expiry_is_atomic_with_motor_command_and_renewal():
    framework = (SOURCE / "nxpc_framework.c").read_text(encoding="utf-8")
    motors = framework.split("void motors_set_duty", 1)[1].split(
        "void motors_stop", 1
    )[0]

    irq_off = motors.index("interrupt_state = DisableGlobalIRQ();")
    sticky_gate = motors.index("g_motor_lease_expired", irq_off)
    pwm = motors.index("nxpc__set_motor_pwm(left, right);", sticky_gate)
    renew = motors.index("g_motor_lease_remaining_ms = NXPC_MOTOR_LEASE_MS;", pwm)
    irq_on = motors.index("EnableGlobalIRQ(interrupt_state);", renew)
    disarm = motors.index("nxpc_system__notify_test_motor_lease_expired();", irq_on)

    assert irq_off < sticky_gate < pwm < renew < irq_on < disarm
    assert "mode == NXPC_SYSTEM_MODE_TEST" in motors
    assert "mode == NXPC_SYSTEM_MODE_RACE_RUNNING" in motors
