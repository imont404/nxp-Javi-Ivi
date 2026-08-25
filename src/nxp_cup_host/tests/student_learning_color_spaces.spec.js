const path = require("path");
const { pathToFileURL } = require("url");
const { test, expect } = require("@playwright/test");

const lessonUrl = pathToFileURL(
  path.resolve(__dirname, "../../../docs/learn/color-spaces.html"),
).href;

test("RGB and HSV lesson works directly from a local file", async ({ page }) => {
  const browserErrors = [];
  const requests = [];
  page.on("pageerror", (error) => browserErrors.push(error.message));
  page.on("request", (request) => requests.push(request.url()));
  page.on("console", (message) => {
    if (message.type() === "error") {
      browserErrors.push(message.text());
    }
  });

  await page.goto(lessonUrl);

  await expect(page).toHaveTitle(/RGB and HSV Coordinates/);
  await expect(page.getByRole("heading", { name: "RGB cube" })).toBeVisible();
  await expect(page.getByRole("heading", { name: "HSV cone" })).toBeVisible();
  await expect(page.getByRole("heading", { name: "Current-V cross-section" })).toBeVisible();
  await expect(page.locator("body")).toHaveAttribute("data-lesson-ready", "true");
  await expect(page.locator("body")).toHaveAttribute("data-three-renderers", "ready");
  await expect(page.locator("canvas")).toHaveCount(3);
  await expect(page.locator("#rgb-byte-output")).toHaveText("229 / 57 / 53");
  await expect(page.locator("#hsv-human-output")).toHaveText("1° / 77% / 90%");

  await page.getByRole("button", { name: "dark" }).click();
  await expect(page.locator("#value-output")).toHaveText("34%");

  await page.locator("#color-picker").evaluate((picker) => {
    picker.value = "#ff0001";
    picker.dispatchEvent(new Event("input", { bubbles: true }));
  });
  await expect(page.locator("#hue-output")).toHaveText("0°");

  await page.locator("#hue-input").evaluate((input) => {
    input.value = "-10";
    input.dispatchEvent(new Event("input", { bubbles: true }));
  });
  await expect(page.locator("#hue-output")).toHaveText("-10°");
  await expect(page.locator("#hue-wrap-output")).toHaveText("350°");
  const negativeHueHex = await page.locator("#hex-output").textContent();

  await page.locator("#hue-input").evaluate((input) => {
    input.value = "350";
    input.dispatchEvent(new Event("input", { bubbles: true }));
  });
  await expect(page.locator("#hue-wrap-output")).toHaveText("350°");
  await expect(page.locator("#hex-output")).toHaveText(negativeHueHex);

  expect(requests).toEqual([lessonUrl]);
  expect(browserErrors).toEqual([]);
});

test("color-space lesson keeps the visual laboratory usable on a phone", async ({ page }) => {
  await page.setViewportSize({ width: 390, height: 844 });
  await page.goto(lessonUrl);

  await expect(page.locator("#rgb-scene canvas")).toBeVisible();
  await expect(page.locator("#hsv-scene canvas")).toBeVisible();
  await expect(page.locator("#hsv-slice-scene canvas")).toBeVisible();
  await expect(page.locator("#red-input")).toBeVisible();
  await expect(page.locator("body")).not.toHaveCSS("overflow-x", "scroll");
});
