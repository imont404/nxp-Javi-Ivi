// Parametric model of the Hiwonder Ackermann Steering Chassis
// Exports hiwonder_ackermann_chassis.glb (meters, Y-up, car faces +Z)
// Node hierarchy is animation-ready:
//   knuckle_L / knuckle_R  -> rotate .y to steer (kingpin pivots)
//   wheel_FL / wheel_FR    -> children of knuckles, rotate .x to spin
//   wheel_RL / wheel_RR    -> rotate .x to spin
//   servo_horn, tie_rod    -> steering linkage pieces
// Origin: ground level, centered on the REAR axle.

import * as THREE from 'three';
import { GLTFExporter } from 'three/addons/exporters/GLTFExporter.js';
import { writeFileSync } from 'node:fs';

// ---------- dimensions from Hiwonder drawings (mm) ----------
const MM = 0.001;
const PLATE_T = 2 * MM;          // plate thickness
const WHEEL_R = 37.5 * MM;       // 75 mm dia
const WHEEL_W = 30 * MM;
const TRACK = 167 * MM;          // 197 overall - 30 wheel width (center-to-center)
const WHEELBASE = 172 * MM;
const REAR_OVERHANG = 45 * MM;   // rear axle to rear plate edge
const PLATE_LEN = 270 * MM;
const FRONT_TIP = PLATE_LEN - REAR_OVERHANG;   // z of nose tip = 225 mm
const BASE_TOP_Y = 19 * MM;      // top surface of lower plate (motor shaft at 37.5)
const TOP_TOP_Y = 75 * MM;       // top surface of upper plate (top of wheels)
const KINGPIN_X = 70 * MM;       // kingpin lateral offset
const AXLE_Z_F = WHEELBASE;      // front axle z
const TIEROD_Z = WHEELBASE - 26 * MM;  // tie rod behind front axle
const TIEROD_X = 52 * MM;        // tie rod ball joint lateral offset

// ---------- materials ----------
const mat = {
  plate:  new THREE.MeshStandardMaterial({ name:'anodized_black', color: 0x1b1c1f, metalness: 0.65, roughness: 0.5 }),
  plate2: new THREE.MeshStandardMaterial({ name:'anodized_black2', color: 0x232529, metalness: 0.65, roughness: 0.45 }),
  brass:  new THREE.MeshStandardMaterial({ name:'brass', color: 0xb9892f, metalness: 0.9, roughness: 0.35 }),
  steel:  new THREE.MeshStandardMaterial({ name:'steel', color: 0xb8bcc2, metalness: 0.9, roughness: 0.3 }),
  steelD: new THREE.MeshStandardMaterial({ name:'steel_dark', color: 0x82878d, metalness: 0.9, roughness: 0.4 }),
  tire:   new THREE.MeshStandardMaterial({ name:'rubber', color: 0x151515, metalness: 0.0, roughness: 0.95 }),
  rimRed: new THREE.MeshStandardMaterial({ name:'rim_red', color: 0xd8332a, metalness: 0.4, roughness: 0.4 }),
  rimBlk: new THREE.MeshStandardMaterial({ name:'rim_black', color: 0x111111, metalness: 0.3, roughness: 0.6 }),
  servo:  new THREE.MeshStandardMaterial({ name:'servo_black', color: 0x0e0e10, metalness: 0.2, roughness: 0.7 }),
  knuck:  new THREE.MeshStandardMaterial({ name:'knuckle_black', color: 0x202226, metalness: 0.7, roughness: 0.4 }),
};

const root = new THREE.Group();
root.name = 'AckermannChassis';

