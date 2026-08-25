cmake_minimum_required(VERSION 3.24)

project(nxp_cup_core0 C ASM)

include("${CMAKE_CURRENT_LIST_DIR}/cmake/mcuxpresso_debug.cmake")

# Student application sources are intentionally plug-and-play. A new .c file
# placed directly in source/app is compiled by the next normal build; matching
# headers need no build-system entry. Replace the two checked-in app entries
# from the captured MCUXpresso list so they are not compiled twice.
file(GLOB NXPC_APP_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_LIST_DIR}/source/app/*.c"
)
list(FILTER MCUX_SOURCES EXCLUDE REGEX "/source/app/[^/]+\\.c$")
list(FILTER MCUX_C_SOURCES EXCLUDE REGEX "/source/app/[^/]+\\.c$")
list(APPEND MCUX_SOURCES ${NXPC_APP_SOURCES})
list(APPEND MCUX_C_SOURCES ${NXPC_APP_SOURCES})

# Sources owned by this repository rather than derived from MCUXpresso project
# metadata. See source/shared/ for why each exists.
set(NXPC_SHARED_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/source/shared/nxpc__sbrk.c"
)

add_executable(nxp_cup_core0 ${MCUX_SOURCES} ${NXPC_SHARED_SOURCES})
set_target_properties(nxp_cup_core0 PROPERTIES
    OUTPUT_NAME "nxp_cup_core0"
    SUFFIX ".axf"
    # Land the image at the top of the build directory rather than nested under
    # the source path, so flashing is <build dir>/nxp_cup_core0.axf regardless of
    # whether the build was driven by a preset or the maintainer CMake wrapper.
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

target_compile_definitions(nxp_cup_core0 PRIVATE ${MCUX_DEFINES})
set_source_files_properties(${NXPC_SHARED_SOURCES} PROPERTIES
    COMPILE_OPTIONS "${MCUX_C_FLAGS}"
)
set(NXPC_EXTRA_DEFINES "" CACHE STRING "Additional AVC compile definitions")
if(NXPC_EXTRA_DEFINES)
    target_compile_definitions(nxp_cup_core0 PRIVATE ${NXPC_EXTRA_DEFINES})
endif()
# Vendored headers that ship only with the MCUXpresso IDE. Listed before the
# MCUXpresso include dirs so the in-repo copy wins and the build does not depend
# on the IDE being installed. See source/shared/cr_section_macros.h.
target_include_directories(nxp_cup_core0 PRIVATE
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

target_link_directories(nxp_cup_core0 PRIVATE ${MCUX_LIBRARY_DIRS})
target_link_options(nxp_cup_core0 PRIVATE ${MCUX_LINK_OPTIONS})
target_link_libraries(nxp_cup_core0 PRIVATE
    "-Wl,--start-group"
    ${MCUX_LIBRARIES}
    "-Wl,--end-group"
)

add_custom_command(TARGET nxp_cup_core0 POST_BUILD
    COMMAND "${CMAKE_SIZE}" "$<TARGET_FILE:nxp_cup_core0>"
    # The MCXN947 ROM ISP write-memory command consumes a flat binary starting
    # at address 0. Keep it beside the AXF so every preset has one unambiguous
    # debugger artifact and one unambiguous ROM-programmer artifact.
    COMMAND "${CMAKE_OBJCOPY}" -O binary --gap-fill 0xFF
        "$<TARGET_FILE:nxp_cup_core0>"
        "${CMAKE_BINARY_DIR}/nxp_cup_core0.bin"
    BYPRODUCTS "${CMAKE_BINARY_DIR}/nxp_cup_core0.bin"
    VERBATIM
)
