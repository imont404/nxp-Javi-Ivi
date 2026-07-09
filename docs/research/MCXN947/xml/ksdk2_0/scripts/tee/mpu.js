/*
 * Copyright 2020-2023 NXP
 * To be used with MCUXpresso Config Tools under its Software License Agreement.
 */

var MPU_FUNCTION_NAME = "BOARD_InitMPU";
var INIT_S_MPU_FUNCTION_NAME = "InitSecureMpu";
var INIT_NS_MPU_FUNCTION_NAME = "InitNonSecureMpu";
var MPU_PERIPHERAL_NAME = "MPU";
var MPU_NS_PERIPHERAL_NAME = "MPU_NS";
var MPU_NS_PREFIX = "MPU_NS";
var MPU_S_PREFIX = "MPU_S";
var MPU_NS_REGISTER_SUFFIX = "";

/**
 * Shared class for Secure and Non-Secure MPUs used to generate code
 * @param {String} definePrefix prefix used in defines - determines whether code is generated for Secure or Non-Secure MPU
 * @param {IScriptMpuConfig} mpuConfig taken from Java configuration
 * @param {String} name of the MPU. Used to generate comment
 * @returns {MpuCode}
 */
MpuCode = function (definePrefix, mpuConfig, name) {
    var self = this;
    self.definePrefix = definePrefix;
    self.mpuConfig = mpuConfig;
    self.name = name;

    // regions for which to generate code
    self.regions = mpu.getMemRegionConfigsToGenerateCodeFor(mpuConfig, true);
    self.memAttrConfigs = self.mpuConfig.getMemoryAttributesConfigs();

    // Non-Secure MPU registers may have additional suffix, e.g. "NS"
    self.suffix = definePrefix === MPU_NS_PREFIX ? MPU_NS_REGISTER_SUFFIX : "";

    // Non-Secure MPU peripheral may have different name, e.g. "MPU_NS"
    self.peripheral = definePrefix === MPU_NS_PREFIX ? MPU_NS_PERIPHERAL_NAME : MPU_PERIPHERAL_NAME;

    self.comment = utils.makeComment(self.name + " configuration");

    // generate all code for MPU, including comment, disabling mpu, code for regions configuration, forcing memory writes and enabling MPU
    self.generate = function (funcGrpPrefix) {
        var resultLines = [];
        resultLines.push(self.comment);
        utils.addEmptyLines(resultLines, 1);
        utils.addAll(resultLines, self.generateHead());
        utils.addEmptyLines(resultLines, 1);
        utils.addAll(resultLines, sharedResources.forceMemoryWrites);
        utils.addEmptyLines(resultLines, 1);
        utils.addAll(resultLines, self.generateMemAttrConfigs());
        utils.addEmptyLines(resultLines, 1);
        utils.addAll(resultLines, self.generateRegions(funcGrpPrefix));
        utils.addEmptyLines(resultLines, 1);
        utils.addAll(resultLines, self.generateTail());
        utils.addEmptyLines(resultLines, 1);
        utils.addAll(resultLines, sharedResources.forceMemoryWrites);
        utils.addEmptyLines(resultLines, 1);
        return resultLines;
    };

    // generate code to disable MPU at the beginning of configuration code
    self.generateHead = function () {
        var resultLines = [];
        resultLines.push(utils.makeComment("Set MPU Control register: Disable MPU"));
        resultLines.push(utils.createAssign(self.getMpuPeripheralName(), self.getRegisterName("CTRL"), javaUtils.formatHexValue(java.math.BigInteger.valueOf(0), 32))); // formatHexValue(value, width in bits)
        return resultLines;
    };

    // generate comment + code for enabling MPU
    self.generateTail = function () {
        var generatedLines = [];
        generatedLines.push(utils.makeComment("Set MPU control register: Enable MPU"));
        var assignValuesCtrl = [];
        assignValuesCtrl.push(javaUtils.createMaskedBitValueString(self.getCValue(self.mpuConfig.isDefaultPrivilegedAccessEnabled()), "MPU_CTRL_PRIVDEFENA_Pos", "MPU_CTRL_PRIVDEFENA_Msk"));
        assignValuesCtrl.push(javaUtils.createMaskedBitValueString(self.getCValue(self.mpuConfig.isHandlerOperationEnabled()), "MPU_CTRL_HFNMIENA_Pos", "MPU_CTRL_HFNMIENA_Msk"));
        assignValuesCtrl.push(javaUtils.createMaskedBitValueString(self.getCValue(self.mpuConfig.isEnabled()), "MPU_CTRL_ENABLE_Pos", "MPU_CTRL_ENABLE_Msk"));
        var assignString = utils.createAssign(self.getMpuPeripheralName(), self.getRegisterName("CTRL"), javaUtils.createAssignValue(assignValuesCtrl, 4));
        utils.addAll(generatedLines, utils.splitLines(assignString));
        return generatedLines;
    };

    // generate code for configuration of MPU Memory Attribute Indirection Registers 0 and 1
    self.generateMemAttrConfigs = function () {
        var ATTRIBUTES_IN_REGISTER = 4; // 4x8 bit attributes in one 32 bit register
        var generatedLines = [];
        var config;
        var attrValue;
        var regValue;
        generatedLines.push(utils.makeComment("Set MPU memory attribute indirection registers"));
        var numberOfRegisters = self.memAttrConfigs.length / ATTRIBUTES_IN_REGISTER;
        for (var regIndex = 0; regIndex < numberOfRegisters; regIndex++) {
            regValue = 0;
            for (var attrIndex = 0; attrIndex < ATTRIBUTES_IN_REGISTER; attrIndex++) {
                attrValue = 0;
                // get config based on the current register + its position inside the register
                config = self.memAttrConfigs[regIndex * ATTRIBUTES_IN_REGISTER + attrIndex];
                var outer = config.getOuterAttributes();
                var inner = config.getInnerAttributes();
                // when memory type === "device", only device attribute is used. All other bits should remain zero
                if (config.getMemoryType().getId() === "device") {
                    attrValue += deviceAttributes.getValue(config.getDeviceAttributes()) << mairAttrReg.DEVICE_POS;
                } else {
                    // if outer is not "device", fill all other bits (device is 0, so there is no need to fill it here)
                    attrValue += self.getMemAttrValue(outer) << mairAttrReg.OUTER_BLOCK_POS;
                    attrValue += self.getMemAttrValue(inner) << mairAttrReg.INNER_BLOCK_POS;
                }
                // save value of the current attribute to the register based on its position
                regValue += attrValue << (attrIndex * mairAttrReg.SIZE);
            }
            // required to force JavaScript to use unsigned value
            var unsignedValue = regValue >>> 0;
            // write to register
            generatedLines.push(utils.createAssign(self.getMpuPeripheralName(), self.getRegisterName("MAIR" + regIndex.toString()), javaUtils.formatHexValue(unsignedValue, 32)));
        }
        return generatedLines;
    };
    
    /**
     * Get value that represents current state of the mpuMemAttributes.
     * This value has 4-bits.
     * @param {type} mpuMemAttributes
     * @returns {Number} 4-bit number representing current state of the mpuMemAttributes
     */
    self.getMemAttrValue = function (mpuMemAttributes) {
        var value = 0;
        if (!mpuMemAttributes || !mpuMemAttributes.isCacheable()) {
            return 4; 
        }
        value += Number(!mpuMemAttributes.isTransient()) << mairAttrReg.TRANSIENT_POS;
        value += Number(mpuMemAttributes.isWriteBacked()) << mairAttrReg.WRITE_BACK_POS;
        value += Number(mpuMemAttributes.isReadAllocated()) << mairAttrReg.READ_ALLOCATION_POS;
        value += Number(mpuMemAttributes.isWriteAllocated()) << mairAttrReg.WRITE_ALLOCATION_POS;

        return value;
    };

    // generate code for a MPU region
    self.generateRegion = function (regionConfig, funcGrpPrefix) {
        var generatedLines = [];
        var regionIdx = regionConfig.getIndex();

        if (!utils.filterProblematicRegions(regionConfig)) {
            scriptApi.logWarning("Set MPU region number " + regionIdx + ", contains an error");
            generatedLines.push(utils.makeComment("Source code for the MPU region number " + regionIdx + " was not generated due to an error in the region's configuration"));
        } else {
            generatedLines.push("/* Set MPU region number */");
            generatedLines.push(utils.createAssign(self.getMpuPeripheralName(), self.getRegisterName("RNR"), regionIdx));
     
            generatedLines.push("/* Region base address */");
            var assignValuesBase = [];
            assignValuesBase.push("(" + utils.createDefineNameBase(funcGrpPrefix + self.definePrefix, regionIdx) + " & MPU_RBAR_BASE_Msk)");
            assignValuesBase.push(javaUtils.createMaskedBitValueString(shareability.getCValue(regionConfig.getShareability()), "MPU_RBAR_SH_Pos", "MPU_RBAR_SH_Msk"));
            assignValuesBase.push(javaUtils.createMaskedBitValueString(accessPermissionsAndPrivileges.getCValue(regionConfig.getAccessPermissions(), regionConfig.getAccessPrivileges()), "MPU_RBAR_AP_Pos", "MPU_RBAR_AP_Msk"));
            assignValuesBase.push(javaUtils.createMaskedBitValueString(executable.getCValue(regionConfig.isExecutable()), "MPU_RBAR_XN_Pos", "MPU_RBAR_XN_Msk"));
            var assignString = utils.createAssign(self.getMpuPeripheralName(), self.getRegisterName("RBAR"), javaUtils.createAssignValue(assignValuesBase, 4));
            utils.addAll(generatedLines, utils.splitLines(assignString));
     
            generatedLines.push("/* Region end address */");
            var assignValuesEnd = [];
            assignValuesEnd.push("(" + utils.createDefineNameEnd(funcGrpPrefix + self.definePrefix, regionIdx) + " & MPU_RLAR_LIMIT_Msk)");
            var selectedAttrConfig = regionConfig.getSelectedAttrConfig();
            if (selectedAttrConfig) {
                assignValuesEnd.push(javaUtils.createMaskedBitValueString(selectedAttrConfig.getIndex() + "U", "MPU_RLAR_AttrIndx_Pos", "MPU_RLAR_AttrIndx_Msk"));
            }
            assignValuesEnd.push(javaUtils.createMaskedBitValueString(self.getCValue(regionConfig.isEnabled()), "MPU_RLAR_EN_Pos", "MPU_RLAR_EN_Msk"));
            assignString = utils.createAssign(self.getMpuPeripheralName(), self.getRegisterName("RLAR"), utils.createAssignValue(assignValuesEnd));
            utils.addAll(generatedLines, utils.splitLines(assignString));
        }
        return generatedLines;
    };

    /**
     * Get register name with proper suffix based on whether MPU is Secure or Non-Secure
     * @param {String} registerName base register name
     * @returns {String} registerName or registerName_NS when this MPU is used to generate code for Non-Secure MPU
     */
    self.getRegisterName = function (registerName) {
        return self.suffix ? registerName + "_" + self.suffix : registerName;
    };

    /**
     * Get MPU peripheral name based on whether MPU is Secure or Non-Secure
     * @returns {String} MPU for Secure MPU or MPU_NS for Non-Secure MPU
     */
    self.getMpuPeripheralName = function () {
        return self.peripheral;
    };

    // generate code for MPU regions
    self.generateRegions = function (funcGrpPrefix) {
        var result = [];
        self.regions.forEach(function (regionConfig) {
            utils.addAll(result, self.generateRegion(regionConfig, funcGrpPrefix));
            utils.addEmptyLines(result, 1);
        });
        return result;
    };

    // get value of enable flag based on its string value
    self.getCValue = function (enabledFlag) {
        return Number(enabledFlag) + "U";
    };
};

