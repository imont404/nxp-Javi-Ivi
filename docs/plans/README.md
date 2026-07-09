# Active Plans

Active execution plans and attached work logs live under this folder.

Use the `dev-std` plan commands from the repository root:

```powershell
uvx --from wn-dev-std==2026.7.2 dev-std plan list
uvx --from wn-dev-std==2026.7.2 dev-std plan create flexio-camera --title "FlexIO Camera Bringup"
uvx --from wn-dev-std==2026.7.2 dev-std plan show flexio-camera
```

Reference material, imported SDK examples, and research notes stay outside this
folder. In particular, `docs/research` and `docs/examples/frdmmcxn947` are
source material for planning and implementation, not active plan roots.
