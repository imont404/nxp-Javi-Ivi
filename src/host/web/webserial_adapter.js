"use strict";

const STREAM = {
  magic: [0x41, 0x56, 0x43, 0x55],
  version: 1,
  headerBytes: 32,
  ruiFrameChunkHeaderBytes: 24,
  logRecordHeaderBytes: 12,
  telemetryScalarHeaderBytes: 16,
  telemetryTextMaxBytes: 48,
  statsReportBytes: 76,
  frameWidth: 320,
  frameHeight: 200,
  frameBytes: 320 * 200 * 2,
  packetPayloadMaxBytes: 16 * 1024 - 32,
  msgIdRuiWriteFrameBufferRaw: 0x01000002,
  msgIdLogText: 0x01000200,
  msgIdStatsReport: 0x01000300,
  msgIdTelemetryScalar: 0x01000500,
  msgIdControlHello: 0x01000403,
  msgIdControlSetChannels: 0x01000404,
  msgIdControlPing: 0x01000405,
  msgIdControlClose: 0x01000406,
  msgIdControlError: 0x01000407,
  msgIdControlSystemAction: 0x01000409,
  packetFlagsKnown: 0x000f,
  packetFlagResponse: 1,
  packetFlagDroppedBefore: 1 << 3,
  frameDataMaxBytes: 16 * 1024 - 32 - 24,
  pixelFormatRgb565Le: 1,
  chunkFlagFrameStart: 1,
  chunkFlagFrameEnd: 2,
  chunkFlagsKnown: 1 | 2 | 4,
  helloResponseBytes: 24,
  channelFrames: 1,
  channelStats: 2,
  channelLogs: 4,
  channelTelemetry: 8,
  streamSourceCamera: 0,
  controlStatusOk: 0,
  controlStatusNotReady: 6,
  controlStatusDenied: 7,
  capabilitySystemActions: 1 << 7,
  systemActionRaceStart: 1,
  systemActionStop: 2,
  raceStartConfirmation: 0x21214f47,
};

// LLM-assisted dashboards normally only need this list changed. Names match the
// first argument to NXPC_DBG_VALUE_* in firmware; no protocol/parser edits are needed.
const DASHBOARD = {
  defaultPlots: ["wheel.left.rpm", "wheel.right.rpm"],
  wheelDiameterMeters: 0.075,
  historySamplesPerSignal: 300,
  maxTelemetrySignals: 32,
  maxSelectedPlots: 6,
  colors: ["#126a5f", "#e4a11b", "#a144a0", "#2474b5", "#d34c4c", "#637b35"],
};

const ui = {
  connectButton: document.getElementById("connectButton"),
  startButton: document.getElementById("startButton"),
  stopButton: document.getElementById("stopButton"),
  resetButton: document.getElementById("resetButton"),
  connectionState: document.getElementById("connectionState"),
  canvas: document.getElementById("frameCanvas"),
  byteRate: document.getElementById("byteRate"),
  frameRate: document.getElementById("frameRate"),
  renderRate: document.getElementById("renderRate"),
  byteCount: document.getElementById("byteCount"),
  frameCount: document.getElementById("frameCount"),
  chunkCount: document.getElementById("chunkCount"),
  lastFrame: document.getElementById("lastFrame"),
  droppedBeforeCount: document.getElementById("droppedBeforeCount"),
  firmwareDropCount: document.getElementById("firmwareDropCount"),
  statsPacketCount: document.getElementById("statsPacketCount"),
  logRecordCount: document.getElementById("logRecordCount"),
  logConsole: document.getElementById("logConsole"),
  telemetrySampleCount: document.getElementById("telemetrySampleCount"),
  telemetryTableBody: document.getElementById("telemetryTableBody"),
  telemetryPlotCanvas: document.getElementById("telemetryPlotCanvas"),
  plotLegend: document.getElementById("plotLegend"),
  plotStatus: document.getElementById("plotStatus"),
  errorCount: document.getElementById("errorCount"),
  resyncCount: document.getElementById("resyncCount"),
  resyncBytes: document.getElementById("resyncBytes"),
  sequenceErrors: document.getElementById("sequenceErrors"),
  endpointBusyCount: document.getElementById("endpointBusyCount"),
  sendErrorCount: document.getElementById("sendErrorCount"),
  dashboardState: document.getElementById("dashboardState"),
  dashboardLive: document.getElementById("dashboardLive"),
  dashboardBattery: document.getElementById("dashboardBattery"),
  dashboardFrameRate: document.getElementById("dashboardFrameRate"),
  dashboardSpeed: document.getElementById("dashboardSpeed"),
  dashboardLeftRpm: document.getElementById("dashboardLeftRpm"),
  dashboardRightRpm: document.getElementById("dashboardRightRpm"),
  dashboardLeftCommand: document.getElementById("dashboardLeftCommand"),
  dashboardRightCommand: document.getElementById("dashboardRightCommand"),
  dashboardSteeringCommand: document.getElementById("dashboardSteeringCommand"),
  dashboardLeftBar: document.getElementById("dashboardLeftBar"),
  dashboardRightBar: document.getElementById("dashboardRightBar"),
  dashboardSteeringMarker: document.getElementById("dashboardSteeringMarker"),
  raceStartButton: document.getElementById("raceStartButton"),
  raceStopButton: document.getElementById("raceStopButton"),
  raceControls: document.getElementById("raceControls"),
  raceControlStatus: document.getElementById("raceControlStatus"),
};

const dashboard = window.NxpCupDashboard.create();
const context = ui.canvas.getContext("2d", { alpha: false });
const imageData = context.createImageData(STREAM.frameWidth, STREAM.frameHeight);
const plotContext = ui.telemetryPlotCanvas.getContext("2d", { alpha: false });
const utf8Decoder = new TextDecoder("utf-8");
const selectedTelemetryNames = new Set(DASHBOARD.defaultPlots.slice(0, DASHBOARD.maxSelectedPlots));

