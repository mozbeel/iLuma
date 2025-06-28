cmake_minimum_required(VERSION 3.10)
project(VulkanApp)

set(CMAKE_CXX_STANDARD 17)
set(ILUMA_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/app")

# Find Vulkan SDK
find_package(Vulkan REQUIRED)

file(GLOB_RECURSE "${ILUMA_ROOT}/src/main.cpp") 
# Add executable
add_executable(VulkanApp app/src/main.cpp)

# Link Vulkan
target_link_libraries(VulkanApp Vulkan::Vulkan)
