"""L0_005 - keep the two-file exercise boundary small and stable."""

import re
from pathlib import Path


REPO = Path(__file__).resolve().parents[2]
SOURCE = REPO / "src/embedded/nxp_cup_core0/source"
PUBLIC_HEADER = SOURCE / "nxp_cup.h"
MAIN = SOURCE / "main.c"
MODE_FILES = (SOURCE / "app/test_mode.c", SOURCE / "app/race_mode.c")
PROTOCOL_HEADER = REPO / "src/common/nxpc_usb_debug/nxpc_usb_debug_protocol.h"
USB_DESCRIPTOR = SOURCE / "usb_device_descriptor.c"
USB_STREAM = SOURCE / "nxpc_io/nxpc_usb_debug_stream.c"
FRAMEWORK = SOURCE / "nxpc_framework.c"
SYSTEM_HEADER = SOURCE / "nxpc_system.h"
SYSTEM_SOURCE = SOURCE / "nxpc_system.c"
MOTOR_CONTROL = SOURCE / "nxpc_io/nxpc__motor_control.c"


def test_public_header_has_the_small_domain_api():
    text = PUBLIC_HEADER.read_text(encoding="utf-8")
    required = {
        "camera_row",
        "color_rgb565_to_yhsv",
        "motors_set_duty",
        "motors_stop",
        "steering_set",
        "wheel_speed_rpm",
        "wheel_speed_mps",
        "telemetry_f32",
        "telemetry_text",
        "telemetry_log",
    }
    missing = sorted(name for name in required if name not in text)
    assert not missing, f"public NXP Cup API is missing {missing}"

    lowered = text.lower()
    assert "student" not in lowered, "audience role leaked into the public API"
    assert "nxpc_" not in lowered, "internal NXP Cup namespace leaked into the public API"
    assert "avc_" not in lowered, "legacy AVC namespace leaked into the public API"
    assert "buffer_owner" not in lowered, "camera ownership internals leaked into the public API"


def test_mode_files_only_include_the_public_api_and_their_own_header():
    for path in MODE_FILES:
        includes = re.findall(r'^#include\s+"([^"]+)"', path.read_text(encoding="utf-8"), re.MULTILINE)
        assert includes == [path.with_suffix(".h").name, "nxp_cup.h"], (
            f"{path.name} reaches outside the public API: {includes}"
        )


def test_main_dispatches_one_callback_per_mode():
    text = MAIN.read_text(encoding="utf-8")
    assert text.count("test_mode_on_frame(frame);") == 1
    assert text.count("race_mode_on_frame(frame);") == 1
    assert "nxpc_framework__take_latest_frame()" in text
    assert "nxpc_framework__finish_frame(frame)" in text


def test_no_prebuilt_race_solution_is_supplied():
    race_text = MODE_FILES[1].read_text(encoding="utf-8")
    forbidden = ("pid", "lane_center", "active_differential", "edge_detector")
    assert not any(term in race_text.lower() for term in forbidden)


def test_old_algorithm_boundary_was_retired():
    assert not (SOURCE / "nxpc_student_algorithm.c").exists()
    assert not (SOURCE / "nxpc_student_algorithm.h").exists()


def test_usb_wire_identity_remains_backward_compatible():
    protocol = PROTOCOL_HEADER.read_text(encoding="utf-8")
    assert "NXPC_DBG_MAGIC (0x55435641u)" in protocol, (
        "changing the deployed AVCU wire magic would break existing viewers and the Android bridge"
    )
    assert "NXPC_DBG_VERSION (1u)" in protocol
    assert "NXPC_DBG_TELEMETRY_TYPE_TEXT (5u)" in protocol
    assert "NXPC_DBG_TELEMETRY_TEXT_MAX_BYTES (48u)" in protocol


def test_framework_telemetry_names_and_queue_capacity_are_protected():
    stream = USB_STREAM.read_text(encoding="utf-8")
    framework = FRAMEWORK.read_text(encoding="utf-8")
    protected = {
        "motor.enabled",
        "motor.left.command",
        "motor.right.command",
        "steering.command",
        "system.mode",
        "system.state",
    }

    assert "NXPC_USB_TELEMETRY_FRAMEWORK_RESERVE (6U)" in stream
    assert "NXPC_USB_TELEMETRY_PARTICIPANT_LIMIT" in stream
    assert "frameworkValue != nxpc_usb_debug_stream__framework_telemetry_name(name)" in stream
    assert "uint8_t participantCount = 0U;" in stream
    assert "participantCount < NXPC_USB_TELEMETRY_PARTICIPANT_LIMIT" in stream
    for name in protected:
        assert f'"{name}"' in stream, f"protected telemetry name is missing: {name}"
        assert f'"{name}"' in framework, f"framework telemetry publisher is missing: {name}"
    assert framework.count("nxpc_usb_debug_stream__framework_telemetry_") == len(protected)


