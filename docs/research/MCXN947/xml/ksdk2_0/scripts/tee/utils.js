/*
 * Copyright 2020-2023 NXP
 * To be used with MCUXpresso Config Tools under its Software License Agreement.
 */

// This object opens a file and writes into the file
OutputFile = function(fileName, coreId) {
    var self = this;

    // name of the generated file
    self.fileName = fileName;

    // output stream
    self.output = scriptApi.createFile(fileName, coreId);

    // write file content and 'close' file object
    self.close = function(content) {
        self.output.write(content);
        self.fileName = "";
        self.output = null;
    };
};

// Object contains frequently used helper functions for processing arrays, strings and comments
Utils = function() {
    var self = this;

    /** Prefix for region define (region start address, end address, etc.) */
    self.REGION_DEFINE_PREFIX = "REGION_"; //$NON-NLS-1$
    /** Suffix for region start address */
    self.REGION_DEFINE_BASE_SUFFIX = "_BASE"; //$NON-NLS-1$
    /** Suffix for region end address */
    self.REGION_DEFINE_END_SUFFIX = "_END"; //$NON-NLS-1$

    // Regex for matching empty strings or strings containing non-printable characters only
    self.emptyStringRegex = new RegExp(/^\s*$/);
    //
    self.emptyLinesRegex = new RegExp(/^\n*$/);

    /**
     * Create define name.
     * @param {String} prefix prefix used at the beginning of the generated define. Underscore is added automatically to given prefix
     * @param {int} index used in the middle of the define
     * @param {String} suffix suffix added behind index when creating a define
     * @returns {String} define name
     */
    self.createDefineName = function (prefix, index, suffix) {
        var withoutPrefix = self.REGION_DEFINE_PREFIX + index + suffix;
        var result = withoutPrefix;
        if (prefix) {
            result = prefix + "_" + withoutPrefix;
        }
        return result;
    };

    /**
     * Create define name with self.REGION_DEFINE_BASE_SUFFIX suffix.
     * @param {String} prefix prefix used at the beginning of the generated define. Underscore is added automatically to given prefix
     * @param {int} index used in the middle of the define
     * @returns {String} define name
     */
    self.createDefineNameBase = function (prefix, index) {
        return self.createDefineName(prefix, index, self.REGION_DEFINE_BASE_SUFFIX);
    };

    /**
     * Create define name with self.REGION_DEFINE_END_SUFFIX suffix.
     * @param {String} prefix prefix used at the beginning of the generated define. Underscore is added automatically to given prefix
     * @param {int} index used in the middle of the define
     * @returns {String} define name
     */
    self.createDefineNameEnd = function (prefix, index) {
        return self.createDefineName(prefix, index, self.REGION_DEFINE_END_SUFFIX);
    };

     /**
     * Generate define symbols
     * @param {Array} regionConfigs configuration of reagions for which to generate defines
     * @param {String} prefix prefix used at the beginning of the generated define. Underscore is added automatically to given prefix
     * @param {int} addrBitWidth address bit width
     * @param {String} comment comment generated before define
     */
    self.generateDefines = function (regionConfigs, prefix, addrBitWidth, comment) {
        var resultLines = [];
        if (regionConfigs.length > 0) {
            resultLines = [comment];
            regionConfigs.forEach(function (regionConfig) {
                var regionNum = regionConfig.getIndex();
                var startAddr = regionConfig.getStartAddr();
                resultLines.push(self.generateDefine(self.createDefineNameBase(prefix, regionNum), javaUtils.formatHexValue(startAddr, addrBitWidth)));
                var endAddr = regionConfig.getEndAddr();
                resultLines.push(self.generateDefine(self.createDefineNameEnd(prefix, regionNum), javaUtils.formatHexValue(endAddr, addrBitWidth)));
            });
        }
        return resultLines;
    };

    // filter problematic regions
    self.filterProblematicRegions = function (memRegionConfig) {
        return !sharedResources.profile.hasError(memRegionConfig);
    };

    // filter disabled configs
    self.filterDisabledRegions = function (memRegionConfig) {
        return memRegionConfig.isEnabled();
    };

    // create assign macro or direct register assigned value code
    self.createAssign = function (peripheral, register, assignValue) {
        var assignMacro = "";
        //
        if (sharedResources.isEnabledSecurityCodeHardening()) {
            // generate internal TEE_WRITE_REGISTER assign macro
            assignMacro += TEE_WR_REG_MACRO + "(&(" + peripheral + "->" + register + "), " + assignValue + ");";
        }
        else {
            // or use TEE tool API for given register value assign
            assignMacro += javaUtils.createAssign(peripheral, register, assignValue, false);
        }
        return assignMacro;
    };

    // create assign value
    self.createAssignValue = function (assignValue) {
        return javaUtils.createAssignValue(assignValue, 4);
    }


    /**
     * Generate macro with the given name and value
     * @param name
     * @param value
     * @return generated code
     * @fixme FIXME TEE team v99 consider to generate value in parenthesis, e.g. `#define MACRO (5U)` (applies to all tools) - discuss with P.H.
     */
    self.generateDefine = function (name, value) {
        return "#define " + name + " " + value; //$NON-NLS-1$
    };

    // function creates a string consisting of characters defined by first argument with length defined by second argument (e. g. utils.makeString('*', 5) will return '*****')
    self.makeString = function(character, count) {
        return Array(count + 1).join(character);
    };

    // function determines, if input string defined by argument contains no printable character (return value will be true for strings "", "  ", "  \n  " etc...)
    self.isEmpty = function(str) {
        return self.emptyStringRegex.test(str);
    };

    // function determines, if input string defined by argument contains at least one printable character (return value will be true for strings "f", "  f", "f  \n  " etc...)
    self.notEmpty = function(str) {
        return !self.isEmpty(str);
    };

    // function trims newlines from the beginning and the end of string
    self.trimNewlines = function(str) {
        return str.replace(/^\n*|\n*$/g, '');
    };

    // function creates and returns usual comment like '/* comment */'
    self.makeComment = function(content) {
        return '/* ' + content + ' */';
    };

    // function creates and returns initialization comment from array of strings defined by content in argument
    self.makeInitComment = function(content) {
        if (self.isEmpty(content)) {
            return "";
        }
        return (
            "/" +
            self.makeString("*", 119) +
            [""].concat(content).join("\n * ") +
            "\n " +
            self.makeString("*", 118) +
            "/"
        );
    };

    // function creates and returns functional group comment from array of strings defined by content and description in arguments
    self.makeFuncComment = function(content, description) {
        if (self.isEmpty(content)) {
            return "";
        }
        return (
            "\n/* FUNCTION " +
            self.makeString("*", 108) +
            "\n *" +
            [""].concat("Function Name : " + content).join("\n * ") +
            [""].concat("Description   : ").join("\n * ") +
            (self.notEmpty(description) ? [""].concat("\n" + self.splitLines(description).map(function (line) {return " * " + line;}).join("\n")).join("\n * ") : "") +
            "\n *\n * END " + 
            self.makeString("*", 112) +
            "/\n"
        );
    };

    // function creates array of strings split by new-line characters
    self.splitLines = function(str) {
        return str.split(/[\r\n]+/);
    };

    // function removes duplicit lines from string defined by argument
    self.removeDuplicitLines = function(str) {
        var linesArray = str.split("\n");
        var uniqueArray = linesArray.filter(function(item, pos) {
            return linesArray.indexOf(item) == pos;
        });
        return uniqueArray.join("\n");
    };

    // function will return an array containing values of object defined by argument (converts associative array to array indexed by numbers)
    self.objectValues = function(o) {
        var keys = Object.keys(o);
        values = [];
        for (var k in keys) {
            values.push(o[keys[k]]);
        }
        return values;
    };

    // function returns indentation string (spaces) of level defined by argument, tab width is set to 2 spaces by default
    // NOTE: IOutWriter that writes content to java supports writeIndented(String, indent) - could be used instead of this impractical indenting
    self.indent = function(level) {
        var tab = "  ";
        return self.makeString(tab, level);
    };

    // function renders section of generated code consisting of a comment in header of section and its content (C source code - second argument)
    self.renderSection = function(content, sectionName) {
        if (self.isEmpty(content)) {
            return "";
        }

        if (self.isEmpty(sectionName)) {
            return content;
        }

        initComment = self.makeInitComment([sectionName]);
        return [initComment, content].join("\n");
    };

    // function wraps input string into clang-off and clang-on directives in order to ignore the fragment code by clang formatter (used for YAML sections)
    self.clangOff = function(str) {
        return self.notEmpty(str)
            ? [self.makeComment('clang-format off'), str, self.makeComment('clang-format on')].join("\n")
            : "";
    };

    // adds empty string into array of strings
    self.addEmptyLines = function(array, numberOfNewLines) {
        var i = 0;
        for (i = 0; i < numberOfNewLines; i++) {
            array.push("");
        }
    };

    /* add all items from array given in 2nd argument to a collection
     if item to add is not an array, item is added directly */
    self.addAll = function(collection, collectionToAdd) {
        if (Array.isArray(collectionToAdd)) {
            if (collectionToAdd.length != 0) collection.push.apply(collection, collectionToAdd);
        } else {
            if (utils.notEmpty(collectionToAdd)) collection.push(collectionToAdd);
        }
    };
};

