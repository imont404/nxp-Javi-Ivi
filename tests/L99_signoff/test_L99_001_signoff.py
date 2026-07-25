"""L99_001 - the release gate.

One command that answers: is this tree fit to hand to students?

Deliberately thin. The real checking lives in L0, L1, and L2; this asserts the
things that must hold at the moment of handing over, and that the governance
tooling itself is satisfied.
"""

import json
import shutil
import subprocess
from pathlib import Path

import pytest

REPO = Path(__file__).resolve().parents[2]
DEV_STD = "wn-dev-std==2026.7.18"


def _have(tool: str) -> bool:
    return shutil.which(tool) is not None


def test_governance_audit_passes():
    """dev-std owns plan hygiene. If it is unhappy, the tree is not ready."""
    if not _have("uvx"):
        pytest.skip("uvx not on PATH; run setup.ps1")
    result = subprocess.run(
        ["uvx", "--from", DEV_STD, "dev-std", "audit", ".", "--scope", "docs.plans"],
        cwd=REPO, capture_output=True, text=True, timeout=900,
    )
    combined = result.stdout + result.stderr
    assert "[FAIL]" not in combined, f"governance audit failed:\n{combined}"
    assert "[PASS]" in combined, f"governance audit produced no result:\n{combined}"


def test_competition_image_is_buildable():
    """The competition preset must exist and be the documented default. Whether
    it currently compiles is L1's job; this asserts it is still the thing we
    would hand over."""
    presets = REPO / "CMakePresets.json"
    assert presets.is_file(), "CMakePresets.json is missing"
    data = json.loads(presets.read_text(encoding="utf-8"))
    competition = next(
        (p for p in data["configurePresets"] if p["name"] == "competition"), None
    )
    assert competition is not None, "the competition preset has been removed"
    display = competition.get("displayName", "")
    assert "START HERE" in display, (
        "the competition preset is no longer marked as the starting point; "
        "students need one obvious default"
    )


def test_student_entry_points_exist():
    """A student arriving at this repository must find a way in."""
    for path, why in (
        (REPO / "setup.ps1", "the one-script install"),
        (REPO / "docs/setup.html", "the page students read first"),
        (REPO / "README.md", "the repository front door"),
    ):
        assert path.is_file(), f"{path.name} is missing - {why}"

    readme = (REPO / "README.md").read_text(encoding="utf-8")
    assert "setup.ps1" in readme, "README does not point at the install script"
    assert "docs/setup.html" in readme, "README does not point at the setup page"


def test_competition_defaults_unchanged():
    """The Rev A competition configuration is what runs on race day. Any change
    to these defaults should be deliberate enough to update this test."""
    config = (REPO / "src/avc/avc_core0/source/avc_config/avc__master_config.h").read_text(
        encoding="utf-8"
    )
    expected = {
        "CONFIG__CAMERA_CAPTURE_BACKEND": "CAMERA_CAPTURE_BACKEND_SMARTDMA_EZH",
        "CONFIG__DISPLAY_PANEL": "DISPLAY_PANEL_ER_TFT020_3",
        "CONFIG__USB_DEBUG_STREAM_ENABLE": "(0)",
        "CONFIG__MOTOR_ENCODER_BACKEND": "MOTOR_ENCODER_BACKEND_DISABLED",
    }
    for knob, value in expected.items():
        # Match the #define that follows the #ifndef guard for this knob.
        idx = config.find(f"#define {knob}")
        assert idx >= 0, f"{knob} is no longer defined"
        line = config[idx:config.find("\n", idx)]
        assert value in line, (
            f"competition default changed: {knob} is '{line.strip()}', expected {value}"
        )
