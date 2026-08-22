const path = require("path");
const { pathToFileURL } = require("url");
const { test, expect } = require("@playwright/test");

const lessonUrl = pathToFileURL(
  path.resolve(__dirname, "../../../docs/learn/color-spaces.html"),
).href;

test("color-space lesson works directly from a local file", async ({ page }) => {
  const browserErrors = [];
  page.on("pageerror", (error) => browserErrors.push(error.message));
  page.on("console", (message) => {
    if (message.type() === "error") {
      browserErrors.push(message.text());
    }
  });

  await page.goto(lessonUrl);

  await expect(page).toHaveTitle(/RGB565 to HSV/);
  await expect(page.getByRole("heading", { name: "One pixel, three views." })).toBeVisible();
  await expect(page.locator("canvas")).toHaveCount(3);
  await expect(page.getByRole("heading", { name: "Explore one RGB slice" })).toHaveCount(0);
  await expect(page.locator("#rgb565")).toHaveText("0xE1C6");
  await expect(page.locator("#yhsv")).toHaveText("108 / 1 / 201 / 231");

  await page.getByRole("button", { name: "near black" }).click();
  await expect(page.locator("#confidence")).toContainText("near black");
  await expect(page.locator("#rgb565")).toHaveText("0x1082");
  expect(browserErrors).toEqual([]);
});

test("color-space lesson keeps the visual laboratory usable on a phone", async ({ page }) => {
  await page.setViewportSize({ width: 390, height: 844 });
  await page.goto(lessonUrl);

  await expect(page.locator("#rgbCube")).toBeVisible();
  await expect(page.locator("#hsvDisk")).toBeVisible();
  await expect(page.locator("#r")).toBeVisible();
  await expect(page.locator("body")).not.toHaveCSS("overflow-x", "scroll");
});
