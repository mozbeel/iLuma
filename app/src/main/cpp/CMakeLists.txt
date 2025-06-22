cmake_minimum_required(VERSION 3.12)
project(iLuma)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(DOWNLOAD_DEPENDENCIES "Download dependencies" TRUE)

# ---------- Cross-platform Configuration ----------
if (CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    message(STATUS "Configuring for Emscripten/WebAssembly")
    set(BX_CONFIG_SUPPORTS_THREADING OFF CACHE BOOL "" FORCE)
    set(BGFX_CONFIG_MULTITHREADED OFF CACHE BOOL "" FORCE)
    set(BGFX_CONFIG_RENDERER_THREAD OFF CACHE BOOL "" FORCE)
    set(BGFX_CUSTOM_TARGETS ON CACHE BOOL "" FORCE)
endif()

if (NOT ANDROID)
    set(SDL_DISABLE_INSTALL ON CACHE BOOL "" FORCE)
    set(SDL3_TEST OFF CACHE BOOL "" FORCE)
    set(SDL_STATIC ON CACHE BOOL "" FORCE)
    set(SDL_SHARED OFF CACHE BOOL "" FORCE)
endif()

# iOS device build (host toolchain adjustments)
if(APPLE AND CMAKE_OSX_SYSROOT MATCHES "iphoneos.*")
    message(STATUS "Configuring for iOS (device build)")
    set(CMAKE_OSX_SYSROOT iphoneos CACHE STRING "" FORCE)
    set(CMAKE_OSX_ARCHITECTURES arm64 CACHE STRING "" FORCE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 13.0 CACHE STRING "" FORCE)
    set(BGFX_CUSTOM_TARGETS ON CACHE BOOL "" FORCE)
endif()

# ---------- BGFX and Tools ----------
set(BGFX_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BGFX_BUILD_TOOLS ON CACHE BOOL "" FORCE)
set(BGFX_INSTALL OFF CACHE BOOL "" FORCE)

# Fetch or find dependencies
if (DOWNLOAD_DEPENDENCIES)
    include(FetchContent)
    if (NOT ANDROID)
        FetchContent_Declare(
            SDL3
            GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
            GIT_TAG "main"
            EXCLUDE_FROM_ALL
        )
        FetchContent_MakeAvailable(SDL3)
    endif()
    FetchContent_Declare(
        bgfx.cmake
        GIT_REPOSITORY "https://github.com/bkaradzic/bgfx.cmake.git"
        GIT_TAG "master"
        EXCLUDE_FROM_ALL
    )
    FetchContent_MakeAvailable(bgfx.cmake)
else()
    find_package(bgfx.cmake CONFIG REQUIRED)
    if (NOT ANDROID)
        find_package(SDL3 CONFIG REQUIRED)
    endif()
endif()
if (ANDROID)
    find_package(SDL3 CONFIG REQUIRED)
endif()

# ---------- Shader Platform Detection ----------
if(WIN32)
    set(SHADER_PLATFORM windows)
    set(SHADER_PROFILE s_5_0)
elseif(ANDROID)
    set(SHADER_PLATFORM android)
    set(SHADER_PROFILE 300_es)
elseif(APPLE AND CMAKE_OSX_SYSROOT MATCHES "iphoneos.*")
    set(SHADER_PLATFORM ios)
    set(SHADER_PROFILE metal)
elseif(APPLE)
    set(SHADER_PLATFORM osx)
    set(SHADER_PROFILE metal)
elseif(EMSCRIPTEN)
    set(SHADER_PLATFORM asm.js)
    set(SHADER_PROFILE 300_es)
elseif(UNIX)
    set(SHADER_PLATFORM linux)
    set(SHADER_PROFILE spirv)
else()
    message(FATAL_ERROR "Unknown platform. Cannot generate shaders.")
endif()
message(STATUS "Shader platform: ${SHADER_PLATFORM}, profile: ${SHADER_PROFILE}")

# ---------- Shader Compilation ----------
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shaders)

# Always use host-built shaderc tool
set(SHADERC_EXECUTABLE $<TARGET_FILE:shaderc>)
set(SHELL_WRAPPER "")
message(STATUS "Using shaderc executable: ${SHADERC_EXECUTABLE}")

# Ensure shaderc is built before generating shaders
add_custom_target(shaders_generated ALL DEPENDS shaderc)

# Vertex shader -> binary
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/shaders/vs_shader.bin
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/shaders
    COMMAND ${SHADERC_EXECUTABLE}
        -f vs_shader.sc
        --type vertex
        --platform ${SHADER_PLATFORM}
        --profile ${SHADER_PROFILE}
        -o ${CMAKE_BINARY_DIR}/shaders/vs_shader.bin
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/vs_shader.sc shaderc
    VERBATIM
)
# Vertex binary -> C++ array
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/shaders/vs_shader.cpp
    COMMAND xxd -i -n vs_shader ${CMAKE_BINARY_DIR}/shaders/vs_shader.bin > ${CMAKE_BINARY_DIR}/shaders/vs_shader.cpp
    DEPENDS ${CMAKE_BINARY_DIR}/shaders/vs_shader.bin
    VERBATIM
)
# Fragment shader -> binary
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/shaders/fs_shader.bin
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/shaders
    COMMAND ${SHADERC_EXECUTABLE}
        -f fs_shader.sc
        --type fragment
        --platform ${SHADER_PLATFORM}
        --profile ${SHADER_PROFILE}
        -o ${CMAKE_BINARY_DIR}/shaders/fs_shader.bin
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/fs_shader.sc shaderc
    VERBATIM
)
# Fragment binary -> C++ array
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/shaders/fs_shader.cpp
    COMMAND xxd -i -n fs_shader ${CMAKE_BINARY_DIR}/shaders/fs_shader.bin > ${CMAKE_BINARY_DIR}/shaders/fs_shader.cpp
    DEPENDS ${CMAKE_BINARY_DIR}/shaders/fs_shader.bin
    VERBATIM
)

set(SHADER_CPP_FILES
    ${CMAKE_BINARY_DIR}/shaders/vs_shader.cpp
    ${CMAKE_BINARY_DIR}/shaders/fs_shader.cpp
)
add_custom_target(shaders_all ALL DEPENDS ${SHADER_CPP_FILES})
add_dependencies(shaders_all shaderc)

# ---------- Project Targets ----------
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/headers/*.h"
                 "${CMAKE_CURRENT_SOURCE_DIR}/headers/*.hpp")

if(ANDROID)
    add_library(iLuma SHARED ${SOURCES} ${HEADERS} ${SHADER_CPP_FILES})
else()
    add_executable(iLuma ${SOURCES} ${HEADERS} ${SHADER_CPP_FILES})
endif()
add_dependencies(iLuma shaders_all)

target_include_directories(iLuma PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/headers"
    "${CMAKE_BINARY_DIR}/shaders"
)

target_link_libraries(iLuma PRIVATE
    SDL3::SDL3
    bgfx
    bimg
    bx
)

if (WIN32)
    set_property(TARGET iLuma PROPERTY WIN32_EXECUTABLE TRUE)
endif()

if(EMSCRIPTEN)
    target_link_options(iLuma PRIVATE
        "-sINITIAL_MEMORY=67108864"
        "-sALLOW_MEMORY_GROWTH=1"
        "-sNO_EXIT_RUNTIME=1"
        "-sASSERTIONS=0"
        "--shell-file=${CMAKE_CURRENT_SOURCE_DIR}/web/base.html"
    )
    set_property(TARGET iLuma PROPERTY SUFFIX ".html")
endif()
