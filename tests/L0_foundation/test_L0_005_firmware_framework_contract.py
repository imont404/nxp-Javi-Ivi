"""L0_005 - keep the two-file exercise boundary small and stable."""

import re
from pathlib import Path


REPO = Path(__file__).resolve().parents[2]
SOURCE = REPO / "src/nxp_cup/nxp_cup_core0/source"
PUBLIC_HEADER = SOURCE / "nxp_cup.h"
MAIN = SOURCE / "main.c"
MODE_FILES = (SOURCE / "app/test_mode.c", SOURCE / "app/race_mode.c")
PROTOCOL_HEADER = REPO / "src/common/nxpc_usb_debug/nxpc_usb_debug_protocol.h"
USB_DESCRIPTOR = SOURCE / "usb_device_descriptor.c"


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
