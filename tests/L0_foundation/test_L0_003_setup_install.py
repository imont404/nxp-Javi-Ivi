"""L0_003 - setup installs only a fully verified pinned host runtime."""

import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import zipfile

import pytest


REPO = Path(__file__).resolve().parents[2]
SETUP = REPO / "setup.ps1"
PAYLOAD_NAMES = [
    "Dear-ImGui-LICENSE.txt",
    "HOST-README.md",
    "nxpc_tool.exe",
    "nxpc_viewer.exe",
    "rblhost.exe",
    "rblhost-LICENSE.txt",
    "SDL2.dll",
    "SDL2-LICENSE.txt",
]
FIXTURE_COMMIT = "f" * 40

pytestmark = pytest.mark.skipif(
    sys.platform != "win32", reason="setup.ps1 is a Windows bootstrap"
)


def _sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def _write_pins(root: Path, archive: Path) -> None:
    pins = {
        "schemaVersion": 1,
        "armGnu": {
            "releaseVersion": "test",
            "compilerVersion": "test",
            "compilerSha256": "0" * 64,
            "archiveName": "unused.zip",
            "directoryName": "unused",
            "url": "https://invalid.example/unused.zip",
            "sha256": "0" * 64,
        },
        "coreTools": {
            "releaseVersion": "1.0.0-test",
            "releaseTag": "core-tools-v1.0.0-test",
            "sourceCommit": FIXTURE_COMMIT,
            "assetName": archive.name,
            "url": "https://invalid.example/core-tools.zip",
            "sha256": _sha256(archive),
            "selfTestArguments": ["/c", "exit", "0"],
            "payloadFiles": PAYLOAD_NAMES,
        },
    }
    (root / "setup.versions.json").write_text(
        json.dumps(pins, indent=2) + "\n", encoding="utf-8"
    )


def _make_archive(root: Path, name: str, *, bad_inner_hash: bool = False) -> Path:
    payload = root / f"payload-{name}"
    payload.mkdir()
    command_interpreter = Path(os.environ["SystemRoot"]) / "System32" / "cmd.exe"
    for filename in PAYLOAD_NAMES:
        destination = payload / filename
        if filename.endswith(".exe"):
            shutil.copy2(command_interpreter, destination)
        else:
            destination.write_text(f"fixture {filename}\n", encoding="utf-8")

    files = []
    for filename in sorted(PAYLOAD_NAMES):
        path = payload / filename
        files.append(
            {"name": filename, "size": path.stat().st_size, "sha256": _sha256(path)}
        )
    if bad_inner_hash:
        files[0]["sha256"] = "0" * 64

    manifest = {
        "schemaVersion": 1,
        "releaseVersion": "1.0.0-test",
        "sourceCommit": FIXTURE_COMMIT,
        "sourceDirty": False,
        "platform": "windows",
        "architecture": "x64",
        "files": files,
    }
    (payload / "manifest.json").write_text(
        json.dumps(manifest, indent=2) + "\n", encoding="utf-8"
    )

    archive = root / f"{name}.zip"
    with zipfile.ZipFile(archive, "w", compression=zipfile.ZIP_DEFLATED) as bundle:
        for path in sorted(payload.iterdir()):
            bundle.write(path, path.name)
    return archive


def _prepare_repo(tmp_path: Path, archive: Path) -> Path:
    root = tmp_path / "repo"
    root.mkdir()
    shutil.copy2(SETUP, root / "setup.ps1")
    _write_pins(root, archive)
    return root


def _run_setup(root: Path, archive: Path | None, *, force: bool = False):
    command = [
        "powershell.exe",
        "-NoProfile",
        "-ExecutionPolicy",
        "Bypass",
        "-File",
        str(root / "setup.ps1"),
        "-SkipArm",
        "-SkipCMake",
        "-SkipNinja",
    ]
    if archive is not None:
        command.extend(["-CoreToolsArchive", str(archive)])
    if force:
        command.append("-Force")
    return subprocess.run(command, cwd=root, text=True, capture_output=True, check=False)


def test_valid_local_archive_installs_and_rerun_is_idempotent(tmp_path):
    archive = _make_archive(tmp_path, "valid")
    root = _prepare_repo(tmp_path, archive)

    first = _run_setup(root, archive)
    assert first.returncode == 0, first.stdout + first.stderr
    installed = root / "out" / "artifacts" / "host"
    assert (installed / "manifest.json").is_file()

    second = _run_setup(root, archive)
    assert second.returncode == 0, second.stdout + second.stderr
    assert "[SKIP] Verified core tools 1.0.0-test" in second.stdout


def test_preseeded_verified_cache_installs_without_network(tmp_path):
    archive = _make_archive(tmp_path, "cached")
    root = _prepare_repo(tmp_path, archive)
    downloads = root / "out" / "downloads"
    downloads.mkdir(parents=True)
    shutil.copy2(archive, downloads / archive.name)

    installed = _run_setup(root, None)

    assert installed.returncode == 0, installed.stdout + installed.stderr
    assert "[CACHE] Verified" in installed.stdout
    assert (root / "out" / "artifacts" / "host" / "manifest.json").is_file()


def test_outer_hash_mismatch_is_rejected_without_replacing_runtime(tmp_path):
    valid = _make_archive(tmp_path, "valid")
    root = _prepare_repo(tmp_path, valid)
    assert _run_setup(root, valid).returncode == 0
    manifest = root / "out" / "artifacts" / "host" / "manifest.json"
    original = manifest.read_bytes()

    corrupt = tmp_path / "corrupt.zip"
    corrupt.write_bytes(b"not the pinned archive")
    failed = _run_setup(root, corrupt, force=True)

    assert failed.returncode != 0
    assert "archive SHA-256 mismatch" in failed.stdout + failed.stderr
    assert manifest.read_bytes() == original


def test_inner_hash_mismatch_is_rejected_and_known_good_is_restored(tmp_path):
    valid = _make_archive(tmp_path, "valid")
    root = _prepare_repo(tmp_path, valid)
    assert _run_setup(root, valid).returncode == 0
    manifest = root / "out" / "artifacts" / "host" / "manifest.json"
    original = manifest.read_bytes()

    bad_inner = _make_archive(tmp_path, "bad-inner", bad_inner_hash=True)
    _write_pins(root, bad_inner)
    failed = _run_setup(root, bad_inner, force=True)

    assert failed.returncode != 0
    assert "does not match manifest.json" in failed.stdout + failed.stderr
    assert manifest.read_bytes() == original
    assert not list((root / "out" / "staging").glob("core-tools-*"))
