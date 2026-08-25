"""L0_007 - standalone student learning-page packaging contracts."""

from __future__ import annotations

import importlib.util
import re
import sys
from pathlib import Path


REPO = Path(__file__).resolve().parents[2]
BUILDER_PATH = REPO / "scripts" / "tools" / "build_learning_pages.py"
OUTPUT_PATH = REPO / "docs" / "learn" / "color-spaces.html"
RGB565_OUTPUT_PATH = REPO / "docs" / "learn" / "rgb565-lookup.html"


def load_builder():
    spec = importlib.util.spec_from_file_location("build_learning_pages", BUILDER_PATH)
    assert spec is not None and spec.loader is not None
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def test_color_space_page_is_current_and_self_contained():
    builder = load_builder()
    expected = builder.render_color_spaces()

    assert OUTPUT_PATH.read_text(encoding="utf-8") == expected
    assert builder.build(check=True) == 0
    assert "Three.js 0.183.2" in expected
    assert "new THREE.WebGLRenderer" in expected
    assert "new THREE.OrbitControls" in expected
    assert "TransformControls" not in expected
    assert 'id="hsv-slice-scene"' in expected
    assert "__LESSON_" not in expected
    assert "__THREE_JS__" not in expected
    assert not re.search(
        r"<(?:script|link)\b[^>]+(?:src|href)\s*=",
        expected,
        flags=re.IGNORECASE,
    )


def test_first_color_space_page_teaches_24_bit_rgb_and_traditional_hsv():
    html = OUTPUT_PATH.read_text(encoding="utf-8")
    authored_page = (
        REPO / "docs" / "learn" / "src" / "color-spaces.template.html"
    ).read_text(encoding="utf-8")

    assert "RGB cube" in html
    assert "HSV cone" in html
    assert "24-bit RGB" in html
    assert "radius = S × V" in html
    assert "S = radius / V" in html
    assert 'min="-360" max="360"' in html
    assert "−10° and 350° point in the same direction" in html
    assert "floor(H × 256 / 360)" not in authored_page
    assert "RGB565" not in authored_page
    assert "lookup" not in authored_page.lower()
    # The complete artifact contains the general-purpose Three.js Color.setHSL
    # API, but the authored lesson must not introduce HSL/lightness terminology.
    assert "HSL" not in authored_page
    assert "lightness" not in authored_page.lower()


def test_rgb565_lookup_page_matches_firmware_table_contract():
    builder = load_builder()
    expected = builder.render_rgb565_lookup()
    authored_page = (
        REPO / "docs" / "learn" / "src" / "rgb565-lookup.template.html"
    ).read_text(encoding="utf-8")

    assert RGB565_OUTPUT_PATH.read_text(encoding="utf-8") == expected
    assert "65,536" in expected
    assert "256 KiB" in expected
    assert "0xVVSSHHYY" in expected
    assert "table[pixel]" in expected
    assert "color_features_t color" in expected
    assert "uint8_t y = color.y" in expected
    assert "hue_sector" in expected
    assert "h8&lt;&lt;8 | s8&lt;&lt;16 | v8&lt;&lt;24" in expected
    assert "Strictly, Y is luma—not physical luminance." in expected
    assert "77R + 150G + 29B + 128" in expected
    assert "new THREE" not in expected
    assert "race decisions" in authored_page
    assert not re.search(
        r"<(?:script|link)\b[^>]+(?:src|href)\s*=",
        expected,
        flags=re.IGNORECASE,
    )
