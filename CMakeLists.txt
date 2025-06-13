cmake_minimum_required(VERSION 3.15)
project(iLuma)

set(SDL3_SHARED OFF)
set(SDL3_STATIC ON)
set(ANDROID_NDK )

# Add SDL3 (assumes it's configured as a CMake project)
add_subdirectory(vendor/SDL)
add_subdirectory(vendor/bgfx.cmake)

# Add your main game/app
if(NOT ANDROID)

add_executable(iLuma src/main.cpp)
else()
add_library(iLuma SHARED src/main.cpp)
endif()

add_library(vendor INTERFACE)
target_link_libraries(vendor INTERFACE SDL3::SDL3 bgfx bimg bx)

if (ANDROID)
    find_library(log-lib log)
    target_link_libraries(vendor INTERFACE EGL GLESv2 android ${log-lib})
endif()

# Then use the interface
target_link_libraries(iLuma PRIVATE vendor)

if(NOT ANDROID)
    target_compile_definitions(iLuma PRIVATE SDL_MAIN_HANDLED SDL_STATIC)
else()
    target_compile_definitions(iLuma PRIVATE SDL_STATIC)
endif()

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