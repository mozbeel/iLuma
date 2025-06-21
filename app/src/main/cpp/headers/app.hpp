#include <SDL3/SDL.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include <chrono>
#if defined(__APPLE__)
#include "TargetConditionals.h"
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

class App {
  SDL_Window *window = nullptr; // Pointer to the SDL window
  SDL_Event event;
  
  std::chrono::time_point<std::chrono::steady_clock> lastTime; // Last time the frame was rendered
  int frames = 0; // Frame counter
  float fps = 0.0f; // Frames per second
  
  const char* renderBackendStr = nullptr; // String representation of the rendering backend
  public:
  // Constructor
  App();
  
    bool running = true; // Flag to control the main loop
    bool initialize();
    bool mainLoop();
    void shutdown();

};