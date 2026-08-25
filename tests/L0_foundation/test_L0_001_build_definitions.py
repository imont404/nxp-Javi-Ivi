"""L0_001 - the build definitions are present and self-contained.

These assert the invariants that let a clean machine build without MCUXpresso.
Each one corresponds to something that was actually broken and got fixed; they
exist so it cannot silently come back.
"""

import json
import re
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]

PRESETS = REPO / "CMakePresets.json"
SOURCE_LIST = REPO / "src/nxp_cup/nxp_cup_core0/cmake/mcuxpresso_debug.cmake"
LINK_DIR = REPO / "src/nxp_cup/nxp_cup_core0/link"
VENDORED_HEADER = REPO / "src/nxp_cup/nxp_cup_core0/source/shared/cr_section_macros.h"


def test_presets_exist():
    assert PRESETS.is_file(), "CMakePresets.json is missing"
    data = json.loads(PRESETS.read_text(encoding="utf-8"))
    visible = [p["name"] for p in data["configurePresets"] if not p.get("hidden")]
    assert visible == ["competition"], f"expected one happy-path preset; found {visible}"
    build_names = [p["name"] for p in data["buildPresets"]]
    assert build_names == ["competition"], f"unexpected build presets: {build_names}"


def test_source_list_is_self_contained():
    """The source list must not hard-code an MCUXpresso install path.

    It is committed and used as-is so a normal build needs neither Python nor
    the IDE; a path into C:/nxp would silently reintroduce the dependency.
    """
    assert SOURCE_LIST.is_file(), "committed source list is missing"
    text = SOURCE_LIST.read_text(encoding="utf-8")
    offenders = [
        line.strip()
        for line in text.splitlines()
        if not line.lstrip().startswith("#")
        and re.search(r"(?i)(c:/nxp|mcuxpressoide_)", line)
    ]
    assert not offenders, f"source list references an MCUXpresso install path: {offenders}"


def test_vendored_header_present():
    """cr_section_macros.h ships only with the IDE, but the firmware needs it
    for memory placement - it is what puts the camera frame buffers in the
    right bank. It must live in the repository."""
    assert VENDORED_HEADER.is_file(), (
        "cr_section_macros.h is not vendored; the build would fall back to the "
        "MCUXpresso include path"
    )
    text = VENDORED_HEADER.read_text(encoding="utf-8")
    assert "__BSS" in text, "vendored header does not define the placement macros"


def test_no_ide_only_library():
    """libcr_newlib_nohost.a ships only with MCUXpresso and breaks the
    standalone toolchain. It was replaced with libnosys.a."""
    scripts = list(LINK_DIR.glob("*.ld"))
    assert scripts, f"no linker scripts found in {LINK_DIR}"
    for script in scripts:
        text = script.read_text(encoding="utf-8")
        # The substitution is explained in a comment, so only flag real GROUP entries.
        entries = re.findall(r'^\s*"([^"]+\.a)"', text, re.MULTILINE)
        assert "libcr_newlib_nohost.a" not in entries, (
            f"{script.name} still links the IDE-only libcr_newlib_nohost.a"
        )


def test_linker_capture_documented():
    """The edited linker script must say it is a capture and why it differs,
    so a drift diff against a regenerated script is expected rather than
    alarming."""
    library_ld = LINK_DIR / "nxp_cup_core0_Debug_library.ld"
    assert library_ld.is_file(), "library linker script missing"
    text = library_ld.read_text(encoding="utf-8")
    assert "STATIC CAPTURE" in text, "linker script does not identify itself as a capture"
    assert "libnosys" in text, "linker script does not explain the library substitution"


def test_bounded_sbrk_present():
    """Swapping to libnosys removed the heap bound NXP's library enforced, so
    the firmware provides its own _sbrk. Without it the heap grows unchecked."""
    sbrk = REPO / "src/nxp_cup/nxp_cup_core0/source/shared/nxpc__sbrk.c"
    assert sbrk.is_file(), "bounded _sbrk is missing; the heap would be unbounded"
    text = sbrk.read_text(encoding="utf-8")
    assert "_pvHeapLimit" in text, "_sbrk does not honour the linker-defined heap limit"
