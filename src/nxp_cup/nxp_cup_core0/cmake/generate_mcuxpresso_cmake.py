#!/usr/bin/env python3
"""Generate Debug CMake settings from MCUXpresso project metadata."""

from __future__ import annotations

import argparse
import glob
import html
import os
import re
import shlex
import sys
import xml.etree.ElementTree as ET
from pathlib import Path


PROJECT_NAME = "nxp_cup_core0"


def cmake_quote(value: str) -> str:
    escaped = value.replace("\\", "/").replace('"', '\\"')
    return f'"{escaped}"'


def strip_outer_quotes(value: str) -> str:
    value = html.unescape(value.strip())
    while len(value) >= 2 and value[0] == value[-1] and value[0] in ("'", '"'):
        value = value[1:-1].strip()
    return value


def is_windows_absolute(value: str) -> bool:
    return re.match(r"^[A-Za-z]:[\\/]", value) is not None


def to_cmake_path(value: str) -> str:
    return value.replace("\\", "/")


def normalize_define(value: str) -> str:
    value = html.unescape(value.strip())
    if "=" not in value:
        return value

    name, raw = value.split("=", 1)
    raw = raw.strip()
    if len(raw) >= 2 and raw[0] == "'" and raw[-1] == "'":
        raw = raw[1:-1]
    return f"{name}={raw}"


def relative_to_project_expr(path: Path, project_dir: Path) -> str:
    rel = os.path.relpath(path, project_dir).replace("\\", "/")
    if rel == ".":
        return "${MCUX_PROJECT_ROOT}"
    return "${MCUX_PROJECT_ROOT}/" + rel


def normalize_path(value: str, project_dir: Path) -> str:
    value = to_cmake_path(strip_outer_quotes(value))
    value = value.replace("${ProjDirPath}", "${MCUX_PROJECT_ROOT}")

    workspace_prefix = "${workspace_loc:/${ProjName}/"
    if value.startswith(workspace_prefix) and value.endswith("}"):
        return "${MCUX_PROJECT_ROOT}/" + value[len(workspace_prefix) : -1]

    if value == "${workspace_loc:/${ProjName}}":
        return "${MCUX_PROJECT_ROOT}"

    if value.startswith("../"):
        return "${MCUX_PROJECT_ROOT}/" + value[3:]

    if is_windows_absolute(value) or value.startswith("/"):
        path = Path(value).resolve()
        try:
            return relative_to_project_expr(path, project_dir)
        except ValueError:
            return to_cmake_path(str(path))

    return value


def option_by_superclass(tool: ET.Element, superclass: str) -> ET.Element | None:
    for option in tool.findall(".//option"):
        if option.get("superClass") == superclass:
            return option
    return None


def option_value(tool: ET.Element, superclass: str) -> str | None:
    option = option_by_superclass(tool, superclass)
    if option is None:
        return None
    return option.get("value")


def option_bool(tool: ET.Element, superclass: str) -> bool:
    option = option_by_superclass(tool, superclass)
    return option is not None and option.get("value", "true").lower() == "true"


def option_list(tool: ET.Element, superclass: str) -> list[str]:
    option = option_by_superclass(tool, superclass)
    if option is None:
        return []
    return [item.get("value", "") for item in option.findall("listOptionValue")]


def find_configuration(root: ET.Element, config_name: str) -> ET.Element:
    for config in root.findall(".//configuration"):
        if config.get("name") == config_name:
            return config
    raise SystemExit(f"configuration not found in .cproject: {config_name}")


def find_tool(config: ET.Element, name: str) -> ET.Element:
    for tool in config.findall(".//tool"):
        if tool.get("name") == name:
            return tool
    raise SystemExit(f"tool not found in .cproject configuration {config.get('name')}: {name}")


def source_roots(config: ET.Element) -> list[tuple[str, list[str]]]:
    entries = config.find("sourceEntries")
    if entries is None:
        return []
    roots: list[tuple[str, list[str]]] = []
    for entry in entries.findall("entry"):
        if entry.get("kind") == "sourcePath":
            name = entry.get("name")
            if name:
                exclusions = [
                    item.strip().replace("\\", "/")
                    for item in entry.get("excluding", "").split("|")
                    if item.strip()
                ]
                roots.append((name.replace("\\", "/"), exclusions))
    return roots