let port = null;
let reader = null;
let keepReading = false;
let streaming = false;
let sessionActive = false;
let helloCapabilities = 0;
let systemMode = "";
let systemState = "";
let raceStartHoldTimer = null;
let leftWheelRpm = 0;
let rightWheelRpm = 0;
let lastRxAt = 0;
let watchdogTimer = null;
let nextControlSequence = 0;
let writeChain = Promise.resolve();
const pendingControlRequests = new Map();
let latestFrame = new Uint8Array(STREAM.frameBytes);
let frameDirty = false;
let plotDirty = true;
let renderedFrames = 0;
let lastStats = {
  time: performance.now(),
  bytes: 0,
  frames: 0,
  rendered: 0,
};

class StreamParser {
  constructor() {
    this.rxBuffer = new Uint8Array(4 * 1024 * 1024);
    this.rxLength = 0;
    this.assemblyFrame = new Uint8Array(STREAM.frameBytes);
    this.displayFrame = new Uint8Array(STREAM.frameBytes);
    this.reset();
  }

  reset() {
    this.rxLength = 0;
    this.bytes = 0;
    this.frames = 0;
    this.chunks = 0;
    this.lastFrameId = -1;
    this.lastChunkId = -1;
    this.lastOffset = -1;
    this.resyncEvents = 0;
    this.resyncBytes = 0;
    this.invalidHeaders = 0;
    this.sequenceErrors = 0;
    this.statsPackets = 0;
    this.logRecords = 0;
    this.telemetrySamples = 0;
    this.telemetryValues = new Map();
    this.telemetryHistory = new Map();
    this.controlPackets = 0;
    this.droppedBeforePackets = 0;
    this.lastStats = null;
    this.expectedPacketSequence = null;
    this.expectedFrameId = null;
    this.expectedOffset = 0;
    this.assemblyFrame.fill(0);
    this.displayFrame.fill(0);
  }

  push(data) {
    lastRxAt = performance.now();
    this.bytes += data.length;
    this.ensureCapacity(this.rxLength + data.length);
    this.rxBuffer.set(data, this.rxLength);
    this.rxLength += data.length;
    this.parse();
  }

  ensureCapacity(needed) {
    if (needed <= this.rxBuffer.length) {
      return;
    }

    let nextSize = this.rxBuffer.length;
    while (nextSize < needed) {
      nextSize *= 2;
    }

    const nextBuffer = new Uint8Array(nextSize);
    nextBuffer.set(this.rxBuffer.subarray(0, this.rxLength));
    this.rxBuffer = nextBuffer;
  }

  parse() {
    while (this.rxLength >= STREAM.headerBytes) {
      const magicIndex = this.findMagic();
      if (magicIndex < 0) {
        const keep = Math.min(this.rxLength, STREAM.magic.length - 1);
        const dropped = this.rxLength - keep;
        if (dropped > 0) {
          this.resyncEvents += 1;
          this.resyncBytes += dropped;
          this.consume(dropped);
        }
        return;
      }

      if (magicIndex > 0) {
        this.resyncEvents += 1;
        this.resyncBytes += magicIndex;
        this.consume(magicIndex);
        if (this.rxLength < STREAM.headerBytes) {
          return;
        }
      }

      const header = this.readHeader();
      if (!this.headerValid(header)) {
        this.invalidHeaders += 1;
        this.consume(1);
        continue;
      }

      const packetLength = STREAM.headerBytes + header.payloadLength;
      if (this.rxLength < packetLength) {
        return;
      }

      this.checkPacketSequence(header);
      if ((header.flags & STREAM.packetFlagDroppedBefore) !== 0) {
        this.droppedBeforePackets += 1;
      }

      if (header.msgId === STREAM.msgIdStatsReport) {
        this.readStats();
        this.statsPackets += 1;
        this.consume(packetLength);
        continue;
      }

      if (this.isControlResponse(header)) {
        this.controlPackets += 1;
        handleControlResponse({
          msgId: header.msgId,
          requestSequence: header.arg0,
          status: header.arg1,
          sessionId: header.arg2,
          payload: this.rxBuffer.slice(STREAM.headerBytes, packetLength),
        });
        this.consume(packetLength);
        continue;
      }

      if (header.msgId === STREAM.msgIdLogText) {
        const record = this.readLogRecord(header);
        if (record) {
          this.logRecords += 1;
          appendLogRecord(record);
        } else {
          this.invalidHeaders += 1;
        }
        this.consume(packetLength);
        continue;
      }

      if (header.msgId === STREAM.msgIdTelemetryScalar) {
        const sample = this.readTelemetryScalar(header);
        if (sample) {
          this.telemetrySamples += 1;
          if (this.recordTelemetry(sample)) {
            updateTelemetryRow(sample);
          }
        } else {
          this.invalidHeaders += 1;
        }
        this.consume(packetLength);
        continue;
      }

      const chunk = this.readFrameChunk(header);
      if (!chunk) {
        this.invalidHeaders += 1;
        this.consume(1);
        continue;
      }

      if (
        (header.flags & STREAM.packetFlagDroppedBefore) !== 0 &&
        (chunk.chunkFlags & STREAM.chunkFlagFrameStart) !== 0
      ) {
        this.expectedFrameId = chunk.frameId;
        this.expectedOffset = 0;
        this.assemblyFrame.fill(0);
      }

      this.checkFrameSequence(chunk);
      this.assemblyFrame.set(
        this.rxBuffer.subarray(STREAM.headerBytes + STREAM.ruiFrameChunkHeaderBytes, packetLength),
        chunk.offset,
      );
      this.chunks += 1;
      this.lastFrameId = chunk.frameId;
      this.lastChunkId = Math.floor(chunk.offset / STREAM.frameDataMaxBytes);
      this.lastOffset = chunk.offset;

      if ((chunk.chunkFlags & STREAM.chunkFlagFrameEnd) !== 0) {
        this.displayFrame.set(this.assemblyFrame);
        latestFrame = this.displayFrame;
        frameDirty = true;
        this.frames += 1;
      }

      this.consume(packetLength);
    }
  }

