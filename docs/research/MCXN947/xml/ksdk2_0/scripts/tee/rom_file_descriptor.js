/*
 * Copyright 2020-2026 NXP
 * To be used with MCUXpresso Config Tools under its Software License Agreement.
 */

var MPU_NS_SUFFIX = "_NS";
var MPU_S_SUFFIX  = "";


var javaUtils = scriptApi.getUtils();

var registers = scriptApi.getProfile().getRegistersInfo();
var registersDB = scriptApi.getProfile().getRegistersDB();


/**
 * Gets bitwise or. Native bitwise operation in Javascript works only with 31-bit numbers because of implicit conversion to signed 32-bit integer.
 * a - value 1
 * b - value 2
 * returns result of bitwise OR
 */
function getBitwiseOr(a, b) {
    if (a < 0 || b < 0) {
        logMessage("One of operands (" + a + ", " + b + ") is negative number.", true);
    }
    var result = 0;
    var mask = 0xFFFF;
    var divisor = mask + 1;
    var i = 0;
    while ((a != 0) || (b != 0)) {
        var r = ((a & mask) | (b & mask));
        for (var j = 0; j < i; j++) {
            r *= divisor;
        }
        result += r;
        a = Math.floor(a / divisor);
        b = Math.floor(b / divisor);
        i++;
    }
    return result;
}

/**
 * Gets value shifted left by a number. Native bitwise operation in Javascript works only with 31-bit numbers because of implicit conversion to signed 32-bit integer.
 * a - value which is shifted left by n 
 * n - number
 * returns shifted value
 */
function getBitwiseShiftLeft(a, n) {
    if (a < 0 || n < 0) {
        logMessage("One of operands (" + a + ", " + n + ") is negative number.", true);
    }
    return (a * Math.pow(2, n));
}

/**
 * Gets reset value of register from database
 * dbReg - reference to database register
 * returns reset value
 */
function getResetValue(dbReg) {
    var value = java.math.BigInteger.valueOf(0);
    var dbBitFields = dbReg.getBitFields();
    for (var fi = 0; fi < dbBitFields.length; fi++) {
        var dbBitField = dbBitFields[fi];
        value = getBitwiseOr(value, getBitwiseShiftLeft(dbBitField.getResetValue().doubleValue(), dbBitField.getOffset()));
    }
    return (value);
} 

/**
 * Translates value of Java configuration enum to C value used in registers
 */
AccessPermissionsAndPrivileges = function() {
    var self = this;

    self.getCValue = function (accessPermissions, accessPrivileges) {
        var value;
        switch (accessPermissions.getId()) {
            case "RW":
                value = 0 << 1;
                break;
            case "RO":
                value = 1 << 1;
                break;
            default:
                value = 0 << 1;
                break;
        }
        switch (accessPrivileges.getId()) {
            case "Priv only":
                value |= 0;
                break;
            case "any":
                value |= 1;
                break;
            default:
                value |= 0;
                break;
        }
        return value;
    };
};

/**
 * Translates value of Java configuration flag to C value used in registers
 * Note: value in register uses opposite logic to value in GUI.
 */
Executable = function() {
    var self = this;

    self.getCValue = function (executable) {
        var value = executable ? 0 : 1;
        return value;
    };
};

DeviceAttributes = function () {
    var self = this;

    self.getValue = function (deviceAttributes) {
        var value;
        switch (deviceAttributes.getId()) {
            case "nGnRnE":
                value = 0;
                break;
            case "nGnRE":
                value = 1;
                break;
            case "nGRE":
                value = 2;
                break;
            case "GRE":
                value = 3;
                break;

            default:
                value = 0;
                break;
        }
        return value;
    };
};

/**
 * Translates value of Java configuration enum to C value used in registers
 */
Shareability = function() {
    var self = this;

    self.getCValue = function (shareability) {
        var value;
        switch (shareability.getId()) {
            case "non_shareable":
                value = 0;
                break;
            case "outer":
                value = 2;
                break;
            case "inner":
                value = 3;
                break;
            default:
                value = 0;
                break;
        }
        return value;
    };
};

/**
 * Offsets of bits in MPU Control Register
 */
