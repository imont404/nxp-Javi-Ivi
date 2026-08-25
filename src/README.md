# Source Components

| Folder | Purpose | Normal command | Student edits |
| --- | --- | --- | --- |
| `embedded` | FRDM-MCXN947 firmware, MCUXpresso project, CMake build, and hardware tools | `.\src\embedded\build.ps1` | Normally only the two mode files named in `embedded/AGENTS.md` |
| `host` | Native Windows camera/telemetry viewer and CLI plus the direct WebSerial viewer | `.\src\host\build.ps1` | No |
| `android` | Maintainer phone USB-host and Wi-Fi telemetry relay | `.\src\android\build.ps1` | No |
| `common` | Shared linked libraries used by the embedded and host components | Built through a consumer | No |

Each buildable component owns its build entry point and detailed README. Read the
nearest `AGENTS.md` before using an LLM to change component code.

The direct WebSerial page and Android relay page currently belong to their respective
consumers. The planned cross-consumer presentation extraction is specified in
`docs/design/shared-web-dashboard.md`. Create `src/web` only together with its real
`build.ps1`, README, agent guide, drift check, and generated-output contract; do not use
`src/common` for browser assets because that folder has embedded linked-source ownership.
