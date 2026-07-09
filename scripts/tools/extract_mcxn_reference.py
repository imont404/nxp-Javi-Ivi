#!/usr/bin/env python3
"""Extract selected chapters from the MCXNx4x reference manual PDF."""

from __future__ import annotations

import argparse
import json
import re
import shutil
from dataclasses import asdict, dataclass
from pathlib import Path

try:
    import fitz
except ImportError:
    print("Error: PyMuPDF is not installed. Run: uv sync")
    raise SystemExit(1)


REPO_ROOT = Path(__file__).resolve().parents[2]
DEFAULT_SOURCE = REPO_ROOT / "docs" / "mcxn947" / "MCXNx4xRM.pdf"
DEFAULT_OUTPUT = REPO_ROOT / "docs" / "research" / "MCXN947" / "extracted" / "MCXNx4xRM"
ALLOWED_CLEAN_ROOT = REPO_ROOT / "docs" / "research" / "MCXN947" / "extracted"

DEFAULT_CHAPTERS = (
    2,   # Introduction: block diagram, system bus priority/arbitration
    4,   # Memory
    12,  # Signal multiplexing
    17,  # System Controller (SYSCON)
    19,  # SmartDMA Controller
    23,  # eDMA Controller
    26,  # INPUTMUX
    33,  # Clocking
    34,  # SCG
    71,  # FlexIO
    73,  # GPIO
    74,  # PINT
    75,  # PORT
)

DEFAULT_RENDER_TERMS = (
    "block diagram",
    "timing diagram",
    "pinout",
    "pin function",
    "signal multiplex",
    "dma request",
    "request source",
    "trigger input",
    "trigger output",
    "clock control",
    "shifter",
    "timer",
)


@dataclass(frozen=True)
class Chapter:
    number: int
    title: str
    toc_level: int
    start_page: int
    end_page: int

    @property
    def directory_name(self) -> str:
        return f"{self.number:02d}_{slugify(self.title)}"


@dataclass(frozen=True)
class PageText:
    number: int
    text: str


def slugify(value: str) -> str:
    slug = re.sub(r"[^A-Za-z0-9]+", "-", value).strip("-").lower()
    return slug[:80].strip("-") or "chapter"


def repo_relative(path: Path) -> str:
    try:
        return path.resolve().relative_to(REPO_ROOT).as_posix()
    except ValueError:
        return str(path.resolve())


def parse_csv_ints(value: str) -> set[int]:
    result: set[int] = set()
    for part in re.split(r"[\s,]+", value.strip()):
        if not part:
            continue
        try:
            result.add(int(part))
        except ValueError as exc:
            raise argparse.ArgumentTypeError(f"invalid chapter number: {part}") from exc
    return result


def parse_csv_strings(value: str) -> tuple[str, ...]:
    return tuple(part.strip() for part in value.split(",") if part.strip())


def chapter_entries_from_toc(toc: list[list[int | str]], page_count: int) -> list[Chapter]:
    candidates: list[tuple[int, str, int, int]] = []
    chapter_title = re.compile(r"^(\d+)\s+(.+)$")

    for entry in toc:
        level = int(entry[0])
        title = normalize_title(str(entry[1]))
        page = int(entry[2])
        match = chapter_title.match(title)
        if not match or page < 1:
            continue
        candidates.append((int(match.group(1)), match.group(2), level, page))

    candidates.sort(key=lambda item: item[3])
    chapters: list[Chapter] = []
    for index, (number, title, level, start_page) in enumerate(candidates):
        if index + 1 < len(candidates):
            end_page = candidates[index + 1][3] - 1
        else:
            end_page = page_count
        chapters.append(
            Chapter(
                number=number,
                title=title,
                toc_level=level,
                start_page=start_page,
                end_page=max(start_page, end_page),
            )
        )
    return chapters


def normalize_title(value: str) -> str:
    return re.sub(r"\s+", " ", value).strip()


def select_chapters(
    chapters: list[Chapter],
    selected_numbers: set[int],
    keywords: tuple[str, ...],
    extract_all: bool,
) -> tuple[list[Chapter], dict[int, list[str]]]:
    matches: dict[int, list[str]] = {}
    selected: list[Chapter] = []
    lowered_keywords = tuple(keyword.lower() for keyword in keywords)

    for chapter in chapters:
        reasons: list[str] = []
        if extract_all:
            reasons.append("all")
        if chapter.number in selected_numbers:
            reasons.append("chapter")
        title_lower = chapter.title.lower()
        for keyword in lowered_keywords:
            if keyword and keyword in title_lower:
                reasons.append(f"keyword:{keyword}")
        if reasons:
            selected.append(chapter)
            matches[chapter.number] = reasons

    return selected, matches