  recordTelemetry(sample) {
    const knownSignal = this.telemetryValues.has(sample.name);
    if (!knownSignal && this.telemetryValues.size >= DASHBOARD.maxTelemetrySignals) {
      return false;
    }

    this.telemetryValues.set(sample.name, sample);
    if (sample.valueType === 5) {
      this.telemetryHistory.delete(sample.name);
      selectedTelemetryNames.delete(sample.name);
      plotDirty = true;
      return true;
    }
    let history = this.telemetryHistory.get(sample.name);
    if (!history) {
      history = [];
      this.telemetryHistory.set(sample.name, history);
    }
    history.push({ timestampMs: sample.timestampMs, value: Number(sample.value) });
    if (history.length > DASHBOARD.historySamplesPerSignal) {
      history.splice(0, history.length - DASHBOARD.historySamplesPerSignal);
    }
    plotDirty = true;
    return true;
  }

  findMagic() {
    const end = this.rxLength - STREAM.magic.length;
    for (let index = 0; index <= end; index += 1) {
      if (
        this.rxBuffer[index] === STREAM.magic[0] &&
        this.rxBuffer[index + 1] === STREAM.magic[1] &&
        this.rxBuffer[index + 2] === STREAM.magic[2] &&
        this.rxBuffer[index + 3] === STREAM.magic[3]
      ) {
        return index;
      }
    }
    return -1;
  }

  readHeader() {
    return {
      version: this.rxBuffer[4],
      headerLength: this.rxBuffer[5],
      flags: this.readU16(6),
      msgId: this.readU32(8),
      sequence: this.readU32(12),
      payloadLength: this.readU32(16),
      arg0: this.readU32(20),
      arg1: this.readU32(24),
      arg2: this.readU32(28),
    };
  }

  headerValid(header) {
    const isFramePacket = header.msgId === STREAM.msgIdRuiWriteFrameBufferRaw;
    const isLogPacket = header.msgId === STREAM.msgIdLogText;
    const isStatsPacket = header.msgId === STREAM.msgIdStatsReport;
    const isTelemetryPacket = header.msgId === STREAM.msgIdTelemetryScalar;
    const isControlPacket = this.isControlResponse(header);
    return (
      header.version === STREAM.version &&
      header.headerLength === STREAM.headerBytes &&
      (header.flags & ~STREAM.packetFlagsKnown) === 0 &&
      header.payloadLength <= STREAM.packetPayloadMaxBytes &&
      ((isFramePacket && header.payloadLength > STREAM.ruiFrameChunkHeaderBytes) ||
        (isLogPacket && header.payloadLength >= STREAM.logRecordHeaderBytes) ||
        (isStatsPacket && header.payloadLength === STREAM.statsReportBytes) ||
        (isTelemetryPacket && header.payloadLength >= STREAM.telemetryScalarHeaderBytes) ||
        (isControlPacket &&
          header.payloadLength <= STREAM.helloResponseBytes &&
          (header.msgId !== STREAM.msgIdControlHello ||
            header.payloadLength === STREAM.helloResponseBytes)))
    );
  }

  isControlResponse(header) {
    return (
      (header.flags & STREAM.packetFlagResponse) !== 0 &&
      [
        STREAM.msgIdControlHello,
        STREAM.msgIdControlSetChannels,
        STREAM.msgIdControlPing,
        STREAM.msgIdControlClose,
        STREAM.msgIdControlError,
      ].includes(header.msgId)
    );
  }

  beginFrameStream() {
    this.expectedFrameId = null;
    this.expectedOffset = 0;
    this.assemblyFrame.fill(0);
  }

  readStats() {
    const base = STREAM.headerBytes;
    this.lastStats = {
      streamSource: this.readU32(base),
      streamFlags: this.readU32(base + 4),
      nextSequence: this.readU32(base + 8),
      currentFrameId: this.readU32(base + 12),
      framesStarted: this.readU32(base + 16),
      framesCompleted: this.readU32(base + 20),
      framesDropped: this.readU32(base + 24),
      packetsSent: this.readU32(base + 28),
      bytesSent: this.readU32(base + 32),
      endpointBusyCount: this.readU32(base + 36),
      sendErrorCount: this.readU32(base + 40),
      rxCommandCount: this.readU32(base + 44),
      controlResponseDropCount: this.readU32(base + 48),
      controlResponseQueueHighWater: this.readU32(base + 52),
      logDropCount: this.readU32(base + 56),
      logQueueHighWater: this.readU32(base + 60),
      telemetryDropCount: this.readU32(base + 64),
      telemetryQueueHighWater: this.readU32(base + 68),
      telemetryCoalesceCount: this.readU32(base + 72),
    };
  }

  readLogRecord(header) {
    const base = STREAM.headerBytes;
    const timestampMs = this.readU32(base);
    const recordId = this.readU32(base + 4);
    const textBytes = this.readU16(base + 8);
    const level = this.rxBuffer[base + 10];
    const categoryBytes = this.rxBuffer[base + 11];
    const categoryStart = base + STREAM.logRecordHeaderBytes;
    const textStart = categoryStart + categoryBytes;
    const payloadEnd = base + header.payloadLength;

    if (level > 4 || textStart + textBytes !== payloadEnd) {
      return null;
    }

    return {
      timestampMs,
      recordId,
      level,
      category: utf8Decoder.decode(this.rxBuffer.slice(categoryStart, textStart)),
      text: utf8Decoder.decode(this.rxBuffer.slice(textStart, payloadEnd)),
    };
  }

