/*
* Copyright 2024-2025 NXP
* To be used with MCUXpresso Config Tools under its Software License Agreement.
*/
// Main part of generated files name
var dtsFileName = scriptApi.getMexFileName();
var dtsFileNamePostfix = "-pinctrl";
var dtsFileNameExt = "dtsi";
// DST include file path 
var pathDTSFileInclude = "<nxp/mcx/" + cpuVariant.toUpperCase() + "-pinctrl.h>"

// Length of the comment in dtsi configuration
var commentLenghtDTS = 80;

// Get left padding of the string
function leftPadding(level) {
    return "	".repeat(level)
}

// Pin configuration groups for each functional group
var groupPinCfgList = new Object();
//Port prefix string
var portPrefixStr = '_PIO'

// Pin object prototype
function Pin (pinItems,index) {
    // Get pin signal item
    let pinSignalItem = pinItems.findItemBySymbol("pin_signal" + index);
    // Get sequence names
    let confSeqNames = getConfigurationSequenceNames(pinSignalItem);

    // Get pin port and pin nr 
    this.pinPortNumber = getPortFromPinName(getPortString(pinSignalItem));
    // Get pin part name
    let pinNamePart = pinSignalItem.getSelectedPinNamePartOnly();

    this.isPortPin = function() {
        return (this.pinPortNumber != null)
    };

    if (this.isPortPin()) {
        this.port = ((this.pinPortNumber != null) ? this.pinPortNumber[0].replace("PORT","") : null);
        this.pinNr = ((this.pinPortNumber != null) ? this.pinPortNumber[1] : null);
        // Create full signal ID
        let peripheralItem = pinItems.findItemBySymbol("peripheral" + index);
        let signalItem = pinItems.findItemBySymbol("signal" + index);
        let fullSignalId = peripheralItem.getTextValue() + "_" + signalItem.getTextValue();
        // Get pin configuration Zepthyr ID
        this.pinCfgId = getPinCfgZephyrId(this.port, this.pinNr, pinNamePart);
        this.hasPinCfgID = function() {
            return (this.pinCfgId != null)
        };

        if (this.hasPinCfgID()) {
            //Create register list
            this.registerList = getRegistersListFromItem(pinSignalItem);
            // Drive strength
            let driveStrengthItem = pinItems.findItemBySymbol("drive_strength" + index);
            if (driveStrengthItem != null) {
                if (getItemAutoSelTextValue(driveStrengthItem)) {
                    this.driveStrength = getItemAutoSelTextValue(driveStrengthItem);
                    this.registerList = mergeRegisterLists(this.registerList,getRegistersListFromItem(driveStrengthItem));
                }
                else {
                    this.driveStrength = "low";
                }
            }
            // Open drain
            let openDrainItem = pinItems.findItemBySymbol("open_drain" + index);
            if ((openDrainItem != null) && getItemTextValue(openDrainItem)) {
                this.driveOpenDrain = getItemTextValue(openDrainItem) == "enable";
                this.registerList = mergeRegisterLists(this.registerList,getRegistersListFromItem(openDrainItem));
            }
            // Pull setting
            let pullEnableItem = pinItems.findItemBySymbol("pull_enable" + index);
            if ((pullEnableItem != null) && getItemTextValue(pullEnableItem)) {
                this.registerList = mergeRegisterLists(this.registerList,getRegistersListFromItem(pullEnableItem));                
                let pullSelectItem = pinItems.findItemBySymbol("pull_select" + index);
                if ((pullSelectItem != null) && getItemTextValue(pullSelectItem)) {
                    this.registerList = mergeRegisterLists(this.registerList,getRegistersListFromItem(pullSelectItem));
                    this.biasPullUp = (getItemTextValue(pullEnableItem) == "enable") && (getItemTextValue(pullSelectItem) == "up");
                    this.biasPullDown = (getItemTextValue(pullEnableItem) == "enable") && (getItemTextValue(pullSelectItem) != "up");
                }
            }
            
            //Slew rate
            let slewRateItem = pinItems.findItemBySymbol("slew_rate" + index);
            if (slewRateItem != null) {
                if (getItemAutoSelTextValue(slewRateItem)) {
                    this.slewRate = getItemAutoSelTextValue(slewRateItem);
                    this.registerList = mergeRegisterLists(this.registerList,getRegistersListFromItem(slewRateItem));
                }
                else {
                    this.slewRate = "fast";
                }
            }

            //Passive filter
            let passiveFilterItem = pinItems.findItemBySymbol("passive_filter" + index);
            if ((passiveFilterItem != null) && getItemTextValue(passiveFilterItem)) {
                this.nxpPassiveFilter = getItemTextValue(passiveFilterItem) == "enable";
                this.registerList = mergeRegisterLists(this.registerList,getRegistersListFromItem(passiveFilterItem));
            }

            //Passive filter
            let inputBufferItem = pinItems.findItemBySymbol("input_buffer" + index);
            if ((inputBufferItem != null) && getItemAutoSelTextValue(inputBufferItem)) {
                this.inputEnable = getItemAutoSelTextValue(inputBufferItem) == "enable";
                this.registerList = mergeRegisterLists(this.registerList,getRegistersListFromItem(inputBufferItem));
            }

            //Filter other registers from register list than port control registers (Zephyr set only port control registers)
            for (let registerInd in this.registerList) {
                if (registerInd.indexOf(portControlRegisterName) < 0) {
                    delete(this.registerList[registerInd]);
                } 
            }
        }
    }

    this.isPinCfgAvailable = function () {
        return (this.isPortPin() && this.hasPinCfgID())
    };
    
    // Get pin configuration id
    this.getPinCgfId = function(){
        return (this.isPinCfgAvailable() ? this.pinCfgId : null)
    };

    // Get pin options
    this.getPinOptions = function(){
        let pinOptions = {};
        if (this.isPinCfgAvailable()) {
            if (this.driveStrength) {
                pinOptions["drive-strength"] = this.driveStrength;
            }
            if (this.driveOpenDrain) {
                pinOptions["drive-open-drain"] = true;
            }
            if (this.biasPullUp){
                pinOptions["bias-pull-up"] = true;
            }
            if (this.biasPullDown){
                pinOptions["bias-pull-down"] = true;
            }
            if (this.slewRate) {
                pinOptions["slew-rate"] = this.slewRate;
            }
            if (this.nxpPassiveFilter){
                pinOptions["nxp,passive-filter"] = true;
            }
            if (this.inputEnable){
                pinOptions["input-enable"] = true;
            }
        }
        else {
            pinOptions = null;
        }
        return pinOptions;
    };

    // Get register list
    this.getRegisterList = function(){
        return (this.isPinCfgAvailable() ? this.registerList : null)
    };
}

