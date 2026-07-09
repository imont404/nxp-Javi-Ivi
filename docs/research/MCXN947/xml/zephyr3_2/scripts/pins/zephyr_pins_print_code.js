            /*
            * Copyright 2023 NXP
            * To be used with MCUXpresso Config Tools under its Software License Agreement.
            */
            //Generate Zephyr pinctrl dtsi file
            PExOut.setOutputFile(dtsFileName + dtsFileNamePostfix + "." + dtsFileNameExt);
            PExOut_gen(warning_message);
            if ((PExProject.getYamlState() != null) && generateExtendedHeader) {
                PExOut_gen('/*');
                PExOut_gen(PExProject.getYamlState());
                PExOut_gen(' */');
                PExOut_gen('');
            }
            PExOut_gen("#include " + pathDTSFileInclude);
            PExOut_gen('');

            PExOut_gen('&pinctrl {');
            PExOut_gen('');
            for (var cfgIndex = 0; cfgIndex < configurationStrategiesSorted.length; cfgIndex++) { // Pin configuration represented by tables in the UI of the tool
                currentComponent = configurationStrategiesSorted[cfgIndex].index;
                if (coreId == componentCoreIds[currentComponent]) {
                    configurationStrategy = configurationStrategiesSorted[cfgIndex].text;
                    let fgDescription = allComponents[currentComponent].getDescription();
                    let fgDescriptionLen = fgDescription.length;

                    if ((configurationStrategiesSorted[cfgIndex].yamlState != null) && generateExtendedHeader) {
                        PExOut_gen('/*');
                        PExOut_gen(configurationStrategiesSorted[cfgIndex].yamlState);
                        PExOut_gen(' */');
                        PExOut_gen('');

                        PExOut_gen('/*' + makeString('*', 117));
                        PExOut_gen(' *');
                        PExOut_gen(' * Configuration Name : ' + configurationStrategy);
                        printStringArray(allComponents[currentComponent].getDescription(), ' * ', 'Description   : ');
                        PExOut_gen(' *');
                        PExOut_gen(' * END ' + makeString('*', 112) + '/');
                    }
                    else {
                        if ((fgDescriptionLen > 1) || (fgDescription[0] != "")) {
                            let multiLineDescription =  (fgDescriptionLen > 1) || (commentLenghtDTS < (fgDescription[0].length + 10))
                            if  (multiLineDescription) {
                                for (let fgDescIndex = 0; fgDescIndex < fgDescription.length; fgDescIndex++) {
                                    fgDescription[fgDescIndex] = formatCommentZephyr(1,fgDescription[fgDescIndex],commentLenghtDTS,"TAB",true);
                                }

                                PExOut_gen(leftPadding(1) + "/*");
                                printStringArray(fgDescription, leftPadding(1) + ' * ',''); 
                                PExOut_gen(leftPadding(1) + " */");
                            }
                            else {
                                PExOut_gen(leftPadding(1) + "/* " + fgDescription[0] + " */");
                            }
                        }
                    }
                    if (registerDatabaseObject != null) {
                        if (allComponents[currentComponent].isErrorInProperty()) {
                            PExOut_gen('/* There are conflicts or other incorrect settings in the configuration, the code below is generated only for ');
                            PExOut_gen(' * those registers which are set correctly and without a conflict. Open this file in Pins Tool for more details.');
                            PExOut_gen(' */');
                            PExOut_gen('');
                        }
                    }

                    PExOut_gen(leftPadding(1) + configurationStrategy + ": " + configurationStrategy + ' {');
                    
                    printPinCfgGroups(currentComponent);
                    PExOut_gen(leftPadding(1) + '};');
                    PExOut_gen('');
                }
            }
            PExOut_gen('};');