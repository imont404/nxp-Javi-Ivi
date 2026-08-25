# Active Plans

Active execution plans and attached work logs live under this folder.

Use the `dev-std` plan commands from the repository root:

```powershell
uvx --from wn-dev-std==2026.7.18 dev-std plan list
uvx --from wn-dev-std==2026.7.18 dev-std plan create flexio-camera --title "FlexIO Camera Bringup"
uvx --from wn-dev-std==2026.7.18 dev-std plan show flexio-camera
```

Reference material, design notes, and teaching material stay outside this
folder. Only files under `docs/plans` are active plan roots.
