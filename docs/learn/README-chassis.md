# Hiwonder Ackermann Steering Chassis — 3D Model (GLB)

Parametric 3D model of the [Hiwonder Ackermann Steering Chassis](https://www.hiwonder.com/products/ackermann-steering-chassis)
(Standard version) for NXP Cup instructional material. Built from the manufacturer's
dimensioned drawings and product photos — not photogrammetry — so it is clean,
lightweight (~1.1 MB), and **rigged for animation** with named pivot nodes.

## Files

| File | Purpose |
|---|---|
| `hiwonder_ackermann_chassis.glb` | The model. Drop into Three.js with `GLTFLoader`. |
| `viewer.html` | Standalone preview (white background) — just open it in a browser (needs internet for the Three.js CDN). Has working Ackermann steering + electronic-differential wheel spin; use its code as a reference implementation. |
| `reference/` | Source materials the model was built from: Hiwonder's dimension drawing, product photos, steering/motor close-ups, wiring diagrams, spec tables, and the raw Shopify product data. |
| `camera_sim.html` | Standalone line-camera simulator: drive the chassis on the pista.svg track (true scale) and see the live 320×240 OV5640 feed with adjustable mount height/tilt/FOV, plus 1-D scan-line intensity profiles with edge detection and lane-offset readout — the same processing students implement on the MCXN947. All tunables are in the `CONFIG` object at the top of the module script for easy integration into other lesson pages. |

## Units, axes, origin

- **Units: meters** (glTF standard). The car is true scale: 270 mm long, 197 mm wide, 75 mm tall.
- **Y up, car faces +Z.** Wheel axles run along X.
- **Origin: ground level, centered under the rear axle** — convenient for turn-radius math,
  since the Ackermann turn center always lies on the rear-axle line (y=0, z=0).

## Key dimensions (from Hiwonder's drawing)

| Parameter | Value |
|---|---|
| Overall length (plate) | 270 mm |
| Overall width (over wheels) | 197 mm |
| Wheelbase | 172 mm |
| Track (wheel center to center) | 167 mm |
| Kingpin-to-kingpin distance | 140 mm |
| Wheel diameter × width | 75 × 30 mm |
| Plate thickness | 2 mm |
| Drive | 2 × 12 V encoder gearmotors (rear), electronic differential |
| Steering | LD-1501MG digital servo, front Ackermann linkage |

## Animation-ready node hierarchy

```
AckermannChassis
├── plate_top / plate_base / plate_front / standoffs / battery
├── motor_L, motor_R
├── wheel_RL, wheel_RR          ← rear wheels: spin with rotation.x
└── steering_group
    ├── knuckle_L               ← steer with rotation.y (pivot IS the kingpin)
    │   └── wheel_FL            ← spin with rotation.x
    ├── knuckle_R               ← steer with rotation.y
    │   └── wheel_FR            ← spin with rotation.x
    ├── tie_rod                 ← reposition each frame to stay attached (see viewer.html)
    └── servo → servo_horn      ← rotate .y with the steer input
```

Positive `rotation.y` on a knuckle steers the wheel to the car's **left**.
Positive `rotation.x` on a wheel rolls the car **forward** (+Z).

## Minimal Three.js usage

```js
import { GLTFLoader } from 'three/addons/loaders/GLTFLoader.js';

const gltf = await new GLTFLoader().loadAsync('hiwonder_ackermann_chassis.glb');
scene.add(gltf.scene);

const kL = gltf.scene.getObjectByName('knuckle_L');
const kR = gltf.scene.getObjectByName('knuckle_R');

// True Ackermann: for steer angle d (bicycle model, + = left),
// the inner wheel turns tighter than the outer wheel.
const L = 0.172, T = 0.140;                     // wheelbase, kingpin track
const R = L / Math.tan(Math.abs(d));            // turn radius at rear axle
const inner = Math.atan(L / (R - T / 2));
const outer = Math.atan(L / (R + T / 2));
kL.rotation.y = d > 0 ? inner : -outer;         // left turn → left wheel is inner
kR.rotation.y = d > 0 ? outer : -inner;
```

`viewer.html` additionally shows how to keep the tie rod visually connected and
how to spin the rear wheels at differential speeds `ω·(R ± track/2)/R`.

## Notes

- Materials are simple PBR colors (anodized black aluminum, brass standoffs,
  steel motors, red rim rings) — no textures, so the file stays small and works
  offline. Lighting in your scene determines how good it looks; use an
  environment map or at least a hemisphere + directional light.
- The model is a faithful *stylization*: plate silhouettes, slots, standoffs,
  motors, servo and linkage match the product's layout and dimensions, but hole
  patterns are representative rather than hole-for-hole exact.
- Rebuild/tweak: the generator script is `build_chassis.mjs` (Node + three.js);
  every dimension is a named constant at the top.
