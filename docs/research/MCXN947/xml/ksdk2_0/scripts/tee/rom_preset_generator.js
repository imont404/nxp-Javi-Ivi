/*
 * Copyright 2020-2026 NXP
 * To be used with MCUXpresso Config Tools under its Software License Agreement.
 */

scriptApi.requireScript('rom_file_descriptor.js');

// main function which collects all register init and TEE tool UI defined configuration for TrustZone preset initialization
function main() {
    new RomFileDescriptor().render();
}

// generation script entry point
main();