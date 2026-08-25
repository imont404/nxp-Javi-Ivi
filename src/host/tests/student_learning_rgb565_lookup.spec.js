const path = require("path");
const { pathToFileURL } = require("url");
const { test, expect } = require("@playwright/test");

const lessonUrl = pathToFileURL(
  path.resolve(__dirname, "../../../docs/learn/rgb565-lookup.html"),
).href;

test("RGB565 lookup lesson works directly from a local file", async ({ page }) => {
  const browserErrors = [];
  const requests = [];
  page.on("pageerror", (error) => browserErrors.push(error.message));
  page.on("request", (request) => requests.push(request.url()));
  page.on("console", (message) => {
    if (message.type() === "error") browserErrors.push(message.text());
  });

  await page.goto(lessonUrl);

  await expect(page).toHaveTitle(/RGB565 and the YHSV Lookup Table/);
  await expect(page.locator("body")).toHaveAttribute("data-lesson-ready", "true");
  await expect(page.getByRole("heading", { name: "One 16-bit pixel" })).toBeVisible();
  await expect(page.getByRole("heading", { name: "A weighted brightness estimate" })).toBeVisible();
  await expect(page.getByRole("heading", { name: "Move the work out of the frame loop" })).toBeVisible();
  await expect(page.getByRole("heading", { name: "What one table row precomputes" })).toBeVisible();
  await expect(page.getByText("uint8_t y = color.y", { exact: false })).toBeVisible();
  await expect(page.locator("#lut-canvas")).toBeVisible();
  await expect(page.locator("#rgb565-word span")).toHaveCount(16);
  await expect(page.locator("#rgb565-output")).toHaveText("0xE1C6");
  await expect(page.locator("#expanded-output")).toHaveText("231 / 56 / 49");
  await expect(page.locator("#packed-output")).toHaveText("0xE7C9016C");
  await expect(page.locator("#luma-y-output")).toHaveText("Y = 108");

  await page.getByRole("button", { name: "red" }).click();
  await expect(page.locator("#rgb565-output")).toHaveText("0xF800");
  await expect(page.locator("#packed-output")).toHaveText("0xFFFF004D");
  await expect(page.locator("#y-output")).toHaveText("77");
  await expect(page.locator("#luma-y-output")).toHaveText("Y = 77");
  await expect(page.locator("#h-output")).toHaveText("0");
  await expect(page.locator("#s-output")).toHaveText("255");
  await expect(page.locator("#v-output")).toHaveText("255");

  expect(requests).toEqual([lessonUrl]);
  expect(browserErrors).toEqual([]);
});

test("RGB565 lookup laboratory remains usable on a phone", async ({ page }) => {
  await page.setViewportSize({ width: 390, height: 844 });
  await page.goto(lessonUrl);

  await expect(page.locator("#selected-swatch")).toBeVisible();
  await expect(page.locator("#lut-map")).toBeVisible();
  await expect(page.locator("#packed-output")).toBeVisible();
  await expect(page.locator("body")).not.toHaveCSS("overflow-x", "scroll");
});