/**
 * Class for generating code for security peripheral (e.g. AHB, TRDC)
 * @returns {SecurityPeripheralCode}
 */
SecurityPeripheralCode = function() {
    var self = this;

    var securityPeripheralType = sharedResources.profile.getSecurityType();

    self.getSecPeriphSpecificComment = function () {
        if (securityPeripheralType === "AHB") {
            return javaUtils.generateHeader("AHB Security Level Configurations", true, "-")
                + "/* Configuration of AHB Secure Controller\n"
                + "   Possible values for every memory sector or peripheral rule:\n"
                + "    0    Non-secure, User access allowed.\n"
                + "    1    Non-secure, Privileged access allowed.\n"
                + "    2    Secure, User access allowed.\n"
                + "    3    Secure, Privileged access allowed.\n*/\n";
        } else if (securityPeripheralType === "TRDC") {
            // FIXME TEE team v99 generate proper comment
            return javaUtils.generateHeader("TRDC Security Level Configurations", true, "-");
        }
    };

    self.pinsComment = "Pins: Reading GPIO state\n";
    self.pinsDescription = "Possible values for every pin:\n"
                        + " 0b0    Deny\n"
                        + " 0b1    Allow\n";

    self.interruptsMaskingComment = "Interrupts: Interrupt handling by Core1\n";
    self.interruptsMaskingDescription = "Possible values for every interrupt:\n"
                            + " 0b0    Deny\n"
                            + " 0b1    Allow\n";

    self.interruptsSecurityComment = "Interrupts: Interrupt security configuration\n";
    self.interruptsSecurityDescription = "Possible values for every interrupt:\n"
                            + " 0b0    Secure\n"
                            + " 0b1    Non-secure\n";

    self.generate = function (funcGrp) {
        var resultLines = [];
        var checkersConfig = funcGrp.getCheckersConfig();
        var initStrategy = (sharedResources.generateCodeSecurityHardening) ? "to_after_reset" : "to_1s";
        //
        resultLines.push(self.comment);
        utils.addAll(resultLines, self.getSecPeriphSpecificComment());

        mpcRegistersMap = javaUtils.createRegistersMap(checkersConfig.getCheckerConfig("MPC").getBitFieldsInfo(), sharedResources.generateModifiedRegistersOnly);
        mpcCode = javaUtils.writeRegistersMap("Security level configuration of MPC checker", false, mpcRegistersMap, true, null, sharedResources.generateCodeSecurityHardening);
        resultLines.push(mpcCode);

        ppcRegistersMap = javaUtils.createRegistersMap(checkersConfig.getCheckerConfig("PPC").getBitFieldsInfo(), sharedResources.generateModifiedRegistersOnly);
        ppcCode = javaUtils.writeRegistersMap("Security level configuration of PPC checker", false, ppcRegistersMap, initStrategy, null, sharedResources.generateCodeSecurityHardening);
        resultLines.push(ppcCode);

        mastersRegistersMap = javaUtils.createRegistersMap(funcGrp.getMastersConfig().getBitFieldsInfo(), sharedResources.generateModifiedRegistersOnly);
        mastersCode = javaUtils.writeRegistersMap("Security level configuration of masters", false, mastersRegistersMap, true, null, sharedResources.generateCodeSecurityHardening);
        resultLines.push(mastersCode);

        pinsRegistersMap = javaUtils.createRegistersMap(funcGrp.getPinsConfig().getBitFieldsInfo(), sharedResources.generateModifiedRegistersOnly);
        pinsCode = javaUtils.writeRegistersMap(self.pinsComment, true, pinsRegistersMap, true, self.pinsDescription, sharedResources.generateCodeSecurityHardening);
        resultLines.push(pinsCode);

        interruptsMaskingRegistersMap = javaUtils.createRegistersMap(funcGrp.getInterruptsConfig().getInterruptsMaskingConfig().getBitFieldsInfo(), sharedResources.generateModifiedRegistersOnly);
        interruptsMaskingCode = javaUtils.writeRegistersMap(self.interruptsMaskingComment, true, interruptsMaskingRegistersMap, true, self.interruptsMaskingDescription, sharedResources.generateCodeSecurityHardening);
        resultLines.push(interruptsMaskingCode);

        interruptsSecurityRegistersMap = javaUtils.createRegistersMap(funcGrp.getInterruptsConfig().getInterruptsSecurityConfig().getBitFieldsInfo(), sharedResources.generateModifiedRegistersOnly);
        interruptsSecurityCode = javaUtils.writeRegistersMap(self.interruptsSecurityComment, true, interruptsSecurityRegistersMap, true, self.interruptsSecurityDescription, sharedResources.generateCodeSecurityHardening);
        resultLines.push(interruptsSecurityCode);

        // Global options bit-field corrections
        var globalBitFieldsInfos = funcGrp.getGlobalOptionsConfig().getBitFieldsInfo();
        var globalOptionsCode = self.updateHardcodedBitField(globalBitFieldsInfos, "AIRCR", "VECTKEY", "0x05FA");
        resultLines.push(globalOptionsCode);

        // apply indentation to all lines and add empty line between parts of code
        return resultLines.filter(utils.notEmpty)
                .map(utils.trimNewlines)
                .map(function (part) {
                    return utils.splitLines(part).map(function (line) {
                        return utils.indent(2) + line;
                    }).join("\n");
        }).join("\n\n");
    };
    
    /**
     * Add a hardcoded bit field value of specified register. Some bitFieldInfo of the register must be already among bitFieldInfos.
     * @param bitFieldInfos collection of bitFieldInfos into which the hardcoded bit field value will be added to
     * @param register that contains given bit field to work with
     * @param bitFieldAlias the name of given bit field to work with        
     * @param hardcodedValue value of given bit field to work with
     */
    self.addHardcodedBitField = function(bitFieldInfos, register, bitFieldAlias, hardcodedValue) {
        for (var i = 0; i < bitFieldInfos.length; i++) {
            // add bit field info only when the register is already used
            if (bitFieldInfos[i].getRegisterAPI().getName() === register) {
                var registers = sharedResources.profile.getRegistersDB();
                if (registers == null) {
                    scriptApi.reportError("Failed to create the bit field " + bitFieldAlias + " of the register " + register);
                    return;
                }
                var newBitField = javaUtils.createBitFieldInfo(registers, bitFieldInfos[i].getPeripheral(), register, bitFieldAlias, hardcodedValue);
                if (newBitField) {
                    bitFieldInfos.add(newBitField);
                    break;
                }
            }
        }
    };

    /**
     * Update a harddcoded bit field value of specified register. Some bitFieldInfo of the register must be already among bitFieldInfos.
     * Remove all bit field values for given specific register present in the bitFieldInfos collection (List<BitFieldInfo>), in case of the security 
     * hardening is enabled for the generated code (i.e. removed register initialization is performed within "read-modify-write-mask" macro instead).
     * @param bitFieldInfos collection of bitFieldInfos into which the hardcoded bit field value will be updated
     * @param register that contains given bit field to work with
     * @param bitFieldAlias the name of given bit field to work with
     * @param hardcodedValue value of given bit field to work with
     */
     self.updateHardcodedBitField = function(bitFieldInfos, register, bitFieldAlias, hardcodedValue) {
        var registers = sharedResources.profile.getRegistersDB();
        var resultLines = [];
        if (registers == null) {
            scriptApi.reportError("Failed to load registers DB for update of the bit field " + bitFieldAlias + " of the register " + register);
            return;
        }
        // add hardcoded value first
        self.addHardcodedBitField(bitFieldInfos, register, bitFieldAlias, hardcodedValue);
        // remove all values for given specific register in case of security hardening macros enabled in code output and update resultLines code
        if (sharedResources.generateCodeSecurityHardening) {
            var bitFieldInfosTemp = [];
            var bitFieldInfosBackup = [];
            //
            for (var i = 0; i < bitFieldInfos.length; i++) {
                var newBitField = bitFieldInfos[i];
                if (newBitField.getRegisterAPI().getName() != register) {
                    bitFieldInfosBackup.push(newBitField); // store original register bit field value in the extra list
                }
                else {
                    bitFieldInfosTemp.push(newBitField); // store given updated register hardcoded bit field value in extra list
                }
            }
            // create resultLines code for updated bitFieldInfosTemp and bitFieldInfosBackup
            bitFieldsRegistersMap = javaUtils.createRegistersMap(bitFieldInfosTemp, sharedResources.generateModifiedRegistersOnly);
            resultLines = javaUtils.writeRegistersMap("Global Options", false, bitFieldsRegistersMap, false, null, sharedResources.generateCodeSecurityHardening);
            resultLines = resultLines.replace("TEE_READ_MODIFY_WRITE_REGISTER", "TEE_READ_MODIFY_WRITE_MASK_REGISTER").replace(");", ", 0x0000FFFF);");
    
            bitFieldsRegistersMap = javaUtils.createRegistersMap(bitFieldInfosBackup, sharedResources.generateModifiedRegistersOnly);
            resultLines += javaUtils.writeRegistersMap(null, false, bitFieldsRegistersMap, false, null, sharedResources.generateCodeSecurityHardening);
        } else {
            // create resultLines code for original bitFieldInfos
            bitFieldsRegistersMap = javaUtils.createRegistersMap(bitFieldInfos, sharedResources.generateModifiedRegistersOnly);
            resultLines += javaUtils.writeRegistersMap("Global Options", false, bitFieldsRegistersMap, false, null, sharedResources.generateCodeSecurityHardening);
        }
        return resultLines;
    };

    self.comment = "/* " + securityPeripheralType + " configuration */";
};

