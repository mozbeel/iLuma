cmake_minimum_required(VERSION 3.10)
project(iLuma)

set(CMAKE_CXX_STANDARD 17)
set(ILUMA_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/app")
set(ILUMA_USE_VULKAN OFF)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)

file(GLOB_RECURSE SOURCES "${ILUMA_ROOT}/src/*.cpp") 
file(GLOB_RECURSE HEADERS "${ILUMA_ROOT}/src/headers/*.hpp" "${ILUMA_ROOT}/src/headers/*.h") 
file(GLOB_RECURSE SHADERS "${ILUMA_ROOT}/src/shaders/*.frag" "${ILUMA_ROOT}/src/shaders/*.vert" "${ILUMA_ROOT}/src/shaders/*.comp")

add_executable(iLuma ${SOURCES} ${SHADERS} ${HEADERS})

source_group(TREE "${ILUMA_ROOT}/src/shaders" PREFIX "Shaders" FILES ${SHADERS})
source_group(TREE "${ILUMA_ROOT}/src/headers" PREFIX "Headers" FILES ${HEADERS})

if (WIN32)
  find_package(Vulkan)

  if (Vulkan_FOUND)
    message(STATUS "Vulkan headers & loader found.")
    set(ILUMA_USE_VULKAN ON)
    set(GLSLC_PATH "$ENV{VULKAN_SDK}/Bin/glslc.exe")
    if(NOT EXISTS ${GLSLC_PATH})
        message(FATAL_ERROR "glslc.exe not found at ${GLSLC_PATH}. Please check Vulkan SDK installation.")
    endif()
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

  set(SPIRV_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders")  # This includes config (Debug/Release)

  foreach(SHADER ${SHADERS})
    get_filename_component(FILE_NAME ${SHADER} NAME)
    set(SPIRV_FILE "${SPIRV_DIR}/${FILE_NAME}.spv")

    add_custom_command(
        OUTPUT ${SPIRV_FILE}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${SPIRV_DIR}
        COMMAND ${GLSLC_PATH} -o ${SPIRV_FILE} ${SHADER}
        DEPENDS ${SHADER}
        COMMENT "Compiling shader ${FILE_NAME} to SPIR-V"
        VERBATIM
    )
    list(APPEND SPIRV_SHADERS ${SPIRV_FILE})
  endforeach()

  add_custom_target(shaders ALL DEPENDS ${SPIRV_SHADERS})
  add_dependencies(iLuma shaders)

  add_custom_command(TARGET iLuma POST_BUILD
    COMMAND ${CMAKE_COMMAND} -DSRC="${SPIRV_DIR}" -DDEST="$<TARGET_FILE_DIR:iLuma>"
            -P "${CMAKE_SOURCE_DIR}/CopyShaders.cmake"
    COMMENT "Copying compiled SPIR-V shaders to binary directory"
  )

endif()