function Group(pinCfgId, pinOptions, registerList) {
    this.pinCfgIds = new Array();
    this.pinCfgIds.push(pinCfgId);
    this.groupPinOptions = pinOptions;
    this.groupRegisterList = registerList;
    this.addPinCfgId = function(pinCfgId) {
        if ((typeof this.pinCfgIds === "undefined") || (this.pinCfgIds == null)){
            this.pinCfgIds = new Array(); 
        }
        this.pinCfgIds.push(pinCfgId);
    };
    this.addRegistersList= function(registerList) {
        if ((typeof this.groupRegisterList === "undefined") || (this.groupRegisterList == null)){
            this.groupRegisterList = registerList;  
        } else {
            this.groupRegisterList = mergeRegisterLists(registerList, this.groupRegisterList);
        }
    };
    this.getPinCfgIds = function() {
        return (((typeof this.groupRegisterList !== "undefined") && (this.groupRegisterList != null)) ? this.pinCfgIds.sort() : null)
    };
    this.getGroupPinOptions = function(){
        return (((typeof this.groupPinOptions !== "undefined") && (this.groupPinOptions != null)) ? this.groupPinOptions : null)
    };
    this.getGroupRegisterList = function(){
        return (((typeof this.groupRegisterList !== "undefined") && (this.groupRegisterList != null)) ? this.groupRegisterList : null)
    };
    this.getPinCfgIdsText = function() {
        let  pinCfgIdsText = "";
        if ((typeof this.pinCfgIds !== "undefined") && (this.pinCfgIds != null)) {
            pinCfgIdsText = leftPadding(3) + "pinmux = <";
            for (let pinCfgIdIndex = 0; pinCfgIdIndex < this.pinCfgIds.length; pinCfgIdIndex++) {
                pinCfgIdsText += this.pinCfgIds[pinCfgIdIndex] + ((pinCfgIdIndex < (this.pinCfgIds.length - 1)) ? ">,\n" + leftPadding(4) + "<" : ">;");
            }
        }     
        return pinCfgIdsText
    };
    this.getGroupPinOptionsText = function() {
        let optionsText = ""
        if ((typeof this.groupPinOptions !== "undefined") && (this.groupPinOptions != null)) {
            for (let optionParameter in this.groupPinOptions) {
                if (this.groupPinOptions[optionParameter] !== null) {
                    optionsText += ((optionsText == "" ) ? "" : "\n") + leftPadding(3) +  
                        optionParameter +
                        ((typeof this.groupPinOptions[optionParameter] !== "boolean") ? 
                        ' = "' + this.groupPinOptions[optionParameter] + '"'
                            :
                        "")     
                    +   ";"
                }
            }
        }
        return optionsText
    }
}

