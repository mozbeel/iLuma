#ifdef ILUMA_USE_VULKAN
#include <renderer_vk.hpp>


VulkanRenderer::VulkanRenderer(const char** m_extraExtensions, int m_extraExtensionsCount, SDL_Window* window) {
  this->m_extraExtensions = m_extraExtensions;
  this->m_extraExtensionsCount = m_extraExtensionsCount;
  this->window = window;
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
  createInfo.ppEnabledExtensionNames = extensions.data();
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

std::vector<const char*> VulkanRenderer::getRequiredExtensions() {
  std::vector<const char*> requiredExtensions;
  for (int i = 0; i < m_extraExtensionsCount; ++i) {
    requiredExtensions.emplace_back(m_extraExtensions[i]);
  }
  requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

  return requiredExtensions;  // Add missing return statement
}

void VulkanRenderer::createSurface() {

    if (!SDL_Vulkan_CreateSurface(window, m_vkInstance, nullptr, &m_vkSurface)) {
        throw std::runtime_error("Failed to create Vulkan surface with SDL!");
    }
}


void VulkanRenderer::pickPhysicalDevice() { 
  std::multimap<int, VkPhysicalDevice> candidates;

  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(m_vkInstance, &deviceCount, nullptr);
  if (deviceCount == 0) {
    throw std::runtime_error("failed to find GPUs with Vulkan support!");
  }
  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(m_vkInstance, &deviceCount, devices.data());

  for (const auto& device : devices) {
    int score = rateDeviceSuitability(device);
    candidates.insert(std::make_pair(score, device));
  }
  if (candidates.rbegin()->first > 0) {
    m_vkPhysicalDevice = candidates.rbegin()->second;
  } else {
    throw std::runtime_error("failed to find a suitable GPU!");
  }
}

int VulkanRenderer::rateDeviceSuitability(VkPhysicalDevice device) {
  VkPhysicalDeviceProperties deviceProperties;
  VkPhysicalDeviceFeatures deviceFeatures;
  vkGetPhysicalDeviceProperties(device, &deviceProperties);
  vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

  int score = 0;

  if (!deviceFeatures.geometryShader) {
    return 0;
  }

  if (!findQueueFamilies(device).isComplete()) {
    return 0;
  }

  bool extensionsSupported = checkDeviceExtensionSupport(device);

  if (!extensionsSupported) {
    return 0;
  }

  bool swapChainAdequate = false;
  SwapChainSupportDetails swapChainSupport = querySwapchainSupport(device);
  swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();

  if (!swapChainAdequate) {
      return 0;
  }

  if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
    score += 1000;
  }

  score += deviceProperties.limits.maxImageDimension2D;


  return score;
}

SwapChainSupportDetails VulkanRenderer::querySwapchainSupport(VkPhysicalDevice device) {
    SwapChainSupportDetails details;
    
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_vkSurface, &details.capabilities);
    
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_vkSurface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_vkSurface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_vkSurface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_vkSurface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

bool VulkanRenderer::checkDeviceExtensionSupport(VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
    

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}


QueueFamilyIndices VulkanRenderer::findQueueFamilies(VkPhysicalDevice device) {
  QueueFamilyIndices indices;
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

  int i = 0;
  for (const auto& queueFamily : queueFamilies) {
    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
    }
    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_vkSurface, &presentSupport);

    if (presentSupport) {
        indices.presentFamily = i;
    }

    if (indices.isComplete()) {
      break;
    }
    i++;
  }
  return indices;
}

void VulkanRenderer::createLogicalDeivce(){
  QueueFamilyIndices indices = findQueueFamilies(m_vkPhysicalDevice);

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

  float queuePriority = 1.0f;
  for (uint32_t queueFamily : uniqueQueueFamilies) {
      VkDeviceQueueCreateInfo queueCreateInfo{};
      queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queueCreateInfo.queueFamilyIndex = queueFamily;
      queueCreateInfo.queueCount = 1;
      queueCreateInfo.pQueuePriorities = &queuePriority;
      queueCreateInfos.push_back(queueCreateInfo);
  }

  VkDeviceQueueCreateInfo queueCreateInfo {};
  queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
  queueCreateInfo.queueCount = 1;
  queueCreateInfo.pQueuePriorities = &queuePriority;

  VkPhysicalDeviceFeatures deviceFeatures{};

  VkDeviceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
  createInfo.pQueueCreateInfos = queueCreateInfos.data();
  createInfo.pEnabledFeatures = &deviceFeatures;

  createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
  createInfo.ppEnabledExtensionNames = deviceExtensions.data();

  if (enableValidationLayers) {
      createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
      createInfo.ppEnabledLayerNames = validationLayers.data();
  }
  else {
      createInfo.enabledLayerCount = 0;
  }

  CHECK_VK(vkCreateDevice(m_vkPhysicalDevice, &createInfo, nullptr, &m_vkDevice));
  vkGetDeviceQueue(m_vkDevice, indices.graphicsFamily.value(), 0, &m_vkGraphicsQueue);
  vkGetDeviceQueue(m_vkDevice, indices.presentFamily.value(), 0, &m_vkPresentQueue);
}

VkSurfaceFormatKHR VulkanRenderer::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB
            && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}
VkPresentModeKHR VulkanRenderer::chooseSwapPresentMode(std::vector<VkPresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }
    
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanRenderer::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    else {
        int width, height;
        SDL_GetWindowSizeInPixels(window, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width,
            capabilities.minImageExtent.width,
            capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height,
            capabilities.minImageExtent.height,
            capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

void VulkanRenderer::createSwapChain() {
    SwapChainSupportDetails swapChainSupport = querySwapchainSupport(m_vkPhysicalDevice);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_vkSurface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = findQueueFamilies(m_vkPhysicalDevice);
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }
    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    CHECK_VK(vkCreateSwapchainKHR(m_vkDevice, &createInfo, nullptr, &m_vkSwapChain));
    vkGetSwapchainImagesKHR(m_vkDevice, m_vkSwapChain, &imageCount, nullptr);
    m_vkSwapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(m_vkDevice, m_vkSwapChain, &imageCount, m_vkSwapChainImages.data());

    m_vkSwapChainImageFormat = surfaceFormat.format;
    m_vkSwapChainExtent = extent;
}

void VulkanRenderer::createImageViews() {
    m_vkSwapChainImageViews.resize(m_vkSwapChainImages.size());

    for (size_t i = 0; i < m_vkSwapChainImages.size(); i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = m_vkSwapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = m_vkSwapChainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        CHECK_VK(vkCreateImageView(m_vkDevice, &createInfo, nullptr, &m_vkSwapChainImageViews[i]));
    }
}

void VulkanRenderer::createGraphicsPipeline() {
    auto vertShaderCode = readFile("shaders/basic.vert.spv");
    auto fragShaderCode = readFile("shaders/basic.frag.spv");

    // TODO: Draw the rest of the owl .)
}

void VulkanRenderer::initVulkan() {
  createInstance();
  createSurface();
  pickPhysicalDevice();
  createLogicalDeivce();
  createSwapChain();
  createImageViews();
  createGraphicsPipeline();
}

void VulkanRenderer::cleanupVulkan() {
  for (auto imageView : m_vkSwapChainImageViews) {
    vkDestroyImageView(m_vkDevice, imageView, nullptr);
  }
  vkDestroySwapchainKHR(m_vkDevice, m_vkSwapChain, nullptr);
  vkDestroyDevice(m_vkDevice, nullptr);
  vkDestroySurfaceKHR(m_vkInstance, m_vkSurface, nullptr);
  vkDestroyInstance(m_vkInstance, nullptr);
}

#endif