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


def _preset_names() -> set[str]:
    data = json.loads(PRESETS.read_text(encoding="utf-8"))
    return {p["name"] for p in data["configurePresets"] if not p.get("hidden")}


def test_setup_script_exists():
    assert SETUP_SCRIPT.is_file(), "setup.ps1 is missing"
    text = SETUP_SCRIPT.read_text(encoding="utf-8")
    # Everything the documentation says it provisions.
    for tool in ("arm-gnu-toolchain", "Kitware.CMake", "Ninja-build.Ninja",
                 "astral-sh.uv", "LLVM.LLVM"):
        assert tool in text, f"setup.ps1 does not provision {tool}"


def test_setup_script_does_not_persist_environment():
    """The script promises it changes no environment state. Enforce it, because
    a student's machine is not ours to modify."""
    text = SETUP_SCRIPT.read_text(encoding="utf-8")
    assert "setx" not in text.lower(), "setup.ps1 must not persist environment variables"
    assert "[Environment]::SetEnvironmentVariable" not in text, (
        "setup.ps1 must not persist environment variables"
    )


def test_docs_reference_real_presets():
    """Every preset named in the setup page must exist."""
    assert SETUP_DOC.is_file(), "docs/setup.html is missing"
    html = SETUP_DOC.read_text(encoding="utf-8")
    known = _preset_names()
    # Preset names appear in the page as <code>name</code> table entries.
    quoted = set(re.findall(r"<code>([a-z0-9][a-z0-9\-]{3,})</code>", html))
    referenced = {q for q in quoted if q in known or q.startswith(("flexio-", "encoder-diag"))}
    assert referenced, "the setup page names no presets at all"
    unknown = referenced - known
    assert not unknown, f"setup page references presets that do not exist: {sorted(unknown)}"


def test_docs_commands_are_current():
    """The page must document the preset workflow, not a superseded one."""
    html = SETUP_DOC.read_text(encoding="utf-8")
    assert "cmake --preset competition" in html, "setup page does not document the preset workflow"
    assert "setup.ps1" in html, "setup page does not mention the provisioning script"


def test_docs_do_not_teach_rtt():
    """Students debug through the LCD and the USB frame stream. They are not
    given a J-Link, so an RTT command on the setup page is an instruction they
    cannot follow.

    This is worth a test rather than a convention: RTT is the easiest way for a
    maintainer to watch a running board, so it drifts into student-facing docs
    without anyone deciding it should.
    """
    html = SETUP_DOC.read_text(encoding="utf-8")
    offenders = [
        line.strip()
        for line in html.splitlines()
        if re.search(r"(?i)rtt\.ps1|JLinkRTT|_SEGGER_RTT", line)
    ]
    assert not offenders, (
        "docs/setup.html tells students to run RTT, which needs a debugger they "
        f"do not have: {offenders}"
    )


def test_docs_point_at_the_debug_tools_students_have():
    """The flip side of the above: having removed RTT, the page must still say
    how to see what the car is doing."""
    html = SETUP_DOC.read_text(encoding="utf-8")
    assert re.search(r"(?i)\bLCD\b", html), (
        "setup page does not mention the on-board LCD"
    )
    assert re.search(r"(?i)USB", html), (
        "setup page does not mention the USB frame stream"
    )


def test_docs_warn_about_motors():
    """encoder-diag-motors spins the wheels four seconds after reset, without
    being asked. A page that omits that is a page that breaks a car."""
    html = SETUP_DOC.read_text(encoding="utf-8")
    assert "on blocks" in html, "setup page does not warn that the motors preset drives the wheels"


def test_toolchain_discovery_prefers_local():
    """MCUXpresso must be a fallback, not the first choice, or the one-script
    install is decorative."""
    toolchain = REPO / "src/avc/avc_core0/cmake/mcuxpresso-toolchain.cmake"
    assert toolchain.is_file(), "toolchain file is missing"
    text = toolchain.read_text(encoding="utf-8")
    assert "AVC_ARM_TOOLCHAIN_DIR" in text, "toolchain file has no override hook"
    assert "out/toolchains" in text, "toolchain file does not look for the provisioned toolchain"
    local_at = text.index("out/toolchains")
    mcux_at = text.index("MCUXpressoIDE")
    assert local_at < mcux_at, (
        "MCUXpresso is searched before the locally provisioned toolchain"
    )
