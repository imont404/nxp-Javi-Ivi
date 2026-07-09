/*
 * Copyright 2020-2023 NXP
 * To be used with MCUXpresso Config Tools under its Software License Agreement.
 */

// Object assembles parts of generated tzm_config.c file
CFileDescriptor = function() {
    var self = this;

    // function adds a notification and initial parts of the tzm_config.c file content (defined by argument) and returns wrapped content of the file
    self.wrap = function(mainContent) {
        return [sharedResources.notification, sharedResources.getGlobalYaml()]
            .concat(mainContent)
            .filter(utils.notEmpty)
            .join("\n");
    };

    // function assembles and returns a final content of the file from its parts
    // (inludes, definitions, TEE yaml and boot function)
    self.render = function() {
        return self.wrap([
            utils.renderSection(self.includes, "Included files"),
            utils.renderSection(self.getSecureFaultHandlerPrototype(), ""),
            utils.renderSection(self.getDefinitions(), "Definitions"),
            utils.renderSection(self.getConfigForFuncGrpCode(), ""),
            utils.renderSection(self.getEntryCodeFunction(), ENTRY_FUNCTION_NAME + " function")
        ]);
    };

    // code generated for SAU
    self.sauCode = function (funcGrp) {
        return new SauCode().generate(funcGrp)
            .map(function (line) {
                return utils.indent(2) + line;
            }).join("\n");
    };

    // code generated for current security peripheral
    self.securityPeripheralCode = function (funcGrp) {
        return new SecurityPeripheralCode().generate(funcGrp);
    };

    // generated definitons
    self.getDefinitions = function () {
        var linesArray = [];
        utils.addEmptyLines(linesArray, 1);
        utils.addAll(linesArray, new SauDefines().generateDefines(32));
        // utils.addEmptyLines(linesArray, 1);
        utils.addAll(linesArray, new SecureMpuDefines().generateDefines(32));
        // utils.addEmptyLines(linesArray, 1);
        utils.addAll(linesArray, new NonSecureMpuDefines().generateDefines(32));
        //
        return linesArray
            .join("\n");
    };

    // string representing all include directives
    self.includes = ['#include "fsl_common.h"',
        '#include "' + H_FILE_NAME + '"\n'
    ].join("\n");

        
    self.faultHandlerSection = ['void SecureFault_Handler(void);',
        '#pragma required = SecureFault_Handler\n'
    ].join("\n")

    self.getSecureFaultHandlerPrototype = function () {
        var linesArray = [];
        // define SecureFault_Handler prototype function before use, if security code hardening is enabled
        if (sharedResources.isEnabledSecurityCodeHardening()) {
            utils.addAll(linesArray, utils.makeComment("SecureFault Handler used when Cortex-M33 instruction glitch resilient ASM code for register writes fails"));
            utils.addAll(linesArray, self.faultHandlerSection)
        }
        //
        return linesArray
            .join("\n");
    }

    // returns whether a code should be generated for an MPU
    self.isMpuCodeGenerationOn = function (mpuConfig) {
        return (mpuConfig !== null) && mpuConfig.isEnabled();
    };

    // string that calls MpusInit function if it is generated
    self.callMpusInit = function (funcGrp) {
        // in case legacy source code is not generated, MpusInit is called from entry function
        if (LEGACY_SOURCE_NAMES) {
            if (self.isMpuCodeGenerationOn(mpu.getSecureMpuConfig(funcGrp)) || self.isMpuCodeGenerationOn(mpu.getNonSecureMpuConfig(funcGrp))) {
                var lines = [];
                var funcGrpPrefix = funcGrp.getIdPrefix();

                lines.push(utils.makeComment("MPUs initialization"));

                if (self.isMpuCodeGenerationOn(mpu.getSecureMpuConfig(funcGrp))){
                    lines.push(funcGrpPrefix + INIT_S_MPU_FUNCTION_NAME + "();");
                }

                if (self.isMpuCodeGenerationOn(mpu.getNonSecureMpuConfig(funcGrp))){
                    lines.push(funcGrpPrefix + INIT_NS_MPU_FUNCTION_NAME + "();");
                }

                utils.addEmptyLines(lines, 1);

                return lines.map(function (line) {
                        return utils.indent(2) + line;
                    }).join("\n");
            }
        }
        return "";
    };

    // code generated for Secure MPU
    self.secureMpuCode = function (mpuConfig, funcGrpPrefix) {
        return new SecureMpuCode(mpuConfig).generate(funcGrpPrefix)
            .map(function (line) {
                return utils.indent(2) + line;
            }).join("\n");
    };

    // code generated for Non-Secure MPU
    self.nonSecureMpuCode = function (mpuConfig, funcGrpPrefix) {
        return new NonSecureMpuCode(mpuConfig).generate(funcGrpPrefix)
            .map(function (line) {
                return utils.indent(2) + line;
            }).join("\n");
    };

    // code for initialization
    self.initCode = function (funcGrp) {
        // in case of legacy code, securityPeripheralCode init is part of initBootFunction
        if (LEGACY_SOURCE_NAMES) {
            return [self.callMpusInit(funcGrp), self.sauCode(funcGrp), self.securityPeripheralCode(funcGrp)]
                    .filter(utils.notEmpty)
                    .join("\n");
        // in case of non-legacy code, securityPeripheralCode init is part of initAHBSEFuntion
        } else {
            return [self.callMpusInit(funcGrp), self.sauCode(funcGrp)]
                    .filter(utils.notEmpty)
                    .join("\n");
        }
    };

    // get a definition of BOARD_InitTrustZone() function and its implementation
    self.initBootFunction = function (funcGrp) {
        return "void " + funcGrp.getIdPrefix() + INIT_FUNCTION_NAME + "()\n{\n"
            + self.initCode(funcGrp)
            + "\n}\n";
    };

    // generated YAML config for functions
    self.getYamlForFuncGrpCode = function (funcGrp) {
        var linesArray = [];
        utils.addAll(linesArray, sharedResources.getYamlForFuncGrp(funcGrp));
        return linesArray
            .join("\n");
    };

    //
    self.getConfigForFuncGrpCode = function () {
        var resultLines = [];
        //
        allFuncGroups = sharedResources.profile.getFunctionalGroups();
        allFuncGroups.forEach(function (funcGrp) {
            var funcGrpLines = [];
            var funcGrpPrefix = funcGrp.getIdPrefix();
            utils.addAll(funcGrpLines, self.getYamlForFuncGrpCode(funcGrp));
            utils.addAll(funcGrpLines, utils.makeFuncComment(funcGrp.getName(), funcGrp.getDescription()));
            //
            if (utils.notEmpty(self.getInitSecureMpuFunction(funcGrp))) {
                utils.addAll(funcGrpLines, utils.renderSection(self.getInitSecureMpuFunction(funcGrp), funcGrpPrefix + INIT_S_MPU_FUNCTION_NAME + " function"));
            }
            if (utils.notEmpty(self.getInitNonSecureMpuFunction(funcGrp))) {
                utils.addAll(funcGrpLines, utils.renderSection(self.getInitNonSecureMpuFunction(funcGrp), funcGrpPrefix + INIT_NS_MPU_FUNCTION_NAME + " function"));
            }
            utils.addAll(funcGrpLines, utils.renderSection(self.initBootFunction(funcGrp), funcGrpPrefix + INIT_FUNCTION_NAME + " function"));
            if (utils.notEmpty(self.getInitSecureMpuFunction(funcGrp)) || utils.notEmpty(self.getInitNonSecureMpuFunction(funcGrp))) {
                utils.addAll(funcGrpLines, utils.renderSection(self.getInitMpuFunction(funcGrp), funcGrpPrefix + MPU_FUNCTION_NAME + " function"));
            }
            utils.addAll(funcGrpLines, utils.renderSection(self.getInitAHBSEFunction(funcGrp), funcGrpPrefix + AHBSE_FUNCTION_NAME + " function"));
            utils.addAll(funcGrpLines, utils.renderSection(self.getEntryCodeFuncGrpCode(funcGrp), funcGrpPrefix + INIT_GRPFUNC_NAME + " function"));
            utils.addAll(resultLines, funcGrpLines);
        });
        //
        return resultLines.join("\n");
    };

    self.getEntryCodeFuncGrpCode = function (funcGrp) {
        //
        var entryCodeFuncGrpPrefix = funcGrp.getIdPrefix();
        if (LEGACY_SOURCE_NAMES) {
            return "";
        } else {
            var lines = "void " + entryCodeFuncGrpPrefix + INIT_GRPFUNC_NAME + "()\n"
                +  "{\n";
            // MpusInit is enabled from UI
            if (self.getInitMpuFunction(funcGrp) != "") {
                lines += utils.indent(2) + entryCodeFuncGrpPrefix + MPU_FUNCTION_NAME + "();\n";
            }
            lines += utils.indent(2) + entryCodeFuncGrpPrefix + INIT_FUNCTION_NAME + "();\n"
                + utils.indent(2) + entryCodeFuncGrpPrefix + AHBSE_FUNCTION_NAME + "();\n"
                + "}\n";
            return lines;
        }
    };

    // entry function code - call of MPU, TrustZone, AHB etc. init functions 
    self.getEntryCodeFunction = function () {
        var lines = "void " + ENTRY_FUNCTION_NAME + "()\n"
            + "{\n";
        // Set entry code function name per LEGACY_SOURCE_NAMES mode, funcGrp prefix always included (MCUCM-9708)
        var entryCodeFuncName = (LEGACY_SOURCE_NAMES) ? INIT_FUNCTION_NAME : INIT_GRPFUNC_NAME;
        //
        allFuncGroups = sharedResources.profile.getFunctionalGroups();
        allFuncGroups.forEach(function (funcGrp) {
            if (funcGrp.isCalledFromDefaultInit()) {
                lines += utils.indent(2) + funcGrp.getIdPrefix() + entryCodeFuncName + "();\n";  // func call from default init, funcGrp prefix included
            }
        });
        //
        lines += "}";
        return lines;
    };
    
    // creates InitAHBSE function
    self.getInitAHBSEFunction = function (funcGrp) {
        if (LEGACY_SOURCE_NAMES) {
            return "";
        } else {
            return "void " + funcGrp.getIdPrefix() + AHBSE_FUNCTION_NAME + "()\n{\n" + self.securityPeripheralCode(funcGrp) + "\n}\n";
        }
    };

    // creates InitMPU function
    self.getInitMpuFunction = function (funcGrp) {
        var lines = "";
        var funcGrpPrefix = funcGrp.getIdPrefix();
        if (LEGACY_SOURCE_NAMES) {
            return lines;
        } else {
            if (self.isMpuCodeGenerationOn(mpu.getSecureMpuConfig(funcGrp))) {
                lines += utils.indent(2) + funcGrpPrefix + INIT_S_MPU_FUNCTION_NAME + "();\n";
            }
            if (self.isMpuCodeGenerationOn(mpu.getNonSecureMpuConfig(funcGrp))) {
                lines += utils.indent(2) + funcGrpPrefix + INIT_NS_MPU_FUNCTION_NAME + "();\n";
            }
            if (lines != "") {
                lines = "void " + funcGrpPrefix + MPU_FUNCTION_NAME + "()\n"
                    + "{\n" 
                    + utils.indent(2) + utils.makeComment("MPUs initialization") + "\n"
                    + lines 
                    + "}\n";
            }
            return lines;
        }
    };

    // creates InitSecureMpu function
    self.getInitSecureMpuFunction = function (funcGrp) {
        var mpuConfig = mpu.getSecureMpuConfig(funcGrp);
        if (self.isMpuCodeGenerationOn(mpuConfig)) {
            return "void " + funcGrp.getIdPrefix() + INIT_S_MPU_FUNCTION_NAME + "()\n{\n"
                + self.secureMpuCode(mpuConfig, funcGrp.getIdPrefix())
                + "\n}\n";
        }
        return "";
    };

    // creates InitNonSecureMpu function
    self.getInitNonSecureMpuFunction = function (funcGrp) {
        var mpuConfig = mpu.getNonSecureMpuConfig(funcGrp);
        if (self.isMpuCodeGenerationOn(mpuConfig)) {
            return "void " + funcGrp.getIdPrefix() + INIT_NS_MPU_FUNCTION_NAME + "()\n{\n"
                + self.nonSecureMpuCode(mpuConfig, funcGrp.getIdPrefix())
                + "\n}\n";
        }
        return "";
    };
};