/*
 * Copyright 2016 Freescale Semiconductor
 * Copyright 2016-2017 NXP
 * To be used with MCUXpresso Config Tools under its Software License Agreement.
 */

/**
 * Functions for making SDK structure field values from clock model state
 *
 */
 
// FieldVal object specifies object which is returned by SDKMapper.getFieldValue function.
var FieldVal = function (value, description, number) {
   this.value = value;
   this.description = description;
   this.number = number;
}; 
 
// SDKMapper object allows mapping a id to sdk enumeration symbol, a number or #define symbol (including "#define <symbol> ..." generation).
// SDKMapper object allows mapping a id to sdk enumeration symbol, a number or #define symbol (including "#define <symbol> ..." generation).
SDKMapper = {
  // Reference on define container in c file 
  setCWriterDefineContainer: null,
  
  // Internal function of SDKMapper object. This function adds a #define into container specified by setCWriterDefineContainer
  addCDefine: function (defineSymbol, defineValue, defineComment, formatOptions) {
    if (this.setCWriterDefineContainer != null) {
      this.setCWriterDefineContainer.addDefine(defineSymbol, defineValue, defineComment, formatOptions);
    }            
  },
  
  /* Gets value FieldVal (value, comment and number) for specified id. FieldVal.value can be formated according to parameter formatOptions.
   * Founded value (FieldVal.value) can be replaced by a symbolic constant according to defineSymbolname parameter.
   * cfg - reference to configuration object
   * fieldID - ID of item which should be mapped to number or enumaration or define symbol based on a setting
   * defineSymbolName - if null then no define is added into container specified by setCWriterDefineContainer
   *                  - if "" then define is added into the container named according to data in "mapping" part of this SDKMapper object and the alternative symbol is returned as .value
   *                  - if "<not_empty_string>" then define is added into the container named according to this parameter and the alternative symbol is returned as .value                   
   * formatOptions - determines format of result number in #define, e.g. ["hex"] or ["hex","unsigned"], see parameter options for function OutputUtils.formatField
   *                 it is ignored if defineSymbolName is null       
   * return value - object FieldVal 
   */       
  getFieldValue: function (cfg, fieldID, defineSymbolName, formatOptions) {
     var map = this.mapping[fieldID];               //default reference to the ID mapping object
     if (map == null) {
       scriptApi.logError("[DEBUG] Unsupported field ID " + fieldID + "."); 
       return null;
     }
     if (map[HwAbstr.getMcuFamily(Gen)] != null) {  //use the default reference if there is no Mcu series mapping object
       map = map[HwAbstr.getMcuFamily(Gen)];
     }
     if (formatOptions == null && map.formatOptions) {
       if ("formatOptions" in map) {
         formatOptions = map.formatOptions;
       }
     }
     switch (map.type) {
       case "enum":    
         var kval = map.keyFunc(cfg);
         if (kval == null) {
           scriptApi.logError("[DEBUG] " + map.keyFunc.toString() + " returns null."); 
           return null;
         }
         for (var i=0; i< map.enumMap.length; i++) {
           if (BigNumber.equal(map.enumMap[i][0], kval)) {
             var ret = new FieldVal(map.enumMap[i][1], map.enumMap[i][2], map.enumMap[i][0]);
             if (defineSymbolName != null) { 
               if (defineSymbolName == "") { 
                 if (map.enumMap[i][3] != null && map.enumMap[i][3] != "") {
                   this.addCDefine(map.enumMap[i][1], map.enumMap[i][0], map.enumMap[i][2], formatOptions);
                 }
               }
               else {
                 this.addCDefine(defineSymbolName, map.enumMap[i][0], map.enumMap[i][2], formatOptions);
                 ret.value = defineSymbolName;
               }
             }
             else {
               if (map.enumMap[i][3] != null && map.enumMap[i][3] != "") {
                 ret.value = map.enumMap[i][0];
               }
             }
             return ret;
           }
         }
         scriptApi.logError("[DEBUG] " + "enuMap for " + fieldID + " does not contain value " + kval + "."); 
         return null;
       case "number":
             var ret = map.expr(cfg);
             ret.number = ret.value;
             if (defineSymbolName != null) {
               if (defineSymbolName == "") { 
                 if ("defineSymbol" in map) {
                   var defineSymbol = map.defineSymbol(cfg, ret);
                   this.addCDefine(defineSymbol, ret.value, ret.description, formatOptions);
                   ret.value = defineSymbol;
                 }
               }
               else {
                 this.addCDefine(defineSymbolName, ret.value, ret.description, formatOptions);
               }
             }
             return ret;
       case "multiEnum":
         // Usable for "mask" type enumerations. There can be enumerations which doesn't define anything for state =0. Then in case of there is no enum state in final code, a define needs to be created
         // Examples result code: "kMCG_PllEnableIndependent | kMCG_PllEnableInStop", "kMCG_PllEnableIndependent", "MCG_PLL_DISABLE_INDEPENDENT_DISABLE_IN_STOP"
         var code = "";
         var comment = "";
         // Iterate throught all functions in funcMap, create expression/comment from corresponding row in enumMap and add defines if there is "defineSymbol" in the row
         for (var i=0; i< map.funcMap.length; i++) {
           if (map.funcMap[i][0](cfg)) {
             var enm = map.enumMap[map.funcMap[i][1]];
             if (defineSymbolName != null) {
               if (enm[3] != null && enm[3] != "") {
                 this.addCDefine(enm[1], enm[0], enm[2], formatOptions);
               }
               code = code + ((code == "") ? "" : " | ") + enm[1];
             }
             else {
               if (enm[3] != null && enm[3] != "") {
                 code = code + ((code == "") ? "" : " | ") + "0x" + BigNumber.toString(enm[0],16) + "U";
               }
               else {
                 code = code + ((code == "") ? "" : " | ") + enm[1];
               }
             }
             comment = comment + ((comment == "") ? "" : ", ") + enm[2];
           }
         }
         // If all functions returned zeros, use symbol "0"
         if (code == "") {
           if (map.enumMap[0] != null) {
             if (defineSymbolName != null) {
               if (map.enumMap[0][3] != null && map.enumMap[0][3] != "") {
                 this.addCDefine(map.enumMap[0][1], map.enumMap[0][0], map.enumMap[0][2], formatOptions);
               }
               code = code + ((code == "") ? "" : " | ") + map.enumMap[0][1];
             }
             else {
               if (map.enumMap[0][3] != null && map.enumMap[0][3] != "") {
                 code = code + ((code == "") ? "" : " | ") + map.enumMap[0][0];
               }
               else {
                 code = code + ((code == "") ? "" : " | ") + "0x" + BigNumber.toString(map.enumMap[0][1],16) + "U";
               }
             }
             comment = comment + ((comment == "") ? "" : ", ") + map.enumMap[0][2];
           }
           else {
             scriptApi.logError("[DEBUG] " + "enuMap for " + fieldID + " does not contain value 0."); 
             return null;
           }
         }
         if ("comment" in map) {
           comment = map.comment(cfg);
         }
         if (isNaN(code)) {
           code = "(" + code + ")";
         }
         return new FieldVal(code, comment);
       default:                         
         return null;                  
     }
  },
      
      // Data part of this SDKMapper object. "mapping" contains field ids of certain type and theirs settings.
      // Type of the id can be "number", "enum" or "multiEnum".
      // defineSymbol in an enumMap table or defineSymbol function in <id> object ensures definiton of symbolic constant
      // e.g. there is defined "#define MCG_IRCLK_DISABLE   0U" for [0, "MCG_IRCLK_DISABLE",      "MCGIRCLK disabled", "defineSymbol"],
      //      or "#define RTC_OSC_CAP_LOAD_30PF   0x3c00" for "rtc.oscCapLoad" id
      mapping : {     
         ///////////////////////// soscConfig ///////////////////////////////////                 
         "soscConfig.CLOCK_SetSysOscMonitorMode": {
              type: "enum",
              keyFunc: 
                 (function (cfg) {return (ScriptBitFields.getMultiShiftedBitFieldConfigValues(cfg, [["SCG0::SOSCCSR","SOSCCMRE"], ["SCG0::SOSCCSR","SOSCCM"]])) }),
              enumMap:[ 
                 [ScriptBitFields.getMultiShiftedValues([["SCG0::SOSCCSR", "SOSCCMRE"],["SCG0::SOSCCSR", "SOSCCM"]],[0,0]), "kSCG_SysOscMonitorDisable", "System OSC Clock Monitor is disabled"],
                 [ScriptBitFields.getMultiShiftedValues([["SCG0::SOSCCSR", "SOSCCMRE"],["SCG0::SOSCCSR", "SOSCCM"]],[0,1]), "kSCG_SysOscMonitorInt", "Clock Monitor generates interrupt when error detected"],
                 [ScriptBitFields.getMultiShiftedValues([["SCG0::SOSCCSR", "SOSCCMRE"],["SCG0::SOSCCSR", "SOSCCM"]],[1,0]), "kSCG_SysOscMonitorDisable",       "System OSC Clock Monitor is disabled"],
                 [ScriptBitFields.getMultiShiftedValues([["SCG0::SOSCCSR", "SOSCCMRE"],["SCG0::SOSCCSR", "SOSCCM"]],[1,1]), "kSCG_SysOscMonitorReset", "Clock Monitor generates reset when error detected"],
              ]
        },   

      ///////////////////////// fircConfig ///////////////////////////////////         
        "fircConfig.trimConfig": { //Trim config is not supported yet.
           type: "enum",   
           //keyFunc : (function (cfg) { return cfg.getBitFieldValueAsBigInteger("SCG0::FIRCCSR","FIRCTREN") }),
           keyFunc : (function (cfg) { return 0 }),
           enumMap:[ 
                 [0, "NULL", "Fast IRC Trim disabled"],                 
           ]  
        },
   
        "fircConfig.trimConfig.trimMode": {  // Only avaliable when SCG0::FIRCCSR[FIRCTREN] is set
               type: "enum",
               keyFunc: 
                  (function (cfg) {return  (ScriptBitFields.getMultiShiftedBitFieldConfigValues(cfg, [["SCG0::FIRCCSR","FIRCTRUP"], ["SCG0::FIRCCSR","FIRCTREN"]])) }),          
               enumMap: [     
                  [ScriptBitFields.getMultiShiftedValues([["SCG0::FIRCCSR","FIRCTRUP"],["SCG0::FIRCCSR","FIRCTREN"]],[0,1]), "kSCG_FircTrimNonUpdate", "FIRC trim is enabled but the trim value update is disabled"],          
                  [ScriptBitFields.getMultiShiftedValues([["SCG0::FIRCCSR","FIRCTRUP"],["SCG0::FIRCCSR","FIRCTREN"]],[1,1]), "kSCG_FircTrimUpdate", "FIRC trim is enabled and trim value update is enabled"],          
               ]
            },

        "fircConfig.trimConfig.trimSrc": { 
               type: "enum",
               keyFunc : (function (cfg) { return cfg.getBitFieldValueAsBigInteger("SCG0::FIRCTCFG", "TRIMSRC") }),
               enumMap:[    
                     [0, "kSCG_FircTrimSrcUsb0",   "USB0 start of frame (1kHz)"],                        
                     [2, "kSCG_FircTrimSrcSysOsc",   "Trim source is System OSC"],
                     [3, "kSCG_FircTrimSrcRtcOsc",   "Trim source is RTC OSC (32.768 kHz)"],        
               ]  
            },
        "fircConfig.trimConfig.trimDiv": {
               type: "number",   
               expr: (function (cfg) {
                      var tmp = cfg.getBitFieldValueAsBigInteger("SCG0::FIRCTCFG", "TRIMDIV");
                      return new FieldVal(tmp, "Divided by " + cfg.getValueAsText("SCG.FIRC_TRIMDIV.scale"));
                  })
            },
        "fircConfig.trimConfig.trimCoar": {           
               type: "number",   
               expr: (function (cfg) {
                        var tmp = 0;//cfg.getBitFieldValueAsBigInteger("SCG0::FIRCSTAT", "TRIMCOAR");
                        return new FieldVal(tmp, "Trim value, see Reference Manual for more information");
                      })   
            },
        "fircConfig.trimConfig.trimFine": {           
               type: "number",   
               expr: (function (cfg) {
                        var tmp = 0;//cfg.getBitFieldValueAsBigInteger("SCG0::FIRCSTAT", "TRIMFINE");
                        return new FieldVal(tmp, "Trim value, see Reference Manual for more information");
                      })   
            },
         ///////////////////////// Pll0Config ///////////////////////////////////
          "pll0Config.CLOCK_SetPll0MonitorMode": {
              type: "enum",
              keyFunc: 
                 (function (cfg) {return (ScriptBitFields.getMultiShiftedBitFieldConfigValues(cfg, [["SCG0::APLLCSR","APLLCMRE"], ["SCG0::APLLCSR","APLLCM"]])) }),
              enumMap:[ 
                 [ScriptBitFields.getMultiShiftedValues([["SCG0::APLLCSR", "APLLCMRE"],["SCG0::APLLCSR", "APLLCM"]],[0,0]), "kSCG_Pll0MonitorDisable", "Pll0 Monitor is disabled"],
                 [ScriptBitFields.getMultiShiftedValues([["SCG0::APLLCSR", "APLLCMRE"],["SCG0::APLLCSR", "APLLCM"]],[0,1]), "kSCG_Pll0MonitorInt", "Clock Monitor generates interrupt when error detected"],
                 [ScriptBitFields.getMultiShiftedValues([["SCG0::APLLCSR", "APLLCMRE"],["SCG0::APLLCSR", "APLLCM"]],[1,0]), "kSCG_Pll0MonitorDisable",       "Pll0 Monitor is disabled"],
                 [ScriptBitFields.getMultiShiftedValues([["SCG0::APLLCSR", "APLLCMRE"],["SCG0::APLLCSR", "APLLCM"]],[1,1]), "kSCG_Pll0MonitorReset", "Clock Monitor generates reset when error detected"],
              ]
            },                  
            "pll1Config.CLOCK_SetPll1MonitorMode": {
              type: "enum",
              keyFunc: 
                 (function (cfg) {return (ScriptBitFields.getMultiShiftedBitFieldConfigValues(cfg, [["SCG0::SPLLCSR","SPLLCMRE"], ["SCG0::SPLLCSR","SPLLCM"]])) }),
              enumMap:[ 
                 [ScriptBitFields.getMultiShiftedValues([["SCG0::SPLLCSR", "SPLLCMRE"],["SCG0::SPLLCSR", "SPLLCM"]],[0,0]), "kSCG_Pll1MonitorDisable", "Pll1 Monitor is disabled"],
                 [ScriptBitFields.getMultiShiftedValues([["SCG0::SPLLCSR", "SPLLCMRE"],["SCG0::SPLLCSR", "SPLLCM"]],[0,1]), "kSCG_Pll1MonitorInt", "Clock Monitor generates interrupt when error detected"],
                 [ScriptBitFields.getMultiShiftedValues([["SCG0::SPLLCSR", "SPLLCMRE"],["SCG0::SPLLCSR", "SPLLCM"]],[1,0]), "kSCG_Pll1MonitorDisable",       "Pll1 Monitor is disabled"],
                 [ScriptBitFields.getMultiShiftedValues([["SCG0::SPLLCSR", "SPLLCMRE"],["SCG0::SPLLCSR", "SPLLCM"]],[1,1]), "kSCG_Pll1MonitorReset", "Clock Monitor generates reset when error detected"],
              ]
            },                    
         ///////////////////////// sircConfig ///////////////////////////////////
         "sircConfig.trimConfig": { //Trim config is not supported yet.
          type: "enum",   
          //keyFunc : (function (cfg) { return cfg.getBitFieldValueAsBigInteger("SCG0::SIRCCSR","SIRCTREN") }),
          keyFunc : (function (cfg) { return 0 }),
          enumMap:[ 
                [0, "NULL", "Slow IRC Trim disabled"],                 
          ]  
       },
        "sircConfig.trimConfig.trimMode": {  // Only avaliable when SCG0::SIRCCSR[SIRCTREN] is set
                type: "enum",
                keyFunc: 
                  (function (cfg) {return  (ScriptBitFields.getMultiShiftedBitFieldConfigValues(cfg, [["SCG0::SIRCCSR","SIRCTRUP"], ["SCG0::SIRCCSR","SIRCTREN"]])) }),          
                enumMap: [
                  //[0, "SCG_LPFLL_TRIM_DISABLE", "SCG_LPFLL_TRIMUPDATE disabled", "defineSymbol"],          
                  [ScriptBitFields.getMultiShiftedValues([["SCG0::SIRCCSR","SIRCTRUP"],["SCG0::SIRCCSR","SIRCTREN"]],[0,1]), "kSCG_SircTrimNonUpdate", "SIRC trim is enabled but the trim value update is disabled"],          
                  [ScriptBitFields.getMultiShiftedValues([["SCG0::SIRCCSR","SIRCTRUP"],["SCG0::SIRCCSR","SIRCTREN"]],[1,1]), "kSCG_SircTrimUpdate", "SIRC trim is enabled and trim value update is enabled"],          
                ]
            },
        "sircConfig.trimConfig.trimSrc": { 
                type: "enum",
                keyFunc : (function (cfg) { return cfg.getBitFieldValueAsBigInteger("SCG0::SIRCTCFG", "TRIMSRC") }),
                enumMap:[         
                      [2, "kSCG_SircTrimSrcSysOsc",   "Trim source is System OSC"],
                      [3, "kSCG_SircTrimSrcRtcOsc",   "Trim source is RTC OSC (32.768 kHz)"],        
                ]  
            },
        "sircConfig.trimConfig.trimDiv": {
                type: "number",   
                expr: (function (cfg) {
                      var tmp = cfg.getBitFieldValueAsBigInteger("SCG0::SIRCTCFG", "TRIMDIV");
                      return new FieldVal(tmp, "Divided by " + cfg.getValueAsText("SCG.SIRC_TRIMDIV.scale"));
                  })
            },
        "sircConfig.trimConfig.cltrim": {           
                type: "number",   
                expr: (function (cfg) {
                        var tmp = 0;//cfg.getBitFieldValueAsBigInteger("SCG0::SIRCSTAT", "TRIMCOAR");
                        return new FieldVal(tmp, "Trim value, see Reference Manual for more information");
                      })   
            },
        "sircConfig.trimConfig.ccotrim": {           
                type: "number",   
                expr: (function (cfg) {
                        var tmp = 0;//cfg.getBitFieldValueAsBigInteger("SCG0::SIRCSTAT", "TRIMFINE");
                        return new FieldVal(tmp, "Trim value, see Reference Manual for more information");
                      })   
            },        

            ///////////////////////// osc32_RunConfig ///////////////////////////////////
         "rosc.CLOCK_SetRoscMonitorMode":{
              type: "enum",
              keyFunc: (function (cfg) { return (ScriptBitFields.getMultiShiftedBitFieldConfigValues(cfg, [["SCG0::ROSCCSR", "ROSCCMRE"],["SCG0::ROSCCSR", "ROSCCM"]])) }),
              enumMap:[ 
                 [ScriptBitFields.getMultiShiftedValues([["SCG0::ROSCCSR", "ROSCCMRE"],["SCG0::ROSCCSR", "ROSCCM"]],[0,0]), "kSCG_RoscMonitorDisable", "Monitor disabled"],       
                 [ScriptBitFields.getMultiShiftedValues([["SCG0::ROSCCSR", "ROSCCMRE"],["SCG0::ROSCCSR", "ROSCCM"]],[0,1]), "kSCG_RoscMonitorInt", "Interrupt when the rtc osc error is detected"],
                 [ScriptBitFields.getMultiShiftedValues([["SCG0::ROSCCSR", "ROSCCMRE"],["SCG0::ROSCCSR", "ROSCCM"]],[1,0]), "kSCG_RoscMonitorDisable",       "Monitor disabled"],          
                 [ScriptBitFields.getMultiShiftedValues([["SCG0::ROSCCSR", "ROSCCMRE"],["SCG0::ROSCCSR", "ROSCCM"]],[1,1]), "kSCG_RoscMonitorReset", "Reset when the rtc osc error is detected"],          
              ]     
         }, 
         
         "roscClock": {
           type: "number",
           expr: (function (cfg) {
                     var freq = cfg.getValueAsText("XTAL32K2_toWAKE_clock.outFreq");
                     if (freq != "N/A") {
                       freq = cfg.getValueAsBigInteger("XTAL32K2_toWAKE_clock.outFreq");
                     }
                     return new FieldVal(freq, "ROSC clock frequency: " + freq + "Hz");
                  })
        },
        "coreClock": {
          type: "number",   
          expr: (function (cfg) {                      
                    var freq = cfg.getValueAsText("System_clock.outFreq");
                    if (freq != "N/A") {                      
                      freq = cfg.getValueAsBigInteger("System_clock.outFreq"); 
                    }                      
                    return new FieldVal(freq, "Core clock frequency: " + freq + "Hz");  
                 })   
        },
        ///////////////////////// usbphy ///////////////////////////////////

        "usbphy.pfdFrac": {
          type: "number",   
          expr: (function (cfg) { 
                    var tmp = cfg.getBitFieldValue("USBPHY::PFDA", "PFD0_FRAC");
                    return new FieldVal(tmp, "PFD fractional divider: divided by " + cfg.getValueAsText("USB_PHY.PFD_FRAC_DIV.scale"));
                 }),   
          defineSymbol: (function(cfg, exprResult) { return ("USBPHY_PFD_FRAC_DIV_" + cfg.getValueAsText("USB_PHY.PFD_FRAC_DIV.scale")); }),
        },       

        "usbphy.pfdClkSel": {
             type: "enum",
             keyFunc : (function (cfg) { return cfg.getBitFieldValue("USBPHY::ANACTRL", "PFD_CLK_SEL") }),
             enumMap:[ 
                [0, "USBPHY_PFD_CLK_SEL_OSCERCLK_UNDIV_CLK", "PFD clock select: OSCERCLK_UNDIV clock", "defineSymbol"],
                [1, "USBPHY_PFD_CLK_SEL_PFD_CLK_DIV_4",      "PFD clock select: pfd_clk clock divided by 4", "defineSymbol"],
                [2, "USBPHY_PFD_CLK_SEL_PFD_CLK_DIV_2",      "PFD clock select: pfd_clk clock divided by 2", "defineSymbol"],
                [3, "USBPHY_PFD_CLK_SEL_PFD_CLK",            "PFD clock select: pfd_clk clock", "defineSymbol"],
             ]          
        }, 
        "usbphy.CLOCK_SetUpllMonitorMode": {
          type: "enum",
          keyFunc: 
            (function (cfg) {return (ScriptBitFields.getMultiShiftedBitFieldConfigValues(cfg, [["SCG0::UPLLCSR","UPLLCMRE"], ["SCG0::UPLLCSR","UPLLCM"]])) }),
          enumMap:[ 
            [ScriptBitFields.getMultiShiftedValues([["SCG0::UPLLCSR", "UPLLCMRE"],["SCG0::UPLLCSR", "UPLLCM"]],[0,0]), "kSCG_UpllMonitorDisable", "Upll Monitor is disabled"],
            [ScriptBitFields.getMultiShiftedValues([["SCG0::UPLLCSR", "UPLLCMRE"],["SCG0::UPLLCSR", "UPLLCM"]],[0,1]), "kSCG_UpllMonitorInt", "Clock Monitor generates interrupt when error detected"],
            [ScriptBitFields.getMultiShiftedValues([["SCG0::UPLLCSR", "UPLLCMRE"],["SCG0::UPLLCSR", "UPLLCM"]],[1,0]), "kSCG_UpllMonitorDisable",       "Upll Monitor is disabled"],
            [ScriptBitFields.getMultiShiftedValues([["SCG0::UPLLCSR", "UPLLCMRE"],["SCG0::UPLLCSR", "UPLLCM"]],[1,1]), "kSCG_UpllMonitorReset", "Clock Monitor generates reset when error detected"],
          ]
        }, 
        "RunPowerMode": {
        type: "enum",
        keyFunc: 
          (function (cfg) {return cfg.getValueAsText("RunPowerMode") }),
        enumMap:[ 
          ["MD", "kMD_Mode"],
          ["SD", "kSD_Mode"],
          ["OD", "kOD_Mode"],
        ]
        },
        "vbat.initTrim": {
             type: "enum",
             keyFunc : (function (cfg) { return cfg.getBitFieldValue("VBAT0::OSCCFGA", "INIT_TRIM") }),
             enumMap:[ 
                [0, "kVBAT_OscInitTrim8000ms", "OSC_32K Initialization trim - startup time 8 s"],
                [1, "kVBAT_OscInitTrim4000ms", "OSC_32K Initialization trim - startup time 4 s"],
                [2, "kVBAT_OscInitTrim2000ms", "OSC_32K Initialization trim - startup time 2 s"],
                [3, "kVBAT_OscInitTrim1000ms", "OSC_32K Initialization trim - startup time 1 s"],
                [4, "kVBAT_OscInitTrim500ms", "OSC_32K Initialization trim - startup time 0.5 s"],
                [5, "kVBAT_OscInitTrim250ms", "OSC_32K Initialization trim - startup time 0.25 s"],
                [6, "kVBAT_OscInitTrim125ms", "OSC_32K Initialization trim - startup time 0.125 s"],
                [7, "kVBAT_OscInitTrimHalfms", "OSC_32K Initialization trim - startup time 0.5 ms"]
             ]          
        },       
        "vbat.ampGain": {
             type: "enum",
             keyFunc : (function (cfg) { return cfg.getBitFieldValue("VBAT0::OSCCTLA", "COARSE_AMP_GAIN") }),
             enumMap:[ 
                [0, "kVBAT_OscCoarseAdjustment05", "OSC_32K Amplifier gain adjustment (gm) 0.5 uA/V"],
                [1, "kVBAT_OscCoarseAdjustment10", "OSC_32K Amplifier gain adjustment (gm) 1.0 uA/V"],
                [2, "kVBAT_OscCoarseAdjustment18", "OSC_32K Amplifier gain adjustment (gm) 1.8 uA/V"],
                [3, "kVBAT_OscCoarseAdjustment33", "OSC_32K Amplifier gain adjustment (gm) 3.3 uA/V"]
             ]          
        },
        "vbat.xtalCap": {
             type: "enum",
             keyFunc : (function (cfg) { return cfg.getBitFieldValue("VBAT0::OSCCTLA", "XTAL_CAP_SEL") }),
             enumMap:[ 
                [0, "kVBAT_OscXtal0pFCap", "OSC_32K internal capacitance for XTAL pin 0pF"],
                [1, "kVBAT_OscXtal2pFCap", "OSC_32K internal capacitance for XTAL pin 2pF"],
                [2, "kVBAT_OscXtal4pFCap", "OSC_32K internal capacitance for XTAL pin 4pF"],
                [3, "kVBAT_OscXtal6pFCap", "OSC_32K internal capacitance for XTAL pin 6pF"],
                [4, "kVBAT_OscXtal8pFCap", "OSC_32K internal capacitance for XTAL pin 8pF"],
                [5, "kVBAT_OscXtal10pFCap", "OSC_32K internal capacitance for XTAL pin 10pF"],
                [6, "kVBAT_OscXtal12pFCap", "OSC_32K internal capacitance for XTAL pin 12pF"],
                [7, "kVBAT_OscXtal14pFCap", "OSC_32K internal capacitance for XTAL pin 14pF"],
                [8, "kVBAT_OscXtal16pFCap", "OSC_32K internal capacitance for XTAL pin 16pF"],
                [9, "kVBAT_OscXtal18pFCap", "OSC_32K internal capacitance for XTAL pin 18pF"],
                [10, "kVBAT_OscXtal20pFCap", "OSC_32K internal capacitance for XTAL pin 20pF"],
                [11, "kVBAT_OscXtal22pFCap", "OSC_32K internal capacitance for XTAL pin 22pF"],
                [12, "kVBAT_OscXtal24pFCap", "OSC_32K internal capacitance for XTAL pin 24pF"],
                [13, "kVBAT_OscXtal26pFCap", "OSC_32K internal capacitance for XTAL pin 26pF"],
                [14, "kVBAT_OscXtal28pFCap", "OSC_32K internal capacitance for XTAL pin 28pF"],
                [15, "kVBAT_OscXtal30pFCap", "OSC_32K internal capacitance for XTAL pin 30pF"]
             ]          
        },
        "vbat.extalCap": {
             type: "enum",
             keyFunc : (function (cfg) { return cfg.getBitFieldValue("VBAT0::OSCCTLA", "EXTAL_CAP_SEL") }),
             enumMap:[ 
                [0, "kVBAT_OscExtal0pFCap", "OSC_32K internal capacitance for EXTAL pin 0pF"],
                [1, "kVBAT_OscExtal2pFCap", "OSC_32K internal capacitance for EXTAL pin 2pF"],
                [2, "kVBAT_OscExtal4pFCap", "OSC_32K internal capacitance for EXTAL pin 4pF"],
                [3, "kVBAT_OscExtal6pFCap", "OSC_32K internal capacitance for EXTAL pin 6pF"],
                [4, "kVBAT_OscExtal8pFCap", "OSC_32K internal capacitance for EXTAL pin 8pF"],
                [5, "kVBAT_OscExtal10pFCap", "OSC_32K internal capacitance for EXTAL pin 10pF"],
                [6, "kVBAT_OscExtal12pFCap", "OSC_32K internal capacitance for EXTAL pin 12pF"],
                [7, "kVBAT_OscExtal14pFCap", "OSC_32K internal capacitance for EXTAL pin 14pF"],
                [8, "kVBAT_OscExtal16pFCap", "OSC_32K internal capacitance for EXTAL pin 16pF"],
                [9, "kVBAT_OscExtal18pFCap", "OSC_32K internal capacitance for EXTAL pin 18pF"],
                [10, "kVBAT_OscExtal20pFCap", "OSC_32K internal capacitance for EXTAL pin 20pF"],
                [11, "kVBAT_OscExtal22pFCap", "OSC_32K internal capacitance for EXTAL pin 22pF"],
                [12, "kVBAT_OscExtal24pFCap", "OSC_32K internal capacitance for EXTAL pin 24pF"],
                [13, "kVBAT_OscExtal26pFCap", "OSC_32K internal capacitance for EXTAL pin 26pF"],
                [14, "kVBAT_OscExtal28pFCap", "OSC_32K internal capacitance for EXTAL pin 28pF"],
                [15, "kVBAT_OscExtal30pFCap", "OSC_32K internal capacitance for EXTAL pin 30pF"]
             ]          
        },
        "vbat.capTrim": {
             type: "enum",
             keyFunc : (function (cfg) { return cfg.getBitFieldValue("VBAT0::OSCCFGA", "CAP_TRIM") }),
             enumMap:[ 
                [0, "kVBAT_OscCapTrimDefault", "OSC_32K Capacitor Trim - Default"],
                [1, "kVBAT_OscCapTrim1us", "OSC_32K Capacitor Trim -1 us"],
                [2, "kVBAT_OscCapTrim2us", "OSC_32K Capacitor Trim -2 us or +3.5 us (when OSCCFGA[CAP2_TRIM] == 1)"],
                [3, "kVBAT_OscCapTrim2andhalfus", "OSC_32K Capacitor Trim -2.5 us or +1 us (when OSCCFGA[CAP2_TRIM] == 1)"]
             ]          
        },
        "vbat.dlyTrim": {
             type: "enum",
             keyFunc : (function (cfg) { return cfg.getBitFieldValue("VBAT0::OSCCFGA", "DLY_TRIM") }),
             enumMap:[ 
                [0, "kVBAT_OscDlyTrim0", "OSC_32K Delay Trim - P current 9 nA and N current 6 nA"],
                [1, "kVBAT_OscDlyTrim1", "OSC_32K Delay Trim - P current 13 nA and N current 6 nA"],
                [3, "kVBAT_OscDlyTrim3", "OSC_32K Delay Trim - P current 4 nA and N current 6 nA"],
                [4, "kVBAT_OscDlyTrim4", "OSC_32K Delay Trim - P current 9 nA and N current 4 nA"],
                [5, "kVBAT_OscDlyTrim5", "OSC_32K Delay Trim - P current 13 nA and N current 4 nA"],
                [6, "kVBAT_OscDlyTrim6", "OSC_32K Delay Trim - P current 4 nA and N current 4 nA"],
                [7, "kVBAT_OscDlyTrim7", "OSC_32K Delay Trim - P current 9 nA and N current 2 nA"],
                [8, "kVBAT_OscDlyTrim8", "OSC_32K Delay Trim - P current 13 nA and N current 2 nA"],
                [9, "kVBAT_OscDlyTrim9", "OSC_32K Delay Trim - P current 4 nA and N current 2 nA"]
             ]          
        },
        "vbat.cap2Trim": {
             type: "enum",
             keyFunc : (function (cfg) { return cfg.getBitFieldValue("VBAT0::OSCCFGA", "CAP2_TRIM") }),
             enumMap:[ 
                [0, "kVBAT_OscCap2Trim0", "OSC_32K CAP2_TRIM value 0"],
                [1, "kVBAT_OscCap2Trim1", "OSC_32K CAP2_TRIM value 1"]
             ]          
        },
        "vbat.cmpTrim": {
             type: "enum",
             keyFunc : (function (cfg) { return cfg.getBitFieldValue("VBAT0::OSCCFGA", "CMP_TRIM") }),
             enumMap:[ 
                [0, "kVBAT_OscCmpTrim760mv", "OSC_32K Comparator Trim - 760mV"],
                [1, "kVBAT_OscCmpTrim770mv", "OSC_32K Comparator Trim - 770mV"],
                [3, "kVBAT_OscCmpTrim740mv", "OSC_32K Comparator Trim - 740mV"]
             ]          
        },
        "vbat.mode": {
             type: "enum",
             keyFunc : (function (cfg) { return cfg.getBitFieldValue("VBAT0::OSCCTLA", "MODE_EN") }),
             enumMap:[ 
                [0, "kVBAT_OscNormalModeEnable", "OSC_32K Normal Mode"],
                [1, "kVBAT_OscStartupModeEnable", "OSC_32K Startup Mode"],
                [3, "kVBAT_OscLowpowerModeEnable", "OSC_32K Low Power Mode"]
             ]          
       }      
     } // mapping  
};    