  readTelemetryScalar(header) {
    const base = STREAM.headerBytes;
    const timestampMs = this.readU32(base);
    const sampleId = this.readU32(base + 4);
    const valueBits = this.readU32(base + 8);
    const nameBytes = this.readU16(base + 12);
    const valueType = this.rxBuffer[base + 14];
    const unitsBytes = this.rxBuffer[base + 15];
    const nameStart = base + STREAM.telemetryScalarHeaderBytes;
    const unitsStart = nameStart + nameBytes;
    const textStart = unitsStart + unitsBytes;
    const payloadEnd = base + header.payloadLength;
    const textValue = valueType === 5;
    const textBytes = textValue ? valueBits : 0;

    if (
      nameBytes === 0 || valueType < 1 || valueType > 5 ||
      (textValue && (textBytes === 0 || textBytes > STREAM.telemetryTextMaxBytes || unitsBytes !== 0)) ||
      textStart + textBytes !== payloadEnd
    ) {
      return null;
    }

    let value;
    if (valueType === 1) {
      value = valueBits | 0;
    } else if (valueType === 2) {
      value = valueBits;
    } else if (valueType === 3) {
      const bits = new ArrayBuffer(4);
      const view = new DataView(bits);
      view.setUint32(0, valueBits, true);
      value = view.getFloat32(0, true);
    } else if (valueType === 4) {
      value = valueBits !== 0;
    } else {
      value = utf8Decoder.decode(this.rxBuffer.slice(textStart, payloadEnd));
    }

    return {
      timestampMs,
      sampleId,
      valueType,
      value,
      name: utf8Decoder.decode(this.rxBuffer.slice(nameStart, unitsStart)),
      units: utf8Decoder.decode(this.rxBuffer.slice(unitsStart, textStart)),
    };
  }

  readFrameChunk(header) {
    const base = STREAM.headerBytes;
    const dataLength = header.payloadLength - STREAM.ruiFrameChunkHeaderBytes;
    const chunk = {
      frameId: this.readU32(base),
      offset: this.readU32(base + 4),
      frameBytes: this.readU32(base + 8),
      width: this.readU16(base + 12),
      height: this.readU16(base + 14),
      pixelFormat: this.readU16(base + 16),
      bufferId: this.readU16(base + 18),
      chunkFlags: this.readU32(base + 20),
      dataLength,
    };

    if (
      chunk.frameBytes !== STREAM.frameBytes ||
      chunk.width !== STREAM.frameWidth ||
      chunk.height !== STREAM.frameHeight ||
      chunk.pixelFormat !== STREAM.pixelFormatRgb565Le ||
      chunk.bufferId !== 0 ||
      chunk.offset >= chunk.frameBytes ||
      chunk.offset + dataLength > chunk.frameBytes ||
      (chunk.chunkFlags & ~STREAM.chunkFlagsKnown) !== 0 ||
      header.arg0 !== chunk.frameId ||
      header.arg1 !== chunk.offset ||
      header.arg2 !== dataLength
    ) {
      return null;
    }

    return chunk;
  }

  checkPacketSequence(header) {
    if (this.expectedPacketSequence !== null && header.sequence !== this.expectedPacketSequence) {
      this.sequenceErrors += 1;
    }
    this.expectedPacketSequence = header.sequence + 1;
  }

  checkFrameSequence(chunk) {
    if (
      this.expectedFrameId !== null &&
      (chunk.frameId !== this.expectedFrameId || chunk.offset !== this.expectedOffset)
    ) {
      this.sequenceErrors += 1;
    }

    if (chunk.offset === 0 && (chunk.chunkFlags & STREAM.chunkFlagFrameStart) === 0) {
      this.sequenceErrors += 1;
    }

    if ((chunk.chunkFlags & STREAM.chunkFlagFrameEnd) !== 0) {
      if (chunk.offset + chunk.dataLength !== STREAM.frameBytes) {
        this.sequenceErrors += 1;
      }
      this.expectedFrameId = chunk.frameId + 1;
      this.expectedOffset = 0;
    } else {
      if (chunk.dataLength !== STREAM.frameDataMaxBytes) {
        this.sequenceErrors += 1;
      }
      this.expectedFrameId = chunk.frameId;
      this.expectedOffset = chunk.offset + chunk.dataLength;
    }
  }

  readU16(offset) {
    return this.rxBuffer[offset] | (this.rxBuffer[offset + 1] << 8);
  }

  readU32(offset) {
    return (
      (this.rxBuffer[offset] |
        (this.rxBuffer[offset + 1] << 8) |
        (this.rxBuffer[offset + 2] << 16) |
        (this.rxBuffer[offset + 3] << 24)) >>>
      0
    );
  }

  consume(count) {
    if (count <= 0) {
      return;
    }
    this.rxBuffer.copyWithin(0, count, this.rxLength);
    this.rxLength -= count;
  }
}

const parser = new StreamParser();

function setConnectionState(text) {
  dashboard.setConnection({ connected: port !== null, live: streaming, label: text });
}

function setButtons() {
  const connected = port !== null;
  const actionsSupported = sessionActive && (helloCapabilities & STREAM.capabilitySystemActions) !== 0;
  const raceWaiting = systemMode === "RACE / WAITING";
  const raceRunning = systemMode === "RACE RUNNING";
  const raceMode = raceWaiting || raceRunning;
  const raceReady = systemMode === "RACE / WAITING" && systemState === "READY TO START";
  ui.connectButton.textContent = connected ? "Disconnect" : "Connect";
  ui.startButton.disabled = !connected || !sessionActive || streaming;
  ui.stopButton.disabled = !connected || !sessionActive || !streaming;
  ui.resetButton.disabled = !connected;
  ui.raceStartButton.disabled = !actionsSupported || !raceReady;
  ui.raceStartButton.classList.toggle("ready", actionsSupported && raceReady);
  ui.raceStopButton.disabled = !actionsSupported;
  ui.raceControls.hidden = !raceMode;
  ui.raceControls.classList.toggle("waiting", raceWaiting);
  ui.raceControls.classList.toggle("running", raceRunning);
  ui.raceStartButton.hidden = !raceWaiting;
  ui.raceStopButton.hidden = !raceMode;
  ui.raceControlStatus.textContent = "";
}

