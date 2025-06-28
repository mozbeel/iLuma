#ifdef ILUMA_USE_VULKAN
#  include <vulkan/vulkan.h>

#include <iostream>
#include <vector>
#include <cstring>
#include <map>

#define SDL_WINDOW_NAME "Vulkan Window (iLuma)"

const std::vector<const char*> validationLayers = {
  "VK_LAYER_KHRONOS_validation"
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
      throw std::runtime_error("Vulkan error"); \
    } \
  } while (0)

class VulkanRenderer {
private:
  VkInstance m_vkInstance;
  const char** m_extraExtensions;
  int m_extraExtensionsCount;
  
  
  void createInstance();
  bool checkValidationSupport();
  std::vector<const char*> getRequiredExtensions();
  void pickPhysicalDevice();
  bool isDeviceSuitable(VkPhysicalDevice device);
public:
  VulkanRenderer(const char** m_extraExtensions, int m_extraExtensionsCount);
  void initVulkan();

  void cleanupVulkan();
};


#endif