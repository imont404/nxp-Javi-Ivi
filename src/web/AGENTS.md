# Shared Web Dashboard Agent Guide

This component owns transport-neutral dashboard markup, styling, and presentation
logic plus the deterministic generation of the host WebSerial and Android relay pages.

Keep WebSerial, USB control, and direct RGB565 assembly in the host adapter. Keep
WebSocket setup, relay video selection, `AVCJ`/`AVC4`/`AVCR`, Media Source Extensions,
and relayed `AVCU` parsing in the Android adapter. Generated pages must be standalone,
committed, free of runtime network assets, and current according to `build.ps1 -Check`.

Do not change firmware protocol values, Android native lifecycle ownership, relay queue
bounds, or vehicle controls while editing dashboard presentation.
