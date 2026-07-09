/*
 * Copyright 2020-2023 NXP
 * To be used with MCUXpresso Config Tools under its Software License Agreement.
 */

var AHBSE_FUNCTION_NAME = "BOARD_InitAHBSE";

/**
 * Class for generating code for security peripheral (e.g. AHB, TRDC)
 * @returns {SecurityPeripheralCode}
 */
SecurityPeripheralCode = function() {
    var self = this;

    var securityPeripheralType = sharedResources.profile.getSecurityType();

    self.getSecPeriphSpecificComment = function () {
        if (securityPeripheralType === "AHB" || securityPeripheralType === "AHBSC_RWX") {
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

    self.pinsSecurityComment = "Pins: Access protection on IO pins and GPIO port IRQ/DMA requests\n";
    self.pinsSecurityDescription = "Possible values for security control:\n"
                        + "*-----------=----------=---------------------=---------------------*\n"
                        + "| Bit-field |  Config  |   Security access   |  Privilege access   |\n"
                        + "|   value   |  state   | control (PCNS/ICNS) | control (PCNP/ICNP) |\n"
                        + "*-----------+----------+---------------------+---------------------*\n"
                        + "|    0b00   |  S-Priv  |        Secure       |      Privilege      |\n"
                        + "|    0b01   |  S-User  |        Secure       |    Non-Privilege    |\n"
                        + "|    0b10   |  NS-Priv |      Non-Secure     |      Privilege      |\n"
                        + "|    0b11   |  NS-User |      Non-Secure     |    Non-Privilege    |\n"
                        + "*-----------=----------=---------------------=---------------------*\n"
                        + "NOTE:\n"
                        + "Pin Control Non-Secure (PCNS) & Non-Privilege (PCNP) configurable for every IO pin,\n"
                        + "Interrupt Control Non-Secure (ICNS) & Non-Privilege (ICNP) for requests 0 a 1 only.\n"

    self.interruptsSecurityComment = "Interrupts: Interrupt security configuration\n";
    self.interruptsSecurityDescription = "Possible values for every interrupt:\n"
                            + " 0b0    Secure\n"
                            + " 0b1    Non-secure\n";

    self.triggersComment = "Triggers: Configuration of the response action for an intrusion event\n";
    self.triggersDescription = "Possible values for every event:\n"
                + "*--------------=------------=-------------------=------------------*\n"
                + "|   INx_SEL0   |  INx_SEL1  |  Signal selected  |  Writable field  |\n"
                + "*--------------+------------+-------------------+------------------*\n"
                + "|     0b10     |    0b10    |        No         |   Yes (default)  |\n"
                + "|     0b01     |    0b10    |        Yes        |        Yes       |\n"
                + "|       -      |  != 0b10   |        Yes        |        No        |\n"
                + "| 0b00 or 0b11 |      -     |        Yes        |        No        |\n"
                + "*--------------=------------=-------------------=------------------*\n"
                
    self.generate = function (funcGrp) {
        var resultLines = [];
        var checkersConfig = funcGrp.getCheckersConfig();
        var initStrategy = "to_after_reset";
        var isMCXN11 = ["MCXN235", "MCXN236"].indexOf(scriptApi.getProfile().getMcuInfo().getPartNumber()) > -1
        var isMCXNFamily =  scriptApi.getProfile().getMcuInfo().getSeries() === "MCX MCXN";
        //
        resultLines.push(self.comment);
        utils.addAll(resultLines, self.getSecPeriphSpecificComment());

        mpcRegistersMap = javaUtils.createRegistersMap(checkersConfig.getCheckerConfig("MPC").getBitFieldsInfo(), sharedResources.generateModifiedRegistersOnly);
        mpcCode = javaUtils.writeRegistersMap("Security level configuration of MPC checker", false, mpcRegistersMap, true, null, sharedResources.generateCodeSecurityHardening);
        resultLines.push(mpcCode);

        ppcRegistersMap = javaUtils.createRegistersMap(checkersConfig.getCheckerConfig("PPC").getBitFieldsInfo(), sharedResources.generateModifiedRegistersOnly);
        ppcCode = javaUtils.writeRegistersMap("Security level configuration of PPC checker", false, ppcRegistersMap, initStrategy, null, sharedResources.generateCodeSecurityHardening);
        ppcCode = self.addTwoHardcodedMasks(ppcCode, "AHBSC", "AHB_PERIPHERAL0_SLAVE_PORT_P12_SLAVE_RULE2", "0xFFFFFFFCU", "0x00000003U", sharedResources.generateCodeSecurityHardening);
        ppcCode = self.addTwoHardcodedMasks(ppcCode, "AHBSC", "AHB_PERIPHERAL1_SLAVE_PORT_P13_SLAVE_RULE2", "0xFFFFFFFCU", "0x00000003U", sharedResources.generateCodeSecurityHardening);
        resultLines.push(ppcCode);

        mbcRegistersMap = javaUtils.createRegistersMap(checkersConfig.getCheckerConfig("MBC0").getBitFieldsInfo(), sharedResources.generateModifiedRegistersOnly);
        mbcCode = javaUtils.writeRegistersMap("Security level configuration of MBC checker", false, mbcRegistersMap, true, null, false);
        resultLines.push(mbcCode);

        mastersRegistersMap = javaUtils.createRegistersMap(funcGrp.getMastersConfig().getBitFieldsInfo(), sharedResources.generateModifiedRegistersOnly);
        mastersCode = javaUtils.writeRegistersMap("Security level configuration of masters", false, mastersRegistersMap, true, null, sharedResources.generateCodeSecurityHardening);
        resultLines.push(mastersCode);

        pinsSecurityRegistersMap = javaUtils.createRegistersMap(funcGrp.getPinsConfig().getBitFieldsInfo(), sharedResources.generateModifiedRegistersOnly);
        pinsReadMaskCode = javaUtils.writeRegistersMap(self.pinsComment, true, pinsSecurityRegistersMap, true, self.pinsDescription, sharedResources.generateCodeSecurityHardening);
        pinsReadMaskCode = utils.splitLines(pinsReadMaskCode)
                            .filter(utils.notEmpty)
                            .map(function (line) {
                                    return (line.startsWith("TEE_WRITE_REGISTER(&(GPIO") || line.startsWith("GPIO")) ? "" : line;
                                })
                            .join("\n\n");
        if(isMCXN11) {
            pinsReadMaskCode = self.addTwoHardcodedMasks(pinsReadMaskCode, "AHBSC", "SEC_GPIO_MASK\\[0\\]", "0xC0000000U", "0x3FFFFFFFU", sharedResources.generateCodeSecurityHardening);
        }
        if(isMCXNFamily) {
            resultLines.push(pinsReadMaskCode)
        }
                        
        pinsSecurityCode = javaUtils.writeRegistersMap(self.pinsSecurityComment, true, pinsSecurityRegistersMap, true, self.pinsSecurityDescription, false);
        pinsSecurityCode = utils.splitLines(pinsSecurityCode)
                            .filter(utils.notEmpty)
                            .map(function (line) {
                                  return (line.startsWith("TEE_WRITE_REGISTER(&(AHBSC") || line.startsWith("AHBSC")) ? "" : line;
                            })
                            .join("\n\n");
        clockEnableCode = "/* Enable clock gates to GPIO */\n" +
            "CLOCK_EnableClock(kCLOCK_Gpio0);\n" + 
            "CLOCK_EnableClock(kCLOCK_Gpio1);\n" + 
            "CLOCK_EnableClock(kCLOCK_Gpio2);\n" + 
            "CLOCK_EnableClock(kCLOCK_Gpio3);\n" + 
            "CLOCK_EnableClock(kCLOCK_Gpio4);\n"
        if(isMCXNFamily) {
            resultLines.push(clockEnableCode)
        }
        resultLines.push(pinsSecurityCode)

        interruptsMaskingRegistersMap = javaUtils.createRegistersMap(funcGrp.getInterruptsConfig().getInterruptsMaskingConfig().getBitFieldsInfo(), sharedResources.generateModifiedRegistersOnly);
        interruptsMaskingCode = javaUtils.writeRegistersMap(self.interruptsMaskingComment, true, interruptsMaskingRegistersMap, true, self.interruptsMaskingDescription, sharedResources.generateCodeSecurityHardening);
        resultLines.push(interruptsMaskingCode);

        interruptsSecurityRegistersMap = javaUtils.createRegistersMap(funcGrp.getInterruptsConfig().getInterruptsSecurityConfig().getBitFieldsInfo(), sharedResources.generateModifiedRegistersOnly);
        interruptsSecurityCode = javaUtils.writeRegistersMap(self.interruptsSecurityComment, true, interruptsSecurityRegistersMap, true, self.interruptsSecurityDescription, sharedResources.generateCodeSecurityHardening);
        resultLines.push(interruptsSecurityCode);

        if(funcGrp.getGlobalOptionsConfig().getOptionValue("_trigger_sources_")) {
            triggersRegistersMap = javaUtils.createRegistersMap(funcGrp.getTriggersConfig().getBitFieldsInfo(), sharedResources.generateModifiedRegistersOnly);
            triggersSecurityCode = javaUtils.writeRegistersMap(self.triggersComment, true, triggersRegistersMap, true, self.triggersDescription, sharedResources.generateCodeSecurityHardening);
            resultLines.push(triggersSecurityCode);
        }

        // Global options bit-field corrections
        var globalBitFieldsInfos = funcGrp.getGlobalOptionsConfig().getBitFieldsInfo();
        self.addHardcodedBitField(globalBitFieldsInfos, "AIRCR", "VECTKEY", "0x05FA");
        globalOptionsRegistersMap = javaUtils.createRegistersMap(globalBitFieldsInfos, sharedResources.generateModifiedRegistersOnly);
        globalOptionsCode = javaUtils.writeRegistersMap("Global Options", false, globalOptionsRegistersMap, false, null, sharedResources.generateCodeSecurityHardening);
        globalOptionsCode = self.addTwoHardcodedMasks(globalOptionsCode, "GPIO0", "LOCK", "0x0000000FU", "0xFFFFFFF0U", sharedResources.generateCodeSecurityHardening);
        globalOptionsCode = self.addTwoHardcodedMasks(globalOptionsCode, "GPIO1", "LOCK", "0x0000000FU", "0xFFFFFFF0U", sharedResources.generateCodeSecurityHardening);
        globalOptionsCode = self.addTwoHardcodedMasks(globalOptionsCode, "GPIO2", "LOCK", "0x0000000FU", "0xFFFFFFF0U", sharedResources.generateCodeSecurityHardening);
        globalOptionsCode = self.addTwoHardcodedMasks(globalOptionsCode, "GPIO3", "LOCK", "0x0000000FU", "0xFFFFFFF0U", sharedResources.generateCodeSecurityHardening);
        globalOptionsCode = self.addTwoHardcodedMasks(globalOptionsCode, "GPIO4", "LOCK", "0x0000000FU", "0xFFFFFFF0U", sharedResources.generateCodeSecurityHardening);
        globalOptionsCode = self.addTwoHardcodedMasks(globalOptionsCode, "GPIO5", "LOCK", "0x0000000FU", "0xFFFFFFF0U", sharedResources.generateCodeSecurityHardening);
        globalOptionsCode = self.addHardcodedMask(globalOptionsCode, "SCB", "AIRCR", "0x0000FFFFU", sharedResources.generateCodeSecurityHardening);
        if(isMCXN11) {
            globalOptionsCode = self.addTwoHardcodedMasks(globalOptionsCode, "SCB", "CPACR", "0xFF0FFFFFU", "0x00F00000U", sharedResources.generateCodeSecurityHardening);
            globalOptionsCode = self.addTwoHardcodedMasks(globalOptionsCode, "SCB", "NSACR", "0xFFFFF3FFU", "0x00000C00U", sharedResources.generateCodeSecurityHardening);
        } else {
            globalOptionsCode = self.addTwoHardcodedMasks(globalOptionsCode, "SCB", "CPACR", "0xFF0FFFFCU", "0x00F00003U", sharedResources.generateCodeSecurityHardening);
            globalOptionsCode = self.addTwoHardcodedMasks(globalOptionsCode, "SCB", "NSACR", "0xFFFFF3FEU", "0x00000C01U", sharedResources.generateCodeSecurityHardening);
        }
        globalOptionsCode = self.addTwoHardcodedMasks(globalOptionsCode, "AHBSC", "SEC_GP_REG_LOCK", "0xFFEAAFFAU", "0x00155005", sharedResources.generateCodeSecurityHardening);
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
     * Add a hardcoded mask when calling macro robust against glitch protection, transforming the call
     * from TEE_READ_MODIFY_WRITE_REGISTER to TEE_READ_MODIFY_WRITE_MASK_REGISTER function
     * @param generatedCode code already generated from register data
     * @param peripheral peripheral containing the register
     * @param register specific register assignment to be transformed
     * @param mask mask to be applied as final argument of TEE_READ_MODIFY_WRITE_MASK_REGISTER function
     * @param generateCodeSecurityHardening boolean specifying if macro is enabled; if not generatedCode is returned unchanged
     */
    self.addHardcodedMask = function(generatedCode, peripheral, register, mask, generateCodeSecurityHardening) {
        return self.addTwoHardcodedMasks(generatedCode, peripheral, register, null, mask, generateCodeSecurityHardening)
    }

    /**
     * Add a hardcoded mask when calling macro robust against glitch protection, transforming the call
     * from TEE_WRITE_REGISTER to TEE_READ_MODIFY_WRITE_MASK_REGISTER function
     * @param generatedCode code already generated from register data
     * @param peripheral peripheral containing the register
     * @param register specific register assignment to be transformed
     * @param mask1 mask to be applied as first argument of TEE_READ_MODIFY_WRITE_MASK_REGISTER function
     * @param mask2 mask to be applied as third argument of TEE_READ_MODIFY_WRITE_MASK_REGISTER function
     * @param generateCodeSecurityHardening boolean specifying if macro is enabled; if not generatedCode is returned unchanged
     */
    self.addTwoHardcodedMasks = function(generatedCode, peripheral, register, mask1, mask2, generateCodeSecurityHardening) {
        if(!generateCodeSecurityHardening) {
            return generatedCode;
        } else if (mask1 != null) {
            return generatedCode
            .replace(new RegExp("(TEE_WRITE_REGISTER)(.*\(" + peripheral + "->" + register + "\).*)(\\\);)", "g"), 
            (_, __, p2) => {
                value = p2.replace(new RegExp(".*\\s"), "");
                register = register.replace(new RegExp("\\\\", "g"), "");
                return `TEE_READ_MODIFY_WRITE_MASK_REGISTER(&(${peripheral}->${register}), ${mask1}, ${value}, ${mask2});`;
            });
        } else {
            return generatedCode
            .replace(new RegExp("(TEE_READ_MODIFY_WRITE_REGISTER)(.*\(" + peripheral + "->" + register + "\).*)(\\\);)", "g"), 
            (_, __, p2) => {
                return `TEE_READ_MODIFY_WRITE_MASK_REGISTER${p2}, ${mask2});`;
            });
        }
    };

    self.comment = "/* " + (securityPeripheralType == "AHBSC_RWX"? "AHB" : securityPeripheralType) + " configuration */";
};