CtrlAttrReg = function () {
    var self = this;

    // privileged access to the default map enable bit at position 2
    self.PRIVDEFENA = 2;
    // hard-fault and nmi operation enable bit at position 1
    self.HFNMIENA = 1;
    // mpu enable bit at position 0
    self.ENABLE = 0;
}

/**
 * Offsets of bits in MPU Memory Attribute Indirection Registers
 */
MairAttrReg = function () {
    var self = this;
    
    // inner block is at bits 0-3
    self.INNER_BLOCK_POS = 0;
    // outer block is at bits 4-7
    self.OUTER_BLOCK_POS = 4;
    
    // write allocation bit in a block is at position 0
    self.WRITE_ALLOCATION_POS = 0;
    // read allocation bit in a block is at position 1
    self.READ_ALLOCATION_POS = 1;
    // write-back bit in a block is at position 2
    self.WRITE_BACK_POS = 2;
    // transient bit in a block is at position 3
    self.TRANSIENT_POS = 3;
    
    // when device is used, its value is at position 2-3
    self.DEVICE_POS = 2;
    
    // number of bits that defines one memory attribute indirection 
    self.SIZE = 8;
};

/**
 * Offsets of bits in MPU Memory Region Base Address Registers
 */
RbarAttrReg = function () {
    var self = this;

    // base bit in a block is at position 5
    self.BASE_POS = 5;
    // shareability bit in a block is at position 3
    self.SH_POS = 3;
    // access permissions bit in a block is at position 1
    self.AP_POS = 1;
    // execute never bit in a block is at position 0
    self.XN_POS = 0;
}

/**
 * Offsets of bits in MPU Memory Region Limit Address Registers
 */
RlarAttrReg = function () {
    var self = this;

    // limit address bits in a block is at position 5
    self.LIMIT_POS = 5;
    // attribute index bits in a block is at position 3
    self.AI_POS = 1;
    // enable region bit in a block is at position 0
    self.EN_POS = 0;
}

/**
 * Offsets of bits in SCB::SHCSR Register
 */
ScbShcsrAttrReg = function () {
    var self = this;
    
    // base bit SECUREFAULTENA in a block is at position 19
    self.SECUREFAULTENA_POS = 19;

    self.getCValue = function (bfval, pos) {
        return ((bfval & (1 << pos)) >> pos);
    }
}

/**
 * Miscellaneous CM33 control settings configuration
 */
MiscCtrlReg = function () {
    var self = this;
    self.value = 0;
    //
    self.setCValue = function (value, bitFieldName) {
        var cval = 0;
        switch (bitFieldName) {
            // SCB::SHCSCR
            case "SCB::SHCSR::SECUREFAULTENA":
                cval = scbShcsrAttrReg.getCValue(value, scbShcsrAttrReg.SECUREFAULTENA_POS) << 0;
                break;
            default:
                break;
        }
        self.value += cval;
    };
    //
    self.getCValue = function () {
        return self.value;
    };
} 

// Class for processing SAU configuration
SauReg = function () {
    var self = this;

    self.getSauConfig = function (sauConfig) {
        var sauRegions = sauConfig.getMemConfig().getIndexableRegionConfigs();
        for each (var sauRegion in sauRegions) {
            var error = scriptApi.getProfile().hasError(sauRegion);
            if (error) {
                scriptApi.reportWarning("SAU region number " + sauRegion.getIndex() + " contains an error");
            }
            if (sauRegion.isEnabled()) {
                // SAU_RNR
                options['SAU_RNR_' + sauRegion.getIndex()] = error ? 0 : presetUtils.formatHexValue(sauRegion.getIndex(), 32);
            }
            // SAU_RBAR
            var regionStart5BitAligned = sauRegion.getStartAddr().shiftRight(5).shiftLeft(5);
            options['SAU_RBAR_' + sauRegion.getIndex()] = error ? 0 : presetUtils.formatHexValue(regionStart5BitAligned, 32);
            // SAU_RLAR
            var regionEnd5BitAligned = sauRegion.getEndAddr().shiftRight(5).shiftLeft(5);
            var enableValue = sauRegion.isEnabled() ? 1 : 0;
            var nscValue = ((sauRegion.getSecurityLevelId() == 'nsc') ? 1 : 0) << 1;
            var rlar = regionEnd5BitAligned.longValue() + enableValue + nscValue;
            options['SAU_RLAR_' + sauRegion.getIndex()] = error ? 0 : presetUtils.formatHexValue(rlar , 32);
        }
        // SAU_CTRL
        var enabled = sauConfig.isEnabled();
        var allNonSecure = sauConfig.isAllNonSecure();
        options['SAU_CTRL'] = presetUtils.formatHexValue(Number(enabled) + (Number(allNonSecure) << 1), 32);
    }
}

