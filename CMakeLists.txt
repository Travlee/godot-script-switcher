cmake_minimum_required(VERSION 3.31.0...4.3.0)
project(godot_script_switcher)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ADDON_DIR "./../godot/addons/${CMAKE_PROJECT_NAME}/")
# set(ADDON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/godot/addons/${CMAKE_PROJECT_NAME}")

set(EXTERNAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(SUFFIX "debug")
else()
        set(SUFFIX "release")
endif()

if(WIN32)
        set(PLATFORM "windows")
        set(EXT "dll")
elseif(APPLE)
        set(PLATFORM "macos")
        set(EXT "dylib")
else()
        set(PLATFORM "linux")
        set(EXT "so")
endif()

include(FetchContent)

# Set godot-cpp output template for debug/release
set(GODOTCPP_TARGET "template_${SUFFIX}")

FetchContent_Declare(
        godot-cpp
        URL https://github.com/godotengine/godot-cpp/archive/refs/tags/godot-4.5-stable.zip
        SOURCE_DIR     ${EXTERNAL_DIR}godot-cpp
)
FetchContent_MakeAvailable(godot-cpp)

add_library(${CMAKE_PROJECT_NAME} SHARED
        src/register_types.cpp
        src/script_switcher.cpp
)

target_link_libraries(${CMAKE_PROJECT_NAME} godot-cpp)

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${ADDON_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${ADDON_DIR}
        ARCHIVE_OUTPUT_DIRECTORY ${ADDON_DIR}

        PREFIX ""
        SUFFIX ".${EXT}"
        OUTPUT_NAME "${CMAKE_PROJECT_NAME}"
)

# maybe unnecessary
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE TOOLS_ENABLED)

target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE ADDON_DIR="res://addons/${CMAKE_PROJECT_NAME}/")
