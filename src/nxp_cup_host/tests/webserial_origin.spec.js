const { test, expect } = require("@playwright/test");
const path = require("path");
const { pathToFileURL } = require("url");

const viewerUrl = pathToFileURL(path.join(__dirname, "..", "nxpc_usb_debug_viewer.html")).href;

test("Chrome exposes Web Serial to the standalone file origin", async ({ page }) => {
  const pageErrors = [];
  page.on("pageerror", (error) => pageErrors.push(error.message));

  await page.goto(viewerUrl);

  const browserSupport = await page.evaluate(() => ({
    secureContext: window.isSecureContext,
    webSerial: "serial" in navigator,
  }));
  expect(browserSupport).toEqual({ secureContext: true, webSerial: true });
  await expect(page.locator("#connectionState")).toHaveText("Disconnected");
  expect(pageErrors).toEqual([]);
});
