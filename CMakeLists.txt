cmake_minimum_required(VERSION 4.3.0)
project(godot_script_switcher)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ADDON_DIR "./../godot/addons/${CMAKE_PROJECT_NAME}/")
# set(ADDON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/godot/addons/${CMAKE_PROJECT_NAME}")

include(FetchContent)

FetchContent_Declare(
        godot-cpp
        GIT_REPOSITORY https://github.com/godotengine/godot-cpp.git
        GIT_TAG godot-4.5-stable
        SOURCE_DIR     ${CMAKE_CURRENT_SOURCE_DIR}/external/godot-cpp
)
FetchContent_MakeAvailable(godot-cpp)

add_library(${CMAKE_PROJECT_NAME} SHARED
        src/register_types.cpp
        src/script_switcher.cpp
)

target_link_libraries(${CMAKE_PROJECT_NAME} godot-cpp)

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
        PREFIX ""
        LIBRARY_OUTPUT_DIRECTORY ${ADDON_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${ADDON_DIR}
        ARCHIVE_OUTPUT_DIRECTORY ${ADDON_DIR}
        OUTPUT_NAME ${CMAKE_PROJECT_NAME}
)

# maybe unnecessary
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE TOOLS_ENABLED)

target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE ADDON_DIR="${ADDON_DIR}")