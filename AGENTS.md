# Agent Guide

This repository is the organizer-supplied Wavenumber NXP Cup platform for FIT.
It is tuned by students with only a few days of exposure, so favor obvious flows,
visible failures, bounded runtime behavior, and a small editable surface.

## Start here

- Read `README.md` and `src/README.md` for setup and component ownership.
- Read the nearest component `AGENTS.md` before editing under `src`.
- Read `docs/design/firmware-framework.md` for the student API, runtime flow,
  and safety boundaries; hardware reference material lives under
  `docs/components` and `docs/car_chassis`.
- Active execution plans live under `docs/plans`; broad documentation
  reorganization may occur independently, so avoid unrelated docs edits.

Use the pinned plan tooling from the repository root:

```powershell
uvx --from wn-dev-std==2026.7.18 dev-std plan list
uvx --from wn-dev-std==2026.7.18 dev-std plan show <plan-id>
uvx --from wn-dev-std==2026.7.18 dev-std audit . --scope docs.plans
```

## Component commands

```powershell
.\setup.ps1
.\src\embedded\build.ps1
.\src\host\build.ps1
.\src\android\setup.ps1 -AcceptLicenses
.\src\android\build.ps1
```

Repository-owned build products and distributable artifacts belong under
`out`. Do not restore root-level component build wrappers or introduce another
student build/flash workflow.

## Safety and boundaries

- The ordinary embedded build is the Rev A `competition` image: EZH camera,
  ER-TFT020-3 SPI LCD, session-gated USB telemetry, and QDC wheel feedback.
- The normal flash command tries ROM-HID first and falls back to J-Link Commander.
  RTT scripts under `src/embedded/tools` are maintainer tooling.
- Do not supply line following, PID, active differential, or a completed race
  solution. The framework supplies safe primitives; students write the algorithm.
- Preserve the version-1 `AVCU` wire bytes and USB VID/PID unless a plan explicitly
  authorizes a compatibility break.
- Do not change repository remotes, push, or reorganize broad documentation as an
  incidental implementation step.
