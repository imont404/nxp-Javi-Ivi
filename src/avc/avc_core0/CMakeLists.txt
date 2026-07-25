cmake_minimum_required(VERSION 3.24)

project(avc_core0 C ASM)

include("${CMAKE_CURRENT_LIST_DIR}/cmake/mcuxpresso_debug.cmake")

# Sources owned by this repository rather than derived from MCUXpresso project
# metadata. See source/shared/ for why each exists.
set(AVC_SHARED_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/source/shared/avc__sbrk.c"
)

add_executable(avc_core0 ${MCUX_SOURCES} ${AVC_SHARED_SOURCES})
set_target_properties(avc_core0 PROPERTIES
    OUTPUT_NAME "avc_core0"
    SUFFIX ".axf"
    # Land the image at the top of the build directory rather than nested under
    # the source path, so flashing is <build dir>/avc_core0.axf regardless of
    # whether the build was driven by a preset or by build_cmake.ps1.
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

target_compile_definitions(avc_core0 PRIVATE ${MCUX_DEFINES})
set(AVC_EXTRA_DEFINES "" CACHE STRING "Additional AVC compile definitions")
if(AVC_EXTRA_DEFINES)
    target_compile_definitions(avc_core0 PRIVATE ${AVC_EXTRA_DEFINES})
endif()
# Vendored headers that ship only with the MCUXpresso IDE. Listed before the
# MCUXpresso include dirs so the in-repo copy wins and the build does not depend
# on the IDE being installed. See source/shared/cr_section_macros.h.
target_include_directories(avc_core0 PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/source/shared"
    ${MCUX_INCLUDE_DIRS}
)

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
