/*
 * Copyright 2016 Freescale Semiconductor
 * Copyright 2016-2017 NXP
 * To be used with MCUXpresso Config Tools under its Software License Agreement.
 */

var HwAbstr = {

  /**
   * Returns Mcu series
   *
   * @param gen Clock code generation object
   * @return Mcu series, e.g. "Kinetis L"
   */
  getMcuFamily: function(gen) {
    return gen.profile.getMcuInfo().getSeries();
  },
  
  /**
   * Returns currently set Power mode for the configuration
   *
   * @param cfg Current configuration.
   * @return Power mode setting
   */
  getPowerMode: function(cfg) {
    // return Power mode for a given configuration
    return cfg.getValueAsText("powerMode");
  },
  
  /**
   * Returns currently set SCG mode for the configuration
   *
   * @param cfg Current configuration.
   * @return SCG mode setting
   */
  getSCGMode: function(cfg) {
    var powerMode = this.getPowerMode(cfg);
    // return MCG mode for a given configuration
    if (powerMode == "STOP") {
      return "N/A";
    }
    else {
      return cfg.getValueAsText("SCGMode");
    }
  },
  
    /**
   * Returns setting of Initialize USBPHY clock configuration element
   *
   * @param cfg Current configuration.
   * @return Initialize USBPHY clock setting
   */
     getUsbPhySetting: function(cfg) {
      return cfg.getValueAsText("USBPHYConfig");
    },

  /* Checks all configurations on general settings and reports errors, warnings and info.
   * Typically external oscillator settings related to balast connected to the processor. 
   * E.g. settings of capacitors should be the same for all configurations.
   * return value - no data 
   */
  checkGeneralSettings: function() {
    
    var theSame = HwAbstr.isSettingSameInAllConfigurations("SCG.CLK_IN.outFreq", "sosc", "asInteger");
    if (theSame != null && !theSame) {
      scriptApi.logWarning("System OSC frequency is not the same in all configurations in which the oscillator is used. Constant BOARD_XTAL0_CLK_HZ cannot be defined.");
    }
  },
  
  
  /* Checks all configurations and reports errors, warnings and info.
   * return value - no data 
   */
  checkConfigurations: function() {
    for (var i = 0; i < Gen.configs.length; i++) {
      var tempCfg = Gen.configs[i];
      var powerMode = this.getPowerMode(tempCfg);
      var scgMode = this.getSCGMode(tempCfg);
      // SOSC or SIRC are used for VLPR and VLPW
      if (scgMode != "SOSC" && scgMode != "SIRC" && scgMode != "RTCOSC" && scgMode != "FIRC" && scgMode != "PLL0" && scgMode != "PLL1" && scgMode != "UPLL" && scgMode != "ROSC") {
        scriptApi.logError(tempCfg.getName() + " configuration: Entry to " + powerMode + " power mode is not allowed in " + scgMode + " SCG mode.");
      }
    }  
  },


  /* Checks if the setting is the same in all configurations where the setting exists and valid.
   * The validity is determined by second parameter
   * Parameter settingId - clock setting 
   * Parameter enableId - the setting of each configuration is checked only if this element is enabled  
   * Parameter getMethod - asInteger - getValueAsBigInteger function is used; otherwise getValueAsText is used 
   * return - true/false/null 
   */
  isSettingSameInAllConfigurations: function(settingId, enableId, getMethod) {
    var value = null;
    var result = true;
    for (var cfgIndex = 0; cfgIndex < Gen.configs.length; cfgIndex++) {
      var tempCfg = Gen.configs[cfgIndex];
      if (enableId == null || (HwAbstr.clockElementExist(tempCfg, enableId) && HwAbstr.isClockElementUsed(tempCfg, enableId))) {
        var setting = tempCfg.getValueAsText(settingId);
        if (setting != null) {
          if (getMethod == "asInteger") {
            setting = tempCfg.getValueAsBigInteger(settingId);
          }
          if (value == null) {
            value = setting;
          }
          else {
            if (!BigNumber.equal(value, setting)) {
              result = false;
            }
          }
        }
      } 
    }
    if (value == null) {
      return null;
    }
    return result;
  },
  
  
  /* Get array of setting values for all configurations where the setting exists and valid.
   * The validity is determined by second parameter
   * Parameter settingId - clock setting 
   * Parameter enableId - the setting of each configuration is checked only if this element is enabled  
   * Parameter getMethod - asInteger - getValueAsBigInteger function is used; otherwise getValueAsText is used 
   * return - array of values 
   */
  getSettingValuesForAllConfigurations: function(settingId, enableId, getMethod) {
    var result = new Array();
    for (var cfgIndex = 0; cfgIndex < Gen.configs.length; cfgIndex++) {
      var tempCfg = Gen.configs[cfgIndex];
      if (enableId == null || (HwAbstr.clockElementExist(tempCfg, enableId) && HwAbstr.isClockElementUsed(tempCfg, enableId))) {
        var setting = tempCfg.getValueAsText(settingId);
        if (setting != null) {
          if (getMethod == "asInteger") {
            setting = tempCfg.getValueAsBigInteger(settingId);
          }
          var settingExist = false;
          for (var resIndex = 0; resIndex < result.length; resIndex++) {
            if (BigNumber.equal(result[resIndex], setting)) {
              settingExist = true;
            }
          }
          if (!settingExist) {
            result[result.length] = setting;
          }
        }
      } 
    }
    return result;
  },

  /**
   * Returns whether RTC is used
   *
   * @param cfg Clock configuration object
   * return value - true or false 
   */
  isRtcUsed: function(cfg) {
    return HwAbstr.isClockElementUsed(cfg, "rtcUsed");
  },

  /**
   * Returns which clock is selected as the 32 kHz clock source
   *
   * @param cfg Clock configuration object
   * return value - 0b - FRO16K clock output is selected as clock source
   *        value - 1b - OSC32K clock output is selected as clock source
   */
  RtcClockSource: function(cfg) {
    return (cfg.getBitFieldValueAsBigInteger("RTC0::CTRL", "CLK_SEL"));
  },

  /**
   * Returns is the FRO16 kHz clock enabled
   *
   * @param cfg Clock configuration object
   * return value - 0b - FRO16K clock is disabled
   *        value - 1b - FRO16K clock is enabled
   */
  isFRO16KEnabled: function(cfg) {
    return (cfg.getBitFieldValueAsBigInteger("VBAT0::FROCTLA", "FRO_EN"));
  },


  /**
   * Returns whether RtcMonitorMode is used
   *
   * @param cfg Clock configuration object
   * return value - true or false 
   */
  RtcMonitorMode: function(cfg) {
    mode = ScriptBitFields.getMultiShiftedBitFieldConfigValues(cfg, [["SCG0::ROSCCSR", "ROSCCMRE"],["SCG0::ROSCCSR", "ROSCCM"]])
    if ( mode == 0x0) {
      return 0;         /* Monitor is disabled */
    }
    if (mode == 0x10000) {
      return 1;         /* Clock Monitor generates interrupt when error detected */
    }
    if (mode == 0x30000) {
      return 2;         /* Clock Monitor generates reset when error detected */
    }
  },

  /**
   * Returns OSC mode
   *
   * @param cfg Clock configuration object
   * return value - OSC 32K mode
   */
  OSCMode: function(cfg) {
    mode = ScriptBitFields.getMultiShiftedBitFieldConfigValues(cfg, [["VBAT0::OSCCTLA", "OSC_BYP_EN"],["VBAT0::OSCCTLA", "OSC_EN"]])
    return mode;
  },

  /**
   * Returns whether FIRC Trim is enabled
   *
   * @param cfg Clock configuration object
   * return value - true or false 
   */
  isFircTrimEnabled: function(cfg) {

    if (cfg.getBitFieldValueAsBigInteger("SCG0::FIRCCSR","FIRCTREN") == 1) {
      if ((cfg.getBitFieldValueAsBigInteger("SCG0::SOSCCSR","SOSCEN") == 1 && cfg.getBitFieldValueAsBigInteger("SCG0::FIRCTCFG","TRIMSRC") == 2) ||
        (cfg.getBitFieldValueAsBigInteger("VBAT0::OSCCTLA","OSC_EN") == 1 && cfg.getBitFieldValueAsBigInteger("SCG0::FIRCTCFG","TRIMSRC") == 3)) {
        return true;
      } else {
        return false;
      }
    } else {
      return false;
    }

  },
  /**
   * Returns whether SIRC Trim is enabled
   *
   * @param cfg Clock configuration object
   * return value - true or false 
   */
   isSircTrimEnabled: function(cfg) {

    if (cfg.getBitFieldValueAsBigInteger("SCG0::SIRCCSR","SIRCTREN") == 1) {
      if ((cfg.getBitFieldValueAsBigInteger("SCG0::SOSCCSR","SOSCEN") == 1 && cfg.getBitFieldValueAsBigInteger("SCG0::SIRCTCFG","TRIMSRC") == 2) ||
          (cfg.getBitFieldValueAsBigInteger("VBAT0::OSCCTLA","OSC_EN") == 1 && cfg.getBitFieldValueAsBigInteger("SCG0::SIRCTCFG","TRIMSRC") == 3)) {
        return true;
      } else {
        return false;
      }
    } else {
      return false;
    }

   },

  /**
   * Returns whether FIRC Auto Trim is enabled
   *
   * @param cfg Clock configuration object
   * return value - true or false 
   */
  isFircAutoTrimEnabled: function(cfg) {
    return (cfg.getBitFieldValueAsBigInteger("SCG0::FIRCCSR","FIRCTRUP") == 1) ? true : false;
  },
  
    /**
   * Returns Firc trim mode selection
   *
   * @param cfg Clock configuration object
   * return value - trim selection 
   */
     getFircTrimModeSelection: function(cfg) {
      return cfg.getValueAsText("SCG_FIRCCSR_TRIM_CFG");
    },

    /**
   * Returns Sirc trim mode selection
   *
   * @param cfg Clock configuration object
   * return value - trim selection 
   */
     getSircTrimModeSelection: function(cfg) {
      return cfg.getValueAsText("SCG_SIRCCSR_TRIM_CFG");
    },

 /**
   * Returns whether RTC has its own oscillator or not
   *
   * @param cfg Clock configuration object
   * return value - true or false 
   */
  hasRtcOwnOscillator: function() {
    return HwAbstr.clockElementExist(Gen.configs[0], "rtcExtalPin");
  },
  

 /**
   * Returns whether RTC has clock output to peripherals
   *
   * @param cfg Clock configuration object
   * return value - true or false 
   */
  hasRtcOutputToPeripherals: function() {
    return HwAbstr.clockElementExist(Gen.configs[0], "rtcOut32kClk")
  },

 /**
   * Returns whether SIM has clock output pin
   *
   * @param cfg Clock configuration object
   * return value - true or false 
   */
  hasClkOutputPin: function() {
    return HwAbstr.clockElementExist(Gen.configs[0], "clkoutClock")
  },  

  /**
   * Returns bit-field value
   *
   * @param cfg Clock configuration object
   * @param bitFieldId Bit-field id, see this.bitFields
   * return Bit-field value  
   */
  getBitFieldValue: function(cfg, bitFieldId) {
    var bitField = this.bitFields[bitFieldId];
    if (bitField == null) {
      scriptApi.logError("[DEBUG] Function HwAbstr.getBitFieldValue() doesn't support bit-field " + bitField);
      return -1; 
    }
    var value = cfg.getBitFieldValueAsBigInteger(bitField[0], bitField[1]);
    if (value == null) {
      scriptApi.logError("[DEBUG] Unknown value for " + bitField[0] + "[" + bitField[1] + "]"); 
    } 
    return (value);
  },
 
  // Object which access element/component ids. It is used in functions clockElementExist and isClockElementUsed
  clockIds: {
      "scgFirc"     : "SCG.FIRC",
      "scgSirc"     : "SCG.SIRC",
      "sosc"        : "SCG.SOSC",
     // "rtcExtalPin" : "OSC32.XTAL32.enable",  //it is not usable with isClockElementUsed function 
      //"rtcClkIn"    : "SIM.RTC_CLK_EXT_IN",
      "rtc"         : "RTC",

      "rtcUsed"     : "RTC.LP_OSC",

      "pll0"        :  "SCG.PLL0",
      "pll1"        :  "SCG.PLL1",
      "upll"        :  "USB_PHY.PLL",
      "clkoutClock" : "SYSCON.CLKOUT",
      "usbphyPll"   : "USB_PHY.PLL",
      "usbPfdClk"   : "USB_PHY.USB_PLL_CLK",
      // Clocks IDs for SYSCON
      "adc0Clock"   : "SYSCON.ADC0CLK"

  },
  

  /**
   * Returns whether a clock element/component exists
   *
   * @param cfg Clock configuration object
   * @param id Setting id, see this.clockIds
   * return value - true or false 
   */
  clockElementExist: function(cfg, id) {
    if (cfg == null) {
      scriptApi.logError("[DEBUG] Function HwAbstr.clockElementExist() doesn't get right configuration (" + id + ")");
      return false;
    }
    var settingId = this.clockIds[id];
    if (settingId == null) {
      scriptApi.logError("[DEBUG] Function HwAbstr.clockElementExist() doesn't support setting " + id);
      return false; 
    }
    //settingId +=  ".enable";
    return cfg.existsId(settingId);
  },
  

  /**
   * Returns whether a clock element is used
   *
   * @param cfg Clock configuration object
   * @param id Setting id, see this.clockIds
   * return value - true or false 
   */
  isClockElementUsed: function(cfg, id) {
    var settingId = this.clockIds[id];
    if (settingId == null) {
      scriptApi.logError("[DEBUG] Function HwAbstr.isClockElementUsed() doesn't support setting " + id);
      return false; 
    }
    settingId +=  ".enable";
    var setting = cfg.getValueAsText(settingId);
    if (setting == null) {
      scriptApi.logError("[DEBUG] Unknown id: " + settingId); 
      return false; 
    } 
    return (setting == "true");
  },

  /**
   * Returns the voltage setting of the DCDC and LDO_CORE VDD Regulator Voltage
   *
   * @param cfg Clock configuration object
   * return value - 01b - Regulate to Mid Drive Voltage (1.0V)
   *        value - 10b - Regulate to Normal Voltage (1.1V)
   *        value - 10b - Regulate to overdrive Voltage (1.2V)
   */
     CoreLdoVddLvl: function(cfg) {
      return (cfg.getBitFieldValueAsBigInteger("SPC0::ACTIVE_CFG", "CORELDO_VDD_LVL"));
    }    
               
} // HwAbstr object prototype


