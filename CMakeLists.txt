cmake_minimum_required(VERSION 3.15)
project(MyForgeApp)

# 1) Paths to your locally built Forge
set(FORGE_ROOT    ${CMAKE_CURRENT_SOURCE_DIR}/forge)
set(FORGE_LIB_DIR ${FORGE_ROOT}/PC/VS2022/$<CONFIG>)

# 2) Your executable
add_executable(${PROJECT_NAME}
    app/src/main.cpp
)

# 3) Tell CMake where to find headers
target_include_directories(${PROJECT_NAME} PRIVATE
    ${FORGE_ROOT}/Common_3/OS
    ${FORGE_ROOT}/Common_3/Renderer
    ${FORGE_ROOT}/Common_3/Utilities
    ${FORGE_ROOT}/Middleware_3
)

# 4) Tell CMake where to find the .lib/.a files
link_directories(${FORGE_LIB_DIR})

# 5) Link against the Forge static libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Renderer
    OS
    ResourceLoader
    gainputstatic
    # …etc
)

# 6) Copy shaders & assets
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/Shaders
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/Shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)