SharedResources = function() {
    var self = this;
    // global profile object
    self.profile = scriptApi.getProfile();

    // generate modified registers only
    self.generateModifiedRegistersOnly = self.profile.isGenerateCodeModifiedRegistersOnly();

    // generate security hardening code
    self.generateCodeSecurityHardening = false;

    // generate code with security hardening macros in output
    self.enableSecurityCodeHardening = function (useMacros) {
        self.generateCodeSecurityHardening = useMacros;
    };

    // get security code hardening enablement status
    self.isEnabledSecurityCodeHardening =  function() {
        return self.generateCodeSecurityHardening;
    };

    // get list of cores information in JSON
    self.coresJsons = JSON.parse(self.profile.getMcuInfo().getCoresList());

    // coresList is an array containing list of coreIds of each MCU cores, e. g.: ['core0', 'core1']
    self.coresIds = Object.keys(self.coresJsons);

    // get global yaml for current TEE configuration
    self.getGlobalYaml = function() {
        globalYaml = self.profile.getYaml();     // [MCUCM-9744] call just once to improve script responsiveness within TEE tool
        return globalYaml ? "\n"+utils.clangOff(globalYaml)+"\n" : "";
    };

    // get yaml for TEE configuration of given function
    self.getYamlForFuncGrp = function(funcGrp) {
        funcGrpYaml = funcGrp.getYaml();         // [MCUCM-9744] call just once to improve script responsiveness within TEE tool
        return utils.notEmpty(funcGrpYaml) ? utils.clangOff(funcGrpYaml) : "";
    };

    // string containing C comment with initial notification about manual modification of generated files (will be generated at the top of each generated files)
    self.notification = utils.makeInitComment([
        "This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file",
        "will be overwritten if the respective MCUXpresso Config Tools is used to update this file."
    ]);

    self.forceMemoryWrites = [utils.makeComment("Force memory writes before continuing"),
    "__DSB();",
    utils.makeComment("Flush and refill pipeline with updated permissions"),
    "__ISB();"];
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
        return value + "U";
    };
};

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
        return value + "U";
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
        return value + "U";
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