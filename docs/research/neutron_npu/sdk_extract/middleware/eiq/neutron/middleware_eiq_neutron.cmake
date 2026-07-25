# Add set(CONFIG_USE_middleware_eiq_neutron true) in config.cmake to use this component

include_guard(GLOBAL)
message("${CMAKE_CURRENT_LIST_FILE} component is included.")

      target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
          ${CMAKE_CURRENT_LIST_DIR}/common/include
          ${CMAKE_CURRENT_LIST_DIR}/driver/include
        )

    if(CONFIG_USE_COMPONENT_CONFIGURATION)
  message("===>Import configuration from ${CMAKE_CURRENT_LIST_FILE}")

  
            if(CONFIG_TOOLCHAIN STREQUAL iar)
      target_compile_options(${MCUX_SDK_PROJECT_NAME} PUBLIC
            )
      endif()
      
  endif()

      if((CONFIG_TOOLCHAIN STREQUAL mcux OR CONFIG_TOOLCHAIN STREQUAL mdk OR CONFIG_TOOLCHAIN STREQUAL iar) AND (CONFIG_DEVICE_ID STREQUAL MCXN947 OR CONFIG_DEVICE_ID STREQUAL MCXN547))
    target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE
    -Wl,--start-group
          ${CMAKE_CURRENT_LIST_DIR}/mcxn/libNeutronDriver.a
          ${CMAKE_CURRENT_LIST_DIR}/mcxn/libNeutronFirmware.a
        -Wl,--end-group
    )
    endif()

        if((CONFIG_TOOLCHAIN STREQUAL mcux OR CONFIG_TOOLCHAIN STREQUAL mdk OR CONFIG_TOOLCHAIN STREQUAL iar) AND CONFIG_DEVICE_ID STREQUAL MIMXRT798S)
    target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE
    -Wl,--start-group
          ${CMAKE_CURRENT_LIST_DIR}/rt700/cm33/libNeutronDriver.a
          ${CMAKE_CURRENT_LIST_DIR}/rt700/cm33/libNeutronFirmware.a
        -Wl,--end-group
    )
    endif()

        if((CONFIG_TOOLCHAIN STREQUAL xcc OR CONFIG_TOOLCHAIN STREQUAL xtensa) AND CONFIG_CORE_ID STREQUAL hifi4 AND CONFIG_DEVICE_ID STREQUAL MIMXRT798S)
    target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE
    -Wl,--start-group
          ${CMAKE_CURRENT_LIST_DIR}/rt700/hifi4/libNeutronDriver.a
          ${CMAKE_CURRENT_LIST_DIR}/rt700/hifi4/libNeutronFirmware.a
        -Wl,--end-group
    )
    endif()

  