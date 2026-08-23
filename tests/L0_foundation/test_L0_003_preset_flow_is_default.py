"""L0_003 - the CMake preset flow is the default path, not an alternative.

flash.ps1 and rtt.ps1 used to default to the MCUXpresso output directory, which
made the preset flow something you had to opt into with a hand-typed path. These
assert the default stayed flipped, because the failure mode is silent: a script
that quietly resolves to a stale image from a different build system still
flashes something, and the board still runs.
"""

import json
import re
from pathlib import Path

import pytest

REPO = Path(__file__).resolve().parents[2]

FLASH = REPO / "flash.ps1"
RTT = REPO / "rtt.ps1"
BUILD = REPO / "build.ps1"
SHARED = REPO / "scripts/tools/nxpc_image_common.ps1"
PRESETS = REPO / "CMakePresets.json"

ENTRY_POINTS = pytest.mark.parametrize("script", [FLASH, RTT], ids=["flash", "rtt"])


def _text(path: Path) -> str:
    assert path.is_file(), f"{path.name} is missing"
    return path.read_text(encoding="utf-8")


def test_shared_resolver_exists():
    """One resolver, so the two scripts cannot drift apart on what a preset
    name means."""
    text = _text(SHARED)
    for func in ("Resolve-NxpCupImage", "Resolve-NxpCupArmTool", "Get-NxpCupPresetNames"):
        assert f"function {func}" in text, f"{func} is missing from the shared resolver"


def test_canonical_build_is_the_competition_preset_wrapper():
    text = _text(BUILD)
    assert re.search(r'\[string\]\$Preset\s*=\s*"competition"', text)
    assert 'cmake @configureArguments' in text
    assert 'cmake --build --preset $Preset' in text
    assert 'mcuxpressoidec' not in text.lower()


def test_flash_backend_is_explicit_until_evaluation_finishes():
    text = _text(FLASH)
    assert '[ValidateSet("Ozone", "Rom", "JLink")]' in text
    assert 'Choose a flash backend explicitly' in text
    assert 'nxpc_tool.exe' in text, "ROM-HID backend is missing"
    assert 'jlink_common.ps1' in text, "maintainer J-Link backend is missing"


@ENTRY_POINTS
def test_defaults_to_competition_preset(script):
    text = _text(script)
    assert re.search(r'\[string\]\$Preset\s*=\s*"competition"', text), (
        f"{script.name} does not default to the competition preset"
    )
    assert "Resolve-NxpCupImage" in text, (
        f"{script.name} does not use the shared image resolver"
    )


@ENTRY_POINTS
def test_no_legacy_default_path(script):
    """The old default. If it comes back as a fallback, the preset flow stops
    being the thing that actually runs."""
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
    """-CMake selected the preset flow back when it was optional. It must survive
    as an accepted no-op so old commands and notes do not break."""
    text = _text(script)
    assert "[switch]$CMake" in text, (
        f"{script.name} no longer accepts -CMake; existing commands would fail"
    )
    used = [
        line.strip()
        for line in text.splitlines()
        if not line.lstrip().startswith("#") and re.search(r"\$CMake\b", line)
        and "[switch]" not in line
    ]
    assert not used, f"{script.name} still branches on -CMake: {used}"


def test_rtt_does_not_hardcode_mcuxpresso_tools():
    """rtt.ps1 needs arm-none-eabi-nm to find _SEGGER_RTT. Hard-coding the
    MCUXpresso plugin path made the default flow depend on an install the
    one-script setup never creates."""
    text = _text(RTT)
    offenders = [
        line.strip()
        for line in text.splitlines()
        if not line.lstrip().startswith("#")
        and re.search(r"(?i)mcuxpressoide_", line)
    ]
    assert not offenders, f"rtt.ps1 hard-codes an MCUXpresso tool path: {offenders}"
    assert "Resolve-NxpCupArmTool" in text, (
        "rtt.ps1 does not resolve arm-none-eabi-nm from the provisioned toolchain"
    )


def test_resolver_prefers_provisioned_toolchain():
    """Same discovery order as the CMake toolchain file, or the scripts and the
    build could disagree about which compiler produced the image."""
    text = _text(SHARED)
    local_at = text.index("out\\toolchains")
    path_at = text.index("Get-Command")
    assert local_at < path_at, (
        "the shared resolver searches PATH before the provisioned toolchain"
    )


def test_resolver_rejects_unknown_presets():
    """A typo must be reported as a typo. Falling through to 'file not found'
    sends you looking at the build instead of the command."""
    text = _text(SHARED)
    assert "Unknown preset" in text, (
        "the resolver does not validate preset names against CMakePresets.json"
    )


def test_documented_preset_examples_exist():
    """Every preset AGENTS.md shows in a -Preset example must be real."""
    agents = (REPO / "AGENTS.md").read_text(encoding="utf-8")
    known = {
        p["name"]
        for p in json.loads(PRESETS.read_text(encoding="utf-8"))["configurePresets"]
        if not p.get("hidden")
    }
    referenced = set(re.findall(r"-Preset\s+([a-z0-9][a-z0-9\-]*)", agents))
    assert referenced, "AGENTS.md shows no -Preset examples"
    unknown = referenced - known
    assert not unknown, f"AGENTS.md uses presets that do not exist: {sorted(unknown)}"