// ---------- helper: rounded-rect hole path ----------
function roundedRectPath(cx, cz, w, h, r) {
  cx *= MM; cz *= MM; w *= MM; h *= MM; r *= MM;
  const p = new THREE.Path();
  const x0 = cx - w / 2, y0 = cz - h / 2, x1 = cx + w / 2, y1 = cz + h / 2;
  p.moveTo(x0 + r, y0);
  p.lineTo(x1 - r, y0); p.absarc(x1 - r, y0 + r, r, -Math.PI / 2, 0, false);
  p.lineTo(x1, y1 - r); p.absarc(x1 - r, y1 - r, r, 0, Math.PI / 2, false);
  p.lineTo(x0 + r, y1); p.absarc(x0 + r, y1 - r, r, Math.PI / 2, Math.PI, false);
  p.lineTo(x0, y0 + r); p.absarc(x0 + r, y0 + r, r, Math.PI, Math.PI * 1.5, false);
  return p;
}
function circlePath(cx, cz, r) {
  const p = new THREE.Path();
  p.absarc(cx * MM, cz * MM, r * MM, 0, Math.PI * 2, false);
  return p;
}

// helper: extrude a flat plate from an outline in (x, z) mm coords.
// topY = y of the top surface. Returns mesh.
function plate(outlineMM, holes, topY, material, name) {
  const shape = new THREE.Shape();
  outlineMM.forEach(([x, z], i) => {
    if (i === 0) shape.moveTo(x * MM, z * MM); else shape.lineTo(x * MM, z * MM);
  });
  shape.closePath();
  if (holes) holes.forEach(h => shape.holes.push(h));
  const g = new THREE.ExtrudeGeometry(shape, { depth: PLATE_T, bevelEnabled: false, curveSegments: 10 });
  // shape was built in (x, +z-up-the-plane); extrude goes +z(local). Rotate so plane lies in XZ, thickness in Y.
  g.rotateX(-Math.PI / 2);           // (x, y=depth, z from shape-y)... results in z = -shapeY
  g.scale(1, 1, -1);                  // fix mirroring so shape "up" = +Z (car front)
  g.computeVertexNormals();
  const m = new THREE.Mesh(g, material);
  m.position.y = topY - PLATE_T;      // extrude put thickness from 0..+T in Y
  m.name = name;
  return m;
}

// ---------- upper plate (full length, angular racing silhouette) ----------
const topOutline = [
  // right side, rear -> front (x, z in mm); mirrored for left
  [45, -45], [58, -38], [60, -20],           // chamfered rear corner
  [60, 95], [52, 108],                       // notch inward
  [52, 138], [60, 150],                      // back out at front wing
  [60, 163],
  [22, 213], [13, 225],                      // taper to nose
  [-13, 225], [-22, 213],
  [-60, 163], [-60, 150], [-52, 138],
  [-52, 108], [-60, 95], [-60, -20],
  [-58, -38], [-45, -45],
];
const topHoles = [
  // transverse vent slats over the motor bay
  roundedRectPath(0, -18, 88, 7, 3.5),
  roundedRectPath(0, -2, 88, 7, 3.5),
  roundedRectPath(0, 14, 88, 7, 3.5),
  roundedRectPath(0, 30, 88, 7, 3.5),
  roundedRectPath(0, 46, 88, 7, 3.5),
  // longitudinal slots mid-plate
  roundedRectPath(-36, 85, 8, 46, 4),
  roundedRectPath(36, 85, 8, 46, 4),
  // center rounded window
  roundedRectPath(0, 92, 26, 16, 6),
  // front oval slots
  roundedRectPath(-20, 148, 8, 34, 4),
  roundedRectPath(20, 148, 8, 34, 4),
  // a few mounting holes
  circlePath(0, 190, 2.5), circlePath(-30, 60, 2), circlePath(30, 60, 2),
  circlePath(-40, -30, 2), circlePath(40, -30, 2), circlePath(0, 120, 2),
];
root.add(plate(topOutline, topHoles, TOP_TOP_Y, mat.plate, 'plate_top'));

// ---------- lower plate (rear 2/3, carries the motors) ----------
const baseOutline = [
  [42, -45], [55, -38], [55, 118], [40, 140], [-40, 140], [-55, 118], [-55, -38], [-42, -45],
];
const baseHoles = [
  roundedRectPath(0, 70, 60, 18, 8),
  roundedRectPath(-38, 30, 7, 40, 3.5), roundedRectPath(38, 30, 7, 40, 3.5),
  circlePath(-45, 100, 2), circlePath(45, 100, 2), circlePath(0, 110, 2.5),
];
root.add(plate(baseOutline, baseHoles, BASE_TOP_Y, mat.plate2, 'plate_base'));

