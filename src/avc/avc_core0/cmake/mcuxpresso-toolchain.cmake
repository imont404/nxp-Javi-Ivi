set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

if(NOT DEFINED MCUXPRESSO_IDE)
    if(DEFINED ENV{MCUXPRESSO_IDE})
        set(MCUXPRESSO_IDE "$ENV{MCUXPRESSO_IDE}" CACHE PATH "MCUXpresso IDE directory")
    elseif(APPLE)
        set(MCUXPRESSO_IDE "/Applications/MCUXpressoIDE_25.6.136/ide" CACHE PATH "MCUXpresso IDE directory")
    elseif(WIN32)
        set(MCUXPRESSO_IDE "C:/nxp/MCUXpressoIDE_25.6.136/ide" CACHE PATH "MCUXpresso IDE directory")
    else()
        set(MCUXPRESSO_IDE "" CACHE PATH "MCUXpresso IDE directory")
    endif()
endif()

file(TO_CMAKE_PATH "${MCUXPRESSO_IDE}" MCUXPRESSO_IDE)

set(_MCUXPRESSO_TOOL_DIRS "${MCUXPRESSO_IDE}/tools/bin")
if(EXISTS "${MCUXPRESSO_IDE}/plugins")
    file(GLOB _MCUXPRESSO_TOOL_PLUGINS LIST_DIRECTORIES true "${MCUXPRESSO_IDE}/plugins/com.nxp.mcuxpresso.tools.*_*")
    list(SORT _MCUXPRESSO_TOOL_PLUGINS ORDER DESCENDING)
    foreach(_plugin IN LISTS _MCUXPRESSO_TOOL_PLUGINS)
        list(APPEND _MCUXPRESSO_TOOL_DIRS "${_plugin}/tools/bin")
    endforeach()
endif()

function(_mcux_find_tool out_var tool_name description)
    find_program(
        ${out_var}
        NAMES "${tool_name}" "${tool_name}.exe"
        HINTS ${_MCUXPRESSO_TOOL_DIRS}
        NO_DEFAULT_PATH
    )
    if(NOT ${out_var})
        find_program(${out_var} NAMES "${tool_name}" "${tool_name}.exe")
    endif()
    if(NOT ${out_var})
        message(FATAL_ERROR "${description} not found. Set MCUXPRESSO_IDE to the MCUXpresso IDE 'ide' directory.")
    endif()
    set(${out_var} "${${out_var}}" CACHE FILEPATH "${description}" FORCE)
endfunction()

_mcux_find_tool(CMAKE_C_COMPILER "arm-none-eabi-gcc" "MCUXpresso Arm GCC")
set(CMAKE_ASM_COMPILER "${CMAKE_C_COMPILER}" CACHE FILEPATH "MCUXpresso Arm GCC assembler driver" FORCE)
_mcux_find_tool(CMAKE_OBJCOPY "arm-none-eabi-objcopy" "MCUXpresso objcopy")
_mcux_find_tool(CMAKE_SIZE "arm-none-eabi-size" "MCUXpresso size")
_mcux_find_tool(CMAKE_AR "arm-none-eabi-ar" "MCUXpresso ar")
_mcux_find_tool(CMAKE_RANLIB "arm-none-eabi-ranlib" "MCUXpresso ranlib")

get_filename_component(MCUXPRESSO_TOOLCHAIN_BIN "${CMAKE_C_COMPILER}" DIRECTORY)

set(CMAKE_FIND_ROOT_PATH "${MCUXPRESSO_TOOLCHAIN_BIN}/..")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
