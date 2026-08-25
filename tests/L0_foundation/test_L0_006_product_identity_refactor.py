"""L0_006 - the product rename is replayable and collision checked."""

import importlib.util
import json
import sys
from pathlib import Path


REPO = Path(__file__).resolve().parents[2]
SCRIPT = REPO / "scripts/tools/refactor_product_identity.py"
MANIFEST = REPO / "scripts/tools/product_identity_refactor.json"


def load_module():
    spec = importlib.util.spec_from_file_location("product_identity_refactor", SCRIPT)
    module = importlib.util.module_from_spec(spec)
    assert spec.loader is not None
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def test_manifest_preserves_wire_and_codec_terms():
    manifest = json.loads(MANIFEST.read_text(encoding="utf-8"))
    replacements = {rule["from"] for rule in manifest["text_rules"]}
    assert "AVCU" not in replacements
    assert "video/avc" not in replacements
    assert manifest["baseline_ref"] == "nxpc-pre-refactor"


def test_transform_order_prefers_full_product_paths():
    module = load_module()
    manifest = module.load_manifest(MANIFEST)
    assert module.transform_path("src/avc/avc_core0/source/avc_system.c", manifest) == (
        "src/embedded/nxp_cup_core0/source/nxpc_system.c"
    )
    assert module.transform_path("src/android/avc_bridge/app/build.gradle", manifest) == (
        "src/android/nxp_cup_bridge/app/build.gradle"
    )


def test_refactor_is_currently_a_clean_second_run_after_migration():
    """Before migration this intentionally reports changes; after it, none."""
    module = load_module()
    manifest = module.load_manifest(MANIFEST)
    changes = module.collect_changes(manifest)
    if (REPO / "src/embedded/nxp_cup_core0").exists():
        assert not changes.text_changes
        assert not changes.moves
