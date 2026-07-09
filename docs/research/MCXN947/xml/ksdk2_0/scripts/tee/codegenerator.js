/*
 * Copyright 2020-2023 NXP
 * To be used with MCUXpresso Config Tools under its Software License Agreement.
 */

var ENTRY_FUNCTION_NAME = "BOARD_InitBootTEE";
var INIT_GRPFUNC_NAME = "BOARD_InitTEE";
var INIT_FUNCTION_NAME = "BOARD_InitTrustZone";
var TEE_WR_REG_MACRO = "TEE_WRITE_REGISTER";
var TEE_RMF_REG_MACRO = "TEE_READ_MODIFY_WRITE_REGISTER";

// Set legacy options global file names schema
// NOTE: Global options config API changed for TEE script profile since Config Tools v10 [MCUCM-9380].
if (scriptApi.getProfile().getToolOptionsConfig().getOptionValue("_legacy_source_names_")) {
    var LEGACY_SOURCE_NAMES = true;
    var C_FILE_NAME = "tzm_config.c";
    var H_FILE_NAME = "tzm_config.h";
    var H_FILE_DEFINE = "_TZM_CONFIG_H_";
} else {
    var LEGACY_SOURCE_NAMES = false;
    var C_FILE_NAME = "resource_config.c";
    var H_FILE_NAME = "resource_config.h";
    var H_FILE_DEFINE = "_RESOURCE_CONFIG_H_";
}

scriptApi.requireScript('utils.js');
scriptApi.requireScript('sau.js');
scriptApi.requireScript('mpu.js');
scriptApi.requireScript('security_peripheral.js');
scriptApi.requireScript('h_file_descriptor.js');
scriptApi.requireScript('c_file_descriptor.js');

// main function will iterate over all MCU cores, creates list of corresponding functional groups and if it is not empty, generates peripherals.c/.h source files
function main() {
    for (var coreIndex = 0; coreIndex < sharedResources.coresIds.length; coreIndex++) {
        var coreId = sharedResources.coresIds[coreIndex];
        if (sharedResources.coresJsons[coreId].enabled) {
            var code_type = scriptApi.getProfile().getToolOptionsConfig().getOptionValueAsString("_output_type_");
            //
            if (code_type == "c_code" || code_type == null) {
                new OutputFile("/trustzone/" + C_FILE_NAME, coreId).close(new CFileDescriptor().render());
                new OutputFile("/trustzone/" + H_FILE_NAME, coreId).close(new HFileDescriptor().render());
                break;
            } else {
                scriptApi.requireScript('rom_file_descriptor.js');
                new RomFileDescriptor().render();
                break;
            }
        }
    }
}

// global objects declarations
var javaUtils = scriptApi.getUtils();
var javaStaticUtils = Java.type("com.nxp.swtools.utils.scripting.api.ScriptUtils");
var utils = new Utils();
var sharedResources = new SharedResources();
var shareability = new Shareability();
var accessPermissionsAndPrivileges = new AccessPermissionsAndPrivileges();
var executable = new Executable();
var deviceAttributes = new DeviceAttributes();
var mairAttrReg = new MairAttrReg();
var sau = new Sau();
var mpu = new Mpu();

// functional groups will be filled from main() function for each core separatelly
var functionalGroupsMapper;

// configure security code instruction glitch hardening, in case it is not requested .. control per UI option to EN/DIS the resilient code registers write patch
sharedResources.enableSecurityCodeHardening(scriptApi.getProfile().getToolOptionsConfig().getOptionValue("_resilient_code_reg_writes_"));
// generation script entry point
main();