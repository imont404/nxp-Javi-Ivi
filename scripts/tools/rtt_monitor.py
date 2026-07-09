#!/usr/bin/env python3
"""Read SEGGER RTT output from the AVC MCXN947 target using PyLink."""

from __future__ import annotations

import argparse
import os
import re
import subprocess
import sys
import time
from pathlib import Path

try:
    import pylink
except ImportError:
    print("Error: pylink-square is not installed. Run: uv sync")
    raise SystemExit(1)


def default_jlink_dll() -> str:
    candidates = [
        r"C:\Program Files\SEGGER\JLink_V940\JLink_x64.dll",
        r"C:\Program Files\SEGGER\JLink_V844\JLink_x64.dll",
        r"C:\Program Files\SEGGER\JLink_V892\JLink_x64.dll",
    ]
    for candidate in candidates:
        if os.path.exists(candidate):
            return candidate
    return candidates[0]


def find_rtt_address(axf_file: Path, nm_path: Path) -> int:
    output = subprocess.check_output(
        [str(nm_path), "-n", str(axf_file)],
        text=True,
        errors="replace",
    )
    for line in output.splitlines():
        match = re.match(r"^\s*([0-9A-Fa-f]+)\s+\w\s+_SEGGER_RTT\s*$", line)
        if match:
            return int(match.group(1), 16)
    raise RuntimeError(f"could not find _SEGGER_RTT in {axf_file}")


def open_jlink(lib: pylink.Library, serial: str | None) -> pylink.JLink:
    jlink = pylink.JLink(lib=lib)
    if serial:
        try:
            jlink.open(serial_no=int(serial))
        except TypeError:
            jlink.open(int(serial))
    else:
        jlink.open()
    return jlink


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--file", required=True, help="AXF/ELF file containing _SEGGER_RTT")
    parser.add_argument("--nm", required=True, help="arm-none-eabi-nm path")
    parser.add_argument("--dll", default=default_jlink_dll(), help="JLink_x64.dll path")
    parser.add_argument("--device", default="MCXN947_M33_0", help="J-Link device name")
    parser.add_argument("--serial", default="", help="J-Link serial number")
    parser.add_argument("--speed", type=int, default=4000, help="SWD speed in kHz")
    parser.add_argument("--channel", type=int, default=0, help="RTT up-buffer index")
    parser.add_argument("--timeout", type=float, default=0, help="Exit after N seconds; 0 runs forever")
    parser.add_argument("--once", action="store_true", help="Exit after the first non-empty read")
    parser.add_argument("--reset", action="store_true", help="Reset and run after connecting")
    args = parser.parse_args()

    axf_file = Path(args.file)
    nm_path = Path(args.nm)
    dll_path = Path(args.dll)

    if not axf_file.exists():
        raise SystemExit(f"Firmware not found: {axf_file}")
    if not nm_path.exists():
        raise SystemExit(f"arm-none-eabi-nm not found: {nm_path}")
    if not dll_path.exists():
        raise SystemExit(f"J-Link DLL not found: {dll_path}")

    rtt_address = find_rtt_address(axf_file, nm_path)

    print(f"Device: {args.device}")
    print(f"Interface: SWD @ {args.speed} kHz")
    print(f"J-Link DLL: {dll_path}")
    print(f"RTT address: 0x{rtt_address:08X}")
    print(f"Channel: {args.channel}")
    print("")

    lib = pylink.Library(dllpath=str(dll_path))
    jlink: pylink.JLink | None = None
    try:
        jlink = open_jlink(lib, args.serial or None)
        print(f"J-Link S/N: {jlink.serial_number}")
        print(f"J-Link FW: {jlink.firmware_version}")

        jlink.set_tif(pylink.enums.JLinkInterfaces.SWD)
        jlink.connect(args.device, speed=args.speed)
        print(f"Target: {jlink.core_name()}")
        print(f"Core ID: 0x{jlink.core_id():08X}")

        jlink.rtt_start(block_address=rtt_address)
        if args.reset:
            jlink.reset(halt=False)
            time.sleep(0.05)

        deadline = time.time() + 5.0
        while time.time() < deadline:
            try:
                up_buffers = jlink.rtt_get_num_up_buffers()
                down_buffers = jlink.rtt_get_num_down_buffers()
                print(f"RTT buffers: {up_buffers} up, {down_buffers} down")
                break
            except pylink.errors.JLinkRTTException:
                time.sleep(0.1)

        print("")
        print("RTT Output:")
        start_time = time.time()
        saw_output = False
        while True:
            try:
                data = jlink.rtt_read(args.channel, 4096)
                if data:
                    saw_output = True
                    print(bytes(data).decode("utf-8", errors="replace"), end="", flush=True)
                    if args.once:
                        break
            except pylink.errors.JLinkRTTException:
                pass

            if args.timeout > 0 and time.time() - start_time >= args.timeout:
                if saw_output:
                    print("")
                break

            time.sleep(0.01)

    except KeyboardInterrupt:
        print("")
    finally:
        if jlink is not None:
            try:
                jlink.rtt_stop()
            except Exception:
                pass
            jlink.close()

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
