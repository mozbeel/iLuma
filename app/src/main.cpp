#ifdef ILUMA_USE_VULKAN
#   include <renderer_vk.hpp>
#endif


#include <iostream>
#include <vector>

int main() {
  if(SDL_Init(SDL_INIT_VIDEO) == false) {
    return EXIT_FAILURE;
  }
  SDL_Window* window = SDL_CreateWindow(SDL_WINDOW_NAME, 1280, 720, 
    #ifdef ILUMA_USE_VULKAN
      SDL_WINDOW_VULKAN
    #else
      0
    #endif
  );
  if (!window) {
    std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
    SDL_Quit();
    return EXIT_FAILURE;
  }

  unsigned int extensionCount = 0;
  if (!SDL_Vulkan_GetInstanceExtensions(&extensionCount)) {
      // handle error
  }

  Uint32 count_instance_extensions;
  const char * const *instance_extensions = SDL_Vulkan_GetInstanceExtensions(&count_instance_extensions);

  if (instance_extensions == NULL) {
    std::cerr << "SDL_Vulkan_GetInstanceExtensions failed: " << SDL_GetError() << "\n";
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_FAILURE;
  }

  int count_extensions = count_instance_extensions + 1;
  SDL_malloc(count_extensions * sizeof(const char *));
  // Fix SDL_malloc cast to correct type
  const char** extensions = static_cast<const char**>(SDL_malloc(count_extensions * sizeof(const char*)));  
  extensions[0] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
  SDL_memcpy(&extensions[1], instance_extensions, count_instance_extensions * sizeof(const char*)); 

  VulkanRenderer* vkRenderer = new VulkanRenderer(extensions, count_extensions, window);

  vkRenderer->initVulkan();

  bool running = true;
  SDL_Event event;

  while(running) {
    while (SDL_PollEvent(&event)) {
      switch(event.type) {
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        case SDL_EVENT_QUIT:
          running = false;
          break;
      }
    }

  }

  vkRenderer->cleanupVulkan();
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}