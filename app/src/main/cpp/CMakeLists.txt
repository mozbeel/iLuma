cmake_minimum_required(VERSION 3.12)
project(iLuma)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(DOWNLOAD_DEPENDENCIES "Download dependencies" TRUE)
# option(IOS "Build for iOS" OFF)
# option(PREBUILT_SHADERS "Use prebuilt shaders" ON)

# if(ANDROID)

#     set(BX_PLATFORM_ANDROID ON CACHE BOOL "Target is Android" FORCE)
#     set(BX_ARCH "${ANDROID_ABI}" CACHE STRING "Target Arch for BX" FORCE)
#     set(BGFX_CONFIG_RENDERER_GLES ON CACHE BOOL "Use GLES for Android" FORCE)
#     set(BX_AMALGAMATED OFF CACHE BOOL "" FORCE)
#     set(BGFX_CUSTOM_TARGETS ON CACHE BOOL "" FORCE)  # <--- this is critical!

#     set(SDL3CPU ${ANDROID_ABI} CACHE STRING "Target CPU for SDL3" FORCE)
#     set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../../../../builds/android)

# endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    message(STATUS "Configuring for Emscripten/WebAssembly")

  # Tell bx/bgfx that threading is not supported
    set(BX_CONFIG_SUPPORTS_THREADING       OFF CACHE BOOL "" FORCE)
    set(BGFX_CONFIG_MULTITHREADED         OFF CACHE BOOL "" FORCE)  # disable bgfx’s worker thread :contentReference[oaicite:0]{index=0}
    set(BGFX_CONFIG_RENDERER_THREAD       OFF CACHE BOOL "" FORCE)

    # Use bgfx.cmake’s custom‐targets mode to avoid examples/tools
    set(BGFX_CUSTOM_TARGETS               ON  CACHE BOOL "" FORCE)
endif()


if (NOT ANDROID)
    set(SDL_DISABLE_INSTALL ON CACHE BOOL "" FORCE)
    set(SDL3_TEST OFF CACHE BOOL "" FORCE)
    set(SDL_STATIC ON CACHE BOOL "" FORCE)
    set(SDL_SHARED OFF CACHE BOOL "" FORCE)
endif()

if(APPLE AND IOS)
    message(STATUS "Configuring for iOS (device build)")
    set(CMAKE_OSX_SYSROOT iphoneos CACHE STRING "" FORCE)
    set(CMAKE_OSX_ARCHITECTURES arm64 CACHE STRING "" FORCE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 13.0 CACHE STRING "" FORCE)
    set(BGFX_CUSTOM_TARGETS OFF CACHE STRING "" FORCE)

    if(NOT DEFINED EFFECTIVE_PLATFORM_NAME)
        set(EFFECTIVE_PLATFORM_NAME "-iphoneos" CACHE STRING "Fallback EFFECTIVE_PLATFORM_NAME")
    endif()

endif()

set(BGFX_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BGFX_BUILD_TOOLS ON CACHE BOOL "" FORCE)
set(BGFX_INSTALL OFF CACHE BOOL "" FORCE)

if(DOWNLOAD_DEPENDENCIES)
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
    # find_package looks for already-installed system packages.
    # Configure with `-DCMAKE_PREFIX_PATH="/path/to/package1;/path/to/package2"`
    # to add search paths.
    find_package(bgfx.cmake CONFIG REQUIRED)
    if (NOT ANDROID)
        find_package(SDL3 CONFIG REQUIRED)
    endif()
endif()


if (ANDROID)
find_package(SDL3 CONFIG REQUIRED)
endif()

set(SHADER_PLATFORM "")
set(SHADER_PROFILE "")


if(WIN32)
    set(SHADER_PLATFORM windows)
    set(SHADER_PROFILE s_5_0)
elseif(ANDROID)
    set(SHADER_PLATFORM android)
    set(SHADER_PROFILE 300_es)
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


file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shaders)

if(ANDROID OR EMSCRIPTEN)
    set(SHADERC_EXECUTABLE shaderc)
    set(SHELL_WRAPPER cmd /C)