def test_system_state_is_typed_before_it_becomes_display_text():
    header = SYSTEM_HEADER.read_text(encoding="utf-8")
    system = SYSTEM_SOURCE.read_text(encoding="utf-8")
    framework = FRAMEWORK.read_text(encoding="utf-8")

    assert "nxpc_system_state_t" in header
    assert "NXPC_SYSTEM_STATE_TEST_DISARMED" in header
    assert "NXPC_SYSTEM_STATE_RACE_READY" in header
    assert "NXPC_SYSTEM_STATE_FAULT_CALLBACK_OVERRUN" in header
    assert "nxpc_system_state_t nxpc_system__state(void)" in system
    assert "const char *nxpc_system__state_label(nxpc_system_state_t state)" in system
    assert "nxpc_system__state_label(nxpc_system__state())" in framework


def test_remote_race_actions_are_typed_and_firmware_validated():
    protocol = PROTOCOL_HEADER.read_text(encoding="utf-8")
    header = SYSTEM_HEADER.read_text(encoding="utf-8")
    system = SYSTEM_SOURCE.read_text(encoding="utf-8")
    stream = USB_STREAM.read_text(encoding="utf-8")

    assert "NXPC_DBG_CONTROL_SYSTEM_ACTION" in protocol
    assert "NXPC_DBG_SYSTEM_ACTION_RACE_START" in protocol
    assert "NXPC_DBG_SYSTEM_ACTION_STOP" in protocol
    assert "NXPC_DBG_RACE_START_CONFIRMATION" in protocol
    assert "NXPC_DBG_CAPABILITY_SYSTEM_ACTIONS" in protocol
    assert "nxpc_system_action_t" in header
    assert "nxpc_system__request_action" in header
    assert "nxpc_system__test_requested()" in system
    assert "g_nxpc_system.mode != NXPC_SYSTEM_MODE_RACE_WAITING" in system
    assert "!g_nxpc_system.camera_frame_seen" in system
    assert "nxpc__disable_motor_control();" in system
    assert "nxpc__set_servo(0.0f);" in system
    assert "request->arg1 != NXPC_DBG_RACE_START_CONFIRMATION" in stream


def test_race_entry_arms_zero_pwm_and_exe_toggles_back_to_waiting():
    system = SYSTEM_SOURCE.read_text(encoding="utf-8")
    framework = FRAMEWORK.read_text(encoding="utf-8")
    motor_control = MOTOR_CONTROL.read_text(encoding="utf-8")

    assert 'DEBUG("Race stopped by EXE; waiting for start.' in system
    assert "nxpc_system__set_mode(NXPC_SYSTEM_MODE_RACE_WAITING);" in system
    assert "nxpc__apply_actuator_mode_transition();" in framework
    assert "mode == NXPC_SYSTEM_MODE_RACE_RUNNING" in framework
    assert "nxpc__enable_motor_control();" in framework
    assert "g_motor_lease_remaining_ms = 0U;" in framework
    assert "static void nxpc__clear_race_display(void)" in framework
    assert "sizeof(g_status_buffer)" in framework
    assert "mode == NXPC_SYSTEM_MODE_RACE_WAITING" in framework
    init_body = framework.split("void nxpc_framework__init(void)", 1)[1].split(
        "void nxpc_framework__service(void)", 1
    )[0]
    assert "nxpc_system__mode() == NXPC_SYSTEM_MODE_RACE_WAITING" in init_body
    assert "nxpc__clear_race_display();" in init_body
    enable_body = motor_control.split("void nxpc__enable_motor_control()", 1)[1].split(
        "void nxpc__disable_motor_control()", 1
    )[0]
    assert "nxpc__write_motor_pwm(0.0f, 0.0f);" in enable_body
    assert enable_body.index("nxpc__write_motor_pwm(0.0f, 0.0f);") < enable_body.index(
        "PORT_SetPinMux"
    )
    assert "PWM_StartTimer(PWM1" in motor_control.split(
        "void nxpc__enable_motor_control()", 1
    )[0]


def test_usb_product_name_is_nxp_cup_telemetry():
    descriptor = USB_DESCRIPTOR.read_text(encoding="utf-8")
    assert "2U + 2U * 17U" in descriptor
    product = re.search(
        r"g_UsbDeviceString2\[\].*?=\s*\{(.*?)\};",
        descriptor,
        re.DOTALL,
    )
    assert product is not None
    characters = re.findall(r"'([^'])',\s*0(?:x00U|U)?", product.group(1))
    assert "".join(characters) == "NXP CUP TELEMETRY"
