/*
 * Copyright 2016 Freescale Semiconductor, Inc.
 * Copyright 2016-2026 NXP
 * To be used with MCUXpresso Config Tools under its Software License Agreement.
 */
var _erm_memory_channel = {
  "ERM0": {
    "0":[{"id":"kERM_MemoryChannelRAMX", "description":"Memory RAMX"}],
    "1":[{"id":"kERM_MemoryChannelRAMA", "description":"Memory RAMA "}],
    "2":[{"id":"kERM_MemoryChannelRAMB", "description":"Memory RAMB"}],
    "3":[{"id":"kERM_MemoryChannelRAMC", "description":"Memory RAMC"}],
    "4":[{"id":"kERM_MemoryChannelRAMD", "description":"Memory RAMD"}],
    "5":[{"id":"kERM_MemoryChannelRAME", "description":"Memory RAME"}],
    "6":[{"id":"kERM_MemoryChannelRAMF", "description":"Memory RAMF"}],
    "7":[{"id":"kERM_MemoryChannelLPCACRAM", "description":"Memory LPCACRAM"}],
    "8":[{"id":"kERM_MemoryChannelPKCRAM", "description":"Memory PKCRAM"}],
    "9":[{"id":"kERM_MemoryChannelFLASH", "description":"Memory FLASH"}]
  }
};
PExProcessor.putPersistentObject("erm_memory_channel", _erm_memory_channel);
_erm_memory_channel = null;
