cmake_minimum_required(VERSION 3.12)
project(iLuma)

option(DOWNLOAD_DEPENDENCIES "Download dependencies" TRUE)
option(IOS "Build for iOS" OFF)

# if(ANDROID)

    # set(BX_PLATFORM_ANDROID ON CACHE BOOL "Target is Android" FORCE)
    # set(BX_ARCH "${ANDROID_ABI}" CACHE STRING "Target Arch for BX" FORCE)
    # set(BGFX_CONFIG_RENDERER_GLES ON CACHE BOOL "Use GLES for Android" FORCE)
    # set(BGFX_BUILD_SHADERC OFF CACHE BOOL "" FORCE)
    # set(BX_AMALGAMATED OFF CACHE BOOL "" FORCE)
    # set(BGFX_CUSTOM_TARGETS ON CACHE BOOL "" FORCE)  # <--- this is critical!

    # set(SDL3CPU ${ANDROID_ABI} CACHE STRING "Target CPU for SDL3" FORCE)
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
    set(CMAKE_OSX_DEPLOYMENT_TARGET 12.0 CACHE STRING "" FORCE)
endif()

set(BGFX_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BGFX_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(BGFX_INSTALL OFF CACHE BOOL "" FORCE)

if(DOWNLOAD_DEPENDENCIES)
    # FetchContent downloads and configures dependencies
    include(FetchContent)
    FetchContent_Declare(
        SDL3
        GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
        GIT_TAG "main"
        EXCLUDE_FROM_ALL
    )
    FetchContent_MakeAvailable(SDL3)

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
    find_package(SDL3 CONFIG REQUIRED)
    find_package(bgfx.cmake CONFIG REQUIRED)
endif()

# Add your main game/app
file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "headers/*.h" "headers/*.hpp")


if(ANDROID)
    add_library(iLuma SHARED ${SOURCES} ${HEADERS})
else()
    add_executable(iLuma ${SOURCES} ${HEADERS})

endif()

target_include_directories(iLuma
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/headers"
)

if (ANDROID)
    target_link_libraries(iLuma PRIVATE
        bgfx
        bimg
        bx
        SDL3::SDL3
        SDL3::SDL3main
        native_app_glue
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

if(WIN32)
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