cmake_minimum_required(VERSION 3.24)

project(nxp_cup_host LANGUAGES CXX)

set(NXPC_HOST_IMGUI_DIR
    "${CMAKE_CURRENT_LIST_DIR}/vendor/imgui"
    CACHE PATH
    "Local Dear ImGui v1.91.9b source directory")
set(NXPC_HOST_SDL2_DIR
    "${CMAKE_CURRENT_LIST_DIR}/../common/egfx/test/sdl/sdl2/cmake"
    CACHE PATH
    "SDL2 CMake package directory")
set(NXPC_HOST_RBLHOST_DIR
    "${CMAKE_CURRENT_LIST_DIR}/vendor/rblhost"
    CACHE PATH
    "Pinned NXP rblhost v0.2.0 runtime directory")

if(NOT EXISTS "${NXPC_HOST_RBLHOST_DIR}/rblhost.exe" OR
   NOT EXISTS "${NXPC_HOST_RBLHOST_DIR}/LICENSE")
    message(FATAL_ERROR "Vendored NXP rblhost v0.2.0 executable/license not found")
endif()

add_library(nxpc_host_core STATIC nxpc_host_core.cpp)
target_compile_features(nxpc_host_core PUBLIC cxx_std_17)
target_include_directories(nxpc_host_core PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
target_sources(nxpc_host_core PRIVATE nxpc_programmer.cpp)
target_link_libraries(nxpc_host_core PUBLIC setupapi advapi32 hid bcrypt)

add_executable(nxpc_tool nxpc_tool.cpp)
target_compile_features(nxpc_tool PRIVATE cxx_std_17)
target_link_libraries(nxpc_tool PRIVATE nxpc_host_core)

add_custom_command(TARGET nxpc_tool POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${NXPC_HOST_RBLHOST_DIR}/rblhost.exe"
        "$<TARGET_FILE_DIR:nxpc_tool>/rblhost.exe"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${NXPC_HOST_RBLHOST_DIR}/LICENSE"
        "$<TARGET_FILE_DIR:nxpc_tool>/rblhost-LICENSE.txt"
)

find_package(SDL2 CONFIG REQUIRED PATHS "${NXPC_HOST_SDL2_DIR}" NO_DEFAULT_PATH)

set(IMGUI_DIR "${NXPC_HOST_IMGUI_DIR}")

if(NOT EXISTS "${IMGUI_DIR}/imgui.h" OR NOT EXISTS "${IMGUI_DIR}/LICENSE.txt")
    message(FATAL_ERROR "Vendored Dear ImGui v1.91.9b source/license not found at ${IMGUI_DIR}")
endif()

add_executable(nxpc_viewer
    nxpc_viewer.cpp
    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_sdlrenderer2.cpp"
)
target_compile_features(nxpc_viewer PRIVATE cxx_std_17)
target_compile_definitions(nxpc_viewer PRIVATE SDL_MAIN_HANDLED=1)
target_include_directories(nxpc_viewer PRIVATE
    "${IMGUI_DIR}"
    "${IMGUI_DIR}/backends"
    ${SDL2_INCLUDE_DIRS}
)
target_link_libraries(nxpc_viewer PRIVATE nxpc_host_core ${SDL2_LIBRARIES} comdlg32)

if(MSVC)
    target_compile_options(nxpc_host_core PRIVATE /W4 /WX /permissive-)
    target_compile_options(nxpc_tool PRIVATE /W4 /WX /permissive-)
    set_source_files_properties(nxpc_viewer.cpp PROPERTIES COMPILE_OPTIONS "/W4;/WX;/permissive-")
endif()

add_custom_command(TARGET nxpc_viewer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL2_BINDIR}/SDL2.dll"
        "$<TARGET_FILE_DIR:nxpc_viewer>/SDL2.dll"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${NXPC_HOST_RBLHOST_DIR}/rblhost.exe"
        "$<TARGET_FILE_DIR:nxpc_viewer>/rblhost.exe"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${NXPC_HOST_RBLHOST_DIR}/LICENSE"
        "$<TARGET_FILE_DIR:nxpc_viewer>/rblhost-LICENSE.txt"
)
