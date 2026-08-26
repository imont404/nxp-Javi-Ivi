#!/usr/bin/env python3
"""Replay and verify the AVC-to-NXP-Cup product identity refactor."""

from __future__ import annotations

import argparse
import fnmatch
import json
import re
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path


REPO = Path(__file__).resolve().parents[2]
DEFAULT_MANIFEST = Path(__file__).with_name("product_identity_refactor.json")
TEXT_PLAIN_SCRIPT = re.compile(
    r'<script\b(?=[^>]*\btype\s*=\s*(?:"text/plain"|\'text/plain\'))[^>]*>'
    r"(?P<body>.*?)</script\s*>",
    re.IGNORECASE | re.DOTALL,
)
BASE64_PAYLOAD = re.compile(r"[A-Za-z0-9+/]*={0,2}\Z")


@dataclass(frozen=True)
class ChangeSet:
    text_changes: tuple[tuple[Path, str], ...]
    moves: tuple[tuple[Path, Path], ...]


def git(*args: str) -> str:
    result = subprocess.run(
        ["git", *args], cwd=REPO, check=True, capture_output=True, text=True
    )
    return result.stdout.strip()


def load_manifest(path: Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def candidate_files() -> list[Path]:
    output = git("ls-files", "--cached", "--others", "--exclude-standard", "-z")
    return sorted(
        (REPO / item).resolve()
        for item in output.split("\0")
        if item and (REPO / item).is_file()
    )


def relative(path: Path) -> str:
    return path.relative_to(REPO).as_posix()


def is_excluded(rel: str, manifest: dict) -> bool:
    return any(rel.startswith(prefix) for prefix in manifest["excluded_prefixes"]) or any(
        fnmatch.fnmatchcase(rel, pattern) for pattern in manifest.get("excluded_globs", [])
    )


def is_ignored_generated(rel: str, manifest: dict) -> bool:
    return any(
        rel.startswith(prefix) for prefix in manifest.get("ignored_generated_prefixes", [])
    )


def transform_path(rel: str, manifest: dict) -> str:
    transformed = rel
    for rule in manifest["path_rules"]:
        transformed = transformed.replace(rule["from"], rule["to"])
    return transformed


def apply_text_rules(text: str, manifest: dict) -> str:
    transformed = text
    for rule in manifest["text_rules"]:
        transformed = transformed.replace(rule["from"], rule["to"])
    return transformed


def embedded_base64_spans(text: str) -> list[tuple[int, int]]:
    spans: list[tuple[int, int]] = []
    for match in TEXT_PLAIN_SCRIPT.finditer(text):
        body = match.group("body")
        compact = "".join(body.split())
        if (
            len(compact) >= 128
            and (len(compact) % 4) == 0
            and BASE64_PAYLOAD.fullmatch(compact)
        ):
            spans.append(match.span("body"))
    return spans


def transform_text(text: str, manifest: dict) -> str:
    """Apply identity rules without altering embedded Base64 assets."""
    transformed: list[str] = []
    cursor = 0
    for start, end in embedded_base64_spans(text):
        transformed.append(apply_text_rules(text[cursor:start], manifest))
        transformed.append(text[start:end])
        cursor = end
    transformed.append(apply_text_rules(text[cursor:], manifest))
    return "".join(transformed)


def mask_embedded_base64(text: str) -> str:
    """Hide payload bytes from stale-name scans while retaining line numbers."""
    masked: list[str] = []
    cursor = 0
    for start, end in embedded_base64_spans(text):
        masked.append(text[cursor:start])
        masked.append(re.sub(r"[^\r\n]", " ", text[start:end]))
        cursor = end
    masked.append(text[cursor:])
    return "".join(masked)


def read_text(path: Path, manifest: dict) -> str | None:
    if path.suffix.lower() in manifest["binary_suffixes"]:
        return None
    try:
        data = path.read_bytes()
        if b"\0" in data:
            return None
        return data.decode("utf-8")
    except UnicodeDecodeError:
        return None


def collect_changes(manifest: dict) -> ChangeSet:
    text_changes: list[tuple[Path, str]] = []
    moves: list[tuple[Path, Path]] = []

    for path in candidate_files():
        rel = relative(path)
        if is_ignored_generated(rel, manifest):
            continue
        if not is_excluded(rel, manifest):
            text_value = read_text(path, manifest)
            if text_value is not None:
                changed = transform_text(text_value, manifest)
                if changed != text_value:
                    text_changes.append((path, changed))

        destination_rel = transform_path(rel, manifest)
        if destination_rel != rel:
            moves.append((path, REPO / destination_rel))

    destinations: dict[Path, Path] = {}
    sources = {source.resolve() for source, _ in moves}
    for source, destination in moves:
        resolved = destination.resolve()
        if resolved in destinations:
            raise RuntimeError(
                f"path collision: {relative(source)} and {relative(destinations[resolved])} "
                f"both map to {relative(destination)}"
            )
        if destination.exists() and resolved not in sources:
            raise RuntimeError(
                f"destination already exists: {relative(source)} -> {relative(destination)}"
            )
        destinations[resolved] = source

    return ChangeSet(tuple(text_changes), tuple(moves))


def verify_apply_baseline(manifest: dict) -> None:
    expected = git("rev-parse", manifest["baseline_ref"])
    actual = git("rev-parse", "HEAD")
    if actual != expected:
        raise RuntimeError(
            f"refactor baseline mismatch: HEAD is {actual}, expected {expected} "
            f"from {manifest['baseline_ref']}"
        )
    if git("status", "--porcelain"):
        migration_in_progress = (
            (REPO / "src/embedded/nxp_cup_core0/.project").is_file()
            and not (REPO / "src/avc/avc_core0/.project").exists()
        )
        if not migration_in_progress:
            raise RuntimeError("refactor apply requires a clean working tree")


def remove_empty_parents(path: Path) -> None:
    parent = path.parent
    while parent != REPO:
        try:
            parent.rmdir()
        except OSError:
            break
        parent = parent.parent


def apply_changes(changes: ChangeSet) -> None:
    for path, text_value in changes.text_changes:
        path.write_text(text_value, encoding="utf-8", newline="")

    for source, destination in sorted(
        changes.moves, key=lambda pair: len(pair[0].parts), reverse=True
    ):
        destination.parent.mkdir(parents=True, exist_ok=True)
        source.rename(destination)
        remove_empty_parents(source)


def stale_findings(manifest: dict) -> list[str]:
    findings: list[str] = []
    path_patterns = [re.compile(pattern, re.IGNORECASE) for pattern in manifest["stale_path_patterns"]]
    text_patterns = [re.compile(pattern) for pattern in manifest["stale_text_patterns"]]
    allowed_text_patterns = [
        re.compile(pattern) for pattern in manifest.get("allowed_stale_text_patterns", [])
    ]

    for path in candidate_files():
        rel = relative(path)
        if is_ignored_generated(rel, manifest):
            continue
        if is_excluded(rel, manifest):
            continue
        for pattern in path_patterns:
            if pattern.search(rel):
                findings.append(f"path: {rel}")
                break

        text_value = read_text(path, manifest)
        if text_value is None:
            continue
        scan_text = mask_embedded_base64(text_value)
        for line_number, line in enumerate(scan_text.splitlines(), start=1):
            if any(pattern.search(line) for pattern in allowed_text_patterns):
                continue
            if any(pattern.search(line) for pattern in text_patterns):
                findings.append(f"text: {rel}:{line_number}: {line.strip()}")
    return findings


def print_changes(changes: ChangeSet) -> None:
    for path, _ in changes.text_changes:
        print(f"TEXT {relative(path)}")
    for source, destination in changes.moves:
        print(f"MOVE {relative(source)} -> {relative(destination)}")
    print(f"{len(changes.text_changes)} text file(s), {len(changes.moves)} path move(s)")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--manifest", type=Path, default=DEFAULT_MANIFEST)
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("--dry-run", action="store_true")
    group.add_argument("--apply", action="store_true")
    group.add_argument("--check", action="store_true")
    args = parser.parse_args()
    manifest = load_manifest(args.manifest.resolve())

    if args.check:
        findings = stale_findings(manifest)
        if findings:
            print("\n".join(findings))
            print(f"{len(findings)} stale product-identity occurrence(s)", file=sys.stderr)
            return 1
        print("No stale owned AVC identifiers or paths found.")
        return 0

    changes = collect_changes(manifest)
    print_changes(changes)
    if args.dry_run or not (changes.text_changes or changes.moves):
        return 0

    verify_apply_baseline(manifest)
    apply_changes(changes)
    print("Refactor applied. Run --dry-run and --check before committing.")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, RuntimeError, subprocess.CalledProcessError) as exc:
        print(f"error: {exc}", file=sys.stderr)
        raise SystemExit(2)
