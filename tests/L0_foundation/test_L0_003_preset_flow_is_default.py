"""L0_003 - one competition build is the normal path.

The root scripts must resolve to the CMake competition image unless a
maintainer deliberately supplies -File or -Mcux. Keeping one visible preset
prevents a student from silently building or flashing a diagnostic image.
"""

import json
import re
from pathlib import Path

import pytest

REPO = Path(__file__).resolve().parents[2]

FLASH = REPO / "src/embedded/flash.ps1"
RTT = REPO / "src/embedded/tools/rtt.ps1"
BUILD = REPO / "src/embedded/build.ps1"
SHARED = REPO / "src/embedded/tools/lib/nxpc_image_common.ps1"
PRESETS = REPO / "src/embedded/CMakePresets.json"

ENTRY_POINTS = pytest.mark.parametrize("script", [FLASH, RTT], ids=["flash", "rtt"])


def _text(path: Path) -> str:
    assert path.is_file(), f"{path.name} is missing"
    return path.read_text(encoding="utf-8")


def test_only_competition_preset_is_visible():
    data = json.loads(_text(PRESETS))
    visible = [p["name"] for p in data["configurePresets"] if not p.get("hidden")]
    build_names = [p["name"] for p in data["buildPresets"]]
    assert visible == ["competition"]
    assert build_names == ["competition"]


def test_shared_resolver_exists():
    text = _text(SHARED)
    for func in ("Resolve-NxpCupImage", "Resolve-NxpCupArmTool"):
        assert f"function {func}" in text, f"{func} is missing from the shared resolver"
    assert "Get-NxpCupPresetNames" not in text


def test_canonical_build_has_no_variant_selector():
    text = _text(BUILD)
    assert "$Preset" not in text
    assert '$preset = "competition"' in text
    assert "cmake @configureArguments" in text
    assert "cmake --build --preset $preset" in text
    assert "mcuxpressoidec" not in text.lower()


def test_flash_defaults_to_rom_then_jlink_commander():
    text = _text(FLASH)
    assert '[ValidateSet("Rom", "JLink")]' in text
    assert "OzonePath" not in text
    assert '-Backend Ozone' not in text
    assert "nxpc_tool.exe" in text, "ROM-HID backend is missing"
    assert "jlink_common.ps1" in text, "maintainer J-Link backend is missing"
    assert '$automaticBackend = [string]::IsNullOrWhiteSpace($Backend)' in text
    assert '$Backend = "JLink"' in text
    assert "J-Link Commander fallback" in text
    assert '[string]$JLinkPath = ""' in text
    assert "NXPC_JLINK_PATH" in text
    assert 'Join-Path $programFilesRoot "SEGGER"' in text
    assert "VersionDigits" in text
    assert "J-Link Commander: $JLinkPath" in text

    rom_at = text.index("& $hostTool program --image $binFile")
    jlink_at = text.index("& $JLinkPath @jlinkArgs")
    assert rom_at < jlink_at, "the automatic flow must try ROM before J-Link"


@ENTRY_POINTS
def test_entry_points_use_fixed_competition_resolver(script):
    text = _text(script)
    assert "$Preset" not in text
    assert "Resolve-NxpCupImage" in text


def test_resolver_defaults_to_competition_build():
    text = _text(SHARED)
    assert "out\\build\\embedded\\competition\\nxp_cup_core0.axf" in text
    assert "Unknown preset" not in text
    assert "-Preset" not in text


@ENTRY_POINTS
def test_no_legacy_default_path(script):
    text = _text(script)
    offenders = [
        line.strip()
        for line in text.splitlines()
        if not line.lstrip().startswith("#")
        and re.search(r"defaultAxf|nxp_cup_core0-\$Configuration", line)
    ]
    assert not offenders, (
        f"{script.name} still resolves a default image itself: {offenders}"
    )


@ENTRY_POINTS
def test_cmake_switch_is_a_no_op(script):
    """Keep old -CMake commands accepted while the fixed CMake path is normal."""
    text = _text(script)
    assert "[switch]$CMake" in text
    used = [
        line.strip()
        for line in text.splitlines()
        if not line.lstrip().startswith("#") and re.search(r"\$CMake\b", line)
        and "[switch]" not in line
    ]
    assert not used, f"{script.name} still branches on -CMake: {used}"


def test_rtt_does_not_hardcode_mcuxpresso_tools():
    text = _text(RTT)
    offenders = [
        line.strip()
        for line in text.splitlines()
        if not line.lstrip().startswith("#")
        and re.search(r"(?i)mcuxpressoide_", line)
    ]
    assert not offenders, f"rtt.ps1 hard-codes an MCUXpresso tool path: {offenders}"
    assert "Resolve-NxpCupArmTool" in text


def test_resolver_prefers_provisioned_toolchain():
    text = _text(SHARED)
    local_at = text.index("out\\toolchains")
    path_at = text.index("Get-Command")
    assert local_at < path_at, (
        "the shared resolver searches PATH before the provisioned toolchain"
    )
