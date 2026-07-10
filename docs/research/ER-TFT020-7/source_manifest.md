# ER-TFT020-7 Source Manifest

This manifest records the external collateral for the alternate 240x320
BuyDisplay/EastRising display work.

Automated command-line download attempts on 2026-07-10 were blocked by
`buydisplay.com` with HTTP 403 / Cloudflare challenge responses. Files marked
`blocked` should be downloaded manually through a browser or retried later with
a working fetch path, then placed under `docs/research/ER-TFT020-7/downloads`.

| Artifact | Source URL | Local status | Notes |
| --- | --- | --- | --- |
| Product page | `https://www.buydisplay.com/color-2-inch-240x320-ips-tft-lcd-display-mcu-8080-interface` | Cataloged, not mirrored | Public HTML page; title says MCU 8080, specs/datasheet say SPI and 6800 8-bit parallel. |
| Display datasheet | `https://www.buydisplay.com/download/manual/ER-TFT020-7_Datasheet.pdf` | Blocked | 22-page module datasheet, update date shown on product page as 2025-08-21. |
| ST7789VW controller datasheet | `https://www.buydisplay.com/download/ic/ST7789.pdf` | Local copy available as `downloads/ST7789vw_existing_local_copy.pdf` | Existing repo copy came from `docs/lcd/ST7789vw.pdf`; direct BuyDisplay download was blocked. |
| Capacitive touch controller datasheet | `https://www.buydisplay.com/download/ic/DS-CST816S_DS_V1.3.pdf` | Blocked | Only needed if the purchased display includes/uses the CTP option. |
| 22-pin FPC connector drawing | `https://www.buydisplay.com/download/connector/ER-CON22HT-1.pdf` | Blocked | Needed for mechanical/connector confirmation. |
| 8051 interfacing and demo code | `https://www.buydisplay.com/8051/ER-TFT020-7_8051_Tutorial.zip` | Blocked | Key source for interface-mode straps and reference command sequence. |
| 8051 dev-board page | `https://www.buydisplay.com/8051-microcontroller-development-board-for-tft-display-er-tft020-7` | Cataloged, not mirrored | Lists ER-DBT020-7 user guide, MCU datasheet, MCU-board schematic, and adapter-board schematic. |
| ER-DBT020-7 user guide | `https://www.buydisplay.com/download/manual/ER-DBT020-7_UserGuide.pdf` | Blocked | User guide confirms the demo-board interface list as SPI plus 6800 8-bit parallel. |
| 8051 MCU-board schematic | `https://www.buydisplay.com/download/manual/8051_MCU_Board%20Schematic.pdf` | Blocked | Useful for demo-board signal naming; not enough by itself to prove panel FPC mapping. |
| Adapter-board schematic | Linked from the ER-DBT020-7 dev-board page | Blocked, exact URL unresolved | Web page link text is visible, but the exact URL did not resolve through the automated tools. Recheck manually. |
| STC12LE5A60S2 MCU datasheet | `https://www.buydisplay.com/download/ic/STC12LE5A60S2-ENG.pdf` | Blocked | Low priority unless the 8051 demo code requires MCU register interpretation. |

## Current Local Files

| Path | Source | Purpose |
| --- | --- | --- |
| `downloads/ST7789vw_existing_local_copy.pdf` | Copied from `docs/lcd/ST7789vw.pdf` | Temporary local ST7789 reference for interface modes, TE commands, and timing. |

## Follow-up Download Checklist

- Download `ER-TFT020-7_Datasheet.pdf`.
- Download `ST7789.pdf` if a fresh BuyDisplay copy is required.
- Download `ER-CON22HT-1.pdf`.
- Download `ER-TFT020-7_8051_Tutorial.zip` and extract it under `extracted`.
- Download `ER-DBT020-7_UserGuide.pdf`.
- Download both dev-board schematics, especially the adapter-board schematic.
- If touch is present, download `DS-CST816S_DS_V1.3.pdf`.
