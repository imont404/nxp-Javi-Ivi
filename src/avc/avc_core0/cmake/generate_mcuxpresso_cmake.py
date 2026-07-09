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


PROJECT_NAME = "avc_core0"


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


def source_roots(config: ET.Element) -> list[str]:
    entries = config.find("sourceEntries")
    if entries is None:
        return []
    roots: list[str] = []
    for entry in entries.findall("entry"):
        if entry.get("kind") == "sourcePath":
            name = entry.get("name")
            if name:
                roots.append(name.replace("\\", "/"))
    return roots


def included_makefiles(debug_dir: Path) -> list[Path]:
    top_makefile = debug_dir / "makefile"
    if not top_makefile.exists():
        return sorted(debug_dir.rglob("subdir.mk"))

    makefiles: list[Path] = []
    for raw_line in top_makefile.read_text(encoding="utf-8", errors="replace").splitlines():
        line = raw_line.strip()
        if not line.startswith("-include ") or not line.endswith("/subdir.mk"):
            continue
        rel = line.removeprefix("-include ").strip()
        makefiles.append(debug_dir / rel)
    return makefiles


def source_list_from_makefile(makefile: Path) -> tuple[list[str], list[str]]:
    variables = {"C_SRCS": [], "S_UPPER_SRCS": [], "S_SRCS": [], "ASM_SRCS": []}
    active: str | None = None
    for raw_line in makefile.read_text(encoding="utf-8", errors="replace").splitlines():
        line = raw_line.strip()
        match = re.match(r"^(C_SRCS|S_UPPER_SRCS|S_SRCS|ASM_SRCS) \+=", line)
        if match:
            active = match.group(1)
            continue
        if active is None:
            continue
        if not line:
            active = None
            continue
        item = line.rstrip("\\").strip().replace("\\", "/")
        if item:
            variables[active].append(item)

    c_sources = variables["C_SRCS"]
    asm_sources = variables["S_UPPER_SRCS"] + variables["S_SRCS"] + variables["ASM_SRCS"]
    return c_sources, asm_sources


def extract_source_flags(command: str) -> list[str]:
    flags: list[str] = []
    for match in re.finditer(r"(?<!\S)(-O[0-3gs]|-flto|-ffat-lto-objects)(?!\S)", command):
        flag = match.group(1)
        if flag not in flags:
            flags.append(flag)
    return flags


def source_flags_from_makefile(makefile: Path, c_sources: list[str]) -> dict[str, list[str]]:
    default_flags: list[str] = []
    specific_flags: dict[str, list[str]] = {}
    current_rule_source: str | None = None

    for raw_line in makefile.read_text(encoding="utf-8", errors="replace").splitlines():
        if raw_line and not raw_line[0].isspace() and ":" in raw_line:
            deps = raw_line.split(":", 1)[1].strip().split()
            current_rule_source = None
            for dep in deps:
                dep = dep.replace("\\", "/")
                if dep.endswith(".c") or "%.c" in dep:
                    current_rule_source = dep
                    break
            continue

        line = raw_line.strip()
        if not line.startswith("arm-none-eabi-gcc"):
            continue

        flags = extract_source_flags(line)
        if not current_rule_source:
            continue
        if "%" in current_rule_source:
            default_flags = flags
        else:
            specific_flags[current_rule_source] = flags

    result: dict[str, list[str]] = {}
    for source in c_sources:
        result[source] = specific_flags.get(source, default_flags)
    return result


def parse_makefile_sources(
    project_dir: Path, config_name: str
) -> tuple[list[str], list[str], list[str], dict[str, list[str]]]:
    debug_dir = project_dir / config_name
    if not debug_dir.exists():
        return [], [], [], {}

    all_c_sources: list[str] = []
    all_asm_sources: list[str] = []
    ordered_sources: list[str] = []
    source_flags: dict[str, list[str]] = {}

    for makefile in included_makefiles(debug_dir):
        if not makefile.exists():
            continue
        c_sources, asm_sources = source_list_from_makefile(makefile)
        all_c_sources.extend(c_sources)
        all_asm_sources.extend(asm_sources)
        ordered_sources.extend(c_sources + asm_sources)
        source_flags.update(source_flags_from_makefile(makefile, c_sources))

    return all_c_sources, all_asm_sources, ordered_sources, source_flags


def glob_sources(project_dir: Path, roots: list[str]) -> tuple[list[str], list[str], list[str]]:
    c_sources: list[str] = []
    asm_sources: list[str] = []
    for root in roots:
        base = project_dir / root
        for source in sorted(glob.glob(str(base / "**" / "*.c"), recursive=True)):
            c_sources.append("../" + Path(source).relative_to(project_dir).as_posix())
        for pattern in ("*.S", "*.s"):
            for source in sorted(glob.glob(str(base / "**" / pattern), recursive=True)):
                asm_sources.append("../" + Path(source).relative_to(project_dir).as_posix())
    return c_sources, asm_sources, c_sources + asm_sources


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
    parser.add_argument("--project-dir", default="src/avc/avc_core0", help="MCUXpresso project directory")
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
    c_sources, asm_sources, ordered_sources, per_source_flags = parse_makefile_sources(project_dir, config_name)
    source_mode = f"{config_name} generated makefiles"
    if not c_sources and not asm_sources:
        c_sources, asm_sources, ordered_sources = glob_sources(project_dir, roots)
        per_source_flags = {}
        source_mode = ".cproject source roots"

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

    linker_script = "${MCUX_PROJECT_ROOT}/link/avc_core0_Debug.ld"
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
        "# Generated by cmake/generate_mcuxpresso_cmake.py.",
        f"# Source list mode: {source_mode}.",
        "# Regenerate after MCUXpresso project/build setting changes.",
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