def extract_text(doc: fitz.Document, chapter: Chapter) -> list[PageText]:
    pages: list[PageText] = []
    for page_number in range(chapter.start_page, chapter.end_page + 1):
        page = doc.load_page(page_number - 1)
        text = page.get_text("text", sort=True).rstrip()
        pages.append(PageText(number=page_number, text=text))
    return pages


def write_text(chapter_dir: Path, source_pdf: Path, chapter: Chapter, pages: list[PageText]) -> None:
    text_file = chapter_dir / "text.md"
    lines = [
        f"# {chapter.number} {chapter.title}",
        "",
        f"Source: `{repo_relative(source_pdf)}`",
        f"Pages: {chapter.start_page}-{chapter.end_page}",
        "",
    ]
    for page in pages:
        lines.extend(
            [
                f"<!-- page {page.number} -->",
                "",
                page.text,
                "",
            ]
        )
    text_file.write_text("\n".join(lines).rstrip() + "\n", encoding="utf-8")


def extract_embedded_images(
    doc: fitz.Document,
    chapter: Chapter,
    chapter_dir: Path,
    min_dimension: int,
    max_images: int,
) -> list[dict[str, object]]:
    if max_images <= 0:
        return []

    image_dir = chapter_dir / "embedded_images"
    records: list[dict[str, object]] = []
    seen_xrefs: set[int] = set()

    for page_number in range(chapter.start_page, chapter.end_page + 1):
        page = doc.load_page(page_number - 1)
        for page_image_index, image_info in enumerate(page.get_images(full=True), start=1):
            xref = int(image_info[0])
            width = int(image_info[2])
            height = int(image_info[3])
            if xref in seen_xrefs or width < min_dimension or height < min_dimension:
                continue
            seen_xrefs.add(xref)

            image = doc.extract_image(xref)
            extension = image.get("ext", "bin")
            image_dir.mkdir(parents=True, exist_ok=True)
            filename = f"page_{page_number:04d}_image_{page_image_index:02d}_xref_{xref}.{extension}"
            path = image_dir / filename
            path.write_bytes(image["image"])
            records.append(
                {
                    "page": page_number,
                    "file": repo_relative(path),
                    "width": width,
                    "height": height,
                    "xref": xref,
                }
            )
            if len(records) >= max_images:
                return records

    return records


def render_key_pages(
    doc: fitz.Document,
    chapter: Chapter,
    chapter_dir: Path,
    pages: list[PageText],
    terms: tuple[str, ...],
    max_pages: int,
    dpi: int,
) -> list[dict[str, object]]:
    if max_pages <= 0:
        return []

    lowered_terms = tuple(term.lower() for term in terms if term.strip())
    selected_pages: list[tuple[PageText, str]] = []
    for page in pages:
        lower_text = page.text.lower()
        match = next((term for term in lowered_terms if term in lower_text), "")
        if match:
            selected_pages.append((page, match))
        if len(selected_pages) >= max_pages:
            break

    rendered_dir = chapter_dir / "rendered_pages"
    records: list[dict[str, object]] = []
    zoom = dpi / 72.0
    matrix = fitz.Matrix(zoom, zoom)

    for page_text, match in selected_pages:
        rendered_dir.mkdir(parents=True, exist_ok=True)
        page = doc.load_page(page_text.number - 1)
        pixmap = page.get_pixmap(matrix=matrix, alpha=False)
        filename = f"page_{page_text.number:04d}.png"
        path = rendered_dir / filename
        pixmap.save(str(path))
        records.append(
            {
                "page": page_text.number,
                "file": repo_relative(path),
                "matched_term": match,
                "dpi": dpi,
            }
        )

    return records


def write_metadata(
    chapter_dir: Path,
    chapter: Chapter,
    match_reasons: list[str],
    embedded_images: list[dict[str, object]],
    rendered_pages: list[dict[str, object]],
) -> dict[str, object]:
    metadata = {
        **asdict(chapter),
        "directory": repo_relative(chapter_dir),
        "match_reasons": match_reasons,
        "embedded_images": embedded_images,
        "rendered_pages": rendered_pages,
    }
    (chapter_dir / "metadata.json").write_text(
        json.dumps(metadata, indent=2, sort_keys=True) + "\n",
        encoding="utf-8",
    )
    return metadata


def write_index(
    output_dir: Path,
    source_pdf: Path,
    doc: fitz.Document,
    chapters: list[Chapter],
    selected_metadata: list[dict[str, object]],
    args: argparse.Namespace,
) -> None:
    index = {
        "source_pdf": repo_relative(source_pdf),
        "page_count": doc.page_count,
        "default_chapters": list(DEFAULT_CHAPTERS),
        "all_chapters": [asdict(chapter) for chapter in chapters],
        "selected_chapters": selected_metadata,
        "options": {
            "chapters": sorted(args.chapters),
            "keywords": list(args.keywords),
            "all": args.all,
            "render_terms": list(args.render_terms),
            "max_rendered_pages": args.max_rendered_pages,
            "render_dpi": args.render_dpi,
            "max_embedded_images": args.max_embedded_images,
            "min_image_dimension": args.min_image_dimension,
        },
    }
    (output_dir / "index.json").write_text(
        json.dumps(index, indent=2, sort_keys=True) + "\n",
        encoding="utf-8",
    )


