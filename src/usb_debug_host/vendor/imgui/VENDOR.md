# Dear ImGui vendor record

- Upstream: `https://github.com/ocornut/imgui`
- Version: `v1.91.9b`
- License: MIT; see `LICENSE.txt` in this directory.
- Imported: 2026-08-23

This retained subset contains the Dear ImGui core plus the SDL2 and SDL_Renderer2
backends used by `avc_viewer`. Keeping these sources in the repository makes the
host build independent of GitHub and of the Bunny Vision working tree.