// Class for processing MPU configuration 
MpuReg = function () {
    var self = this;

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
    }    

    /**
     * Get list of MemRegionConfigs for which code should be generated.
     * @param mpuConfig
     * @param withoutProblematicRegions whether to filter out error configurations
     * @return list of MemRegionConfigs for which code should be generated
     */
    self.getMemRegionConfigsToGenerateCodeFor = function (mpuConfig, withoutProblematicRegions) {
        var result = [];
        if (mpuConfig !== null) {
            var memRegionConfigs = mpuConfig.getMpuMemConfig().getRegionConfigs();
            var generateSourcesForDisabledRegions = mpuConfig.isGenerateSourcesForDisabledRegionsOn();
            for (var iterator = memRegionConfigs.iterator(); iterator.hasNext(); ) {
                var memRegionConfig = iterator.next();
                result.push(memRegionConfig);
            }
            if (withoutProblematicRegions) {
                result = result.filter(self.filterProblematicRegions);
            }
            if (!generateSourcesForDisabledRegions) {
                result = result.filter(self.filterDisabledRegions);
            }
        }
        return result;
    };

    /**
     * Get MPU region config address for RBAR, RLAR register init
     * @param regionConfig 
     * @returns null
     */
    self.getRegionConfig = function (regionConfig) {
        var regionIdx = regionConfig.getIndex();
        var regValue = 0;
        //if (!self.filterProblematicRegions(regionConfig)) {
        //    scriptApi.logWarning("Configuration of MPU region number " + regionIdx + ", contains an error");
        //    options['MPU_RBAR' + regionIdx.toString() + self.mpuRegSuffix] = presetUtils.formatHexValue(regValue, 32);
        //    options['MPU_RLAR' + regionIdx.toString() + self.mpuRegSuffix] = presetUtils.formatHexValue(regValue, 32);
        //} else {
            if (regionConfig.isEnabled()) {
                options['MPU_RNR_' + regionIdx.toString() + self.mpuRegSuffix] = presetUtils.formatHexValue(regionIdx, 32);
            }
            var startAddr = regionConfig.getStartAddr().shiftRight(5).shiftLeft(5);
            var endAddr = regionConfig.getEndAddr().shiftRight(5).shiftLeft(5);
            regValue += Number(startAddr);
            regValue += shareability.getCValue(regionConfig.getShareability()) << rbarAttrReg.SH_POS;
            regValue += accessPermissionsAndPrivileges.getCValue(regionConfig.getAccessPermissions(), regionConfig.getAccessPrivileges()) << rbarAttrReg.AP_POS;
            regValue += executable.getCValue(regionConfig.isExecutable()) << rbarAttrReg.XN_POS;
            options['MPU_RBAR_' + regionIdx.toString() + self.mpuRegSuffix] = presetUtils.formatHexValue(regValue, 32);
            var selectedAttrConfig = regionConfig.getSelectedAttrConfig();
            regValue = Number(endAddr) + (selectedAttrConfig ? selectedAttrConfig.getIndex() << rlarAttrReg.AI_POS : 0) + (Number(regionConfig.isEnabled()) << rlarAttrReg.EN_POS);
            options['MPU_RLAR_' + regionIdx.toString() + self.mpuRegSuffix] = presetUtils.formatHexValue(regValue, 32);
        //}
    }

    // filter problematic configs
    self.filterProblematicRegions = function (memRegionConfig) {
        return !presetUtils.profile.hasError(memRegionConfig);
    }
        
    // filter disabled configs
    self.filterDisabledRegions = function (memRegionConfig) {
        if (!memRegionConfig.isEnabled()) {
            var regionIdx = memRegionConfig.getIndex();
            options['MPU_RBAR_' + regionIdx.toString() + self.mpuRegSuffix] = presetUtils.formatHexValue(0, 32);
            options['MPU_RLAR_' + regionIdx.toString() + self.mpuRegSuffix] = presetUtils.formatHexValue(0, 32);
        }
        return memRegionConfig.isEnabled();
    }

    /**
     * Get MPU configuration registers
     * @param mpuConfig
     * @param mpuRegSuffix
     * @returns null
     */
    self.getMpuConfig = function (mpuConfig, mpuRegSuffix) {
        self.mpuRegSuffix = mpuRegSuffix;
        // MAIR
        var mpuConfigAttributes = mpuConfig.getMemoryAttributesConfigs();
        var ATTRIBUTES_IN_REGISTER = 4;
        var numberOfRegisters = mpuConfigAttributes.length / ATTRIBUTES_IN_REGISTER;
        var regValue;
        var attrValue;
        var config;
        for (var regIndex = 0; regIndex < numberOfRegisters; regIndex++) {
            regValue = 0;
            if (mpuConfig.isEnabled()) {
                for (var attrIndex = 0; attrIndex < ATTRIBUTES_IN_REGISTER; attrIndex++) {
                    attrValue = 0;
                    config = mpuConfigAttributes[regIndex * ATTRIBUTES_IN_REGISTER + attrIndex];
                    var outer = config.getOuterAttributes();
                    var inner = config.getInnerAttributes();
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
            }
            // required to force JavaScript to use an unsigned value
            var unsignedValue = regValue >>> 0;
            options['MPU_MAIR' + regIndex.toString() + mpuRegSuffix] = presetUtils.formatHexValue(unsignedValue, 32);
        }
        // RBAR, RLAR
        var mpuConfigRegions = self.getMemRegionConfigsToGenerateCodeFor(mpuConfig, false);
        mpuConfigRegions.forEach(function (regionConfig) {
            self.getRegionConfig(regionConfig);
        });
        // CTRL
        regValue = (Number(mpuConfig.isDefaultPrivilegedAccessEnabled()) << ctrlAttrReg.PRIVDEFENA) + 
                   (Number(mpuConfig.isHandlerOperationEnabled()) << ctrlAttrReg.HFNMIENA) + 
                   (Number(mpuConfig.isEnabled()) << ctrlAttrReg.ENABLE);
        options['MPU_CTRL' + mpuRegSuffix] = presetUtils.formatHexValue(regValue, 32);
    }
}

