# Source Components

| Folder | Purpose | Normal command | Student edits |
| --- | --- | --- | --- |
| `embedded` | FRDM-MCXN947 firmware, MCUXpresso project, CMake build, and hardware tools | `.\src\embedded\build.ps1` | Normally only the two mode files named in `embedded/AGENTS.md` |
| `host` | Native Windows camera/telemetry viewer and CLI plus the direct WebSerial viewer | `.\src\host\build.ps1` | No |
| `android` | Maintainer phone USB-host and Wi-Fi telemetry relay | `.\src\android\build.ps1` | No |
| `common` | Shared linked libraries used by the embedded and host components | Built through a consumer | No |

Each buildable component owns its build entry point and detailed README. Read the
nearest `AGENTS.md` before using an LLM to change component code.

