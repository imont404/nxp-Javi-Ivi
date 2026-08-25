+++
type = "plan"
id = "student-learning-site"
status = "pending"
created = "2026-08-22"

[[steps]]
id = "capture-teaching-intent"
title = "Capture the audience, teaching sequence, terminology, and framework-versus-solution boundary"
status = "done"

[[steps]]
id = "seed-color-space-visualization"
title = "Seed a dependency-free RGB565 and HSV interactive visualization that works offline and on a static host"
status = "done"
depends_on = ["capture-teaching-intent"]

[[steps]]
id = "site-information-architecture"
title = "Define the multi-page English student site, navigation, presentation mode, setup ownership, and durable URLs"
status = "pending"
depends_on = ["seed-color-space-visualization"]

[[steps]]
id = "shared-visual-language"
title = "Create projector-readable shared styling and reusable interactive components without a build-time framework"
status = "pending"
depends_on = ["site-information-architecture"]

[[steps]]
id = "camera-color-lesson"
title = "Complete the camera, RGB565 quantization, RGB cube, HSV cone, hue-confidence, and LUT lesson"
status = "pending"
depends_on = ["shared-visual-language"]

[[steps]]
id = "pixel-and-scanline-lab"
title = "Add an Excel-like pixel grid and scanline laboratory for thresholds, masks, timing, and student-authored edge logic"
status = "pending"
depends_on = ["camera-color-lesson"]

[[steps]]
id = "embedded-control-lessons"
title = "Add frame-budget, motor, steering, encoder-feedback, and PID-concept pages without providing race algorithms"
status = "pending"
depends_on = ["shared-visual-language"]

[[steps]]
id = "setup-integration"
title = "Integrate the authoritative setup workflow without duplicating or weakening docs/setup.html"
status = "pending"
depends_on = ["site-information-architecture"]

[[steps]]
id = "live-camera-option"
title = "Evaluate optional Web Serial frame input while preserving sample-data and offline teaching paths"
status = "pending"
depends_on = ["pixel-and-scanline-lab"]

[[steps]]
id = "static-host-decision"
title = "Select and prove GitHub Pages or another static host with stable links and no server-side requirements"
status = "pending"
depends_on = ["site-information-architecture"]

[[steps]]
id = "offline-package"
title = "Produce and validate a downloadable offline copy for unreliable conference and travel networks"
status = "pending"
depends_on = ["camera-color-lesson", "setup-integration"]

[[steps]]
id = "spanish-materials"
title = "Translate the frozen lessons and have technical teaching language reviewed by a Spanish-capable reviewer"
status = "pending"
depends_on = ["camera-color-lesson", "pixel-and-scanline-lab", "embedded-control-lessons", "setup-integration"]

[[steps]]
id = "classroom-rehearsal"
title = "Rehearse the site on the actual projector and student devices, including keyboard, touch, offline, and low-resolution use"
status = "pending"
depends_on = ["offline-package", "static-host-decision"]

[[steps]]
id = "design-doc-intent-audit"
title = "Audit every lesson against firmware definitions, course intent, and the no-race-solution boundary"
status = "pending"
depends_on = ["spanish-materials", "classroom-rehearsal"]

[[steps]]
id = "test-runtime-impact-audit"
title = "Audit browser coverage, accessibility checks, link checks, offline packaging, and test runtime"
status = "pending"
depends_on = ["classroom-rehearsal"]

[[steps]]
id = "external-review"
title = "Obtain independent technical, teaching, and Spanish-language review"
status = "pending"
depends_on = ["design-doc-intent-audit", "test-runtime-impact-audit"]

[[exit_criteria]]
id = "conceptual-accuracy"
title = "RGB, RGB565, YHSV, hue reliability, and camera-pipeline explanations match the firmware and cited definitions"
status = "pending"

[[exit_criteria]]
id = "interactive-not-solution"
title = "Interactive lessons teach primitives and experimentation without supplying line-following, steering, PID, or race strategy"
status = "pending"

[[exit_criteria]]
id = "offline-static"
title = "The complete site works from static hosting and from the downloadable offline package without network dependencies"
status = "pending"

[[exit_criteria]]
id = "setup-single-source"
title = "The site exposes one authoritative setup workflow and does not create conflicting build or flash instructions"
status = "pending"

