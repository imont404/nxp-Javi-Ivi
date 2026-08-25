(() => {
  "use strict";

  const $ = (id) => document.getElementById(id);
  const clampByte = (value) => Math.min(255, Math.max(0, Math.round(value)));
  const toHex = (value, width) => value.toString(16).toUpperCase().padStart(width, "0");

  function rgbHex({ r, g, b }) {
    return `#${toHex(r, 2)}${toHex(g, 2)}${toHex(b, 2)}`;
  }

  function rgb888ToRgb565({ r, g, b }) {
    return (((r & 0xf8) << 8) | ((g & 0xfc) << 3) | (b >> 3)) & 0xffff;
  }

  function expandRgb565(pixel) {
    const r5 = (pixel >> 11) & 0x1f;
    const g6 = (pixel >> 5) & 0x3f;
    const b5 = pixel & 0x1f;
    return {
      r: (r5 << 3) | (r5 >> 2),
      g: (g6 << 2) | (g6 >> 4),
      b: (b5 << 3) | (b5 >> 2),
    };
  }

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

  function quantizeUnit(value) {
    return Math.min(255, Math.max(0, Math.floor(value * 255 + 0.5)));
  }

  function lookupFeatures(pixel) {
    const rgb = expandRgb565(pixel);
    const hsv = rgbToHsv(rgb);
    const y = ((rgb.r * 77) + (rgb.g * 150) + (rgb.b * 29) + 128) >> 8;
    const h = Math.floor(hsv.h * 256 / 360) & 0xff;
    const s = quantizeUnit(hsv.s);
    const v = quantizeUnit(hsv.v);
    const packed = ((y | (h << 8) | (s << 16) | (v << 24)) >>> 0);
    return { y, h, s, v, packed, rgb };
  }

  function formatDifference(value) {
    if (value > 0) return `+${value}`;
    if (value < 0) return `−${Math.abs(value)}`;
    return "0";
  }

  function drawLutMap() {
    const canvas = $("lut-canvas");
    const context = canvas.getContext("2d", { alpha: false });
    const image = context.createImageData(256, 256);
    for (let pixel = 0; pixel < 65536; pixel += 1) {
      const rgb = expandRgb565(pixel);
      const offset = pixel * 4;
      image.data[offset] = rgb.r;
      image.data[offset + 1] = rgb.g;
      image.data[offset + 2] = rgb.b;
      image.data[offset + 3] = 255;
    }
    context.putImageData(image, 0, 0);
  }

  function buildBitWord() {
    const host = $("rgb565-word");
    for (let index = 0; index < 16; index += 1) {
      const bit = document.createElement("span");
      bit.className = index < 5 ? "bit-r" : index < 11 ? "bit-g" : "bit-b";
      bit.dataset.position = String(15 - index);
      bit.textContent = "0";
      host.append(bit);
    }
  }

  let requestedRgb = { r: 229, g: 57, b: 53 };
  let selectedPixel = rgb888ToRgb565(requestedRgb);

  function update() {
    const features = lookupFeatures(selectedPixel);
    const storedRgb = features.rgb;
    const storedHex = rgbHex(storedRgb);
    const bits = selectedPixel.toString(2).padStart(16, "0");

    $("selected-swatch").style.background = storedHex;
    $("color-picker").value = storedHex.toLowerCase();
    $("hex-output").value = storedHex;
    $("rgb888-output").textContent = `${requestedRgb.r} / ${requestedRgb.g} / ${requestedRgb.b}`;
    $("rgb565-output").textContent = `0x${toHex(selectedPixel, 4)}`;
    $("expanded-output").textContent = `${storedRgb.r} / ${storedRgb.g} / ${storedRgb.b}`;
    $("error-output").textContent = [
      storedRgb.r - requestedRgb.r,
      storedRgb.g - requestedRgb.g,
      storedRgb.b - requestedRgb.b,
    ].map(formatDifference).join(" / ");
    $("index-output").value = `index 0x${toHex(selectedPixel, 4)}`;
    $("packed-output").value = `0x${toHex(features.packed, 8)}`;
    $("y-output").value = features.y;
    $("h-output").value = features.h;
    $("s-output").value = features.s;
    $("v-output").value = features.v;

    const lumaContributions = {
      r: storedRgb.r * 77 / 256,
      g: storedRgb.g * 150 / 256,
      b: storedRgb.b * 29 / 256,
    };
    ["r", "g", "b"].forEach((channel) => {
      const contribution = lumaContributions[channel];
      const weight = { r: 77, g: 150, b: 29 }[channel];
      $(`luma-${channel}-expression`).textContent = `${storedRgb[channel]} × ${weight} / 256`;
      $(`luma-${channel}-output`).value = contribution.toFixed(1);
      $(`luma-${channel}-bar`).style.width = `${contribution / 255 * 100}%`;
      $(`luma-sum-${channel}`).style.width = `${contribution / 255 * 100}%`;
    });
    $("luma-y-output").value = `Y = ${features.y}`;

    $("rgb565-word").querySelectorAll("span").forEach((bit, index) => {
      bit.textContent = bits[index];
      bit.dataset.on = bits[index];
    });

    const lowByte = selectedPixel & 0xff;
    const highByte = selectedPixel >> 8;
    $("lut-cursor").style.left = `${((lowByte + 0.5) / 256) * 100}%`;
    $("lut-cursor").style.top = `${((highByte + 0.5) / 256) * 100}%`;
  }

  function setFromRgb(rgb) {
    requestedRgb = {
      r: clampByte(rgb.r),
      g: clampByte(rgb.g),
      b: clampByte(rgb.b),
    };
    selectedPixel = rgb888ToRgb565(requestedRgb);
    update();
  }

  function setFromPixel(pixel) {
    selectedPixel = Math.min(0xffff, Math.max(0, pixel | 0));
    requestedRgb = expandRgb565(selectedPixel);
    update();
  }

  function selectMapPosition(event) {
    const map = $("lut-map");
    const bounds = map.getBoundingClientRect();
    const x = Math.min(255, Math.max(0, Math.floor((event.clientX - bounds.left) * 256 / bounds.width)));
    const y = Math.min(255, Math.max(0, Math.floor((event.clientY - bounds.top) * 256 / bounds.height)));
    setFromPixel((y << 8) | x);
  }

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

  const map = $("lut-map");
  map.addEventListener("pointerdown", (event) => {
    map.setPointerCapture(event.pointerId);
    selectMapPosition(event);
  });
  map.addEventListener("pointermove", (event) => {
    if (map.hasPointerCapture(event.pointerId)) selectMapPosition(event);
  });

  buildBitWord();
  drawLutMap();
  update();
  document.body.dataset.lessonReady = "true";
})();
