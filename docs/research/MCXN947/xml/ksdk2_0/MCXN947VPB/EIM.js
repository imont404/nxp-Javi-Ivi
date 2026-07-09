/*
 * Copyright 2016 Freescale Semiconductor, Inc.
 * Copyright 2016-2026 NXP
 * To be used with MCUXpresso Config Tools under its Software License Agreement.
 */
var _eim_error_injection_channel_enable = {
  "EIM0": {
    "0x800000":[{"id":"kEIM_MemoryChannelPKCRAMEnable", "description":"Memory channel 8(PKCRAM) error injection enable"}],
    "0x1000000":[{"id":"kEIM_MemoryChannelLPCACRAMEnable", "description":"Memory channel 7(LPCACRAM) error injection enable"}],
    "0x2000000":[{"id":"kEIM_MemoryChannelRAMFEnable", "description":"Memory channel 6(RAMF) error injection enable"}],
    "0x4000000":[{"id":"kEIM_MemoryChannelRAMEEnable", "description":"Memory channel 5(RAME) error injection enable"}],
    "0x8000000":[{"id":"kEIM_MemoryChannelRAMDEnable", "description":"Memory channel 4(RAMD) error injection enable"}],
    "0x10000000":[{"id":"kEIM_MemoryChannelRAMCEnable", "description":"Memory channel 3(RAMC) error injection enable"}],
    "0x20000000":[{"id":"kEIM_MemoryChannelRAMBEnable", "description":"Memory channel 2(RAMB) error injection enable"}],
    "0x40000000":[{"id":"kEIM_MemoryChannelRAMAEnable", "description":"Memory channel 1(RAMA) error injection enable "}],
    "0x80000000":[{"id":"kEIM_MemoryChannelRAMXEnable", "description":"Memory channel 0(RAMX) error injection enable"}]
  }
};
PExProcessor.putPersistentObject("eim_error_injection_channel_enable", _eim_error_injection_channel_enable);
_eim_error_injection_channel_enable = null;

var _eim_memory_channel = {
  "EIM0": {
    "0":[{"id":"kEIM_MemoryChannelRAMX", "description":"Memory RAMX"}],
    "1":[{"id":"kEIM_MemoryChannelRAMA", "description":"Memory RAMA "}],
    "2":[{"id":"kEIM_MemoryChannelRAMB", "description":"Memory RAMB"}],
    "3":[{"id":"kEIM_MemoryChannelRAMC", "description":"Memory RAMC"}],
    "4":[{"id":"kEIM_MemoryChannelRAMD", "description":"Memory RAMD"}],
    "5":[{"id":"kEIM_MemoryChannelRAME", "description":"Memory RAME"}],
    "6":[{"id":"kEIM_MemoryChannelRAMF", "description":"Memory RAMF"}],
    "7":[{"id":"kEIM_MemoryChannelLPCACRAM", "description":"Memory LPCACRAM"}],
    "8":[{"id":"kEIM_MemoryChannelPKCRAM", "description":"Memory PKCRAM"}]
  }
};
PExProcessor.putPersistentObject("eim_memory_channel", _eim_memory_channel);
_eim_memory_channel = null;