/**
 * Get pin configuration (mux) Zephyr ID
 */
function getPinCfgZephyrId(port, pinNr, pinNamePart) {
    if ((port != null) && (pinNr != null) && (pinNamePart != null)){
        let tempStr = (pinNamePart + ((pinNamePart.match(/((^PT[A-Z]\d+)|(^P\d+_\d+)|(^PIO\d+_\d+))/) != null) ? "" : portPrefixStr + port + (port.match(/\d+/) ? "_": "") + pinNr));
        tempStr = ((pinNamePart.match(/^SMARTDMA_PIO.*/) != null) ? "N9X_MUX('" + port + "'," + pinNr + ",7)" : tempStr);
        return tempStr;
    }
    return null;
}

/**
 * Get configuration sequence name
 */
function getConfigurationSequenceNames(item){
    //Configuration sequence
    let seq = item.getRegisterConfigurationSequence();
    let seqNames = new Array();
    for (let seqInd = 0; seqInd < seq.length; seqInd++) {
        seqNames.push(seq[seqInd].getName());
    }
    return seqNames
}

/**
 * Get registers list from the pin configuration item
 */
function getRegistersListFromItem(item){
    let registerList = new Object();
    //Configuration sequence
    let seq = item.getRegisterConfigurationSequence();
    for (let seqInd = 0; seqInd < seq.length; seqInd++) {
        // Registers
        let regModifs = seq[seqInd].getRegistersConfigurations()
        for (let regInd = 0; regInd < regModifs.length; regInd++) {
            let regItem = regModifs[regInd];
            // Get Register name and mask values
            let regFullNameItem = regItem.getRegisterName();
            let clrRegMaskItem = regItem.getClrRegMask();
            let setRegMaskItem = regItem.getSetRegMask();
            if (registerList[regFullNameItem] == null) {
                registerList[regFullNameItem] = new Object();
                registerList[regFullNameItem].registerName = regFullNameItem;
                registerList[regFullNameItem].registerClrMask = clrRegMaskItem;
                registerList[regFullNameItem].registerSetMask = setRegMaskItem;
            }
            else {
                registerList[regFullNameItem].registerClrMask = getBitwiseOr(clrRegMaskItem, registerList[regFullNameItem].registerClrMask);
                registerList[regFullNameItem].registerSetMask = getBitwiseOr(setRegMaskItem, registerList[regFullNameItem].registerSetMask);
            }
        } 
    }
    return registerList
}

/**
 * Merge register lists
 */
function mergeRegisterLists(list1,list2) {
   for (let regIndex in list2) {
        let registerNameList2 = list2[regIndex].registerName;
        if (list1[registerNameList2] == null) {
            list1[registerNameList2] = new Object();
            list1[registerNameList2].registerName = registerNameList2;
            list1[registerNameList2].registerClrMask = list2[regIndex].registerClrMask;
            list1[registerNameList2].registerSetMask = list2[regIndex].registerSetMask;
        }
        else {
            list1[registerNameList2].registerClrMask = getBitwiseOr(list2[regIndex].registerClrMask, list1[registerNameList2].registerClrMask);
            list1[registerNameList2].registerSetMask = getBitwiseOr(list2[regIndex].registerSetMask, list1[registerNameList2].registerSetMask);
        }
    }
    return list1;
}

/**
 * Get property item text value 
 */
function getItemTextValue(item) {
    if (item != null) {
        return (((item.getTextValue() == "_n_a_") || (item.getTextValue() == "no_init") || item.isValueAutoSelected()) ? null : item.getTextValue()) ;
    }
    return null;
}

/**
 * Get property item text value Auto Selected
 */
function getItemAutoSelTextValue(item) {
    if (item != null) {
        return (((item.getTextValue() == "_n_a_") || (item.getTextValue() == "no_init"))? null : item.getTextValue()) ;
    }
    return null;
}

/** 
 * Function gets block comment as an argument and splits the comment to multiple lines
 * with indentation not longer than 115 characters
 */
