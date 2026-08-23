# NXP Cup Product Identity and Compatibility

The current organizer-supplied platform is the **Wavenumber NXP Cup** project. New
repository, firmware, host-tool, Android, artifact, and documentation names use
`nxp_cup`, `nxp-cup`, or the compact C namespace `nxpc` according to the conventions
of each platform.

The migration is replayed by
`scripts/tools/refactor_product_identity.py` from the manifest beside it. The checked
baseline is the Git ref `nxpc-pre-refactor`. The tool performs a dry run by default,
rejects collisions, requires the exact clean baseline before applying changes, and
checks for stale owned identifiers and paths afterward.

## Classification

| Occurrence | Treatment |
| --- | --- |
| Owned firmware identifiers and paths | Rename from `avc`/`AVC` to `nxpc`/`NXPC` |
| Firmware project and artifacts | Use `nxp_cup_core0`; the legacy name exists only in baseline/tag history |
| Public API | Keep short domain names in `nxp_cup.h`; do not expose internal prefixes |
| Host and Android products | Rename to NXP Cup names with all in-repository consumers moved together |
| Protocol source identifiers | Rename to `nxpc`, preserving every numeric value and packed layout |
| Protocol wire magic | Preserve the four version-1 bytes `AVCU` (`0x55435641` little-endian) |
| USB identities | Preserve runtime `1FC9:0094` and ROM-HID `1FC9:014F` |
| H.264 Advanced Video Coding | Preserve standards terms such as `video/avc` |
| Research, examples, plan work logs, and imported/vendor content | Preserve as historical or third-party material |
| Current prose | Use NXP Cup; use AVC only when explaining historical naming |

The compatibility boundary is behavior and wire values, not legacy source names. The
firmware, browser viewer, native host tool, and Android bridge are rebuilt together so
their private types and constants can be renamed without creating aliases in the small
firmware API.