function updateDashboardTelemetry(sample) {
  dashboard.updateTelemetry(sample);
  if (sample.name === "system.mode") systemMode = String(sample.value);
  if (sample.name === "system.state") systemState = String(sample.value);

  if (sample.name === "system.mode" || sample.name === "system.state") {
    setButtons();
  }
}

function formatNumber(value) {
  return new Intl.NumberFormat(undefined, { maximumFractionDigits: 0 }).format(value);
}

function formatRate(bytesPerSecond) {
  return `${(bytesPerSecond / (1024 * 1024)).toFixed(3)} MiB/s`;
}

function rgb565ToImageData(source, target) {
  for (let src = 0, dst = 0; src < source.length; src += 2, dst += 4) {
    const value = source[src] | (source[src + 1] << 8);
    const red = (value >> 11) & 0x1f;
    const green = (value >> 5) & 0x3f;
    const blue = value & 0x1f;

    target[dst] = (red << 3) | (red >> 2);
    target[dst + 1] = (green << 2) | (green >> 4);
    target[dst + 2] = (blue << 3) | (blue >> 2);
    target[dst + 3] = 255;
  }
}

function renderLoop() {
  if (frameDirty) {
    rgb565ToImageData(latestFrame, imageData.data);
    context.putImageData(imageData, 0, 0);
    frameDirty = false;
    renderedFrames += 1;
  }
  if (plotDirty) {
    drawTelemetryPlots();
    plotDirty = false;
  }
  window.requestAnimationFrame(renderLoop);
}

function formatPlotValue(value) {
  const absolute = Math.abs(value);
  if (absolute !== 0 && (absolute >= 100000 || absolute < 0.001)) {
    return value.toExponential(3);
  }
  return Number(value.toPrecision(6)).toString();
}

function drawTelemetryPlots() {
  const width = ui.telemetryPlotCanvas.width;
  const height = ui.telemetryPlotCanvas.height;
  const rootStyle = getComputedStyle(document.documentElement);
  const surface = rootStyle.getPropertyValue("--surface").trim() || "#ffffff";
  const line = rootStyle.getPropertyValue("--line").trim() || "#d7dde4";
  const muted = rootStyle.getPropertyValue("--muted").trim() || "#5b6775";
  const series = [];

  for (const name of selectedTelemetryNames) {
    const history = parser.telemetryHistory.get(name);
    if (history && history.length > 0) {
      series.push({ name, history, sample: parser.telemetryValues.get(name) });
    }
  }

  plotContext.fillStyle = surface;
  plotContext.fillRect(0, 0, width, height);
  ui.plotLegend.replaceChildren();
  ui.telemetryPlotCanvas.dataset.seriesCount = String(series.length);
  ui.telemetryPlotCanvas.dataset.maxHistorySamples = String(
    series.reduce((maximum, item) => Math.max(maximum, item.history.length), 0),
  );

  const left = 54;
  const right = 18;
  const top = 18;
  const bottom = 32;
  const plotWidth = width - left - right;
  const plotHeight = height - top - bottom;

  plotContext.strokeStyle = line;
  plotContext.lineWidth = 1;
  plotContext.beginPath();
  for (let division = 0; division <= 4; division += 1) {
    const y = top + (plotHeight * division) / 4;
    plotContext.moveTo(left, y);
    plotContext.lineTo(width - right, y);
  }
  plotContext.stroke();

  if (series.length === 0) {
    ui.plotStatus.textContent = selectedTelemetryNames.size
      ? "Waiting for selected telemetry"
      : "No signals selected";
    plotContext.fillStyle = muted;
    plotContext.font = "14px Segoe UI, sans-serif";
    plotContext.textAlign = "center";
    plotContext.fillText(ui.plotStatus.textContent, width / 2, height / 2);
    return;
  }

  let firstTimestamp = Number.POSITIVE_INFINITY;
  let lastTimestamp = Number.NEGATIVE_INFINITY;
  for (const item of series) {
    firstTimestamp = Math.min(firstTimestamp, item.history[0].timestampMs);
    lastTimestamp = Math.max(lastTimestamp, item.history[item.history.length - 1].timestampMs);
  }
  const timeSpan = Math.max(lastTimestamp - firstTimestamp, 1);

  series.forEach((item, index) => {
    const color = DASHBOARD.colors[index % DASHBOARD.colors.length];
    let minimum = Number.POSITIVE_INFINITY;
    let maximum = Number.NEGATIVE_INFINITY;
    for (const point of item.history) {
      minimum = Math.min(minimum, point.value);
      maximum = Math.max(maximum, point.value);
    }
    const padding = maximum === minimum ? Math.max(Math.abs(maximum) * 0.01, 1e-9) : 0;
    const plotMinimum = minimum - padding;
    const plotMaximum = maximum + padding;
    const valueSpan = plotMaximum - plotMinimum;

    plotContext.strokeStyle = color;
    plotContext.fillStyle = color;
    plotContext.lineWidth = 2;
    plotContext.beginPath();
    item.history.forEach((point, pointIndex) => {
      const x = left + ((point.timestampMs - firstTimestamp) / timeSpan) * plotWidth;
      const y = top + (1 - (point.value - plotMinimum) / valueSpan) * plotHeight;
      if (pointIndex === 0) {
        plotContext.moveTo(x, y);
      } else {
        plotContext.lineTo(x, y);
      }
    });
    plotContext.stroke();
    if (item.history.length === 1) {
      plotContext.beginPath();
      plotContext.arc(left, top + plotHeight / 2, 3, 0, Math.PI * 2);
      plotContext.fill();
    }

    const legendItem = document.createElement("span");
    legendItem.className = "plot-legend-item";
    const swatch = document.createElement("span");
    swatch.className = "plot-swatch";
    swatch.style.backgroundColor = color;
    const label = document.createElement("span");
    const units = item.sample.units ? ` ${item.sample.units}` : "";
    label.textContent = `${item.name}: ${formatPlotValue(minimum)}..${formatPlotValue(maximum)}${units}`;
    legendItem.append(swatch, label);
    ui.plotLegend.appendChild(legendItem);
  });

  plotContext.fillStyle = muted;
  plotContext.font = "12px Segoe UI, sans-serif";
  plotContext.textAlign = "left";
  plotContext.fillText(`${firstTimestamp} ms`, left, height - 10);
  plotContext.textAlign = "right";
  plotContext.fillText(`${lastTimestamp} ms`, width - right, height - 10);
  ui.plotStatus.textContent = `${series.length} plotted / ${selectedTelemetryNames.size} selected`;
}

