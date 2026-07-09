cmake_minimum_required(VERSION 3.24)

project(avc_core0 C ASM)

include("${CMAKE_CURRENT_LIST_DIR}/cmake/mcuxpresso_debug.cmake")

add_executable(avc_core0 ${MCUX_SOURCES})
set_target_properties(avc_core0 PROPERTIES
    OUTPUT_NAME "avc_core0"
    SUFFIX ".axf"
)

target_compile_definitions(avc_core0 PRIVATE ${MCUX_DEFINES})
target_include_directories(avc_core0 PRIVATE ${MCUX_INCLUDE_DIRS})

foreach(source IN LISTS MCUX_C_SOURCES)
    get_filename_component(source_dir "${source}" DIRECTORY)
    set(source_compile_options
        ${MCUX_C_FLAGS}
        "-fmacro-prefix-map=${source_dir}/="
    )
    if(WIN32)
        file(TO_NATIVE_PATH "${source_dir}" native_source_dir)
        list(APPEND source_compile_options "-fmacro-prefix-map=${native_source_dir}\\=")
    endif()
    set_source_files_properties("${source}" PROPERTIES
        COMPILE_OPTIONS "${source_compile_options}"
    )
endforeach()

set_source_files_properties(${MCUX_ASM_SOURCES} PROPERTIES
    COMPILE_OPTIONS "${MCUX_ASM_FLAGS}"
)

if(MCUX_SOURCE_OPTIONS_CODE)
    cmake_language(EVAL CODE "${MCUX_SOURCE_OPTIONS_CODE}")
endif()

target_link_directories(avc_core0 PRIVATE ${MCUX_LIBRARY_DIRS})
target_link_options(avc_core0 PRIVATE ${MCUX_LINK_OPTIONS})
target_link_libraries(avc_core0 PRIVATE
    "-Wl,--start-group"
    ${MCUX_LIBRARIES}
    "-Wl,--end-group"
)

add_custom_command(TARGET avc_core0 POST_BUILD
    COMMAND "${CMAKE_SIZE}" "$<TARGET_FILE:avc_core0>"
    VERBATIM
)
