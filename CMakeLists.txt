cmake_minimum_required(VERSION 3.10)
project(iLuma)

set(CMAKE_CXX_STANDARD 17)
set(ILUMA_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/app")
set(ILUMA_USE_VULKAN OFF)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)

file(GLOB_RECURSE SOURCES "${ILUMA_ROOT}/src/*.cpp") 
file(GLOB_RECURSE SHADERS "${ILUMA_ROOT}/src/*.*")
# Add executable
add_executable(iLuma ${SOURCES})
source_group(TREE "${ILUMA_ROOT}/shaders" PREFIX "Shaders" FILES ${SHADERS})

if (WIN32)
  find_package(Vulkan)

  if (Vulkan_FOUND)
    message(STATUS "Vulkan headers & loader found.")
    set(ILUMA_USE_VULKAN ON)
  else()
    message(WARNING "Vulkan loader NOT found. Disabling Vulkan support.")
    set(ILUMA_USE_VULKAN OFF)

  endif()

endif()
target_include_directories(iLuma PRIVATE "${ILUMA_ROOT}/src/headers")

if (NOT ILUMA_VR)
 include(FetchContent)

  # Declare SDL3 as an external project
  FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG "main"     # or whatever the latest tag is
  )

  # Download & add SDL3 subdirectory
  FetchContent_MakeAvailable(SDL3)

  set(MIDDLEWARE SDL3::SDL3)

endif()

if (ILUMA_USE_VULKAN)
  target_link_libraries(iLuma Vulkan::Vulkan ${MIDDLEWARE})
  target_compile_definitions(iLuma PRIVATE ILUMA_USE_VULKAN)
endif()