def resource_is_under(resource: str, root: str) -> bool:
    return resource == root or resource.startswith(root + "/")


def resource_is_excluded(resource: str, root: str, exclusions: list[str]) -> bool:
    if not resource_is_under(resource, root):
        return True

    relative = resource[len(root) :].lstrip("/")
    for exclusion in exclusions:
        if relative == exclusion or relative.startswith(exclusion + "/"):
            return True
    return False


def resolve_project_location_uri(location_uri: str, project_dir: Path) -> Path | None:
    if not location_uri or location_uri.startswith("virtual:"):
        return None

    location_uri = html.unescape(location_uri.strip()).replace("\\", "/")

    parent_match = re.match(r"^PARENT-(\d+)-PROJECT_LOC(?:/(.*))?$", location_uri)
    if parent_match:
        base = project_dir
        for _ in range(int(parent_match.group(1))):
            base = base.parent
        rest = parent_match.group(2) or ""
        return (base / rest).resolve()

    if location_uri.startswith("PROJECT_LOC/"):
        return (project_dir / location_uri.removeprefix("PROJECT_LOC/")).resolve()

    if is_windows_absolute(location_uri) or location_uri.startswith("/"):
        return Path(location_uri).resolve()

    return (project_dir / location_uri).resolve()


def linked_resources(project_dir: Path) -> dict[str, tuple[int, Path | None]]:
    project_file = project_dir / ".project"
    if not project_file.exists():
        return {}

    links: dict[str, tuple[int, Path | None]] = {}
    tree = ET.parse(project_file)
    for link in tree.findall(".//linkedResources/link"):
        name = link.findtext("name", "").replace("\\", "/")
        type_text = link.findtext("type", "")
        location_uri = link.findtext("locationURI", "")
        if not name or not type_text:
            continue
        links[name] = (int(type_text), resolve_project_location_uri(location_uri, project_dir))

    return links


def source_file_kind(path: Path) -> str | None:
    suffix = path.suffix.lower()
    if suffix == ".c":
        return "c"
    if suffix in (".s", ".asm") or path.suffix == ".S":
        return "asm"
    return None


def iter_source_files(base: Path) -> list[Path]:
    sources: list[Path] = []
    for pattern in ("*.c", "*.S", "*.s", "*.asm"):
        sources.extend(Path(source).resolve() for source in glob.glob(str(base / "**" / pattern), recursive=True))
    return sorted(sources, key=lambda path: path.as_posix().lower())


def glob_sources(
    project_dir: Path, roots: list[tuple[str, list[str]]]
) -> tuple[list[str], list[str], list[str], dict[str, str]]:
    c_sources: list[str] = []
    asm_sources: list[str] = []
    ordered_sources: list[str] = []
    source_resources: dict[str, str] = {}
    seen: set[Path] = set()
    links = linked_resources(project_dir)

    def add_source(path: Path, resource: str) -> None:
        path = path.resolve()
        if path in seen:
            return

        kind = source_file_kind(path)
        if kind is None:
            return

        source = str(path)
        if kind == "c":
            c_sources.append(source)
        else:
            asm_sources.append(source)
        ordered_sources.append(source)
        source_resources[source] = resource
        seen.add(path)

    for root, exclusions in roots:
        base = project_dir / root
        if base.exists():
            for source in iter_source_files(base):
                resource = root + "/" + source.relative_to(base).as_posix()
                if not resource_is_excluded(resource, root, exclusions):
                    add_source(source, resource)

        for resource, (resource_type, path) in sorted(links.items()):
            if path is None or not resource_is_under(resource, root):
                continue

            if resource_type == 1:
                if not resource_is_excluded(resource, root, exclusions):
                    add_source(path, resource)
                continue

            if resource_type == 2 and path.exists():
                for source in iter_source_files(path):
                    child_resource = resource + "/" + source.relative_to(path).as_posix()
                    if not resource_is_excluded(child_resource, root, exclusions):
                        add_source(source, child_resource)

    return c_sources, asm_sources, ordered_sources, source_resources