/**
 * Class used to hold information about MPUs from Java configuration and shared code for MpuCode, MpuDefines and for both S and NS MPU
 * @returns {Mpu}
 */
Mpu = function() {
    var self = this;

    self.getSecureMpuConfig = function (funcGrp) {
        return funcGrp.getMpuConfigs().get(0).getSecureMpuConfig();
    };

    self.getNonSecureMpuConfig = function (funcGrp) {
        //
        return funcGrp.getMpuConfigs().get(0).getNonSecureMpuConfig();
    };

    /**
     * Get list of MemRegionConfigs for which code should be generated.
     * @param withoutProblematicRegions whether to filter out error configurations
     * @return list of MemRegionConfigs for which code should be generated
     */
    self.getMemRegionConfigsToGenerateCodeFor = function (mpuConfig, withoutProblematicRegions) {
        var result = [];
        if (mpuConfig != null) {
            var memRegionConfigs = mpuConfig.getMpuMemConfig().getRegionConfigs();
            var generateSourcesForDisabledRegions = mpuConfig.isGenerateSourcesForDisabledRegionsOn();
            for (var iterator = memRegionConfigs.iterator(); iterator.hasNext(); ) {
                var memRegionConfig = iterator.next();
                result.push(memRegionConfig);
            }
            if (withoutProblematicRegions) {
                result = result.filter(utils.filterProblematicRegions);
            }
            if (!generateSourcesForDisabledRegions) {
                result = result.filter(utils.filterDisabledRegions);
            }
        }
        return result;
    };
};

