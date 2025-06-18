cmake_minimum_required(VERSION 3.15)
project(iLuma)

set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL3_DISABLE_INSTALL ON CACHE BOOL "" FORCE)

set(APP_ROOT ../../..)
set(BGFX_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Add SDL3 (assumes it's configured as a CMake project)
add_subdirectory(${APP_ROOT}/vendor/SDL ${APP_ROOT}/vendor/SDL EXCLUDE_FROM_ALL)
add_subdirectory(${APP_ROOT}/vendor/bgfx.cmake ${APP_ROOT}/vendor/bgfx.cmake)

# Add your main game/app
if(NOT ANDROID)

add_executable(iLuma main.cpp)
else()
add_library(native-activity SHARED main.cpp)
endif()

add_library(vendor INTERFACE)
target_link_libraries(vendor INTERFACE SDL3::SDL3 bgfx bimg bx)

if (ANDROID)
    find_library(log-lib log)
    add_library(native_app_glue STATIC "C:/Users/moz04/AppData/Local/Android/Sdk/ndk/29.0.13599879/sources/android/native_app_glue/android_native_app_glue.c")
    target_include_directories(native-activity PRIVATE "C:/Users/moz04/AppData/Local/Android/Sdk/ndk/29.0.13599879/sources/android/native_app_glue")

    target_link_libraries(vendor INTERFACE EGL GLESv2 android ${log-lib} native_app_glue)
endif()


# Then use the interface
if (ANDROID)
    target_compile_definitions(native-activity PRIVATE SDL_STATIC SDL_MAIN_HANDLED)
    target_link_libraries(native-activity PRIVATE vendor)
else()
    target_compile_definitions(iLuma PRIVATE SDL_STATIC SDL_MAIN_HANDLED)
    target_link_libraries(iLuma PRIVATE vendor)
endif()


if (ANDROID)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
endif()

get_target_property(SDL3_TYPE SDL3::SDL3 TYPE)
message(STATUS "SDL3 is being built as: ${SDL3_TYPE}")
