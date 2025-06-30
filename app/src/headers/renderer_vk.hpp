#ifdef ILUMA_USE_VULKAN
#include <vulkan/vulkan.h>

#include <glm/glm.hpp>

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
#include <array>


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

const int MAX_FRAMES_IN_FLIGHT = 2;

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

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    static VkVertexInputBindingDescription getBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
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
  std::vector<VkFramebuffer> m_vkSwapChainFramebuffers;

  VkRenderPass m_vkRenderPass;
  VkPipelineLayout m_vkPipelineLayout;
  VkPipeline m_vkGraphicsPipeline;

  VkCommandPool m_vkCommandPool;

  VkBuffer m_vkVertexBuffer;
  VkDeviceMemory m_vkVertexBufferMemory;

  std::vector<VkCommandBuffer> m_vkCommandBuffers;

  std::vector<VkSemaphore> m_vkImageAvailableSemaphores;
  std::vector<VkSemaphore> m_vkRenderFinishedSemaphores;
  std::vector<VkFence> m_vkInFlightFences;

  uint32_t currentFrame = 0;

  const char** m_extraExtensions;
  int m_extraExtensionsCount;

  SDL_Window* m_window;
  SDL_Event m_event;
  
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
  void recreateSwapChain();
  void cleanupSwapChain();
  void cleanupSyncObjects();

  void createRenderPass();

  void createGraphicsPipeline();
  VkShaderModule createShaderModule(const std::vector<char>& code);

  void createFramebuffers();
  void createCommandPool();

  const std::vector<Vertex> vertices = {
      {{0.0f, -0.5f, 1.0f}, {1.0f, 0.0f, 0.0f}},
      {{0.5f, 0.5f, 1.0f}, {0.0f, 1.0f, 0.0f}},
      {{-0.5f, 0.5f, 1.0f}, {0.0f, 0.0f, 1.0f}}

  };
  void createVertexBuffer();

  uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
  
  void createCommandBuffers();

  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
  void createSyncObjects();

public:
  bool framebufferResized = false;

  VulkanRenderer(const char** m_extraExtensions, int m_extraExtensionsCount, SDL_Window* window, SDL_Event event);
  void init();
  void draw();
  void cleanup();
};


#endif