// ---------- front lower (steering/servo) plate ----------
const servoPlateTopY = 14 * MM;
const frontOutline = [
  [35, 142], [62, 158], [62, 186], [35, 205], [18, 218], [-18, 218], [-35, 205], [-62, 186], [-62, 158], [-35, 142],
];
const frontHoles = [ circlePath(0, 160, 3), circlePath(-50, 172, 2.5), circlePath(50, 172, 2.5) ];
root.add(plate(frontOutline, frontHoles, servoPlateTopY, mat.plate2, 'plate_front'));

// ---------- brass standoffs ----------
function standoff(x, z, y0, y1, r = 2.5 * MM) {
  const g = new THREE.CylinderGeometry(r, r, y1 - y0, 12);
  const m = new THREE.Mesh(g, mat.brass);
  m.position.set(x * MM, (y0 + y1) / 2, z * MM);
  return m;
}
const standoffs = new THREE.Group();
standoffs.name = 'standoffs';
// rear: base plate -> top plate
[[-48, -35], [48, -35], [-48, 55], [48, 55], [-45, 125], [45, 125]].forEach(([x, z]) =>
  standoffs.add(standoff(x, z, BASE_TOP_Y, TOP_TOP_Y - PLATE_T)));
// front: servo plate -> top plate (long gold posts, visually distinctive)
[[-55, 165], [55, 165], [-30, 205], [30, 205]].forEach(([x, z]) =>
  standoffs.add(standoff(x, z, servoPlateTopY, TOP_TOP_Y - PLATE_T)));
root.add(standoffs);

// ---------- rear drive motors (two silver gearmotors, coaxial on rear axle) ----------
function motor(side) { // side = +1 right, -1 left
  const g = new THREE.Group();
  g.name = side > 0 ? 'motor_R' : 'motor_L';
  const gearbox = new THREE.Mesh(new THREE.CylinderGeometry(18.5 * MM, 18.5 * MM, 42 * MM, 24), mat.steel);
  gearbox.rotation.z = Math.PI / 2;
  gearbox.position.x = side * 48 * MM;
  const body = new THREE.Mesh(new THREE.CylinderGeometry(16.5 * MM, 16.5 * MM, 34 * MM, 24), mat.steelD);
  body.rotation.z = Math.PI / 2;
  body.position.x = side * 20 * MM;
  const cap = new THREE.Mesh(new THREE.CylinderGeometry(16.5 * MM, 16.5 * MM, 4 * MM, 24), mat.servo);
  cap.rotation.z = Math.PI / 2;
  cap.position.x = side * 5 * MM;   // black end cap toward the center
  const shaft = new THREE.Mesh(new THREE.CylinderGeometry(3 * MM, 3 * MM, 16 * MM, 10), mat.steel);
  shaft.rotation.z = Math.PI / 2;
  shaft.position.x = side * 77 * MM;
  const coupling = new THREE.Mesh(new THREE.CylinderGeometry(6 * MM, 6 * MM, 12 * MM, 12), mat.brass);
  coupling.rotation.z = Math.PI / 2;
  coupling.position.x = side * 72 * MM;
  g.add(gearbox, body, cap, shaft, coupling);
  g.position.set(0, WHEEL_R, 0);
  return g;
}
root.add(motor(1), motor(-1));

