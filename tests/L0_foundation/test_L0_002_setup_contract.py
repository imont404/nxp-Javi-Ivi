"""L0_002 - the Windows setup and component entry points agree with the tree."""

import json
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]

SETUP_SCRIPT = REPO / "setup.ps1"
SETUP_VERSIONS = REPO / "setup.versions.json"
ROOT_README = REPO / "README.md"
SRC_README = REPO / "src/README.md"
PRESETS = REPO / "src/embedded/CMakePresets.json"
FIRMWARE_BUILD = REPO / "src/embedded/build.ps1"
HOST_BUILD = REPO / "src/host/build.ps1"
ANDROID_BUILD = REPO / "src/android/build.ps1"
ANDROID_TOOL_BUILD = REPO / "src/android/tools/build-project.ps1"
HOST_VIEWER = REPO / "src/host/nxpc_viewer.cpp"
ANDROID_RELAY_VIEWER = (
    REPO / "src/android/nxp_cup_bridge/app/src/main/res/raw/relay_viewer.html"
)


def test_setup_script_exists():
    assert SETUP_SCRIPT.is_file(), "setup.ps1 is missing"
    assert SETUP_VERSIONS.is_file(), "setup.versions.json is missing"
    text = SETUP_SCRIPT.read_text(encoding="utf-8") + SETUP_VERSIONS.read_text(
        encoding="utf-8"
    )
    for tool in (
        "arm-gnu-toolchain",
        "Kitware.CMake",
        "Ninja-build.Ninja",
        "astral-sh.uv",
        "MartinStorsjo.LLVM-MinGW.UCRT",
    ):
        assert tool in text, f"setup.ps1 does not provision {tool}"


def test_setup_pins_an_immutable_core_tools_release():
    pins = json.loads(SETUP_VERSIONS.read_text(encoding="utf-8"))
    core = pins["coreTools"]
    assert core["releaseVersion"] == "1.0.0"
    assert core["releaseTag"] == "core-tools-v1.0.0"
    assert core["sourceCommit"] == "d87d3d13d8d47d13d637a22243c1f7f0b9e9137c"
    assert core["assetName"] == "nxp-cup-core-tools-win-x64-1.0.0.zip"
    assert f"/releases/download/{core['releaseTag']}/{core['assetName']}" in core["url"]
    assert "latest" not in core["url"].lower()
    assert len(core["sha256"]) == 64
    assert core["selfTestArguments"] == ["selftest"]


def test_setup_pins_and_verifies_the_arm_archive():
    pins = json.loads(SETUP_VERSIONS.read_text(encoding="utf-8"))
    arm = pins["armGnu"]
    assert arm["releaseVersion"] == "14.2.rel1"
    assert arm["compilerVersion"] == "14.2.1"
    assert len(arm["compilerSha256"]) == 64
    assert len(arm["sha256"]) == 64
    setup = SETUP_SCRIPT.read_text(encoding="utf-8")
    assert "Get-FileHash" in setup
    assert "Get-VerifiedArchive" in setup


def test_setup_script_does_not_persist_environment():
    text = SETUP_SCRIPT.read_text(encoding="utf-8")
    assert "setx" not in text.lower()
    assert "[Environment]::SetEnvironmentVariable" not in text


def test_setup_compiler_builds_the_native_host():
    setup = SETUP_SCRIPT.read_text(encoding="utf-8")
    host_build = HOST_BUILD.read_text(encoding="utf-8")
    assert "MartinStorsjo.LLVM-MinGW.UCRT" in setup
    assert 'Get-Command "clang++"' in host_build
    assert '"Ninja Multi-Config"' in host_build
    assert 'out/build/host/cmake-clang' in host_build
    assert '[ValidateSet("Clang", "MSVC")]' in host_build


def test_component_builds_publish_under_out():
    firmware = FIRMWARE_BUILD.read_text(encoding="utf-8")
    host = HOST_BUILD.read_text(encoding="utf-8")
    android = ANDROID_BUILD.read_text(encoding="utf-8")
    ignore = (REPO / ".gitignore").read_text(encoding="utf-8")
    assert "out\\artifacts\\embedded" in firmware
    assert "nxp_cup_core0.axf" in firmware
    assert "out/artifacts/host" in host
    assert "nxpc_viewer.exe" in host
    assert "out\\artifacts\\android" in android
    assert '"nxp_cup_bridge.apk"' in android
    assert "app-debug.apk" in android
    assert "out/" in ignore


def test_host_viewer_uses_published_firmware_location():
    viewer = HOST_VIEWER.read_text(encoding="utf-8")
    assert 'fs::path("out") / "artifacts" / "embedded" / "nxp_cup_core0.bin"' in viewer
    assert "GetModuleFileNameA" in viewer
    assert "out\\build\\embedded\\competition\\nxp_cup_core0.bin" not in viewer
    assert 'ImGui::Button("Program and reconnect"' in viewer
    assert "Erase application flash" not in viewer
    assert "erase_confirmation" not in viewer


def test_android_build_uses_provisioned_gradle_and_supports_offline_builds():
    text = ANDROID_TOOL_BUILD.read_text(encoding="utf-8")
    assert "$script:NxpCupAndroidGradle" in text
    assert "gradlew.bat" not in text
    assert '"--offline"' in text


def test_android_relay_viewer_has_prominent_video_mode_controls():
    viewer = ANDROID_RELAY_VIEWER.read_text(encoding="utf-8")
    assert "aspect-ratio: 8 / 5" in viewer
    for mode in ("jpeg", "h264", "raw"):
        assert f'data-video="{mode}"' in viewer
        assert f'?video={mode}&amp;replace=1' in viewer


def test_readmes_are_the_current_component_map():
    root = ROOT_README.read_text(encoding="utf-8")
    source_map = SRC_README.read_text(encoding="utf-8")
    for command in (
        ".\\src\\embedded\\build.ps1",
        ".\\src\\host\\build.ps1",
        ".\\src\\android\\build.ps1",
    ):
        assert command in root
    for component in ("embedded", "host", "android", "common"):
        assert component in source_map


def test_only_competition_preset_is_student_facing():
    data = json.loads(PRESETS.read_text(encoding="utf-8"))
    visible = [p["name"] for p in data["configurePresets"] if not p.get("hidden")]
    assert visible == ["competition"]


def test_toolchain_discovery_prefers_local():
    toolchain = REPO / "src/embedded/nxp_cup_core0/cmake/mcuxpresso-toolchain.cmake"
    text = toolchain.read_text(encoding="utf-8")
    assert "NXPC_ARM_TOOLCHAIN_DIR" in text
    assert "out/toolchains" in text
    assert text.index("out/toolchains") < text.index("MCUXpressoIDE")