elseif(IOS)
    set(SHADERC_EXECUTABLE "${bgfx_BINARY_DIR}/cmake/bgfx/Debug-iphoneos/shaderc.app/shaderc")
else()
    set(SHADERC_EXECUTABLE $<TARGET_FILE:shaderc>)
    if(NOT SHADERC_EXECUTABLE)
        message(FATAL_ERROR "shaderc executable not found!")
    endif()
    set(SHELL_WRAPPER "")
    message(STATUS "Shaderc executable target file: $<TARGET_FILE:shaderc>")
endif()
message(STATUS "Running shaderc with: ${SHADERC_EXECUTABLE} -f ${CMAKE_SOURCE_DIR}/shaders/vs_shader.sc --type vertex --platform ${SHADER_PLATFORM} --profile ${SHADER_PROFILE} -o ${CMAKE_BINARY_DIR}/shaders/vs_shader.bin")


# Compile vertex shader .sc -> binary blob
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/shaders/vs_shader.bin
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/shaders
    COMMAND ${SHELL_WRAPPER} ${SHADERC_EXECUTABLE}
        -f vs_shader.sc
        --type vertex
        --platform ${SHADER_PLATFORM}
        --profile ${SHADER_PROFILE}
        -o ${CMAKE_BINARY_DIR}/shaders/vs_shader.bin
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/vs_shader.sc
    VERBATIM
)

# Convert binary blob -> C array using xxd, then rename variables with sed
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/shaders/vs_shader.cpp
    COMMAND xxd -i -n vs_shader ${CMAKE_BINARY_DIR}/shaders/vs_shader.bin > ${CMAKE_BINARY_DIR}/shaders/vs_shader.cpp
    DEPENDS ${CMAKE_BINARY_DIR}/shaders/vs_shader.bin
    VERBATIM
)

# Compile fragment shader .sc -> binary blob
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/shaders/fs_shader.bin
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/shaders
    COMMAND ${SHELL_WRAPPER} ${SHADERC_EXECUTABLE}
        -f fs_shader.sc
        --type fragment
        --platform ${SHADER_PLATFORM}
        --profile ${SHADER_PROFILE}
        -o ${CMAKE_BINARY_DIR}/shaders/fs_shader.bin
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/fs_shader.sc
    VERBATIM
)

# Convert fragment binary blob -> C array using xxd, then rename variables with sed
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

add_custom_target(shaders_generated ALL DEPENDS ${SHADER_CPP_FILES})
message(STATUS "Shader cpp files: ${SHADER_CPP_FILES}")

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "headers/*.h" "headers/*.hpp")

if(ANDROID)
    add_library(iLuma SHARED ${SOURCES} ${HEADERS} ${SHADER_CPP_FILES})
else()
    add_executable(iLuma ${SOURCES} ${HEADERS} ${SHADER_CPP_FILES})
endif()

add_dependencies(shaders_generated shaderc)
add_dependencies(iLuma shaders_generated)

target_include_directories(iLuma
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/headers"
        "${CMAKE_BINARY_DIR}/shaders"
)

if (ANDROID)
    target_link_libraries(iLuma PRIVATE
        bgfx
        bimg
        bx
        SDL3::SDL3
        android
        log
        GLESv2
        EGL
        OpenSLES
        m
        atomic
    )
else()
    target_compile_definitions(iLuma PRIVATE SDL_STATIC)
    target_link_libraries(iLuma
        PRIVATE
            SDL3::SDL3
            bgfx
            bimg
            bx
    )
endif()

if (WIN32)
    set_property(TARGET iLuma PROPERTY WIN32_EXECUTABLE TRUE)

endif()

if(EMSCRIPTEN)
    # Create a html webpage
    target_link_options(iLuma PRIVATE
        "-sINITIAL_MEMORY=67108864"
        "-sALLOW_MEMORY_GROWTH=1"
        "-sNO_EXIT_RUNTIME=1"
        "-sASSERTIONS=0"
        "--shell-file=${CMAKE_CURRENT_SOURCE_DIR}/web/base.html"
    )   
    set_property(TARGET iLuma PROPERTY SUFFIX ".html")
endif()
