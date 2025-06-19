cmake_minimum_required(VERSION 3.22)

if(ANDROID)
    project(native-activity)

    set(BX_PLATFORM_ANDROID ON CACHE BOOL "Target is Android" FORCE)
    set(BX_ARCH "${ANDROID_ABI}" CACHE STRING "Target Arch for BX" FORCE)
    set(BGFX_CONFIG_RENDERER_GLES ON CACHE BOOL "Use GLES for Android" FORCE)
    set(BGFX_BUILD_SHADERC OFF CACHE BOOL "" FORCE)
    set(BX_AMALGAMATED OFF CACHE BOOL "" FORCE)

    set(SDL3CPU ${ANDROID_ABI} CACHE STRING "Target CPU for SDL3" FORCE)
else()
    project(iLuma)
endif()


set(SDL_DISABLE_INSTALL ON CACHE BOOL "" FORCE)
set(SDL3_TEST OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)


set(APP_ROOT ../../..)
set(BGFX_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BGFX_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(BGFX_INSTALL OFF CACHE BOOL "" FORCE)
set(BGFX_CUSTOM_TARGETS ON CACHE BOOL "" FORCE)  # <--- this is critical!

# Add SDL3 (assumes it's configured as a CMake project)
add_subdirectory(${APP_ROOT}/vendor/SDL ${CMAKE_BINARY_DIR}/SDL-build EXCLUDE_FROM_ALL)
add_subdirectory(${APP_ROOT}/vendor/bgfx.cmake ${CMAKE_BINARY_DIR}/bgfx-build EXCLUDE_FROM_ALL)

# Add your main game/app
if(NOT ANDROID)

add_executable(iLuma main.cpp)
else()
add_library(native-activity SHARED main.cpp)
endif()

if (ANDROID)
    target_compile_definitions(native-activity PRIVATE SDL_STATIC SDL_MAIN_HANDLED)
else()
    target_compile_definitions(iLuma PRIVATE SDL_STATIC SDL_MAIN_HANDLED)
endif()

if (ANDROID)
    target_compile_definitions(native-activity PRIVATE SDL_STATIC SDL_MAIN_HANDLED)
    add_library(native_app_glue STATIC "${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c")
    target_include_directories(native-activity PRIVATE
        ${APP_ROOT}/vendor/SDL/include
        ${ANDROID_NDK}/sources/android/native_app_glue
    )
    target_link_libraries(native-activity
    PRIVATE
        bgfx
        bimg
        bx
        SDL3::SDL3
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
    target_compile_definitions(iLuma PRIVATE SDL_STATIC SDL_MAIN_HANDLED)
    target_link_libraries(iLuma
        PRIVATE
            SDL3::SDL3
            bgfx
            bimg
            bx
    )
endif()

if (ANDROID)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
endif()

get_target_property(SDL3_TYPE SDL3::SDL3 TYPE)
message(STATUS "SDL3 is being built as: ${SDL3_TYPE}")
message(STATUS "SDL3 CPU target: ${SDL3CPU}")