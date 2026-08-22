const { test, expect } = require("@playwright/test");
const path = require("path");
const { pathToFileURL } = require("url");

const viewerUrl = pathToFileURL(path.join(__dirname, "..", "avc_usb_debug_viewer.html")).href;

test.beforeEach(async ({ page }) => {
  await page.addInitScript(() => {
    const MAGIC = 0x55435641;
    const MSG_FRAME = 0x01000002;
    const MSG_LOG = 0x01000200;
    const MSG_STATS = 0x01000300;
    const MSG_TELEMETRY = 0x01000500;
    const MSG_HELLO = 0x01000403;
    const MSG_SET_CHANNELS = 0x01000404;
    const MSG_CLOSE = 0x01000406;
    const RESPONSE = 1;
    const FRAME_START = 1;
    const FRAME_END = 2;
    const FRAME_BYTES = 320 * 200 * 2;
    const FRAME_CHUNK_BYTES = 16 * 1024 - 32 - 24;
    let controller;
    let txSequence = 0;
    let frameId = 0;
    let telemetrySampleId = 0;

    function u32(view, offset, value) {
      view.setUint32(offset, value >>> 0, true);
    }

    function packet(msgId, flags, arg0, arg1, arg2, payload) {
      const bytes = new Uint8Array(32 + payload.length);
      const view = new DataView(bytes.buffer);
      u32(view, 0, MAGIC);
      view.setUint8(4, 1);
      view.setUint8(5, 32);
      view.setUint16(6, flags, true);
      u32(view, 8, msgId);
      u32(view, 12, txSequence++);
      u32(view, 16, payload.length);
      u32(view, 20, arg0);
      u32(view, 24, arg1);
      u32(view, 28, arg2);
      bytes.set(payload, 32);
      return bytes;
    }

    function enqueue(bytes) {
      const fragmentSizes = [1, 7, 31, 257, 4096];
      let offset = 0;
      let fragment = 0;
      while (offset < bytes.length) {
        const length = Math.min(fragmentSizes[fragment % fragmentSizes.length], bytes.length - offset);
        const part = bytes.slice(offset, offset + length);
        queueMicrotask(() => controller.enqueue(part));
        offset += length;
        fragment += 1;
      }
    }

    function response(msgId, requestSequence, payload = new Uint8Array()) {
      enqueue(packet(msgId, RESPONSE, requestSequence, 0, 1, payload));
    }

    function sendLog() {
      const category = new TextEncoder().encode("system");
      const text = new TextEncoder().encode("USB diagnostic log channel active");
      const payload = new Uint8Array(12 + category.length + text.length);
      const view = new DataView(payload.buffer);
      u32(view, 0, 1234);
      u32(view, 4, 0);
      view.setUint16(8, text.length, true);
      view.setUint8(10, 2);
      view.setUint8(11, category.length);
      payload.set(category, 12);
      payload.set(text, 12 + category.length);
      enqueue(packet(MSG_LOG, 0, 2, 0, 1234, payload));
    }

    function sendStats() {
      const payload = new Uint8Array(76);
      const view = new DataView(payload.buffer);
      u32(view, 0, 0);
      u32(view, 4, 1);
      u32(view, 12, 0);
      u32(view, 16, 1);
      u32(view, 20, 1);
      u32(view, 52, 1);
      u32(view, 60, 1);
      enqueue(packet(MSG_STATS, 0, 0, 0, 0, payload));
    }

    function sendTelemetry() {
      const name = new TextEncoder().encode("system.uptime");
      const units = new TextEncoder().encode("ms");
      const payload = new Uint8Array(16 + name.length + units.length);
      const view = new DataView(payload.buffer);
      u32(view, 0, 1234 + telemetrySampleId);
      u32(view, 4, telemetrySampleId);
      u32(view, 8, 1234 + telemetrySampleId);
      view.setUint16(12, name.length, true);
      view.setUint8(14, 2);
      view.setUint8(15, units.length);
      payload.set(name, 16);
      payload.set(units, 16 + name.length);
      enqueue(packet(MSG_TELEMETRY, 0, 2, telemetrySampleId, 1234 + telemetrySampleId, payload));
      telemetrySampleId += 1;
    }

    window.__avcViewerTestSendTelemetry = sendTelemetry;

    function sendFrame() {
      let offset = 0;
      while (offset < FRAME_BYTES) {
        const dataLength = Math.min(FRAME_CHUNK_BYTES, FRAME_BYTES - offset);
        const payload = new Uint8Array(24 + dataLength);
        const view = new DataView(payload.buffer);
        u32(view, 0, frameId);
        u32(view, 4, offset);
        u32(view, 8, FRAME_BYTES);
        view.setUint16(12, 320, true);
        view.setUint16(14, 200, true);
        view.setUint16(16, 1, true);
        view.setUint16(18, 0, true);
        u32(view, 20, (offset === 0 ? FRAME_START : 0) | (offset + dataLength === FRAME_BYTES ? FRAME_END : 0));
        for (let index = 0; index < dataLength; index += 2) {
          payload[24 + index] = 0xe0;
          payload[24 + index + 1] = 0x07;
        }
        enqueue(packet(MSG_FRAME, 0, frameId, offset, dataLength, payload));
        offset += dataLength;
      }
      frameId += 1;
    }

    function handleWrite(data) {
      const bytes = new Uint8Array(data);
      const view = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
      const msgId = view.getUint32(8, true);
      const requestSequence = view.getUint32(12, true);
      if (msgId === MSG_HELLO) {
        const hello = new Uint8Array(24);
        const helloView = new DataView(hello.buffer);
        u32(helloView, 0, 0x1f);
        u32(helloView, 8, 16 * 1024);
        helloView.setUint16(12, 320, true);
        helloView.setUint16(14, 200, true);
        helloView.setUint16(16, 1, true);
        u32(helloView, 20, 1);
        response(MSG_HELLO, requestSequence, hello);
      } else if (msgId === MSG_SET_CHANNELS) {
        const channels = view.getUint32(20, true);
        response(MSG_SET_CHANNELS, requestSequence);
        if (channels !== 0) {
          sendLog();
          sendTelemetry();
          sendStats();
          sendFrame();
        }
      } else if (msgId === MSG_CLOSE) {
        response(MSG_CLOSE, requestSequence);
      }
    }

    const port = {
      readable: new ReadableStream({
        start(value) {
          controller = value;
        },
      }),
      writable: new WritableStream({
        write(data) {
          handleWrite(data);
        },
      }),
      async open() {},
      async close() {},
    };

    Object.defineProperty(navigator, "serial", {
      configurable: true,
      value: {
        async requestPort() {
          return port;
        },
      },
    });
  });
});