// ---------- wheel builder (black tire, red rim ring, turbine spokes) ----------
function buildWheel(name) {
  const w = new THREE.Group();
  w.name = name;
  // tire: lathe profile with sloped sidewalls (open at the beads; rim rings close it)
  const half = WHEEL_W / 2;
  const prof = [
    [28 * MM, -half], [36 * MM, -half + 3.5 * MM], [WHEEL_R, -half + 8 * MM],
    [WHEEL_R, half - 8 * MM], [36 * MM, half - 3.5 * MM], [28 * MM, half],
  ].map(([r, y]) => new THREE.Vector2(r, y));
  const tire = new THREE.Mesh(new THREE.LatheGeometry(prof, 36), mat.tire);
  tire.rotation.z = Math.PI / 2;   // axle along X
  w.add(tire);
  for (const s of [1, -1]) { // both faces
    const faceX = s * half;
    // red rim lip: annular torus at the tire bead (like the real red ring)
    const ring = new THREE.Mesh(new THREE.TorusGeometry(26.5 * MM, 2.2 * MM, 10, 36), mat.rimRed);
    ring.rotation.y = Math.PI / 2;   // ring plane perpendicular to axle
    ring.position.x = faceX - s * 1.2 * MM;
    w.add(ring);
    // black dish recessed behind the ring
    const dish = new THREE.Mesh(new THREE.CylinderGeometry(25 * MM, 25 * MM, 2 * MM, 36), mat.rimBlk);
    dish.rotation.z = Math.PI / 2;
    dish.position.x = faceX - s * 5 * MM;
    w.add(dish);
    // turbine spokes, proud of the dish
    for (let i = 0; i < 10; i++) {
      const spoke = new THREE.Mesh(new THREE.BoxGeometry(3 * MM, 4.5 * MM, 20 * MM), mat.rimBlk);
      const a = (i / 10) * Math.PI * 2;
      const rMid = 13 * MM;
      spoke.position.set(faceX - s * 2 * MM, Math.sin(a) * rMid, Math.cos(a) * rMid);
      spoke.rotation.x = -a + s * 0.55;   // rake angle -> turbine look
      w.add(spoke);
    }
    const hub = new THREE.Mesh(new THREE.CylinderGeometry(5.5 * MM, 5.5 * MM, 6 * MM, 16), mat.steel);
    hub.rotation.z = Math.PI / 2;
    hub.position.x = faceX - s * 1.5 * MM;
    w.add(hub);
  }
  return w;
}

// rear wheels: direct children of root, at rear axle
const wheelRL = buildWheel('wheel_RL');
wheelRL.position.set(-TRACK / 2, WHEEL_R, 0);
const wheelRR = buildWheel('wheel_RR');
wheelRR.position.set(TRACK / 2, WHEEL_R, 0);
root.add(wheelRL, wheelRR);

// ---------- steering: knuckles (kingpin pivots) with front wheels as children ----------
const steering = new THREE.Group();
steering.name = 'steering_group';

function knuckle(side) { // side -1 = left (car's left = -x)
  const k = new THREE.Group();
  k.name = side < 0 ? 'knuckle_L' : 'knuckle_R';
  k.position.set(side * KINGPIN_X, WHEEL_R, AXLE_Z_F);  // pivot at kingpin

  // kingpin post (vertical brass pin)
  const pin = new THREE.Mesh(new THREE.CylinderGeometry(2.5 * MM, 2.5 * MM, 34 * MM, 10), mat.brass);
  pin.position.y = 0;
  k.add(pin);
  // C-shaped knuckle body
  const body = new THREE.Mesh(new THREE.BoxGeometry(10 * MM, 26 * MM, 14 * MM), mat.knuck);
  body.position.set(side * 4 * MM, 0, 0);
  k.add(body);
  // stub axle out to the wheel
  const stub = new THREE.Mesh(new THREE.CylinderGeometry(3 * MM, 3 * MM, (TRACK / 2 - KINGPIN_X) * 1.9, 10), mat.steel);
  stub.rotation.z = Math.PI / 2;
  stub.position.set(side * (TRACK / 2 - KINGPIN_X) / 2, 0, 0);
  k.add(stub);
  // steering arm: angles inward+rearward to the tie-rod joint
  const armLen = Math.hypot((KINGPIN_X - TIEROD_X), (AXLE_Z_F - TIEROD_Z));
  const arm = new THREE.Mesh(new THREE.BoxGeometry(5 * MM, 4 * MM, armLen), mat.knuck);
  const midX = side * (TIEROD_X - KINGPIN_X) / 2;
  const midZ = (TIEROD_Z - AXLE_Z_F) / 2;
  arm.position.set(midX, -6 * MM, midZ);
  arm.rotation.y = Math.atan2(side * (TIEROD_X - KINGPIN_X), TIEROD_Z - AXLE_Z_F);
  k.add(arm);
  // ball joint
  const ball = new THREE.Mesh(new THREE.SphereGeometry(3.2 * MM, 12, 8), mat.steel);
  ball.position.set(side * (TIEROD_X - KINGPIN_X), -6 * MM, TIEROD_Z - AXLE_Z_F);
  k.add(ball);

  const wheel = buildWheel(side < 0 ? 'wheel_FL' : 'wheel_FR');
  wheel.position.set(side * (TRACK / 2 - KINGPIN_X), 0, 0);
  k.add(wheel);
  return k;
}
steering.add(knuckle(-1), knuckle(1));

