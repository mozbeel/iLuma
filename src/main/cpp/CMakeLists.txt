cmake_minimum_required(VERSION 3.15)
project(iLuma)

set(SDL3_SHARED OFF)
set(SDL3_STATIC ON)

set(PROJECT_ROOT ../../..)

# Add SDL3 (assumes it's configured as a CMake project)
add_subdirectory(${PROJECT_ROOT}/vendor/SDL ${PROJECT_ROOT}/vendor/SDL)
add_subdirectory(${PROJECT_ROOT}/vendor/bgfx.cmake ${PROJECT_ROOT}/vendor/bgfx.cmake)

# Add your main game/app
if(NOT ANDROID)

add_executable(iLuma main.cpp)
else()
add_library(iLuma SHARED main.cpp)
endif()

add_library(vendor INTERFACE)
target_link_libraries(vendor INTERFACE SDL3::SDL3 bgfx bimg bx)


if (ANDROID)
    find_library(log-lib log)
    add_library(native_app_glue STATIC "C:/Users/moz04/AppData/Local/Android/Sdk/ndk/29.0.13599879/sources/android/native_app_glue/android_native_app_glue.c")
    target_include_directories(iLuma PRIVATE "C:/Users/moz04/AppData/Local/Android/Sdk/ndk/29.0.13599879/sources/android/native_app_glue")

    target_link_libraries(vendor INTERFACE EGL GLESv2 android ${log-lib} native_app_glue)
endif()

# Then use the interface
target_link_libraries(iLuma PRIVATE vendor)

target_compile_definitions(iLuma PRIVATE SDL_STATIC)
target_compile_definitions(iLuma PRIVATE SDL_STATIC)

if(NOT ANDROID)
    get_target_property(SDL3_TYPE SDL3::SDL3 TYPE)
    if(SDL3_TYPE STREQUAL "SHARED_LIBRARY")
        add_custom_command(TARGET iLuma POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:SDL3::SDL3>
                $<TARGET_FILE_DIR:iLuma>
            COMMENT "Copying SDL3 runtime to output directory")

    endif()
else()
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
endif()