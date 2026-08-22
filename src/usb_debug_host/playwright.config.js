const path = require("path");

module.exports = {
  testDir: path.join(__dirname, "tests"),
  timeout: 30_000,
  use: {
    channel: "chrome",
    headless: true,
  },
};
