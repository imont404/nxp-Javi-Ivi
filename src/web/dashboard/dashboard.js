"use strict";

(() => {
  const WHEEL_DIAMETER_METERS = 0.075;

  class DashboardPresentation {
    constructor() {
      this.ui = {
        state: document.getElementById("dashboardState"),
        live: document.getElementById("dashboardLive"),
        speed: document.getElementById("dashboardSpeed"),
        battery: document.getElementById("dashboardBattery"),
        frameRate: document.getElementById("dashboardFrameRate"),
        leftRpm: document.getElementById("dashboardLeftRpm"),
        rightRpm: document.getElementById("dashboardRightRpm"),
        leftCommand: document.getElementById("dashboardLeftCommand"),
        rightCommand: document.getElementById("dashboardRightCommand"),
        leftBar: document.getElementById("dashboardLeftBar"),
        rightBar: document.getElementById("dashboardRightBar"),
        steeringCommand: document.getElementById("dashboardSteeringCommand"),
        steeringMarker: document.getElementById("dashboardSteeringMarker"),
        canvas: document.getElementById("frameCanvas"),
        video: document.getElementById("h264Video"),
      };
      this.leftWheelRpm = 0;
      this.rightWheelRpm = 0;
      this.systemMode = "";
      this.systemState = "";
    }

    setConnection({ connected, live, label }) {
      this.ui.live.textContent = live ? "LIVE" : connected ? "CONNECTED" : "OFFLINE";
      this.ui.live.classList.toggle("offline", !live);
      if (!connected) this.setState("");
      if (label !== undefined) document.getElementById("connectionState").textContent = label;
    }

    setState(label) {
      this.ui.state.textContent = label;
      this.ui.state.hidden = !label;
      this.ui.state.className = "state";
      this.ui.state.classList.toggle("disarmed", /DISARMED|WAITING|CENTER POTS/.test(label));
      this.ui.state.classList.toggle("fault", /FAULT|LOST|OVERRUN|ERROR|DISCONNECTED/.test(label));
    }

    setFrameRate(value) {
      const numeric = Number(value);
      this.ui.frameRate.textContent = Number.isFinite(numeric) ? numeric.toFixed(1) : "0.0";
    }

    showVideoSurface(surface) {
      const h264 = surface === "h264";
      this.ui.video.hidden = !h264;
      this.ui.canvas.hidden = h264;
    }

    updateTelemetry(sample) {
      const numeric = Number(sample.value);
      switch (sample.name) {
        case "system.mode":
          this.systemMode = String(sample.value);
          break;
        case "system.state":
          this.systemState = String(sample.value);
          break;
        case "battery.voltage":
          if (Number.isFinite(numeric)) this.ui.battery.textContent = numeric.toFixed(1);
          break;
        case "wheel.left.rpm":
          if (Number.isFinite(numeric)) {
            this.leftWheelRpm = numeric;
            this.ui.leftRpm.textContent = String(Math.round(numeric));
            this.updateSpeed();
          }
          break;
        case "wheel.right.rpm":
          if (Number.isFinite(numeric)) {
            this.rightWheelRpm = numeric;
            this.ui.rightRpm.textContent = String(Math.round(numeric));
            this.updateSpeed();
          }
          break;
        case "motor.left.command":
          if (Number.isFinite(numeric)) this.setCommand(this.ui.leftBar, this.ui.leftCommand, numeric);
          break;
        case "motor.right.command":
          if (Number.isFinite(numeric)) this.setCommand(this.ui.rightBar, this.ui.rightCommand, numeric);
          break;
        case "steering.command":
          if (Number.isFinite(numeric)) {
            const normalized = Math.max(-1, Math.min(1, numeric));
            const degrees = Math.round(normalized * 30);
            this.ui.steeringCommand.textContent = degrees > 0 ? `+${degrees}` : String(degrees);
            this.ui.steeringMarker.style.left = `${50 + normalized * 50}%`;
          }
          break;
        default:
          break;
      }
      if (sample.name === "system.mode" || sample.name === "system.state") {
        this.setState([this.systemMode, this.systemState].filter(Boolean).join(" / ") || "CONNECTED");
      }
    }

    resetTelemetry() {
      this.leftWheelRpm = 0;
      this.rightWheelRpm = 0;
      this.systemMode = "";
      this.systemState = "";
      this.ui.speed.textContent = "0.0";
      this.ui.battery.textContent = "--.-";
      this.ui.leftRpm.textContent = "0";
      this.ui.rightRpm.textContent = "0";
      this.setCommand(this.ui.leftBar, this.ui.leftCommand, 0);
      this.setCommand(this.ui.rightBar, this.ui.rightCommand, 0);
      this.ui.steeringCommand.textContent = "0";
      this.ui.steeringMarker.style.left = "50%";
      this.setState("");
    }

    updateSpeed() {
      const averageRpm = (this.leftWheelRpm + this.rightWheelRpm) / 2;
      const kilometersPerHour = Math.abs(averageRpm) * Math.PI * WHEEL_DIAMETER_METERS * 60 / 1000;
      this.ui.speed.textContent = kilometersPerHour.toFixed(1);
    }

    setCommand(bar, label, value) {
      const normalized = Math.max(-1, Math.min(1, Number(value) || 0));
      const magnitude = Math.abs(normalized);
      bar.style.left = "0%";
      bar.style.width = `${magnitude * 100}%`;
      bar.classList.toggle("medium", magnitude >= 0.5 && magnitude < 0.75);
      bar.classList.toggle("high", magnitude >= 0.75);
      label.textContent = `${Math.round(normalized * 100)}%`;
    }
  }

  window.NxpCupDashboard = Object.freeze({
    create: () => new DashboardPresentation(),
  });
})();