// tie rod (named so a lesson can reposition it with the steering angle)
const tieRod = new THREE.Mesh(
  new THREE.CylinderGeometry(2.5 * MM, 2.5 * MM, TIEROD_X * 2, 10), mat.steel);
tieRod.rotation.z = Math.PI / 2;
tieRod.name = 'tie_rod';
tieRod.position.set(0, WHEEL_R - 6 * MM, TIEROD_Z);
steering.add(tieRod);

// ---------- steering servo (LD-1501MG, hangs in the front bay) ----------
const servoG = new THREE.Group();
servoG.name = 'servo';
const servoBody = new THREE.Mesh(new THREE.BoxGeometry(20 * MM, 40.5 * MM, 40 * MM), mat.servo);
servoBody.position.set(0, servoPlateTopY + 20.25 * MM, 168 * MM);
servoG.add(servoBody);
// mounting flanges
const flange = new THREE.Mesh(new THREE.BoxGeometry(24 * MM, 3 * MM, 54 * MM), mat.servo);
flange.position.set(0, servoPlateTopY + 32 * MM, 168 * MM);
servoG.add(flange);
// output shaft + horn (named, rotates .y to drive the linkage)
const horn = new THREE.Group();
horn.name = 'servo_horn';
horn.position.set(0, servoPlateTopY + 42 * MM, 156 * MM);
const hornShaft = new THREE.Mesh(new THREE.CylinderGeometry(3 * MM, 3 * MM, 6 * MM, 12), mat.steel);
horn.add(hornShaft);
const hornArm = new THREE.Mesh(new THREE.BoxGeometry(4 * MM, 3 * MM, 18 * MM), mat.knuck);
hornArm.position.z = -8 * MM;
horn.add(hornArm);
servoG.add(horn);
// drag link down to tie rod
const dragLink = new THREE.Mesh(new THREE.CylinderGeometry(1.5 * MM, 1.5 * MM, 14 * MM, 8), mat.steel);
dragLink.name = 'drag_link';
dragLink.position.set(0, WHEEL_R + 2 * MM, TIEROD_Z + 2 * MM);
dragLink.rotation.x = 0.5;
servoG.add(dragLink);
steering.add(servoG);

root.add(steering);

// ---------- battery box on the base plate (visual filler, like photos) ----------
const battery = new THREE.Mesh(new THREE.BoxGeometry(60 * MM, 18 * MM, 90 * MM), mat.servo);
battery.name = 'battery';
battery.position.set(0, BASE_TOP_Y + 9 * MM, 78 * MM);
root.add(battery);

// ---------- export ----------
const scene = new THREE.Scene();
scene.add(root);

// minimal polyfills for GLTFExporter in Node
globalThis.FileReader = class {
  readAsArrayBuffer(blob) {
    blob.arrayBuffer().then((ab) => { this.result = ab; this.onloadend?.(); });
  }
  readAsDataURL(blob) {
    blob.arrayBuffer().then((ab) => {
      this.result = 'data:application/octet-stream;base64,' + Buffer.from(ab).toString('base64');
      this.onloadend?.();
    });
  }
};

const exporter = new GLTFExporter();
exporter.parse(
  scene,
  (result) => {
    writeFileSync('hiwonder_ackermann_chassis.glb', Buffer.from(result));
    console.log('wrote hiwonder_ackermann_chassis.glb', result.byteLength, 'bytes');
  },
  (err) => { console.error('EXPORT ERROR', err); process.exit(1); },
  { binary: true }
);