// Class for processing GPIO configuration
GpioReg = function () {
	var self = this;

	/**
	 * Get GPIO configuration registers
	 * @returns null
	 */
	self.getGpioConfig = function () {
		allFuncGroups = presetUtils.profile.getFunctionalGroups();
		allFuncGroups.forEach(function (funcGrp) {
			presetUtils.collectRegistersInfo(funcGrp.getGlobalOptionsConfig().getBitFieldsInfo());  // global options, incl. GPIOx_LOCK regs
			presetUtils.collectRegistersInfo(funcGrp.getPinsConfig().getBitFieldsInfo());  // IO pins config for PCNS/PCNP, ICNS/ICNP regs
		});
	}
}

// Class for processing common utilities
RomPresetUtils = function () {
    var self = this;
    // global profile object
    self.profile = scriptApi.getProfile();

    self.formatHexValue = function (value, width) {
        return scriptApi.getUtils().formatHexValue(value, width);
    }

    // function creates array of strings split by new-line characters
    self.splitLines = function(str) {
        return str.split(/[\r\n]+/);
    };

    // get cores list from current MCU
    self.coresJsons = JSON.parse(self.profile.getMcuInfo().getCoresList());

    // coresList is an array containing list of coreIds of each MCU cores, e.g. ['cm33_core0', 'cm33_core1']
    self.coresIds = Object.keys(self.coresJsons);

	// generate modified registers only
	self.generateModifiedRegistersOnly = self.profile.isGenerateCodeModifiedRegistersOnly();

	// collect registers info from given regs object (e.g. global options config or pins config per funcGrp)
	self.collectRegistersInfo = function (regsObject) {
		regsObjectMap = javaUtils.createRegistersMap(regsObject, self.generateModifiedRegistersOnly);
		regsWriteMap = javaUtils.writeRegistersMap("", true, regsObjectMap, true, "");
		collection = self.splitLines(regsWriteMap);
		collection.forEach(function (line) {
			if (line != "") {
				regItem = line.replace("->","_").replace(';','').split(" = ");
				options[regItem[0]] = regItem[1];
			}
		});
	}

    self.compareSauRegisters = function(a, b) {
        var aMatch = a.match(/SAU_(RNR|RBAR|RLAR)_(\d+)/);
        var bMatch = b.match(/SAU_(RNR|RBAR|RLAR)_(\d+)/);

        if (!aMatch || !bMatch) return a.localeCompare(b);

        var aNum = parseInt(aMatch[2]);
        var bNum = parseInt(bMatch[2]);

        // Sort by region number first
        if (aNum !== bNum) return aNum - bNum;

        // Then by register type (RNR -> RBAR -> RLAR)
        var regOrder = {"RNR": 0, "RBAR": 1, "RLAR": 2};
        return regOrder[aMatch[1]] - regOrder[bMatch[1]];
    };

    self.compareMpuRegisters = function(a, b) {
        var aMatch = a.match(/MPU_(MAIR|RNR|RBAR|RLAR|CTRL)(_\d+)?(_NS)?/);
        var bMatch = b.match(/MPU_(MAIR|RNR|RBAR|RLAR|CTRL)(_\d+)?(_NS)?/);
    
        if (!aMatch || !bMatch) return a.localeCompare(b);
    
        var aType = aMatch[1];
        var bType = bMatch[1];
        var aNum = aMatch[2] ? parseInt(aMatch[2].substring(1)) : -1;
        var bNum = bMatch[2] ? parseInt(bMatch[2].substring(1)) : -1;
        var aSuffix = aMatch[3] || "";
        var bSuffix = bMatch[3] || "";
    
        // Sort by suffix (non-NS before NS)
        if (aSuffix !== bSuffix) return aSuffix.localeCompare(bSuffix);
    
        // Define register type order: MAIR -> RNR -> RBAR -> RLAR -> CTRL
        var regTypeOrder = {"MAIR": 0, "RNR": 1, "RBAR": 2, "RLAR": 3, "CTRL": 4};
    
        // If types differ and either is MAIR or CTRL (no region numbers), sort by type
        if (aType !== bType && (aType === "MAIR" || aType === "CTRL" || bType === "MAIR" || bType === "CTRL")) {
            return regTypeOrder[aType] - regTypeOrder[bType];
        }
    
        // For registers with region numbers (RNR, RBAR, RLAR) or same-type MAIR
        if (aNum !== bNum) return aNum - bNum;
    
        return regTypeOrder[aType] - regTypeOrder[bType];
    };

    self.collectSortedKeys = function (obj) {
        // Define custom sort order
        var sortOrder = ["SAU", "MPU", "MBC0", "GPIO", "NVIC", "ITRC", "AHBSC", "SCB", "SCnSCB"];
        var sortedOptionKeys = Object.keys(obj).sort(function(a, b) {
            var aPrefix = -1;
            var bPrefix = -1;

            // Find which prefix group each key belongs to
            for (var i = 0; i < sortOrder.length; i++) {
                if (a.indexOf(sortOrder[i]) === 0) aPrefix = i;
                if (b.indexOf(sortOrder[i]) === 0) bPrefix = i;
            }

            // Recognized prefixes come before unrecognized ones
            if (aPrefix === -1 && bPrefix !== -1) return 1;
            if (aPrefix !== -1 && bPrefix === -1) return -1;
            if (aPrefix === -1 && bPrefix === -1) return a.localeCompare(b);

            // Different prefix groups - sort by prefix order
            if (aPrefix !== bPrefix) return aPrefix - bPrefix;

            // Same prefix group - apply special handling
            var prefix = sortOrder[aPrefix];

            if (prefix === "SAU") {
                return self.compareSauRegisters(a, b);
            }

            if (prefix === "MPU") {
                return self.compareMpuRegisters(a, b);
            }

            // Same prefix, sort alphabetically
            return a.localeCompare(b);
        });

        return sortedOptionKeys;
    };
}