function appendLogRecord(record) {
  const entry = document.createElement("div");
  const levels = ["TRACE", "DEBUG", "INFO", "WARN", "ERROR"];
  entry.className =
    record.level === 4 ? "log-entry-error" : record.level === 3 ? "log-entry-warning" : "";
  entry.textContent = `${record.timestampMs.toString().padStart(8)} ${levels[record.level]} [${record.category}] ${record.text}`;
  ui.logConsole.appendChild(entry);
  while (ui.logConsole.childElementCount > 200) {
    ui.logConsole.firstElementChild.remove();
  }
  ui.logConsole.scrollTop = ui.logConsole.scrollHeight;
}

function updateTelemetryRow(sample) {
  updateDashboardTelemetry(sample);
  let row = ui.telemetryTableBody.querySelector(`[data-telemetry-name="${CSS.escape(sample.name)}"]`);
  if (!row) {
    row = document.createElement("tr");
    row.dataset.telemetryName = sample.name;
    for (let index = 0; index < 6; index += 1) {
      row.appendChild(document.createElement("td"));
    }
    const plotToggle = document.createElement("input");
    plotToggle.type = "checkbox";
    plotToggle.checked = selectedTelemetryNames.has(sample.name);
    plotToggle.setAttribute("aria-label", `Plot ${sample.name}`);
    plotToggle.addEventListener("change", () => {
      if (plotToggle.checked) {
        if (selectedTelemetryNames.size >= DASHBOARD.maxSelectedPlots) {
          plotToggle.checked = false;
          ui.plotStatus.textContent = `Select at most ${DASHBOARD.maxSelectedPlots} signals`;
          return;
        }
        selectedTelemetryNames.add(sample.name);
      } else {
        selectedTelemetryNames.delete(sample.name);
      }
      plotDirty = true;
    });
    row.children[0].appendChild(plotToggle);
    ui.telemetryTableBody.appendChild(row);
  }

  const plotToggle = row.children[0].querySelector("input");
  const textValue = sample.valueType === 5;
  if (plotToggle) {
    plotToggle.disabled = textValue;
    if (textValue) plotToggle.checked = false;
  }

  const typeNames = ["", "i32", "u32", "f32", "bool", "text"];
  const formattedValue =
    sample.valueType === 3 && Number.isFinite(sample.value) ? sample.value.toPrecision(7) : String(sample.value);
  row.children[1].textContent = sample.name;
  row.children[2].textContent = formattedValue;
  row.children[3].textContent = sample.units || "-";
  row.children[4].textContent = typeNames[sample.valueType];
  row.children[5].textContent = `${sample.timestampMs} ms`;
}

function updateStats() {
  const now = performance.now();
  const elapsed = Math.max((now - lastStats.time) / 1000, 0.001);
  const byteRate = (parser.bytes - lastStats.bytes) / elapsed;
  const frameRate = (parser.frames - lastStats.frames) / elapsed;
  const renderRate = (renderedFrames - lastStats.rendered) / elapsed;

  ui.byteRate.textContent = formatRate(byteRate);
  ui.frameRate.textContent = `${frameRate.toFixed(2)} fps`;
  dashboard.setFrameRate(frameRate);
  ui.renderRate.textContent = `${renderRate.toFixed(2)} fps`;
  ui.byteCount.textContent = formatNumber(parser.bytes);
  ui.frameCount.textContent = formatNumber(parser.frames);
  ui.chunkCount.textContent = formatNumber(parser.chunks);
  ui.lastFrame.textContent = parser.lastFrameId >= 0 ? String(parser.lastFrameId) : "-";
  ui.droppedBeforeCount.textContent = formatNumber(parser.droppedBeforePackets);
  ui.firmwareDropCount.textContent = formatNumber(parser.lastStats ? parser.lastStats.framesDropped : 0);
  ui.statsPacketCount.textContent = formatNumber(parser.statsPackets);
  ui.logRecordCount.textContent = formatNumber(parser.logRecords);
  ui.telemetrySampleCount.textContent = formatNumber(parser.telemetrySamples);
  ui.errorCount.textContent = formatNumber(parser.resyncEvents + parser.invalidHeaders + parser.sequenceErrors);
  ui.resyncCount.textContent = formatNumber(parser.resyncEvents);
  ui.resyncBytes.textContent = formatNumber(parser.resyncBytes);
  ui.sequenceErrors.textContent = formatNumber(parser.sequenceErrors);
  ui.endpointBusyCount.textContent = formatNumber(parser.lastStats ? parser.lastStats.endpointBusyCount : 0);
  ui.sendErrorCount.textContent = formatNumber(parser.lastStats ? parser.lastStats.sendErrorCount : 0);

  lastStats = {
    time: now,
    bytes: parser.bytes,
    frames: parser.frames,
    rendered: renderedFrames,
  };
}

