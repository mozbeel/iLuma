#include <SDL3/SDL.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include <fstream>
#include <vector>
#include <chrono>
#include <cassert>
#include <stdio.h>

#include <bx/math.h>

#if defined(__APPLE__)
#include "TargetConditionals.h"
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#ifdef __ANDROID__
#define LOG_TAG "iLuma"
#include <android/log.h>

#if defined(__APPLE__) && defined(__MACH__)
#  include <TargetConditionals.h>
#  if TARGET_IPHONE_SIMULATOR == 1 || TARGET_OS_IPHONE == 1
#    undef  BX_PLATFORM_OSX
#    define BX_PLATFORM_IOS 1
#  elif TARGET_OS_MAC == 1
#    undef  BX_PLATFORM_IOS
#    define BX_PLATFORM_OSX 1
#  endif
#endif


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
  bgfx::VertexLayout m_layout;
  bgfx::VertexBufferHandle vbh;
  bgfx::ProgramHandle program;
  public:
  // Constructor
  App();
  
    bool running = true; // Flag to control the main loop
    bool initialize();
    bool mainLoop();
    void shutdown();

};

struct PosColorVertex {
  float x, y, z;
  uint32_t abgr;
};

static PosColorVertex s_triangleVertices[] = {
  {  0.5f,  0.5f, 0.0f, 0xff0000ff }, // Top (red)
  { -0.5f, -0.5f, 0.0f, 0xff00ff00 }, // Left (green)
  {  0.5f, -0.5f, 0.0f, 0xffff0000 }, // Right (blue)
};

extern unsigned char vs_shader[];
extern unsigned int vs_shader_len;

extern unsigned char fs_shader[];
extern unsigned int fs_shader_len;
