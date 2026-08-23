"""L2_001 - complexity measured against a tracked baseline.

Existing debt does not fail the build; growth does. The baseline records what
was true on 2026-07-25 so a change that makes a function meaningfully worse is
visible in review rather than discovered later.
"""

import re
import shutil
import subprocess
from pathlib import Path

import pytest

REPO = Path(__file__).resolve().parents[2]
SOURCE = REPO / "src/nxp_cup/nxp_cup_core0/source"
BASELINE = REPO / "docs/quality/lizard-baseline.txt"

CCN_THRESHOLD = 15

# Lizard's warning line, e.g.
#   path/file.c:429: warning: func has 105 NLOC, 17 CCN, 440 token, ...
WARNING = re.compile(
    r"^(?P<path>[^:]+):(?P<line>\d+): warning: (?P<func>\S+) has "
    r"(?P<nloc>\d+) NLOC, (?P<ccn>\d+) CCN"
)


def _have(tool: str) -> bool:
    return shutil.which(tool) is not None


pytestmark = pytest.mark.skipif(not _have("uv"), reason="uv not on PATH; run setup.ps1")


def _parse(text: str) -> dict[str, int]:
    """Map function name -> worst CCN seen. Keyed by name rather than
    file:line so the baseline survives unrelated edits shifting line numbers."""
    out: dict[str, int] = {}
    for line in text.splitlines():
        m = WARNING.match(line.strip())
        if m:
            name = m.group("func")
            ccn = int(m.group("ccn"))
            out[name] = max(out.get(name, 0), ccn)
    return out


def _run_lizard() -> str:
    result = subprocess.run(
        ["uv", "run", "lizard", str(SOURCE), "--CCN", str(CCN_THRESHOLD), "-w"],
        cwd=REPO, capture_output=True, text=True, timeout=600,
    )
    # lizard exits non-zero when it emits warnings; that is expected here.
    return result.stdout + result.stderr


def test_baseline_exists():
    assert BASELINE.is_file(), (
        f"complexity baseline missing at {BASELINE}; regenerate with "
        f"'uv run lizard {SOURCE} --CCN {CCN_THRESHOLD} -w'"
    )


def test_complexity_against_baseline():
    baseline = _parse(BASELINE.read_text(encoding="utf-8"))
    assert baseline, "baseline file contains no lizard warnings to compare against"

    current = _parse(_run_lizard())

    regressions = []
    for name, ccn in sorted(current.items()):
        was = baseline.get(name)
        if was is None:
            regressions.append(
                f"  {name}: CCN {ccn} - newly over the threshold of {CCN_THRESHOLD}"
            )
        elif ccn > was:
            regressions.append(f"  {name}: CCN {was} -> {ccn}")

    assert not regressions, (
        "complexity grew beyond the recorded baseline:\n"
        + "\n".join(regressions)
        + "\n\nEither simplify, or accept it deliberately by regenerating "
          "docs/quality/lizard-baseline.txt and explaining why in the commit."
    )


def test_baseline_is_not_stale():
    """If functions in the baseline no longer exist or have been simplified
    below the threshold, the baseline is carrying dead weight. Not a failure,
    but worth surfacing so it gets tightened."""
    baseline = _parse(BASELINE.read_text(encoding="utf-8"))
    current = _parse(_run_lizard())
    resolved = sorted(set(baseline) - set(current))
    if resolved:
        pytest.skip(
            "baseline lists functions that are no longer over the threshold "
            f"(consider regenerating): {resolved}"
        )
