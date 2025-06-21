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

#ifdef __ANDROID__
#define LOG_TAG "iLuma"
#include <android/log.h>

#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,   LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,    LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,    LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,   LOG_TAG, __VA_ARGS__)
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