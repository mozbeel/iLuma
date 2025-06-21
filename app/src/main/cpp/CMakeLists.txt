cmake_minimum_required(VERSION 3.12)
project(iLuma)

option(DOWNLOAD_DEPENDENCIES "Download dependencies" TRUE)

# if(ANDROID)

    # set(BX_PLATFORM_ANDROID ON CACHE BOOL "Target is Android" FORCE)
    # set(BX_ARCH "${ANDROID_ABI}" CACHE STRING "Target Arch for BX" FORCE)
    # set(BGFX_CONFIG_RENDERER_GLES ON CACHE BOOL "Use GLES for Android" FORCE)
    # set(BGFX_BUILD_SHADERC OFF CACHE BOOL "" FORCE)
    # set(BX_AMALGAMATED OFF CACHE BOOL "" FORCE)
    # set(BGFX_CUSTOM_TARGETS ON CACHE BOOL "" FORCE)  # <--- this is critical!

    # set(SDL3CPU ${ANDROID_ABI} CACHE STRING "Target CPU for SDL3" FORCE)
# endif()


if (NOT ANDROID)
    set(SDL_DISABLE_INSTALL ON CACHE BOOL "" FORCE)
    set(SDL3_TEST OFF CACHE BOOL "" FORCE)
    set(SDL_STATIC ON CACHE BOOL "" FORCE)
    set(SDL_SHARED OFF CACHE BOOL "" FORCE)
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
if(ANDROID)
    add_library(iLuma SHARED main.cpp)
else()
    add_executable(iLuma main.cpp)

endif()

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

# set_property(TARGET iLuma PROPERTY WIN32_EXECUTABLE TRUE)

if(EMSCRIPTEN)
    # Create a html webpage
    set_property(TARGET iLuma PROPERTY SUFFIX ".html")
endif()