[[exit_criteria]]
id = "classroom-usable"
title = "English and Spanish materials are readable on the projector and usable by keyboard, mouse, and touch"
status = "pending"

[[exit_criteria]]
id = "stable-publication"
title = "Published URLs and the offline artifact are documented, reproducible, and tested"
status = "pending"

[[exit_criteria]]
id = "design-doc-intent-audit"
title = "Lessons match implementation and organizer intent"
status = "pending"

[[exit_criteria]]
id = "test-runtime-impact-audit"
title = "Browser tests and runtime impact are reviewed"
status = "pending"

[[exit_criteria]]
id = "external-review"
title = "Independent review is complete"
status = "pending"
+++

# Interactive Student Learning Site

## Audience and Teaching Goal

The primary audience is third- and fourth-year electrical-engineering students who may
have used RGB values but have never studied color spaces or computer vision. The material
should connect familiar Cartesian coordinates to camera pixels, transformations, embedded
representation, and algorithms that operate under a real-time budget.

The site is both a presentation surface and an individual laboratory. It should replace
blackboard-only diagrams while preserving the instructor's ability to reveal a concept in
small steps, manipulate it live, and invite predictions before showing the result.

## Color-Space Lesson Intent

Start with the physical path: Bayer sensor, camera ISP, RGB565 output, MCU memory. Show RGB
as a Cartesian cube and demonstrate why a useful concept such as “red” occupies an awkward
three-dimensional region whose coordinates change with intensity and illumination.

Transform the same selected color into HSV. Teach hue as an angle, saturation as distance
from the gray axis, and value as brightness. Use a cone/hexcone visualization—not as a
claim that HSV has only one drawing convention, but because radius proportional to chroma
(`S * V`) visibly squeezes toward black. Mark hue as unreliable when saturation or value
is low. The existing red filter is an example of composing H/S/V gates, not a supplied
race solution or a privileged color class.

Tie the visualization to the implemented RGB565 lookup table: 65,536 possible input
values, one packed Y/H/S/V output, deterministic host generation, and the reason a change
of representation can turn expensive arithmetic into a bounded lookup.

## Static and Offline Architecture

Keep student pages free of runtime and network dependencies. They must open directly from
disk and from ordinary static hosting. Do not depend on a CDN, server runtime, package
installation, or live hardware for the core lesson. Pinned build-time dependencies are
allowed when their runtime, license notice, lesson CSS, and page JavaScript are embedded
into a reproducibly generated standalone HTML artifact. Three.js uses this model for the
interactive coordinate solids. Optional live Web Serial input may enhance a page but must
not replace bundled examples.

Maintain one canonical lesson stylesheet in the authored source tree and inline that same
stylesheet into each standalone output. Shared navigation and layout should grow from this
source rather than allowing generated pages to become unrelated one-offs.

Keep `docs/setup.html` authoritative until the information-architecture step explicitly
defines a replacement. A static site may link or visually incorporate it, but must not
fork its commands. GitHub Pages is the leading deployment candidate because the repository
already carries static documentation under `docs/`; the plan retains a decision step so
hosting, access, stable URLs, and offline delivery are proven rather than assumed.

The setup-integration step should wait until the English setup content is frozen by the
competition code/documentation cleanup. The optional live-camera step should wait until
USB telemetry has completed realistic transport validation. These are cross-plan
prerequisites recorded here in prose because dev-standard dependency IDs are local to one
plan.

## Student Boundary

The learning site may demonstrate coordinate transforms, quantization, thresholds, masks,
timing, feedback, and generic control concepts. It must not supply a completed edge
detector, lane-center estimator, steering policy, speed controller, PID implementation, or
race strategy. Interactive examples should end at the same framework boundary as the
student firmware.

## Teaching References

- [The Power of Transformation: NXP in Embedded Systems Education](https://www.nxp.com/company/about-nxp/smarter-world-blog/BL-THE-POWER-OF-TRANSFORMATION)
  records the course's existing RGB-to-HSV teaching motivation and the connection among
  mathematics, software, and sensors.
- `scripts/tools/generate_rgb565_color_lut.py` is authoritative for the RGB565 expansion,
  BT.601 luma approximation, HSV conversion, byte quantization, and packed LUT layout.
- `docs/research/AVC_Vision_Pipeline_Design.md` owns the durable embedded vision design and
  performance context.