def source_to_cmake(value: str, project_dir: Path) -> str:
    value = to_cmake_path(value)
    if is_windows_absolute(value) or value.startswith("/"):
        return relative_to_project_expr(Path(value).resolve(), project_dir)
    if value.startswith("../"):
        return "${MCUX_PROJECT_ROOT}/" + value[3:]
    return "${MCUX_PROJECT_ROOT}/" + value


def split_flags(value: str | None) -> list[str]:
    if not value:
        return []
    return [html.unescape(part) for part in shlex.split(value, posix=True)]


def arch_flags(value: str | None) -> list[str]:
    if value and "cm33" in value:
        return ["-mcpu=cortex-m33"]
    return []


def fpu_flags(value: str | None) -> list[str]:
    if value and "fpv5sp.hard" in value:
        return ["-mfpu=fpv5-sp-d16", "-mfloat-abi=hard"]
    return []


def optimization_flags(value: str | None, config_name: str) -> list[str]:
    if value:
        if value.endswith(".general"):
            return ["-Og"]
        if value.endswith(".none"):
            return ["-O0"]
        if value.endswith(".more"):
            return ["-O2"]
        if value.endswith(".most"):
            return ["-O3"]
        if value.endswith(".size"):
            return ["-Os"]
    if config_name.lower() == "debug":
        return ["-O0"]
    return []


def explicit_optimization_flags(value: str | None) -> list[str]:
    if value:
        if value.endswith(".general"):
            return ["-Og"]
        if value.endswith(".none"):
            return ["-O0"]
        if value.endswith(".more"):
            return ["-O2"]
        if value.endswith(".most"):
            return ["-O3"]
        if value.endswith(".size"):
            return ["-Os"]
    return []


def dialect_flags(value: str | None) -> list[str]:
    if value and "gnu99" in value:
        return ["-std=gnu99"]
    return []


def specs_flags(value: str | None) -> list[str]:
    if value and "newlibnano" in value:
        return ["-specs=nano.specs"]
    return []


def map_linker_option(value: str) -> str:
    value = html.unescape(value.strip()).replace('\\"', '"')
    value = value.replace(
        '"${BuildArtifactFileBaseName}.map"',
        f"${{CMAKE_CURRENT_BINARY_DIR}}/{PROJECT_NAME}.map",
    )
    value = value.replace(
        "${BuildArtifactFileBaseName}.map",
        f"${{CMAKE_CURRENT_BINARY_DIR}}/{PROJECT_NAME}.map",
    )
    return "-Wl," + value


def write_list(lines: list[str], name: str, items: list[str]) -> None:
    lines.append(f"set({name}")
    for item in items:
        lines.append(f"    {cmake_quote(item)}")
    lines.append(")")
    lines.append("")


def optional_tool(element: ET.Element, name: str) -> ET.Element | None:
    for tool in element.findall(".//tool"):
        if tool.get("name") == name:
            return tool
    return None


def unique_flags(flags: list[str]) -> list[str]:
    result: list[str] = []
    for flag in flags:
        if flag not in result:
            result.append(flag)
    return result


def c_source_override_flags(tool: ET.Element | None) -> list[str]:
    if tool is None:
        return []

    flags: list[str] = []
    for superclass in (
        "com.crt.advproject.gcc.exe.debug.option.optimization.level",
        "com.crt.advproject.gcc.exe.release.option.optimization.level",
    ):
        flags.extend(explicit_optimization_flags(option_value(tool, superclass)))

    lto_option = option_by_superclass(tool, "com.crt.advproject.gcc.lto")
    if lto_option is not None and lto_option.get("value", "false").lower() == "true":
        flags.extend(["-flto", "-ffat-lto-objects"])

    return unique_flags(flags)


