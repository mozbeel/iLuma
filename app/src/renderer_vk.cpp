#ifdef ILUMA_USE_VULKAN
#include <renderer_vk.hpp>


VulkanRenderer::VulkanRenderer(const char** m_extraExtensions, int m_extraExtensionsCount) {
  this->m_extraExtensions = m_extraExtensions;
  this->m_extraExtensionsCount = m_extraExtensionsCount;
}

void VulkanRenderer::createInstance() {
   if (enableValidationLayers && !checkValidationSupport()) {
    throw std::runtime_error("validation layers requested, but not available!");
  }

  VkApplicationInfo appInfo {};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "iLuma";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 1, 0);
  appInfo.engineVersion = VK_MAKE_VERSION(1, 1, 0);
  appInfo.apiVersion = VK_API_VERSION_1_1;

  VkInstanceCreateInfo createInfo {};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
  createInfo.pApplicationInfo = &appInfo;

  auto extensions = getRequiredExtensions();
  // createInfo.ppEnabledExtensionNames = extensions.data();
  createInfo.enabledExtensionCount = (uint32_t)extensions.size();

  if (enableValidationLayers) {
    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
  } else {
    createInfo.enabledLayerCount = 0;
  }
  
  
  CHECK_VK(vkCreateInstance(&createInfo, nullptr, &m_vkInstance));
}

bool VulkanRenderer::checkValidationSupport() {
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (const char* layerName : validationLayers) {
    bool layerFound = false;

    for (const auto& layerProperties : availableLayers) {
        if (strcmp(layerName, layerProperties.layerName) == 0) {
            layerFound = true;
            break;
        }
    }

    if (!layerFound) {
        return false;
    }
  }

  return true;
}

void VulkanRenderer::pickPhysicalDevice() {
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

  std::multimap<int, VkPhysicalDevice> candidates;

  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(m_vkInstance, &deviceCount, nullptr);
  if (deviceCount == 0) {
    throw std::runtime_error("failed to find GPUs with Vulkan support!");
  }
  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(m_vkInstance, &deviceCount, devices.data());

  for (const auto& device : devices) {
    int score = /*TODO : rateDeviceSuitability(device)*/ 0;
    candidates.insert(std::make_pair(score, device));
    if (isDeviceSuitable(device)) {
      break;
    }
  }
  if (physicalDevice == VK_NULL_HANDLE) {
    throw std::runtime_error("failed to find a suitable GPU!");
  }

}

bool VulkanRenderer::isDeviceSuitable(VkPhysicalDevice device) {
  VkPhysicalDeviceProperties deviceProperties;
  VkPhysicalDeviceFeatures deviceFeatures;
  vkGetPhysicalDeviceProperties(device, &deviceProperties);
  vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
  return deviceFeatures.geometryShader;
}

std::vector<const char*> VulkanRenderer::getRequiredExtensions() {
  std::vector<const char*> requiredExtensions;
  for (int i = 0; i < m_extraExtensionsCount; ++i) {
    requiredExtensions.emplace_back(m_extraExtensions[i]);
  }
  requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

  return requiredExtensions;  // Add missing return statement
}

void VulkanRenderer::initVulkan() {
  createInstance();
  pickPhysicalDevice();
}

void VulkanRenderer::cleanupVulkan() {
  vkDestroyInstance(m_vkInstance, nullptr);
}

#endif