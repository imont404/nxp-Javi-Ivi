# NXP Cup core tools for Windows x64

This folder is the prebuilt student runtime. Keep all files together.

- `nxpc_viewer.exe` shows camera frames and telemetry and can program the
  normal firmware image.
- `nxpc_tool.exe` provides command-line device checks and ROM-HID programming.
- `rblhost.exe` is the pinned NXP ROM-HID programmer used by both tools.

From the repository root, launch the viewer with:

```powershell
.\out\artifacts\host\nxpc_viewer.exe
```

The viewer automatically looks for the normal firmware output at
`out\artifacts\embedded\nxp_cup_core0.bin`. The command-line equivalent is:

```powershell
.\out\artifacts\host\nxpc_tool.exe program --image `
  .\out\artifacts\embedded\nxp_cup_core0.bin
```

Programming uses the board's ROM-HID bootloader and does not require J-Link.
See the repository setup documentation for board bootloader entry and optional
J-Link recovery instructions.

`manifest.json` identifies the release and contains SHA-256 digests for every
payload file. `HOST-README.md` is this file. License notices for the
redistributed components are included.