function formatCommentZephyr(indentation, comment, maxLineLength, indentationType, withoutFirstIndent) {
    var formattedComment = (withoutFirstIndent ? "" : (indentationType == "TAB" ? leftPadding(indentation) : makeString(' ', indentation)));
    var lineLen = (indentationType == "TAB" ? indentation * 4 : indentation) + 3
    var words = comment.split(' ');

    for (var i = 0; i < words.length; i++) {
        if (words[i].length > 0) {
            let  test = lineLen + 1 + words[i].length
            if ((lineLen + 1 + words[i].length) > maxLineLength) {
                formattedComment += '\n' + (indentationType == "TAB" ? leftPadding(indentation) : makeString(' ', indentation)) + ' * ';
                lineLen = (indentationType == "TAB" ? indentation * 4 : indentation) + 3;
            }
            formattedComment += words[i] + ' ';
            lineLen += words[i].length + 1;
        } 
       
    }
    return formattedComment;
}

/**
 * Enrol Zepthyr dts pin configuration register. They also mask registerClrMask and registerSetMask accordingly.
 * pc - index of pin configuration (routing function/Pins component)
 * ccd - index of configuration constant definition
 */
function enrolDTSPinCfg(pc, ccd) {
    var configurationRegisterList = registerList[pc];
    var pin_listProperty = allComponents[pc].findItemBySymbol("pin_list");
    groupPinCfgList[pc] = new Array();

    var numOfItems = pin_listProperty.getItemsCount();
    for (let i = 0; i < numOfItems; i++) {
        let pinCfgZephyr = new Pin(allComponents[pc], i);
        let groupMatch = false;
        let pinCfgId = pinCfgZephyr.getPinCgfId();
        let pinOptions = pinCfgZephyr.getPinOptions();
        let pinRegisterList = pinCfgZephyr.getRegisterList();
        if (pinCfgId != null) {
            if (groupPinCfgList[pc].length == 0) {
                groupPinCfgList[pc].push(new Group(pinCfgId, pinOptions, pinRegisterList));
            } else {
                for (let j = 0; j <  groupPinCfgList[pc].length; j++ ) {
                    if (JSON.stringify(groupPinCfgList[pc][j].getGroupPinOptions()) === JSON.stringify(pinOptions)) {
                        groupMatch = true;
                        groupPinCfgList[pc][j].addPinCfgId(pinCfgId);
                        groupPinCfgList[pc][j].addRegistersList(pinRegisterList);
                        break;
                    }
                }
                if (groupMatch == false) {
                    groupPinCfgList[pc].push(new Group(pinCfgId, pinOptions, pinRegisterList));
                }
            }
        }
    }
    if (projectType == "Zephyr") {
        for (let j = 0; j <  groupPinCfgList[pc].length; j++ ) {
            if (groupPinCfgList[pc][j].getGroupRegisterList != null) {
                let registerListCfg = groupPinCfgList[pc][j].getGroupRegisterList();
                for (let registerName in registerListCfg) {
                    let registerCfg = registerListCfg[registerName];
                    if (configurationRegisterList[registerName] != null) {
                        configurationRegisterList[registerName].registerClrMask = getBitwiseAnd(configurationRegisterList[registerName].registerClrMask ,
                                                                                    getBitwiseNeg32(registerCfg.registerClrMask));
                        configurationRegisterList[registerName].registerSetMask = getBitwiseAnd(configurationRegisterList[registerName].registerSetMask ,
                                                                                    getBitwiseNeg32(registerCfg.registerSetMask));
                    }
                    if ((configurationRegisterList[registerName] != null) && 
                        (configurationRegisterList[registerName].registerClrMask == 0) &&
                        (configurationRegisterList[registerName].registerSetMask == 0)) {
                        //Delete register configuration
                        delete(configurationRegisterList[registerName]);
                    }
                }
            }
        }
    }
}

/**
 * Print pin configuration groups
 * pc - index of pin configuration (routing function/Pins component)
 */
function printPinCfgGroups(pc) {
    let groupPinCfg = groupPinCfgList[pc];
    for (let groupIndex = 0; groupIndex <  groupPinCfg.length; groupIndex++ ) {
        PExOut_gen(leftPadding(2) + "group" + groupIndex + " {");
        PExOut_gen(groupPinCfg[groupIndex].getPinCfgIdsText());
        PExOut_gen(groupPinCfg[groupIndex].getGroupPinOptionsText());
        PExOut_gen(leftPadding(2) + "};");
    }
}