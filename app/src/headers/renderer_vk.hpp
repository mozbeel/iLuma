#ifdef ILUMA_USE_VULKAN
#include <vulkan/vulkan.h>

#include <iostream>
#include <vector>
#include <cstring>
#include <map>
#include <optional>
#include <set>
#include <cstdint>
#include <limits>
#include <fstream>
#include <algorithm>

#ifndef ILUMA_VR
#   include <SDL3/SDL.h>
#   include <SDL3/SDL_vulkan.h>
#endif

#ifdef _WIN32
#  define NOMINMAX
#  include <windows.h>
#  include <vulkan/vulkan_win32.h>
#endif

#define SDL_WINDOW_NAME "Vulkan Window (iLuma)"

const std::vector<const char*> validationLayers = {
  "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NODEBUG
  const bool enableValidationLayers = false;
#else
  const bool enableValidationLayers = true;
#endif

#define CHECK_VK(expr) \
  do { \
    VkResult res = (expr); \
    if (res) { \
      std::cout << "Vulkan error: " << res << std::endl; \
    } \
  } while (0)

static std::vector<char> readFile(const std::string & filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}
#define SHADER_ENTRY_POINT "main"

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;
  
  bool isComplete() {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class VulkanRenderer {
private:
  VkInstance m_vkInstance;
  VkPhysicalDevice m_vkPhysicalDevice = VK_NULL_HANDLE;
  VkDevice m_vkDevice;
  VkQueue m_vkGraphicsQueue;
  VkSurfaceKHR m_vkSurface;
  VkQueue m_vkPresentQueue;
  VkSwapchainKHR m_vkSwapChain;
  std::vector<VkImage> m_vkSwapChainImages;
  VkFormat m_vkSwapChainImageFormat;
  VkExtent2D m_vkSwapChainExtent;
  std::vector<VkImageView> m_vkSwapChainImageViews;
  VkPipelineLayout m_vkPipelineLayout;

  const char** m_extraExtensions;
  int m_extraExtensionsCount;

  SDL_Window* m_window;
  
  void createInstance();
  bool checkValidationSupport();
  bool checkDeviceExtensionSupport(VkPhysicalDevice device);

  std::vector<const char*> getRequiredExtensions();
  void createSurface();
  void pickPhysicalDevice();
  
  int rateDeviceSuitability(VkPhysicalDevice device);

  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

  void createLogicalDeivce();

  SwapChainSupportDetails querySwapchainSupport(VkPhysicalDevice device);
  VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
  VkPresentModeKHR chooseSwapPresentMode(std::vector<VkPresentModeKHR>& availablePresentModes);
  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
  void createSwapChain();
  void createImageViews();

  void createGraphicsPipeline();
  VkShaderModule createShaderModule(const std::vector<char>& code);

public:
  VulkanRenderer(const char** m_extraExtensions, int m_extraExtensionsCount, SDL_Window* window);
  void initVulkan();

  void cleanupVulkan();
};


#endif