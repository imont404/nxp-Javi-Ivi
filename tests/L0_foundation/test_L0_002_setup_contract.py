"""L0_002 - setup.ps1 and docs/setup.html agree with each other and the tree.

docs/setup.html tells a student what to run. These assert the page is not
describing a workflow that no longer exists - the failure mode being a student
following instructions that quietly stopped being true.
"""

import json
import re
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]

SETUP_SCRIPT = REPO / "setup.ps1"
SETUP_DOC = REPO / "docs/setup.html"
PRESETS = REPO / "CMakePresets.json"
HOST_BUILD = REPO / "src" / "nxp_cup_host" / "build_nxpc_viewer.ps1"
ROOT_FIRMWARE_BUILD = REPO / "build.ps1"
ROOT_VIEWER_BUILD = REPO / "build_viewer.ps1"
ROOT_ANDROID_BUILD = REPO / "build_android.ps1"
ANDROID_RELAY_VIEWER = (
    REPO / "src/android/nxp_cup_bridge/app/src/main/res/raw/relay_viewer.html"
)


def _preset_names() -> set[str]:
    data = json.loads(PRESETS.read_text(encoding="utf-8"))
    return {p["name"] for p in data["configurePresets"] if not p.get("hidden")}


def test_setup_script_exists():
    assert SETUP_SCRIPT.is_file(), "setup.ps1 is missing"
    text = SETUP_SCRIPT.read_text(encoding="utf-8")
    # Everything the documentation says it provisions.
    for tool in ("arm-gnu-toolchain", "Kitware.CMake", "Ninja-build.Ninja",
                 "astral-sh.uv", "MartinStorsjo.LLVM-MinGW.UCRT"):
        assert tool in text, f"setup.ps1 does not provision {tool}"


def test_setup_script_does_not_persist_environment():
    """The script promises it changes no environment state. Enforce it, because
    a student's machine is not ours to modify."""
    text = SETUP_SCRIPT.read_text(encoding="utf-8")
    assert "setx" not in text.lower(), "setup.ps1 must not persist environment variables"
    assert "[Environment]::SetEnvironmentVariable" not in text, (
        "setup.ps1 must not persist environment variables"
    )


def test_setup_compiler_builds_the_native_host():
    """The provisioned LLVM package must be the self-contained Windows C++
    toolchain consumed by the SDL2/Dear ImGui host build."""
    setup = SETUP_SCRIPT.read_text(encoding="utf-8")
    host_build = HOST_BUILD.read_text(encoding="utf-8")
    assert "MartinStorsjo.LLVM-MinGW.UCRT" in setup
    assert 'Get-Command "clang++"' in host_build
    assert '"Ninja Multi-Config"' in host_build
    assert 'build/host/nxp_cup_host' in host_build


def test_root_builds_publish_convenient_bins():
    firmware_build = ROOT_FIRMWARE_BUILD.read_text(encoding="utf-8")
    viewer_build = ROOT_VIEWER_BUILD.read_text(encoding="utf-8")
    android_build = ROOT_ANDROID_BUILD.read_text(encoding="utf-8")
    ignore = (REPO / ".gitignore").read_text(encoding="utf-8")
    assert '"bin\\firmware"' in firmware_build
    assert "preset.txt" not in firmware_build
    assert "bin\\host" in viewer_build
    assert "nxpc_viewer.exe" in viewer_build
    assert "bin\\android" in android_build
    assert '"nxp_cup_bridge.apk"' in android_build
    assert "nxp_cup_bridge-debug.apk" not in android_build
    assert "/bin/" in ignore


def test_android_build_uses_provisioned_gradle_and_supports_offline_builds():
    android_build = (REPO / "scripts" / "android" / "build_android.ps1").read_text(
        encoding="utf-8"
    )
    assert "$script:NxpCupAndroidGradle" in android_build
    assert "gradlew.bat" not in android_build
    assert '"--offline"' in android_build


def test_android_relay_viewer_has_prominent_video_mode_controls():
    viewer = ANDROID_RELAY_VIEWER.read_text(encoding="utf-8")
    assert "aspect-ratio: 8 / 5" in viewer
    for mode in ("jpeg", "h264", "raw"):
        assert f'data-video="{mode}"' in viewer
        assert f'?video={mode}&amp;replace=1' in viewer