def source_override_flags(
    config: ET.Element, source_resources: dict[str, str]
) -> dict[str, list[str]]:
    file_flags: dict[str, list[str]] = {}
    folder_flags: list[tuple[str, list[str]]] = []

    for file_info in config.findall(".//fileInfo"):
        resource = file_info.get("resourcePath", "").replace("\\", "/")
        flags = c_source_override_flags(optional_tool(file_info, "MCU C Compiler"))
        if resource and flags:
            file_flags[resource] = flags

    for folder_info in config.findall(".//folderInfo"):
        resource = folder_info.get("resourcePath", "").replace("\\", "/")
        flags = c_source_override_flags(optional_tool(folder_info, "MCU C Compiler"))
        if resource and flags:
            folder_flags.append((resource, flags))

    result: dict[str, list[str]] = {}
    for source, resource in source_resources.items():
        if resource in file_flags:
            result[source] = file_flags[resource]
            continue

        matches = [
            (len(folder), flags)
            for folder, flags in folder_flags
            if resource_is_under(resource, folder)
        ]
        if matches:
            result[source] = max(matches, key=lambda item: item[0])[1]

    return result


def source_option_code(
    source_flags: dict[str, list[str]], project_dir: Path, base_optimization: str
) -> list[str]:
    code: list[str] = []
    for source, flags in sorted(source_flags.items()):
        effective = [flag for flag in flags if flag != base_optimization]
        if not effective:
            continue
        source_expr = source_to_cmake(source, project_dir)
        option_args = " ".join(cmake_quote(flag) for flag in effective)
        code.append(f"set_property(SOURCE \"{source_expr}\" APPEND PROPERTY COMPILE_OPTIONS {option_args})")
    return code


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--project-dir", default="src/nxp_cup/nxp_cup_core0", help="MCUXpresso project directory")
    parser.add_argument("--config", default="Debug", help="MCUXpresso build configuration")
    parser.add_argument("--output", default=None, help="Output CMake include")
    args = parser.parse_args()

    project_dir = Path(args.project_dir).resolve()
    config_name = args.config
    output = Path(args.output).resolve() if args.output else project_dir / "cmake" / "mcuxpresso_debug.cmake"

    tree = ET.parse(project_dir / ".cproject")
    config = find_configuration(tree.getroot(), config_name)
    c_tool = find_tool(config, "MCU C Compiler")
    asm_tool = find_tool(config, "MCU Assembler")
    link_tool = find_tool(config, "MCU Linker")

    roots = source_roots(config)
    c_sources, asm_sources, ordered_sources, source_resources = glob_sources(project_dir, roots)
    per_source_flags = source_override_flags(config, source_resources)
    source_mode = ".cproject source roots + .project linked resources"

    c_defines = [normalize_define(value) for value in option_list(c_tool, "gnu.c.compiler.option.preprocessor.def.symbols")]
    include_dirs = [
        normalize_path(value, project_dir)
        for value in option_list(c_tool, "gnu.c.compiler.option.include.paths")
    ]
    lib_dirs = [normalize_path(value, project_dir) for value in option_list(link_tool, "gnu.c.link.option.paths")]
    libraries = [strip_outer_quotes(value) for value in option_list(link_tool, "gnu.c.link.option.libs")]
    linker_options = [map_linker_option(value) for value in option_list(link_tool, "gnu.c.link.option.other")]

    c_misc = [normalize_path(value, project_dir) for value in split_flags(option_value(c_tool, "gnu.c.compiler.option.misc.other"))]
    c_misc = [value for value in c_misc if value != "-c"]
    c_flags: list[str] = []
    c_flags.extend(dialect_flags(option_value(c_tool, "com.crt.advproject.c.misc.dialect")))
    c_flags.extend(optimization_flags(option_value(c_tool, "com.crt.advproject.gcc.exe.debug.option.optimization.level"), config_name))
    c_flags.extend(split_flags(option_value(c_tool, "gnu.c.compiler.option.optimization.flags")))
    if config_name.lower() == "debug":
        c_flags.extend(["-g3", "-gdwarf-4"])
    c_flags.extend(c_misc)
    if option_by_superclass(c_tool, "com.crt.advproject.gcc.merge.constants") is not None:
        c_flags.append("-fmerge-constants")
    c_flags.extend(arch_flags(option_value(c_tool, "com.crt.advproject.gcc.arch")))
    c_flags.extend(fpu_flags(option_value(c_tool, "com.crt.advproject.gcc.fpu")))
    if option_value(c_tool, "com.crt.advproject.gcc.thumb") == "true":
        c_flags.append("-mthumb")
    c_flags.extend(specs_flags(option_value(c_tool, "com.crt.advproject.gcc.specs")))
    if option_by_superclass(c_tool, "com.crt.advproject.gcc.stackusage") is not None:
        c_flags.append("-fstack-usage")

    asm_flags = ["-x", "assembler-with-cpp"]
    asm_flags.extend(split_flags(option_value(asm_tool, "gnu.both.asm.option.flags.crt")))
    if config_name.lower() == "debug":
        asm_flags.extend(["-g3", "-gdwarf-4"])
    asm_flags.extend(arch_flags(option_value(asm_tool, "com.crt.advproject.gas.arch")))
    asm_flags.extend(fpu_flags(option_value(asm_tool, "com.crt.advproject.gas.fpu")))
    if option_value(asm_tool, "com.crt.advproject.gas.thumb") == "true":
        asm_flags.append("-mthumb")
    asm_flags.extend(specs_flags(option_value(asm_tool, "com.crt.advproject.gas.specs")))

    linker_script = "${MCUX_PROJECT_ROOT}/link/nxp_cup_core0_Debug.ld"
    linker_script_dir = "${MCUX_PROJECT_ROOT}/link"
    link_flags = ["-nostdlib"]
    link_flags.extend(linker_options)
    link_flags.extend(arch_flags(option_value(link_tool, "com.crt.advproject.link.arch")))
    link_flags.extend(fpu_flags(option_value(link_tool, "com.crt.advproject.link.fpu")))
    if option_bool(link_tool, "com.crt.advproject.link.thumb"):
        link_flags.append("-mthumb")
    link_flags.extend(
        [
            f"SHELL:-T {linker_script}",
            f"SHELL:-L {linker_script_dir}",
            "SHELL:-u _printf_float",
        ]
    )

    base_optimization = next((flag for flag in c_flags if re.fullmatch(r"-O[0-3gs]", flag)), "")
    code_lines = source_option_code(per_source_flags, project_dir, base_optimization)

    lines = [
        "# CHECKED-IN SOURCE LIST - this file is the source of truth for the build.",
        "#",
        "# Originally generated by cmake/generate_mcuxpresso_cmake.py from the MCUXpresso",
        "# .cproject source roots and .project linked resources. It is committed and used",
        "# as-is, so a normal build needs neither Python nor MCUXpresso installed.",
        "#",
        "# It is self-contained: every path is relative to MCUX_PROJECT_ROOT, derived from",
        "# this file's own location. No MCUXpresso install path appears here.",
        "#",
        f"# Source list mode: {source_mode}.",
        "#",
        "# After changing MCUXpresso project settings:",
        "#   .\\scripts\\maintainer\\build_cmake.ps1 -CheckDrift    # show what changed, without writing",
        "#   .\\scripts\\maintainer\\build_cmake.ps1 -Regenerate    # accept it, then review and commit",
        "",
        'get_filename_component(MCUX_PROJECT_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)',
        "",
    ]
    write_list(lines, "MCUX_C_SOURCES", [source_to_cmake(value, project_dir) for value in c_sources])
    write_list(lines, "MCUX_ASM_SOURCES", [source_to_cmake(value, project_dir) for value in asm_sources])
    write_list(lines, "MCUX_SOURCES", [source_to_cmake(value, project_dir) for value in ordered_sources])
    write_list(lines, "MCUX_DEFINES", c_defines)
    write_list(lines, "MCUX_INCLUDE_DIRS", include_dirs)
    write_list(lines, "MCUX_C_FLAGS", c_flags)
    write_list(lines, "MCUX_ASM_FLAGS", asm_flags)
    write_list(lines, "MCUX_LINK_OPTIONS", link_flags)
    write_list(lines, "MCUX_LIBRARY_DIRS", lib_dirs)
    write_list(lines, "MCUX_LIBRARIES", libraries)

    lines.append("set(MCUX_SOURCE_OPTIONS_CODE [=[")
    lines.extend(code_lines)
    lines.append("]=])")

    while lines and lines[-1] == "":
        lines.pop()

    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(f"wrote {output}")
    print(f"C sources: {len(c_sources)}")
    print(f"ASM sources: {len(asm_sources)}")
    print(f"source list mode: {source_mode}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