function buildControlPacket(msgId, arg0 = 0, arg1 = 0, arg2 = 0) {
  const requestSequence = nextControlSequence;
  nextControlSequence = (nextControlSequence + 1) >>> 0;
  const packet = new Uint8Array(STREAM.headerBytes);
  packet.set(STREAM.magic, 0);
  const view = new DataView(packet.buffer);
  view.setUint8(4, STREAM.version);
  view.setUint8(5, STREAM.headerBytes);
  view.setUint16(6, 0, true);
  view.setUint32(8, msgId, true);
  view.setUint32(12, requestSequence, true);
  view.setUint32(16, 0, true);
  view.setUint32(20, arg0, true);
  view.setUint32(24, arg1, true);
  view.setUint32(28, arg2, true);
  return { packet, requestSequence };
}

function writePacket(packet) {
  const activePort = port;
  const operation = writeChain.catch(() => {}).then(async () => {
    if (!activePort || activePort !== port || !activePort.writable) {
      throw new Error("CDC port is not writable");
    }
    const writer = activePort.writable.getWriter();
    try {
      await writer.write(packet);
    } finally {
      writer.releaseLock();
    }
  });
  writeChain = operation;
  return operation;
}

function requestControl(msgId, arg0 = 0, arg1 = 0, arg2 = 0, timeoutMs = 2000) {
  const { packet, requestSequence } = buildControlPacket(msgId, arg0, arg1, arg2);
  return new Promise((resolve, reject) => {
    const timeout = window.setTimeout(() => {
      pendingControlRequests.delete(requestSequence);
      reject(new Error(`control response timeout for 0x${msgId.toString(16)}`));
    }, timeoutMs);

    pendingControlRequests.set(requestSequence, { msgId, resolve, reject, timeout });
    void writePacket(packet).catch((error) => {
      window.clearTimeout(timeout);
      pendingControlRequests.delete(requestSequence);
      reject(error);
    });
  });
}

function handleControlResponse(response) {
  const pending = pendingControlRequests.get(response.requestSequence);
  if (!pending) {
    return;
  }
  if (response.msgId !== pending.msgId && response.msgId !== STREAM.msgIdControlError) {
    return;
  }

  window.clearTimeout(pending.timeout);
  pendingControlRequests.delete(response.requestSequence);
  pending.resolve(response);
}

async function readLoop() {
  keepReading = true;
  while (port && port.readable && keepReading) {
    reader = port.readable.getReader();
    try {
      for (;;) {
        const { value, done } = await reader.read();
        if (done || !keepReading) {
          break;
        }
        if (value) {
          parser.push(value);
        }
      }
    } catch (error) {
      setConnectionState(`Read error: ${error.message}`);
    } finally {
      reader.releaseLock();
      reader = null;
    }
  }

  if (port && keepReading) {
    setConnectionState("Read stopped");
  }
}

function startWatchdog() {
  stopWatchdog();
  watchdogTimer = window.setInterval(() => {
    if (!streaming || !port) {
      return;
    }

    const staleMs = performance.now() - lastRxAt;
    if (staleMs > 2500) {
      setConnectionState("No frames from requested camera channel");
    }
    if (staleMs > 5000) {
      lastRxAt = performance.now();
      setConnectionState("Restarting stream");
      parser.beginFrameStream();
      void requestControl(
        STREAM.msgIdControlSetChannels,
        STREAM.channelFrames | STREAM.channelStats | STREAM.channelLogs | STREAM.channelTelemetry,
        STREAM.streamSourceCamera,
      ).catch((error) => setConnectionState(`Restart failed: ${error.message}`));
    }
  }, 1000);
}

function stopWatchdog() {
  if (watchdogTimer !== null) {
    window.clearInterval(watchdogTimer);
    watchdogTimer = null;
  }
}

async function connect() {
  if (!("serial" in navigator)) {
    setConnectionState("WebSerial unavailable");
    return;
  }

  port = await navigator.serial.requestPort();
  await port.open({ baudRate: 115200, bufferSize: 4 * 1024 * 1024 });
  sessionActive = false;
  helloCapabilities = 0;
  systemMode = "";
  systemState = "";
  leftWheelRpm = 0;
  rightWheelRpm = 0;
  dashboard.resetTelemetry();
  streaming = false;
  nextControlSequence = 0;
  writeChain = Promise.resolve();
  parser.reset();
  ui.logConsole.replaceChildren();
  ui.telemetryTableBody.replaceChildren();
  renderedFrames = 0;
  frameDirty = false;
  plotDirty = true;
  lastStats = {
    time: performance.now(),
    bytes: 0,
    frames: 0,
    rendered: 0,
  };
  setConnectionState("CDC open; negotiating telemetry session");
  setButtons();
  void readLoop();

  const hello = await requestControl(STREAM.msgIdControlHello);
  if (hello.status !== STREAM.controlStatusOk || hello.payload.length !== STREAM.helloResponseBytes) {
    throw new Error(`telemetry HELLO failed with status ${hello.status}`);
  }
  const helloView = new DataView(hello.payload.buffer, hello.payload.byteOffset, hello.payload.byteLength);
  helloCapabilities = helloView.getUint32(0, true);
  if ((helloCapabilities & 1) === 0) {
    throw new Error("device does not advertise framed control");
  }
  sessionActive = true;
  setConnectionState(`Telemetry session ${hello.sessionId}`);
  setButtons();
  await startStream();
}

