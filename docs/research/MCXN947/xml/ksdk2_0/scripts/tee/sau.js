/*
 * Copyright 2020-2023 NXP
 * To be used with MCUXpresso Config Tools under its Software License Agreement.
 */

var SAU_PERIPHERAL_NAME = "SAU";
var SAU_PREFIX = "SAU";

Sau = function() {
    var self = this;

     /**
     * Get list of SauMemRegionConfigs for which code should be generated.
     * @param withoutProblematicRegions whether to filter out error configurations
     * @return list of SauMemRegionConfigs for which code should be generated
     */
    self.getSauMemRegionConfigsToGenerateCodeFor = function (funcGrp, withoutProblematicRegions) {
        var sauConfig = funcGrp.getSauConfig();
        if (sauConfig != null) {
            var sauMemRegionConfigs = sauConfig.getMemConfig().getRegionConfigs();
            var generateSourcesForDisabledRegions = sauConfig.isGenerateSourcesForDisabledRegionsOn();
            var result = [];
            for (var iterator = sauMemRegionConfigs.iterator(); iterator.hasNext(); ) {
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

SauDefines = function() {
    var self = this;
    self.profile = sharedResources.profile;

    /**
     * Generate SAU define symbols
     * @param addrBitWidth address bit width
     */
    self.generateDefines = function (addrBitWidth) {
        var resultLines = [];
        allFuncGroups = sharedResources.profile.getFunctionalGroups();
        allFuncGroups.forEach(function (funcGrp) {
            var funcGrpPrefix = funcGrp.getIdPrefix();
            var sauRegionConfigs = sau.getSauMemRegionConfigsToGenerateCodeFor(funcGrp, true);
            var defines = utils.generateDefines(sauRegionConfigs, funcGrpPrefix + SAU_PREFIX, addrBitWidth, "/* SAU region boundaries */");
            if (defines.length > 0) {
                utils.addAll(resultLines, defines);
                if (resultLines.length > 0) utils.addEmptyLines(resultLines, 1);    
            }
        });
        return resultLines;
    };
};

SauCode = function() {
    var self = this;

    // generate all code for SAU, including comment,  disabling sau, code for configuring regions, forcing memory writes and enabling sau
    self.generate = function (funcGrp) {
        var resultLines = [];
        resultLines.push(self.comment);
        utils.addEmptyLines(resultLines, 1);
        utils.addAll(resultLines, self.generateHead());
        utils.addEmptyLines(resultLines, 1);
        utils.addAll(resultLines, self.generateSauRegions(funcGrp));
        utils.addAll(resultLines, sharedResources.forceMemoryWrites);
        utils.addEmptyLines(resultLines, 1);
        utils.addAll(resultLines, self.generateTail(funcGrp));
        utils.addEmptyLines(resultLines, 1);
        return resultLines;
    };

    self.comment = utils.makeComment("SAU configuration");

    // generate code to disable SAU at the beginning of configuration code
    self.generateHead = function () {
        var generatedLines = [];
        generatedLines.push(utils.makeComment("Set SAU Control register: Disable SAU and All Secure"));
        // create assign for SAU->CTRL = 0
        generatedLines.push(utils.createAssign(SAU_PERIPHERAL_NAME, "CTRL", javaUtils.formatHexValue(java.math.BigInteger.valueOf(0), 32))); // formatHexValue(value, width in bits)
        return generatedLines;
    };

    // generate comment + code for enabling SAU
    self.generateTail = function (funcGrp) {
        var generatedLines = [];
        var enabled = funcGrp.getSauConfig().isEnabled();
        var allNonSecure = funcGrp.getSauConfig().isAllNonSecure();
        var comment = ["/* Set SAU Control register: ",
            enabled ? "Enable SAU" : "Disable SAU",
            " and ",
            allNonSecure ? "All Non-Secure" : "All Secure",
            enabled ? " (applied only if disabled)" : "",
            " */"];
        generatedLines.push(comment.join(""));

        var assignValuesCtrl = [];
        assignValuesCtrl.push(javaUtils.createMaskedBitValueString(self.getCValue(allNonSecure), "SAU_CTRL_ALLNS_Pos", "SAU_CTRL_ALLNS_Msk"));
        assignValuesCtrl.push(javaUtils.createMaskedBitValueString(self.getCValue(enabled), "SAU_CTRL_ENABLE_Pos", "SAU_CTRL_ENABLE_Msk"));
        var assignString = utils.createAssign(SAU_PERIPHERAL_NAME, "CTRL", javaUtils.createAssignValue(assignValuesCtrl, 4));
        utils.addAll(generatedLines, utils.splitLines(assignString));
        return generatedLines;
    };

    // generate code for a SAU region
    self.generateSauRegion = function (regionConfig, funcGrpPrefix) {
        var generatedLines = [];
        var regionIdx = regionConfig.getIndex();
        
        if (!utils.filterProblematicRegions(regionConfig)) {
            scriptApi.logWarning("Set SAU region number " + regionIdx + ", contains an error");
            generatedLines.push(utils.makeComment("Source code for the SAU region number " + regionIdx + " was not generated due to an error in the region's configuration"));
        } else {
            generatedLines.push(utils.makeComment("Set SAU region number"));
            generatedLines.push(utils.createAssign(SAU_PERIPHERAL_NAME, "RNR", regionIdx));
    
            generatedLines.push(utils.makeComment("Region base address"));
            generatedLines.push(utils.createAssign(SAU_PERIPHERAL_NAME, "RBAR", utils.createDefineNameBase(funcGrpPrefix + SAU_PREFIX, regionIdx) + " & SAU_RBAR_BADDR_Msk"));
    
            generatedLines.push(utils.makeComment("Region end address"));
            var assignValuesEnd = [];
            assignValuesEnd.push("(" + utils.createDefineNameEnd(funcGrpPrefix + SAU_PREFIX, regionIdx) + " & SAU_RLAR_LADDR_Msk)");
            assignValuesEnd.push(javaUtils.createMaskedBitValueString(self.getSecurityLevelInt(regionConfig.getSecurityLevelId()), "SAU_RLAR_NSC_Pos", "SAU_RLAR_NSC_Msk"));
            assignValuesEnd.push(javaUtils.createMaskedBitValueString(self.getCValue(regionConfig.isEnabled()), "SAU_RLAR_ENABLE_Pos", "SAU_RLAR_ENABLE_Msk"));
            var assignString = utils.createAssign(SAU_PERIPHERAL_NAME, "RLAR", javaUtils.createAssignValue(assignValuesEnd, 4));
            utils.addAll(generatedLines, utils.splitLines(assignString));
        }

        return generatedLines;
    };

    // generate code for SAU regions
    self.generateSauRegions = function (funcGrp) {
        var sauMemRegionConfigs = sau.getSauMemRegionConfigsToGenerateCodeFor(funcGrp, false);
        var result = [];
        sauMemRegionConfigs.forEach(function (regionConfig) {
            utils.addAll(result, self.generateSauRegion(regionConfig, funcGrp.getIdPrefix()));
            utils.addEmptyLines(result, 1);
        });
        return result;
    };

    // get value of security level based on its string value
    self.getSecurityLevelInt = function (getSecurityLevelString) {
        if (getSecurityLevelString.contains("nsc")) {
            return "1U";
        }
        return "0U";
    };

    // get value of enable flag based on its string value
    self.getCValue = function (enabledFlag) {
        var value = Number(enabledFlag);
        return value + "U";
    };
};