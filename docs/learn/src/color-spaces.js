(() => {
  "use strict";

  const $ = (id) => document.getElementById(id);
  const clamp = (value, minimum, maximum) => Math.min(maximum, Math.max(minimum, value));
  const roundByte = (value) => clamp(Math.floor(value * 255 + 0.5), 0, 255);
  const toHexByte = (value) => Math.round(value).toString(16).padStart(2, "0").toUpperCase();
  const rgbHex = (rgb) => `#${toHexByte(rgb.r)}${toHexByte(rgb.g)}${toHexByte(rgb.b)}`;

  function rgbToHsv({ r, g, b }) {
    const red = r / 255;
    const green = g / 255;
    const blue = b / 255;
    const maximum = Math.max(red, green, blue);
    const minimum = Math.min(red, green, blue);
    const delta = maximum - minimum;
    let hue = 0;

    if (delta > 0) {
      if (maximum === red) hue = 60 * (((green - blue) / delta) % 6);
      else if (maximum === green) hue = 60 * (((blue - red) / delta) + 2);
      else hue = 60 * (((red - green) / delta) + 4);
    }
    if (hue < 0) hue += 360;
    return {
      h: hue,
      s: maximum === 0 ? 0 : delta / maximum,
      v: maximum,
    };
  }

  function hsvToRgb({ h, s, v }) {
    const chroma = v * s;
    const sector = (((h % 360) + 360) % 360) / 60;
    const intermediate = chroma * (1 - Math.abs((sector % 2) - 1));
    let prime = [0, 0, 0];
    if (sector < 1) prime = [chroma, intermediate, 0];
    else if (sector < 2) prime = [intermediate, chroma, 0];
    else if (sector < 3) prime = [0, chroma, intermediate];
    else if (sector < 4) prime = [0, intermediate, chroma];
    else if (sector < 5) prime = [intermediate, 0, chroma];
    else prime = [chroma, 0, intermediate];
    const match = v - chroma;
    return {
      r: roundByte(prime[0] + match),
      g: roundByte(prime[1] + match),
      b: roundByte(prime[2] + match),
    };
  }

  function threeColorFromHsv(hue, saturation, value) {
    const rgb = hsvToRgb({ h: hue * 360, s: saturation, v: value });
    return new THREE.Color().setRGB(
      rgb.r / 255,
      rgb.g / 255,
      rgb.b / 255,
      THREE.SRGBColorSpace,
    );
  }

  function makeLine(points, material) {
    const geometry = new THREE.BufferGeometry().setFromPoints(points);
    const line = new THREE.Line(geometry, material);
    if (material.isLineDashedMaterial) line.computeLineDistances();
    return line;
  }

  function replaceLinePoints(line, points) {
    line.geometry.dispose();
    line.geometry = new THREE.BufferGeometry().setFromPoints(points);
    if (line.material.isLineDashedMaterial) line.computeLineDistances();
  }

  function createPoint() {
    const group = new THREE.Group();
    const fill = new THREE.Mesh(
      new THREE.SphereGeometry(0.0425, 24, 16),
      new THREE.MeshBasicMaterial({ color: 0xe53935 }),
    );
    group.add(fill);
    group.userData.fill = fill;
    group.renderOrder = 10;
    return group;
  }

  function createAxisLabel(text, color) {
    const canvas = document.createElement("canvas");
    canvas.width = 128;
    canvas.height = 128;
    const context = canvas.getContext("2d");
    const cssColor = `#${color.toString(16).padStart(6, "0")}`;
    context.fillStyle = "rgba(255, 255, 255, 0.9)";
    context.fillRect(12, 12, 104, 104);
    context.strokeStyle = cssColor;
    context.lineWidth = 7;
    context.strokeRect(12, 12, 104, 104);
    context.fillStyle = cssColor;
    context.font = "700 68px monospace";
    context.textAlign = "center";
    context.textBaseline = "middle";
    context.fillText(text, 64, 68);

    const texture = new THREE.CanvasTexture(canvas);
    texture.colorSpace = THREE.SRGBColorSpace;
    const sprite = new THREE.Sprite(new THREE.SpriteMaterial({
      map: texture,
      transparent: true,
      depthTest: false,
    }));
    sprite.scale.set(0.28, 0.28, 1);
    sprite.renderOrder = 20;
    return sprite;
  }

  class ThreeView {
    constructor(hostId, cameraPosition) {
      this.host = $(hostId);
      this.scene = new THREE.Scene();
      this.scene.background = new THREE.Color(0xffffff);
      this.camera = new THREE.PerspectiveCamera(34, 1, 0.01, 100);
      this.camera.position.fromArray(cameraPosition);
      this.renderer = new THREE.WebGLRenderer({ antialias: true, powerPreference: "high-performance" });
      this.renderer.setPixelRatio(Math.min(window.devicePixelRatio || 1, 2));
      this.renderer.outputColorSpace = THREE.SRGBColorSpace;
      this.host.prepend(this.renderer.domElement);
      this.orbit = new THREE.OrbitControls(this.camera, this.renderer.domElement);
      this.orbit.target.set(0, 0, 0);
      this.orbit.enableDamping = true;
      this.orbit.dampingFactor = 0.08;
      this.orbit.minDistance = 3.2;
      this.orbit.maxDistance = 8;
      this.resizeObserver = new ResizeObserver(() => this.resize());
      this.resizeObserver.observe(this.host);
      this.resize();
    }

    resize() {
      const width = Math.max(280, this.host.clientWidth);
      const height = Math.max(280, this.host.clientHeight);
      this.camera.aspect = width / height;
      this.camera.updateProjectionMatrix();
      this.renderer.setSize(width, height, false);
    }

    render() {
      this.orbit.update();
      this.renderer.render(this.scene, this.camera);
    }
  }

  class RgbCubeView extends ThreeView {
    constructor() {
      super("rgb-scene", [4.2, 3.4, 4.5]);
      const edges = new THREE.LineSegments(
        new THREE.EdgesGeometry(new THREE.BoxGeometry(2, 2, 2)),
        new THREE.LineDashedMaterial({
          color: 0x111111,
          dashSize: 0.09,
          gapSize: 0.055,
          transparent: true,
          opacity: 0.5,
        }),
      );
      edges.computeLineDistances();
      this.scene.add(edges);

      const origin = new THREE.Vector3(-1, -1, -1);
      this.scene.add(
        new THREE.ArrowHelper(new THREE.Vector3(1, 0, 0), origin, 2.45, 0xc01c28, 0.15, 0.08),
        new THREE.ArrowHelper(new THREE.Vector3(0, 1, 0), origin, 2.45, 0x167545, 0.15, 0.08),
        new THREE.ArrowHelper(new THREE.Vector3(0, 0, 1), origin, 2.45, 0x1c54a4, 0.15, 0.08),
      );
      const redLabel = createAxisLabel("R", 0xc01c28);
      const greenLabel = createAxisLabel("G", 0x167545);
      const blueLabel = createAxisLabel("B", 0x1c54a4);
      redLabel.position.set(1.62, -1, -1);
      greenLabel.position.set(-1, 1.62, -1);
      blueLabel.position.set(-1, -1, 1.62);
      this.scene.add(redLabel, greenLabel, blueLabel);

      this.point = createPoint();
      this.scene.add(this.point);
    }

    setColor(rgb) {
      this.point.position.set(
        (rgb.r / 255) * 2 - 1,
        (rgb.g / 255) * 2 - 1,
        (rgb.b / 255) * 2 - 1,
      );
      this.point.userData.fill.material.color.setRGB(rgb.r / 255, rgb.g / 255, rgb.b / 255, THREE.SRGBColorSpace);
    }
  }

  function hueRingGeometry(value) {
    const points = [];
    const colors = [];
    for (let index = 0; index <= 128; index += 1) {
      const hue = index / 128;
      const angle = hue * Math.PI * 2;
      points.push(new THREE.Vector3(value * Math.cos(angle), value * 2 - 1, value * Math.sin(angle)));
      const color = threeColorFromHsv(hue, 1, value);
      colors.push(color.r, color.g, color.b);
    }
    const geometry = new THREE.BufferGeometry().setFromPoints(points);
    geometry.setAttribute("color", new THREE.Float32BufferAttribute(colors, 3));
    return geometry;
  }

  function hueRing(value, opacity = 0.75) {
    return new THREE.Line(
      hueRingGeometry(value),
      new THREE.LineBasicMaterial({ vertexColors: true, transparent: true, opacity }),
    );
  }

  function circlePoints(radius, height, segments = 128) {
    const points = [];
    for (let index = 0; index <= segments; index += 1) {
      const angle = index * Math.PI * 2 / segments;
      points.push(new THREE.Vector3(radius * Math.cos(angle), height, radius * Math.sin(angle)));
    }
    return points;
  }

  function hsvDiskGeometry(value) {
    const positions = [];
    const colors = [];
    const centerColor = threeColorFromHsv(0, 0, value);
    for (let segment = 0; segment < 96; segment += 1) {
      const h0 = segment / 96;
      const h1 = (segment + 1) / 96;
      positions.push(0, 0, 0);
      colors.push(centerColor.r, centerColor.g, centerColor.b);
      [h0, h1].forEach((hue) => {
        const angle = hue * Math.PI * 2;
        positions.push(value * Math.cos(angle), value * Math.sin(angle), 0);
        const color = threeColorFromHsv(hue, 1, value);
        colors.push(color.r, color.g, color.b);
      });
    }
    const geometry = new THREE.BufferGeometry();
    geometry.setAttribute("position", new THREE.Float32BufferAttribute(positions, 3));
    geometry.setAttribute("color", new THREE.Float32BufferAttribute(colors, 3));
    return geometry;
  }

  class HsvConeView extends ThreeView {
    constructor() {
      super("hsv-scene", [3.7, 2.8, 4.2]);
      this.scene.add(makeLine(
        circlePoints(1, 1),
        new THREE.LineBasicMaterial({ color: 0x111111, transparent: true, opacity: 0.82 }),
      ));
      this.scene.add(makeLine(
        [new THREE.Vector3(0, -1, 0), new THREE.Vector3(0, 1.32, 0)],
        new THREE.LineBasicMaterial({ color: 0x111111, transparent: true, opacity: 0.75 }),
      ));
      const hueLabel = createAxisLabel("H", 0x111111);
      const valueLabel = createAxisLabel("V", 0x111111);
      hueLabel.position.set(1.18, 1, 0);
      valueLabel.position.set(0, 1.48, 0);
      this.scene.add(hueLabel, valueLabel);
      for (let index = 0; index < 12; index += 1) {
        const angle = index * Math.PI / 6;
        this.scene.add(makeLine(
          [new THREE.Vector3(0, -1, 0), new THREE.Vector3(Math.cos(angle), 1, Math.sin(angle))],
          new THREE.LineBasicMaterial({ color: 0x111111, transparent: true, opacity: 0.13 }),
        ));
      }

      this.currentRing = hueRing(0.9, 1);
      this.scene.add(this.currentRing);
      this.slice = new THREE.Mesh(
        hsvDiskGeometry(0.9),
        new THREE.MeshBasicMaterial({
          vertexColors: true,
          transparent: true,
          opacity: 0.1,
          side: THREE.DoubleSide,
          depthWrite: false,
        }),
      );
      this.slice.rotation.x = Math.PI / 2;
      this.slice.position.y = 0.8;
      this.scene.add(this.slice);
      this.sliceOutline = makeLine(
        circlePoints(0.912, 0.8),
        new THREE.LineBasicMaterial({ color: 0x111111, transparent: true, opacity: 0.9 }),
      );
      this.scene.add(this.sliceOutline);
      this.point = createPoint();
      this.scene.add(this.point);
      this.radiusGuide = makeLine(
        [new THREE.Vector3(), new THREE.Vector3()],
        new THREE.LineDashedMaterial({
          color: 0x111111,
          dashSize: 0.045,
          gapSize: 0.035,
          transparent: true,
          opacity: 0.58,
        }),
      );
      this.scene.add(this.radiusGuide);
      this.saturationLabel = createAxisLabel("S", 0x111111);
      this.saturationLabel.scale.set(0.22, 0.22, 1);
      this.scene.add(this.saturationLabel);
    }

    setColor(hsv, rgb) {
      const angle = hsv.h * Math.PI / 180;
      const radius = hsv.s * hsv.v;
      this.point.position.set(radius * Math.cos(angle), hsv.v * 2 - 1, radius * Math.sin(angle));
      this.point.userData.fill.material.color.setRGB(rgb.r / 255, rgb.g / 255, rgb.b / 255, THREE.SRGBColorSpace);
      this.currentRing.geometry.dispose();
      this.currentRing.geometry = hueRingGeometry(hsv.v);
      this.slice.geometry.dispose();
      this.slice.geometry = hsvDiskGeometry(hsv.v);
      this.slice.position.y = hsv.v * 2 - 1;
      replaceLinePoints(
        this.sliceOutline,
        circlePoints(hsv.v + 0.012, hsv.v * 2 - 1),
      );
      this.sliceOutline.visible = hsv.v > 0.005;
      replaceLinePoints(this.radiusGuide, [
        new THREE.Vector3(0, hsv.v * 2 - 1, 0),
        this.point.position.clone(),
      ]);
      this.saturationLabel.position.set(
        this.point.position.x * 0.52,
        hsv.v * 2 - 1 + 0.055,
        this.point.position.z * 0.52,
      );
      this.saturationLabel.visible = radius > 0.06;
    }
  }

  class HsvSliceView {
    constructor() {
      this.host = $("hsv-slice-scene");
      this.scene = new THREE.Scene();
      this.scene.background = new THREE.Color(0xffffff);
      this.camera = new THREE.OrthographicCamera(-1.15, 1.15, 1.15, -1.15, 0.01, 10);
      this.camera.position.set(0, 0, 5);
      this.renderer = new THREE.WebGLRenderer({ antialias: true, powerPreference: "high-performance" });
      this.renderer.setPixelRatio(Math.min(window.devicePixelRatio || 1, 2));
      this.renderer.outputColorSpace = THREE.SRGBColorSpace;
      this.host.prepend(this.renderer.domElement);

      this.disk = new THREE.Mesh(
        hsvDiskGeometry(0.9),
        new THREE.MeshBasicMaterial({ vertexColors: true, side: THREE.DoubleSide }),
      );
      this.scene.add(this.disk);
      this.referenceRing = makeLine(
        circlePoints(1, 0).map(({ x, z }) => new THREE.Vector3(x, z, 0.01)),
        new THREE.LineDashedMaterial({
          color: 0x111111,
          dashSize: 0.055,
          gapSize: 0.04,
          transparent: true,
          opacity: 0.5,
        }),
      );
      this.outerRing = makeLine(
        circlePoints(0.9, 0).map(({ x, z }) => new THREE.Vector3(x, z, 0.02)),
        new THREE.LineBasicMaterial({ color: 0x111111, transparent: true, opacity: 0.9 }),
      );
      this.selectedRadiusLine = makeLine(
        [new THREE.Vector3(0, 0, 0.03), new THREE.Vector3(0.7, 0, 0.03)],
        new THREE.LineBasicMaterial({ color: 0x111111, transparent: true, opacity: 0.72 }),
      );
      this.scene.add(this.referenceRing, this.outerRing, this.selectedRadiusLine);

      this.pointMarker = new THREE.Group();
      const pointOutline = new THREE.Mesh(
        new THREE.CircleGeometry(0.0375, 32),
        new THREE.MeshBasicMaterial({ color: 0x111111 }),
      );
      const pointHalo = new THREE.Mesh(
        new THREE.CircleGeometry(0.0325, 32),
        new THREE.MeshBasicMaterial({ color: 0xffffff }),
      );
      this.point = new THREE.Mesh(
        new THREE.CircleGeometry(0.026, 32),
        new THREE.MeshBasicMaterial({ color: 0xe53935 }),
      );
      pointOutline.position.z = 0;
      pointHalo.position.z = 0.001;
      this.point.position.z = 0.002;
      this.pointMarker.add(pointOutline, pointHalo, this.point);
      this.pointMarker.position.z = 0.05;
      this.scene.add(this.pointMarker);

      this.resizeObserver = new ResizeObserver(() => this.resize());
      this.resizeObserver.observe(this.host);
      this.resize();
    }

    resize() {
      const width = Math.max(280, this.host.clientWidth);
      const height = Math.max(280, this.host.clientHeight);
      const halfHeight = 1.15;
      const halfWidth = halfHeight * width / height;
      this.camera.left = -halfWidth;
      this.camera.right = halfWidth;
      this.camera.top = halfHeight;
      this.camera.bottom = -halfHeight;
      this.camera.updateProjectionMatrix();
      this.renderer.setSize(width, height, false);
    }

    setColor(hsv, rgb) {
      const selectedRadius = hsv.s * hsv.v;
      const angle = hsv.h * Math.PI / 180;
      this.disk.geometry.dispose();
      this.disk.geometry = hsvDiskGeometry(hsv.v);
      replaceLinePoints(
        this.outerRing,
        circlePoints(hsv.v, 0).map(({ x, z }) => new THREE.Vector3(x, z, 0.02)),
      );
      replaceLinePoints(this.selectedRadiusLine, [
        new THREE.Vector3(0, 0, 0.03),
        new THREE.Vector3(
          selectedRadius * Math.cos(angle),
          selectedRadius * Math.sin(angle),
          0.03,
        ),
      ]);
      this.pointMarker.position.set(
        selectedRadius * Math.cos(angle),
        selectedRadius * Math.sin(angle),
        0.05,
      );
      this.point.material.color.setRGB(rgb.r / 255, rgb.g / 255, rgb.b / 255, THREE.SRGBColorSpace);
    }

    render() {
      this.renderer.render(this.scene, this.camera);
    }
  }

  const controls = {
    r: $("red-input"), g: $("green-input"), b: $("blue-input"),
    h: $("hue-input"), s: $("saturation-input"), v: $("value-input"),
  };
  let rgb = { r: 229, g: 57, b: 53 };
  let hsv = rgbToHsv(rgb);
  let hueInputDegrees = Math.round(hsv.h) % 360;
  let rgbView;
  let hsvView;
  let hsvSliceView;

  function updateReadout() {
    const hex = rgbHex(rgb);
    const hueDegrees = hueInputDegrees;
    const wrappedHueDegrees = Math.round(hsv.h) % 360;
    const saturationPercent = Math.round(hsv.s * 100);
    const valuePercent = Math.round(hsv.v * 100);
    $("selected-swatch").style.background = hex;
    $("color-picker").value = hex.toLowerCase();
    $("hex-output").value = hex;
    controls.r.value = rgb.r; controls.g.value = rgb.g; controls.b.value = rgb.b;
    controls.h.value = hueDegrees; controls.s.value = saturationPercent; controls.v.value = valuePercent;
    $("red-output").value = rgb.r; $("green-output").value = rgb.g; $("blue-output").value = rgb.b;
    $("hue-output").value = `${hueDegrees}°`;
    $("saturation-output").value = `${saturationPercent}%`;
    $("value-output").value = `${valuePercent}%`;
    $("rgb-byte-output").textContent = `${rgb.r} / ${rgb.g} / ${rgb.b}`;
    $("hsv-human-output").textContent = `${hueDegrees}° / ${saturationPercent}% / ${valuePercent}%`;
    $("hue-wrap-output").textContent = `${wrappedHueDegrees}°`;
    $("chroma-output").textContent = `S × V = ${(hsv.s * hsv.v).toFixed(3)}`;

    const status = $("hue-confidence");
    if (hsv.v < 0.12) status.textContent = "Hue is weak here: the pixel is near the black apex.";
    else if (hsv.s < 0.12) status.textContent = "Hue is weak here: the pixel is close to the gray axis.";
    else status.textContent = "Hue is useful here: the pixel has enough saturation and value.";
    rgbView?.setColor(rgb);
    hsvView?.setColor(hsv, rgb);
    hsvSliceView?.setColor(hsv, rgb);
  }

  function setFromRgb(nextRgb) {
    rgb = {
      r: clamp(Math.round(nextRgb.r), 0, 255),
      g: clamp(Math.round(nextRgb.g), 0, 255),
      b: clamp(Math.round(nextRgb.b), 0, 255),
    };
    hsv = rgbToHsv(rgb);
    hueInputDegrees = Math.round(hsv.h) % 360;
    updateReadout();
  }

  function setFromHsv(nextHsv) {
    hueInputDegrees = clamp(Math.round(nextHsv.h), -360, 360);
    hsv = {
      h: ((hueInputDegrees % 360) + 360) % 360,
      s: clamp(nextHsv.s, 0, 1),
      v: clamp(nextHsv.v, 0, 1),
    };
    rgb = hsvToRgb(hsv);
    updateReadout();
  }

  ["r", "g", "b"].forEach((channel) => controls[channel].addEventListener("input", () => {
    setFromRgb({ r: Number(controls.r.value), g: Number(controls.g.value), b: Number(controls.b.value) });
  }));
  ["h", "s", "v"].forEach((channel) => controls[channel].addEventListener("input", () => {
    setFromHsv({ h: Number(controls.h.value), s: Number(controls.s.value) / 100, v: Number(controls.v.value) / 100 });
  }));
  $("color-picker").addEventListener("input", (event) => {
    const value = event.target.value;
    setFromRgb({
      r: parseInt(value.slice(1, 3), 16),
      g: parseInt(value.slice(3, 5), 16),
      b: parseInt(value.slice(5, 7), 16),
    });
  });
  document.querySelectorAll("[data-color]").forEach((button) => button.addEventListener("click", () => {
    const value = button.dataset.color;
    setFromRgb({
      r: parseInt(value.slice(1, 3), 16),
      g: parseInt(value.slice(3, 5), 16),
      b: parseInt(value.slice(5, 7), 16),
    });
  }));

  try {
    rgbView = new RgbCubeView();
    hsvView = new HsvConeView();
    hsvSliceView = new HsvSliceView();
    document.body.dataset.threeRenderers = "ready";
  } catch (error) {
    console.error("Unable to initialize the Three.js lesson", error);
    document.body.dataset.threeRenderers = "unavailable";
    document.querySelectorAll(".render-fallback").forEach((fallback) => { fallback.hidden = false; });
  }

  updateReadout();
  document.body.dataset.lessonReady = "true";

  function renderFrame() {
    rgbView?.render();
    hsvView?.render();
    hsvSliceView?.render();
    requestAnimationFrame(renderFrame);
  }
  requestAnimationFrame(renderFrame);
})();