def test_docs_reference_real_presets():
    """Every preset named in the setup page must exist."""
    assert SETUP_DOC.is_file(), "docs/setup.html is missing"
    html = SETUP_DOC.read_text(encoding="utf-8")
    known = _preset_names()
    # Preset names appear in the page as <code>name</code> entries.
    quoted = set(re.findall(r"<code>([a-z0-9][a-z0-9\-]{3,})</code>", html))
    referenced = quoted & known
    assert referenced, "the setup page names no presets at all"
    unknown = referenced - known
    assert not unknown, f"setup page references presets that do not exist: {sorted(unknown)}"


def test_docs_commands_are_current():
    """The page must document the preset workflow, not a superseded one."""
    html = SETUP_DOC.read_text(encoding="utf-8")
    assert "cmake --preset competition" in html, "setup page does not document the preset workflow"
    assert "setup.ps1" in html, "setup page does not mention the provisioning script"
    assert ".\\build.ps1" in html, "setup page does not use the canonical root build wrapper"
    assert "QDC wheel-speed feedback" in html, "setup page does not document standard QDC feedback"
    assert ".\\build_android.ps1 -Offline" in html, "setup page does not document offline Android builds"


def test_docs_do_not_teach_probe_scripts():
    """The student path is: build with CMake, flash with Ozone. flash.ps1 and
    rtt.ps1 are maintainer tooling that drive a J-Link directly.

    This is worth a test rather than a convention: those scripts are the fastest
    way for a maintainer to put an image on a board, so they drift into
    student-facing docs without anyone deciding they should.
    """
    html = SETUP_DOC.read_text(encoding="utf-8")
    offenders = [
        line.strip()
        for line in html.splitlines()
        if re.search(r"(?i)rtt\.ps1|flash\.ps1|JLinkRTT|_SEGGER_RTT", line)
    ]
    assert not offenders, (
        "docs/setup.html tells students to run maintainer probe tooling: "
        f"{offenders}"
    )


def test_docs_name_ozone_as_the_flashing_route():
    """Having removed the scripts, the page must still say how to get an image
    onto the board, and point at the image the CMake build actually produces."""
    html = SETUP_DOC.read_text(encoding="utf-8")
    assert re.search(r"(?i)ozone", html), (
        "setup page does not say how to flash the board"
    )
    assert "build\\cmake\\competition\\nxp_cup_core0.axf" in html, (
        "setup page does not name the image the competition preset produces"
    )


def test_docs_point_at_the_debug_tools_students_have():
    """Students observe a running car through the LCD and the USB frame stream,
    not a debugger."""
    html = SETUP_DOC.read_text(encoding="utf-8")
    assert re.search(r"(?i)\bLCD\b", html), (
        "setup page does not mention the on-board LCD"
    )
    assert re.search(r"(?i)USB", html), (
        "setup page does not mention the USB frame stream"
    )


def test_docs_do_not_expose_dangerous_diagnostic_presets():
    """The student page has one happy path; motor-driving diagnostics stay in
    maintainer documentation where their safety procedure travels with them."""
    html = SETUP_DOC.read_text(encoding="utf-8")
    assert "encoder-diag-motors" not in html
    assert "flexio-port1" not in html


def test_toolchain_discovery_prefers_local():
    """MCUXpresso must be a fallback, not the first choice, or the one-script
    install is decorative."""
    toolchain = REPO / "src/nxp_cup/nxp_cup_core0/cmake/mcuxpresso-toolchain.cmake"
    assert toolchain.is_file(), "toolchain file is missing"
    text = toolchain.read_text(encoding="utf-8")
    assert "NXPC_ARM_TOOLCHAIN_DIR" in text, "toolchain file has no override hook"
    assert "out/toolchains" in text, "toolchain file does not look for the provisioned toolchain"
    local_at = text.index("out/toolchains")
    mcux_at = text.index("MCUXpressoIDE")
    assert local_at < mcux_at, (
        "MCUXpresso is searched before the locally provisioned toolchain"
    )