SecureMpuDefines = function() {
    var self = this;

    /**
     * Generate Secure MPU define symbols
     * @param addrBitWidth address bit width
     */
    self.generateDefines = function (addrBitWidth) {
        var resultLines = [];
        allFuncGroups = sharedResources.profile.getFunctionalGroups();
        allFuncGroups.forEach(function (funcGrp) {
            var funcGrpPrefix = funcGrp.getIdPrefix();
            var regionConfigs = mpu.getMemRegionConfigsToGenerateCodeFor(mpu.getSecureMpuConfig(funcGrp), true);
            utils.addAll(resultLines, utils.generateDefines(regionConfigs, funcGrpPrefix + MPU_S_PREFIX, addrBitWidth, utils.makeComment("Secure MPU region boundaries")));
            if (resultLines.length > 0) utils.addEmptyLines(resultLines, 1);
        });
        return resultLines;
    };
};

NonSecureMpuDefines = function() {
    var self = this;

    /**
     * Generate Non-Secure MPU define symbols
     * @param addrBitWidth address bit width
     */
    self.generateDefines = function (addrBitWidth) {
        var resultLines = [];
        allFuncGroups = sharedResources.profile.getFunctionalGroups();
        allFuncGroups.forEach(function (funcGrp) {
            var funcGrpPrefix = funcGrp.getIdPrefix();
            var regionConfigs = mpu.getMemRegionConfigsToGenerateCodeFor(mpu.getNonSecureMpuConfig(funcGrp), true);
            utils.addAll(resultLines, utils.generateDefines(regionConfigs, funcGrpPrefix + MPU_NS_PREFIX, addrBitWidth, utils.makeComment("Non-Secure MPU region boundaries")));
            if (resultLines.length > 0) utils.addEmptyLines(resultLines, 1);
        });
        return resultLines;
    };
};

SecureMpuCode = function(mpuConfig) {
    var self = this;
    self.mpuConfig = mpuConfig;

    self.mpuCode = new MpuCode(MPU_S_PREFIX, self.mpuConfig, "Secure MPU");

    self.generate = function (funcGrpPrefix) {
        return self.mpuCode.generate(funcGrpPrefix);
    };
};

NonSecureMpuCode = function(mpuConfig) {
    var self = this;
    self.mpuConfig = mpuConfig;

    self.mpuCode = new MpuCode(MPU_NS_PREFIX, self.mpuConfig, "Non-Secure MPU");

    self.generate = function (funcGrpPrefix) {
        return self.mpuCode.generate(funcGrpPrefix);
    };
};