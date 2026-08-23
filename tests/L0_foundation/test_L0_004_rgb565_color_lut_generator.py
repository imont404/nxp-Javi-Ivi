"""L0_004 - RGB565 YHSV LUT generation is exhaustive and reproducible."""

from __future__ import annotations

import importlib.util
import sys
from pathlib import Path


REPO = Path(__file__).resolve().parents[2]
GENERATOR_PATH = REPO / "scripts/tools/generate_rgb565_color_lut.py"


def load_generator():
    spec = importlib.util.spec_from_file_location("generate_rgb565_color_lut", GENERATOR_PATH)
    assert spec is not None and spec.loader is not None
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def test_primary_color_features():
    generator = load_generator()

    assert generator.color_features_for_rgb565(0x0000) == generator.ColorFeatures(0, 0, 0, 0)
    assert generator.color_features_for_rgb565(0xFFFF) == generator.ColorFeatures(255, 0, 0, 255)
    assert generator.color_features_for_rgb565(0xF800) == generator.ColorFeatures(77, 0, 255, 255)
    assert generator.color_features_for_rgb565(0x07E0) == generator.ColorFeatures(149, 85, 255, 255)
    assert generator.color_features_for_rgb565(0x001F) == generator.ColorFeatures(29, 170, 255, 255)


def test_table_exhaustively_validates():
    generator = load_generator()
    table = generator.generate_table()

    assert len(table) == 65536
    generator.validate_table(table)
    assert generator.unpack_entry(table[0xF800]) == generator.ColorFeatures(77, 0, 255, 255)


def test_committed_table_is_current():
    generator = load_generator()
    expected = generator.render_include(generator.generate_table())

    assert generator.DEFAULT_OUTPUT.is_file(), "committed RGB565 YHSV table is missing"
    assert generator.DEFAULT_OUTPUT.read_text(encoding="utf-8") == expected, (
        "committed RGB565 YHSV table is stale; run "
        "uv run python scripts/tools/generate_rgb565_color_lut.py"
    )


def test_generated_include_is_reproducible_and_checkable(tmp_path):
    generator = load_generator()
    output = tmp_path / "nxpc_rgb565_yhsv_lut.inc"
    arguments = ["--output", str(output)]

    assert generator.main(arguments) == 0
    first_output = output.read_bytes()
    assert generator.main(arguments) == 0
    assert output.read_bytes() == first_output
    assert generator.main([*arguments, "--check"]) == 0

    output.write_text("stale\n", encoding="utf-8")
    assert generator.main([*arguments, "--check"]) == 1
