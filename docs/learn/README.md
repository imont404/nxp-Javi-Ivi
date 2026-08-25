# Student Learning Pages

The files directly under `docs/learn` are generated, self-contained lesson
artifacts. `color-spaces.html` covers 24-bit RGB and traditional HSV;
`rgb565-lookup.html` covers RGB565 quantization and the packed YHSV lookup
table. Each HTML file must open from `file://` without a server, network
request, CDN, or companion asset.

Maintain readable source under `docs/learn/src`:

- `lesson.css` is the single shared visual language for the lesson set.
- `*.template.html` contains page structure and inline-asset markers.
- page-specific JavaScript contains the lesson interaction.

Three.js is a pinned build-time dependency under
`docs/learn/vendor/three/_build`. Its generated classic-script IIFE is
committed so ordinary lesson generation does not require npm or network
access. Rebuild it only when changing the pinned runtime or exported addons:

```powershell
cd docs\learn\vendor\three\_build
npm install
npm run build
```

Generate or verify all committed standalone pages from the repository root:

```powershell
python scripts\tools\build_learning_pages.py
python scripts\tools\build_learning_pages.py --check
```

Do not hand-edit `docs/learn/color-spaces.html`; edit its source assets and
regenerate it. New lessons should reuse `lesson.css` and the same packager
contract. A lesson that does not need Three.js should omit that payload while
remaining a single standalone HTML file.