def write_readme(output_dir: Path, source_pdf: Path, selected_metadata: list[dict[str, object]]) -> None:
    lines = [
        "# MCXNx4xRM Extracted Chapters",
        "",
        f"Source PDF: `{repo_relative(source_pdf)}`",
        "",
        "This directory is generated by `scripts/tools/extract_mcxn_reference.py`.",
        "Each selected chapter has extracted text plus rendered key pages and embedded raster images when available.",
        "",
        "## Selected Chapters",
        "",
    ]

    for metadata in selected_metadata:
        chapter_dir = metadata["directory"]
        lines.append(
            f"- Chapter {metadata['number']}: {metadata['title']} "
            f"(pages {metadata['start_page']}-{metadata['end_page']}) -> `{chapter_dir}`"
        )

    lines.extend(
        [
            "",
            "## Regenerate",
            "",
            "```powershell",
            "uv run python scripts/tools/extract_mcxn_reference.py",
            "```",
            "",
        ]
    )
    (output_dir / "README.md").write_text("\n".join(lines), encoding="utf-8")


def clean_output(output_dir: Path) -> None:
    resolved_output = output_dir.resolve()
    allowed_root = ALLOWED_CLEAN_ROOT.resolve()
    if resolved_output == allowed_root or allowed_root not in resolved_output.parents:
        raise SystemExit(f"Refusing to clean output outside {allowed_root}: {resolved_output}")
    if resolved_output.exists():
        shutil.rmtree(resolved_output)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", type=Path, default=DEFAULT_SOURCE, help="Reference manual PDF")
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT, help="Extraction output directory")
    parser.add_argument(
        "--chapters",
        type=parse_csv_ints,
        default=set(DEFAULT_CHAPTERS),
        help="Comma/space separated chapter numbers to extract",
    )
    parser.add_argument(
        "--keywords",
        type=parse_csv_strings,
        default=(),
        help="Optional comma separated chapter title keywords to add to the selection",
    )
    parser.add_argument("--all", action="store_true", help="Extract every chapter from the PDF TOC")
    parser.add_argument(
        "--render-terms",
        type=parse_csv_strings,
        default=DEFAULT_RENDER_TERMS,
        help="Comma separated page-text terms used to render visual reference pages",
    )
    parser.add_argument("--max-rendered-pages", type=int, default=4, help="Rendered page limit per chapter")
    parser.add_argument("--render-dpi", type=int, default=120, help="DPI for rendered page PNGs")
    parser.add_argument("--max-embedded-images", type=int, default=20, help="Embedded raster image limit per chapter")
    parser.add_argument("--min-image-dimension", type=int, default=96, help="Skip tiny embedded images")
    parser.add_argument("--keep-existing", action="store_true", help="Do not clean the output directory before writing")
    args = parser.parse_args()

    source_pdf = args.source.resolve()
    output_dir = args.output.resolve()
    if not source_pdf.exists():
        raise SystemExit(f"Reference manual not found: {source_pdf}")

    if not args.keep_existing:
        clean_output(output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)

    doc = fitz.open(source_pdf)
    chapters = chapter_entries_from_toc(doc.get_toc(), doc.page_count)
    selected, match_reasons = select_chapters(chapters, args.chapters, args.keywords, args.all)
    if not selected:
        raise SystemExit("No chapters matched the requested selection")

    selected_metadata: list[dict[str, object]] = []
    for chapter in selected:
        chapter_dir = output_dir / "chapters" / chapter.directory_name
        chapter_dir.mkdir(parents=True, exist_ok=True)
        pages = extract_text(doc, chapter)
        write_text(chapter_dir, source_pdf, chapter, pages)
        embedded_images = extract_embedded_images(
            doc,
            chapter,
            chapter_dir,
            args.min_image_dimension,
            args.max_embedded_images,
        )
        rendered_pages = render_key_pages(
            doc,
            chapter,
            chapter_dir,
            pages,
            args.render_terms,
            args.max_rendered_pages,
            args.render_dpi,
        )
        metadata = write_metadata(
            chapter_dir,
            chapter,
            match_reasons.get(chapter.number, []),
            embedded_images,
            rendered_pages,
        )
        selected_metadata.append(metadata)
        print(f"Extracted chapter {chapter.number}: {chapter.title} ({chapter.start_page}-{chapter.end_page})")

    write_index(output_dir, source_pdf, doc, chapters, selected_metadata, args)
    write_readme(output_dir, source_pdf, selected_metadata)
    print(f"\nOutput: {output_dir}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
