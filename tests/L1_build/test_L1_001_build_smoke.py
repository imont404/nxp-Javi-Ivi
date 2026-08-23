"""L1_001 - every supported build variant still compiles.

This is the gate that makes changes to the source list, the linker scripts, or
the capture backends safe to attempt. It needs no board.

Builds are slow, so the full sweep is opt-in via NXPC_TEST_ALL_PRESETS=1;
the competition image always builds because it is the one that matters.
"""

import json
import os
import shutil
import subprocess
from pathlib import Path

import pytest

REPO = Path(__file__).resolve().parents[2]
PRESETS = REPO / "CMakePresets.json"


def _have(tool: str) -> bool:
    return shutil.which(tool) is not None


def _arm_toolchain_available() -> bool:
    if _have("arm-none-eabi-gcc"):
        return True
    if list((REPO / "out" / "toolchains").glob(
            "arm-gnu-toolchain-*-arm-none-eabi/bin/arm-none-eabi-gcc.exe")):
        return True
    return Path("C:/nxp/MCUXpressoIDE_25.6.136/ide/tools/bin/arm-none-eabi-gcc.exe").is_file()


pytestmark = [
    pytest.mark.skipif(not _have("cmake"), reason="cmake not on PATH; run setup.ps1"),
    pytest.mark.skipif(not _arm_toolchain_available(),
                       reason="no arm-none-eabi toolchain found; run setup.ps1"),
]


def _cmake(*args: str) -> subprocess.CompletedProcess:
    return subprocess.run(["cmake", *args], cwd=REPO,
                          capture_output=True, text=True, timeout=1800)


def _preset_names() -> list[str]:
    data = json.loads(PRESETS.read_text(encoding="utf-8"))
    return [p["name"] for p in data["configurePresets"] if not p.get("hidden")]


def _build(preset: str) -> Path:
    cfg = _cmake("--preset", preset)
    assert cfg.returncode == 0, f"configure failed for {preset}:\n{cfg.stdout}\n{cfg.stderr}"
    build = _cmake("--build", "--preset", preset)
    assert build.returncode == 0, f"build failed for {preset}:\n{build.stdout}\n{build.stderr}"
    axf = REPO / "build" / "cmake" / preset / "nxp_cup_core0.axf"
    assert axf.is_file(), f"{preset} produced no nxp_cup_core0.axf at {axf}"
    return axf


def test_competition_builds():
    """The race image. If only one thing builds, it is this one."""
    axf = _build("competition")
    assert axf.stat().st_size > 0


@pytest.mark.skipif(os.environ.get("NXPC_TEST_ALL_PRESETS") != "1",
                    reason="set NXPC_TEST_ALL_PRESETS=1 to build every preset (slow)")
def test_all_presets_build():
    failures = []
    for preset in _preset_names():
        try:
            _build(preset)
        except AssertionError as exc:  # collect all, so one failure does not hide the rest
            failures.append(f"{preset}: {exc}")
    assert not failures, "presets failed to build:\n" + "\n".join(failures)


@pytest.mark.skipif(not _have("uv"), reason="uv not on PATH; run setup.ps1")
def test_source_list_has_not_drifted():
    """The committed source list is the source of truth. If the MCUXpresso
    project changed without regenerating it, say so here rather than letting
    the difference sit unnoticed."""
    result = subprocess.run(
        ["powershell", "-NoProfile", "-ExecutionPolicy", "Bypass", "-File",
         str(REPO / "scripts/maintainer/build_cmake.ps1"), "-CheckDrift",
         "-BuildDir", str(REPO / "build" / "cmake" / "drift-check")],
        cwd=REPO, capture_output=True, text=True, timeout=1800,
    )
    assert "[DRIFT]" not in result.stdout, (
        "the committed source list differs from MCUXpresso project metadata; "
        "run .\\scripts\\maintainer\\build_cmake.ps1 -Regenerate, then review and commit\n" + result.stdout
    )
