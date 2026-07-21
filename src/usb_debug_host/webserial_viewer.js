"use strict";

const STREAM = {
  magic: [0x41, 0x56, 0x43, 0x55],
  headerBytes: 32,
  ruiFrameChunkHeaderBytes: 24,
  statsReportBytes: 48,
  frameWidth: 320,
  frameHeight: 200,
  frameBytes: 320 * 200 * 2,
  packetPayloadMaxBytes: 16 * 1024 - 32,
  msgIdRuiWriteFrameBufferRaw: 0x01000002,
  msgIdStatsReport: 0x01000300,
  packetFlagsKnown: 0x000f,
  packetFlagDroppedBefore: 1 << 3,
  frameDataMaxBytes: 16 * 1024 - 32 - 24,
  pixelFormatRgb565Le: 1,
  chunkFlagFrameStart: 1,
  chunkFlagFrameEnd: 2,
  chunkFlagsKnown: 1 | 2 | 4,
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
  errorCount: document.getElementById("errorCount"),
  resyncCount: document.getElementById("resyncCount"),
  resyncBytes: document.getElementById("resyncBytes"),
  sequenceErrors: document.getElementById("sequenceErrors"),
  endpointBusyCount: document.getElementById("endpointBusyCount"),
  sendErrorCount: document.getElementById("sendErrorCount"),
};

const context = ui.canvas.getContext("2d", { alpha: false });
const imageData = context.createImageData(STREAM.frameWidth, STREAM.frameHeight);

let port = null;
let reader = null;
let keepReading = false;
let streaming = false;
let lastRxAt = 0;
let watchdogTimer = null;
let latestFrame = new Uint8Array(STREAM.frameBytes);
let frameDirty = false;
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

      const chunk = this.readFrameChunk(header);
      if (!chunk) {
        this.invalidHeaders += 1;
        this.consume(1);
        continue;
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
    const isStatsPacket = header.msgId === STREAM.msgIdStatsReport;
    return (
      header.version === 1 &&
      header.headerLength === STREAM.headerBytes &&
      (header.flags & ~STREAM.packetFlagsKnown) === 0 &&
      header.payloadLength <= STREAM.packetPayloadMaxBytes &&
      ((isFramePacket && header.payloadLength > STREAM.ruiFrameChunkHeaderBytes) ||
        (isStatsPacket && header.payloadLength === STREAM.statsReportBytes))
    );
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
  ui.connectionState.textContent = text;
}

function setButtons() {
  const connected = port !== null;
  ui.connectButton.textContent = connected ? "Disconnect" : "Connect";
  ui.startButton.disabled = !connected || streaming;
  ui.stopButton.disabled = !connected || !streaming;
  ui.resetButton.disabled = !connected;
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
  window.requestAnimationFrame(renderLoop);
}

function updateStats() {
  const now = performance.now();
  const elapsed = Math.max((now - lastStats.time) / 1000, 0.001);
  const byteRate = (parser.bytes - lastStats.bytes) / elapsed;
  const frameRate = (parser.frames - lastStats.frames) / elapsed;
  const renderRate = (renderedFrames - lastStats.rendered) / elapsed;

  ui.byteRate.textContent = formatRate(byteRate);
  ui.frameRate.textContent = `${frameRate.toFixed(2)} fps`;
  ui.renderRate.textContent = `${renderRate.toFixed(2)} fps`;
  ui.byteCount.textContent = formatNumber(parser.bytes);
  ui.frameCount.textContent = formatNumber(parser.frames);
  ui.chunkCount.textContent = formatNumber(parser.chunks);
  ui.lastFrame.textContent = parser.lastFrameId >= 0 ? String(parser.lastFrameId) : "-";
  ui.droppedBeforeCount.textContent = formatNumber(parser.droppedBeforePackets);
  ui.firmwareDropCount.textContent = formatNumber(parser.lastStats ? parser.lastStats.framesDropped : 0);
  ui.statsPacketCount.textContent = formatNumber(parser.statsPackets);
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

async function writeCommand(command) {
  if (!port || !port.writable) {
    return;
  }

  const writer = port.writable.getWriter();
  try {
    await writer.write(new TextEncoder().encode(`${command}\n`));
  } finally {
    writer.releaseLock();
  }
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
      setConnectionState("No frames; check test mode");
    }
    if (staleMs > 5000) {
      parser.reset();
      renderedFrames = 0;
      frameDirty = false;
      lastStats = {
        time: performance.now(),
        bytes: 0,
        frames: 0,
        rendered: 0,
      };
      lastRxAt = performance.now();
      setConnectionState("Restarting stream");
      void writeCommand("START");
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
  setConnectionState("Connected");
  setButtons();
  void readLoop();
  await startStream();
}

async function disconnect() {
  streaming = false;
  keepReading = false;
  stopWatchdog();
  try {
    await writeCommand("STOP");
    if (reader) {
      await reader.cancel();
    }
    if (port) {
      await port.close();
    }
  } finally {
    port = null;
    reader = null;
    setConnectionState("Disconnected");
    setButtons();
  }
}

async function startStream() {
  if (!port) {
    await connect();
  }
  parser.reset();
  renderedFrames = 0;
  frameDirty = false;
  lastRxAt = performance.now();
  lastStats = {
    time: performance.now(),
    bytes: 0,
    frames: 0,
    rendered: 0,
  };
  await writeCommand("START");
  streaming = true;
  startWatchdog();
  setConnectionState("Streaming");
  setButtons();
}

async function stopStream() {
  await writeCommand("STOP");
  streaming = false;
  stopWatchdog();
  setConnectionState(port ? "Connected" : "Disconnected");
  setButtons();
}

function resetStats() {
  parser.reset();
  renderedFrames = 0;
  frameDirty = false;
  lastStats = {
    time: performance.now(),
    bytes: 0,
    frames: 0,
    rendered: 0,
  };
  updateStats();
}

ui.connectButton.addEventListener("click", () => {
  if (port) {
    void disconnect();
  } else {
    void connect();
  }
});
ui.startButton.addEventListener("click", () => void startStream());
ui.stopButton.addEventListener("click", () => void stopStream());
ui.resetButton.addEventListener("click", resetStats);

if (!("serial" in navigator)) {
  setConnectionState("WebSerial unavailable");
  ui.connectButton.disabled = true;
}

renderLoop();
setButtons();
setInterval(updateStats, 1000);