async function disconnect() {
  stopWatchdog();
  try {
    if (sessionActive) {
      try {
        await requestControl(STREAM.msgIdControlClose);
      } catch (error) {
        setConnectionState(`Closing after control error: ${error.message}`);
      }
    }
    streaming = false;
    sessionActive = false;
    helloCapabilities = 0;
    systemMode = "";
    systemState = "";
    leftWheelRpm = 0;
    rightWheelRpm = 0;
    dashboard.resetTelemetry();
    keepReading = false;
    if (reader) {
      await reader.cancel();
    }
    if (port) {
      await port.close();
    }
  } finally {
    port = null;
    reader = null;
    for (const pending of pendingControlRequests.values()) {
      window.clearTimeout(pending.timeout);
      pending.reject(new Error("CDC disconnected"));
    }
    pendingControlRequests.clear();
    setConnectionState("Disconnected");
    setButtons();
  }
}

async function startStream() {
  if (!port) {
    await connect();
    return;
  }
  if (!sessionActive) {
    throw new Error("telemetry session is not active");
  }
  parser.beginFrameStream();
  lastRxAt = performance.now();
  const response = await requestControl(
    STREAM.msgIdControlSetChannels,
    STREAM.channelFrames | STREAM.channelStats | STREAM.channelLogs | STREAM.channelTelemetry,
    STREAM.streamSourceCamera,
  );
  if (response.status !== STREAM.controlStatusOk) {
    throw new Error(`camera channel request failed with status ${response.status}`);
  }
  streaming = true;
  startWatchdog();
  setConnectionState("Streaming");
  setButtons();
  document.querySelector("details.operator").open = false;
}

async function stopStream() {
  const response = await requestControl(STREAM.msgIdControlSetChannels);
  if (response.status !== STREAM.controlStatusOk) {
    throw new Error(`stop request failed with status ${response.status}`);
  }
  streaming = false;
  stopWatchdog();
  setConnectionState(port ? "Telemetry session ready" : "Disconnected");
  setButtons();
}

async function requestSystemAction(action, confirmation = 0) {
  ui.raceControlStatus.textContent = action === STREAM.systemActionStop ? "Sending STOP..." : "Requesting race start...";
  const response = await requestControl(
    STREAM.msgIdControlSystemAction,
    action,
    confirmation,
    0,
  );
  if (response.status === STREAM.controlStatusOk) {
    ui.raceControlStatus.textContent = action === STREAM.systemActionStop ? "STOP accepted; outputs disabled" : "Race start accepted; awaiting state telemetry";
    return;
  }
  if (response.status === STREAM.controlStatusNotReady) {
    ui.raceControlStatus.textContent = "Start rejected: the camera is not ready";
    return;
  }
  if (response.status === STREAM.controlStatusDenied) {
    ui.raceControlStatus.textContent = "Action denied by the firmware state machine";
    return;
  }
  throw new Error(`system action failed with status ${response.status}`);
}

function cancelRaceStartHold() {
  if (raceStartHoldTimer !== null) {
    window.clearTimeout(raceStartHoldTimer);
    raceStartHoldTimer = null;
  }
  ui.raceStartButton.classList.remove("holding");
}

function beginRaceStartHold() {
  if (ui.raceStartButton.disabled || raceStartHoldTimer !== null) return;
  ui.raceStartButton.classList.add("holding");
  ui.raceControlStatus.textContent = "Keep holding to start the race...";
  raceStartHoldTimer = window.setTimeout(() => {
    raceStartHoldTimer = null;
    ui.raceStartButton.classList.remove("holding");
    runUiAction(() => requestSystemAction(STREAM.systemActionRaceStart, STREAM.raceStartConfirmation));
  }, 1500);
}

function resetStats() {
  parser.reset();
  ui.logConsole.replaceChildren();
  ui.telemetryTableBody.replaceChildren();
  renderedFrames = 0;
  frameDirty = false;
  plotDirty = true;
  lastStats = {
    time: performance.now(),
    bytes: 0,
    frames: 0,
    rendered: 0,
  };
  updateStats();
}

function runUiAction(action) {
  void action().catch((error) => {
    setConnectionState(`Error: ${error.message}`);
    setButtons();
  });
}

ui.connectButton.addEventListener("click", () => {
  if (port) {
    runUiAction(disconnect);
  } else {
    runUiAction(connect);
  }
});
ui.startButton.addEventListener("click", () => runUiAction(startStream));
ui.stopButton.addEventListener("click", () => runUiAction(stopStream));
ui.resetButton.addEventListener("click", resetStats);
ui.raceStartButton.addEventListener("pointerdown", (event) => {
  if (event.button === 0) beginRaceStartHold();
});
for (const eventName of ["pointerup", "pointercancel", "pointerleave"]) {
  ui.raceStartButton.addEventListener(eventName, cancelRaceStartHold);
}
ui.raceStartButton.addEventListener("keydown", (event) => {
  if ((event.key === " " || event.key === "Enter") && !event.repeat) {
    event.preventDefault();
    beginRaceStartHold();
  }
});
ui.raceStartButton.addEventListener("keyup", (event) => {
  if (event.key === " " || event.key === "Enter") cancelRaceStartHold();
});
ui.raceStopButton.addEventListener("click", () => {
  cancelRaceStartHold();
  runUiAction(() => requestSystemAction(STREAM.systemActionStop));
});
document.addEventListener("keydown", (event) => {
  if (event.key.toLowerCase() === "f") document.documentElement.requestFullscreen?.();
  if (event.key.toLowerCase() === "d") {
    const diagnostics = document.querySelector("details.diagnostics");
    diagnostics.open = !diagnostics.open;
  }
});

if (!("serial" in navigator)) {
  setConnectionState("WebSerial unavailable");
  ui.connectButton.disabled = true;
}

renderLoop();
setButtons();
setInterval(updateStats, 1000);