test("standalone viewer negotiates and renders mixed traffic", async ({ page }) => {
  const pageErrors = [];
  const requests = [];
  page.on("pageerror", (error) => pageErrors.push(error.message));
  page.on("request", (request) => requests.push(request.url()));

  await page.goto(viewerUrl);
  await page.getByRole("button", { name: "Connect" }).click();

  await expect(page.locator("#connectionState")).toHaveText("Streaming");
  await expect(page.locator("#frameCount")).toHaveText("1", { timeout: 5000 });
  await expect(page.locator("#logRecordCount")).toHaveText("1");
  await expect(page.locator("#logConsole")).toContainText("INFO [system] USB diagnostic log channel active");
  await expect(page.locator("#telemetrySampleCount")).toHaveText("1");
  await expect(page.locator("#telemetryTableBody")).toContainText("system.uptime");
  await expect(page.locator("#telemetryTableBody")).toContainText("1234");
  await expect(page.locator("#telemetryTableBody")).toContainText("u32");
  await expect(page.getByRole("checkbox", { name: "Plot system.uptime" })).toBeChecked();
  await expect(page.locator("#telemetryPlotCanvas")).toHaveAttribute("data-series-count", "1");
  await expect(page.locator("#telemetryPlotCanvas")).toHaveAttribute("data-max-history-samples", "1");
  await expect(page.locator("#plotLegend")).toContainText("system.uptime");
  await expect(page.locator("#errorCount")).toHaveText("0");

  await page.getByRole("button", { name: "Stop" }).click();
  await expect(page.locator("#connectionState")).toHaveText("Telemetry session ready");
  await page.getByRole("button", { name: "Start" }).click();
  await expect(page.locator("#connectionState")).toHaveText("Streaming");
  await expect(page.locator("#frameCount")).toHaveText("2", { timeout: 5000 });
  await expect(page.locator("#telemetrySampleCount")).toHaveText("2");
  await expect(page.locator("#telemetryTableBody tr")).toHaveCount(1);
  await expect(page.locator("#telemetryPlotCanvas")).toHaveAttribute("data-max-history-samples", "2");
  await expect(page.locator("#errorCount")).toHaveText("0");

  await page.evaluate(() => {
    for (let index = 0; index < 305; index += 1) {
      window.__avcViewerTestSendTelemetry();
    }
  });
  await expect(page.locator("#telemetrySampleCount")).toHaveText("307");
  await expect(page.locator("#telemetryPlotCanvas")).toHaveAttribute("data-max-history-samples", "300");

  expect(pageErrors).toEqual([]);
  expect(requests).toEqual([viewerUrl]);
});
