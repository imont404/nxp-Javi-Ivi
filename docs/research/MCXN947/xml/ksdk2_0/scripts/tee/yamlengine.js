/*
 * Copyright 2026 NXP
 * To be used with MCUXpresso Config Tools under its Software License Agreement.
 */
YamlEngine = function() {
    // init
    var self = this;
    
    // generate
    self.generate = function(template, options){
        
        // Parse the template to extract the base configuration
        var lines = template.split('\n');
        var result = [];
        
        // Copy template header until trustZoneRecords section
        for (var i = 0; i < lines.length; i++) {
            if (lines[i].trim().startsWith('family:')) {
                mcuInfo = scriptApi.getProfile().getMcuInfo();
                result.push("family: " + mcuInfo.getPartNumber().toLowerCase());
            } else if (lines[i].trim().startsWith('trustZoneRecords:')) {
                result.push(lines[i]);
                break;
            } else {
                result.push(lines[i]);
            }
        }
        
        // Generate trustZoneRecords entries from options
        for (var register in options) {
                var config = options[register];
                result.push('  -');
                result.push('    # ' + (config.registerId));
                result.push('    # --------------------------------===== Memory or Register Address [Required] =====--------------------------------');
                result.push('    address: \'0x' + (config.registerAddress) + '\'');
                result.push('    # --------------------------===== Mask for register write/read operation [Optional] =====---------------------------');
                result.push('    mask: \'' + (config.mask) + '\'');
                result.push('    # --------------------------===== Value for register write/read operation [Required] =====---------------------------');
                result.push('    value: \'' + (config.hexVal) + '\'');
                result.push('    # ----------------------------------===== Skip Readback Operation [Optional] =====----------------------------------');
                result.push('    skip_readback: ' + (config.skipReadback));
        }
        return result.join('\n');
    };
};