RomFileDescriptor = function() {
    var self = this;
    
    self.render = function() {

        // get all known registers modified by TEE tool UI configuration directly
        for each (var regId in registerIds) {
            //scriptApi.logWarning("registerId: '" + regId + "'"); // DEBUG ONLY
            var regValue = registers.getRegisterValue(regId);
            options[regId.replace('::', '_')] = presetUtils.formatHexValue(regValue, 32);
            //
            if (regId == "SCB::SHCSR") {
                miscCtrlReg.setCValue(regValue, "SCB::SHCSR::SECUREFAULTENA");
            }
        }

        // add MISC_TZM_SETTINGS values
        options['MISC_TZM_SETTINGS'] = presetUtils.formatHexValue(miscCtrlReg.getCValue(), 32);

        // add ITRC_OUTx values
        itrcTrigRegs = [
            "ITRC0_OUT0_SEL0",
            "ITRC0_OUT0_SEL1",
            "ITRC0_OUT1_SEL0",
            "ITRC0_OUT1_SEL1",
            "ITRC0_OUT2_SEL0",
            "ITRC0_OUT2_SEL1",
            "ITRC0_OUT3_SEL0",
            "ITRC0_OUT3_SEL1",
            "ITRC0_OUT4_SEL0",
            "ITRC0_OUT4_SEL1",
            "ITRC0_OUT5_SEL0",
            "ITRC0_OUT5_SEL1",
            "ITRC0_OUT6_SEL0",
            "ITRC0_OUT6_SEL1",
            "ITRC0_OUT0_SEL0_1",
            "ITRC0_OUT0_SEL1_1",
            "ITRC0_OUT1_SEL0_1",
            "ITRC0_OUT1_SEL1_1",
            "ITRC0_OUT2_SEL0_1",
            "ITRC0_OUT2_SEL1_1",
            "ITRC0_OUT3_SEL0_1",
            "ITRC0_OUT3_SEL1_1",
            "ITRC0_OUT4_SEL0_1",
            "ITRC0_OUT4_SEL1_1",
            "ITRC0_OUT5_SEL0_1",
            "ITRC0_OUT5_SEL1_1",
            "ITRC0_OUT6_SEL0_1",
            "ITRC0_OUT6_SEL1_1",
            "ITRC0_OUT0_SEL0_2",
            "ITRC0_OUT0_SEL1_2",
            "ITRC0_OUT1_SEL0_2",
            "ITRC0_OUT1_SEL1_2",
            "ITRC0_OUT2_SEL0_2",
            "ITRC0_OUT2_SEL1_2",
            "ITRC0_OUT3_SEL0_2",
            "ITRC0_OUT3_SEL1_2",
            "ITRC0_OUT4_SEL0_2",
            "ITRC0_OUT4_SEL1_2",
            "ITRC0_OUT5_SEL0_2",
            "ITRC0_OUT5_SEL1_2",
            "ITRC0_OUT6_SEL0_2",
            "ITRC0_OUT6_SEL1_2"
        ];

        // use default registersDB reset values when triggers disabled
        funcGrp = scriptApi.getProfile().getFunctionalGroups()[0];
        if (funcGrp.getGlobalOptionsConfig().getOptionValue("_trigger_sources_") == false) {
            itrcTrigRegs.forEach(function (register) {
                regResetValue = getResetValue(registersDB.getRegisterByFullName(register));
                options[register] = presetUtils.formatHexValue(regResetValue, 32);
            });

        } 

        // add SAU regions info
        var sau = scriptApi.getProfile().getSauConfig();
        if (sau != null) {
            sauCode.getSauConfig(sau);
        }

        // add MPU regions info
        var mpu_s = scriptApi.getProfile().getSecureMpuConfig();
        if (mpu_s != null) {
            mpuCode.getMpuConfig(mpu_s, MPU_S_SUFFIX);
        }
        var mpu_ns = scriptApi.getProfile().getNonSecureMpuConfig();
        if (mpu_ns != null) { 
            mpuCode.getMpuConfig(mpu_ns, MPU_NS_SUFFIX);
        }

        // add IO pins and IRQ/DMA request security control per GPIO ports info
        gpioCode.getGpioConfig();

        // and write ROM preset TrustZone configuration file for enabled core's
        for (var coreIndex = 0; coreIndex < presetUtils.coresIds.length; coreIndex++) {
            var coreId = presetUtils.coresIds[coreIndex];
            if (presetUtils.coresJsons[coreId].enabled) {
                var code_type = scriptApi.getProfile().getToolOptionsConfig().getOptionValueAsString("_output_type_");
                //
                if (code_type == "rom_preset") {
                    scriptApi.requireScript('templatingengine.js');
                    var template = scriptApi.readFile(scriptApi.getFilePath('tee_rom_template'));
                    var finalText = new TemplatingEngine().generate(template, options);
                    scriptApi.createFile("/trustzone/trust_zone.json", coreId).write(finalText);
                    break;
                } else if (code_type == "yaml") {

                    // filter registers and add address
                    configuration = {}
                    var sortedOptionKeys = presetUtils.collectSortedKeys(options);

                    for (var i = 0; i < sortedOptionKeys.length; i++) {
                        var registerId = sortedOptionKeys[i];
                        var optionId = registerId;
                        if (registerId.indexOf("SAU") !== -1) {
                            if (registerId != "SAU_CTRL") {
                                registerId = registerId.slice(0, registerId.lastIndexOf('_'));
                            }
                        }
                        if (registerId.indexOf("MPU") !== -1) {                            
                            if (registerId != "MPU_CTRL" && registerId.indexOf("MPU_MAIR") !== 0) {
                                registerId = registerId.slice(0, registerId.lastIndexOf('_'));
                            }
                        }
                        var peripheral = registersDB.getPeripheralByFullName(registerId);
                        var register = registersDB.getRegisterByFullName(registerId);
                        if (peripheral != null && register != null) {     
                            var resetVal = presetUtils.formatHexValue(register.getResetValue(), 32);
                            var hexVal = options[optionId]
                            if (hexVal != resetVal) {
                                var baseAddr = peripheral.getBaseAddress("core_0", null);
                                var regOffset = register.getOffset();
                                var registerAddress = (baseAddr + regOffset).toString(16).toUpperCase();

                                if (hexVal.endsWith("U")) {
                                    hexVal = hexVal.slice(0, -1);
                                }
                                if (register.getName() == "AIRCR") {
                                    var writeVal = hexVal.replace(/0xFA05/i, "0x05FA");
                                    configuration["AIRCR_WRITE"] = {
                                        registerId: registerId, 
                                        registerAddress: registerAddress, 
                                        hexVal: writeVal, 
                                        mask: "0xFFFF0000",
                                        skipReadback: true
                                    };
                                    var readBackVal = hexVal.replace(/0x05FA/i, "0xFA05");
                                    configuration["AIRCR_READ"] = {
                                        registerId: registerId, 
                                        registerAddress: registerAddress, 
                                        hexVal: readBackVal, 
                                        mask: "0xFFFF0000",
                                        skipReadback: false
                                    };
                                } else {
                                    configuration[optionId] = {
                                        registerId: registerId, 
                                        registerAddress: registerAddress, 
                                        hexVal: hexVal, 
                                        mask: "0x0",
                                        skipReadback: false
                                    };
                                }
                            } else {
                                continue;
                            }
                        }
                    }
                    scriptApi.requireScript('yamlengine.js');
                    var template = scriptApi.readFile(scriptApi.getFilePath('tee_rom_template').replace('.json', '.yaml'));
                    var finalText = new YamlEngine().generate(template, configuration);
                    scriptApi.createFile("/trustzone/trust_zone.yaml", coreId).write(finalText);
                    break;
                }
            }
        }
    }
}

// global objects declaration
var accessPermissionsAndPrivileges = new AccessPermissionsAndPrivileges();
var shareability = new Shareability();
var executable = new Executable();
var deviceAttributes = new DeviceAttributes();
var mairAttrReg = new MairAttrReg();
var rbarAttrReg = new RbarAttrReg();
var rlarAttrReg = new RlarAttrReg();
var ctrlAttrReg = new CtrlAttrReg();
//
var scbShcsrAttrReg = new ScbShcsrAttrReg();
var miscCtrlReg = new MiscCtrlReg();

var sauCode = new SauReg();
var mpuCode = new MpuReg();
var gpioCode = new GpioReg();
var presetUtils = new RomPresetUtils();

var options = {};
var registerIds = registers.getRegisterIds();