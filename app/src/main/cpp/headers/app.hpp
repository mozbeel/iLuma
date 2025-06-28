#include <SDL3/SDL.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include <fstream>
#include <vector>
#include <chrono>
#include <cassert>
#include <stdio.h>
#include <cmath>
#include <cstdlib>

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

#ifdef ILUMA_VR  
  // #ifndef XR_KHR_D3D11_ENABLE_EXTENSION_NAME
  //   #define XR_KHR_D3D11_ENABLE_EXTENSION_NAME "XR_KHR_D3D11_enable"
  // #endif

  #include <vulkan/vulkan.h>      // For Vulkan types (required if XR_USE_GRAPHICS_API_VULKAN is defined)
  #ifdef _WIN32
    #include <windows.h>             // For Win32 types (if XR_USE_PLATFORM_WIN32 is defined)
    #define XR_USE_PLATFORM_WIN32
  #elif __ANDROID__
    #include <jni.h>
    #define XR_USE_PLATFORM_ANDROID
  #elif __linux__
    #define XR_USE_PLATFORM_LINUX
  #endif

  #define XR_USE_GRAPHICS_API_VULKAN
  #include <openxr/openxr.h>
  #include <openxr/openxr_platform.h>
  
  #define BGFX_CONFIG_RENDERER_VULKAN
  #include <bgfx/vulkan_handles.h>

  inline const char* getXrResultString(XrResult result) {
    switch (result) {
      case XR_SUCCESS: return "XR_SUCCESS";
      case XR_TIMEOUT_EXPIRED: return "XR_TIMEOUT_EXPIRED";
      case XR_SESSION_LOSS_PENDING: return "XR_SESSION_LOSS_PENDING";
      case XR_EVENT_UNAVAILABLE: return "XR_EVENT_UNAVAILABLE";
      case XR_SPACE_BOUNDS_UNAVAILABLE: return "XR_SPACE_BOUNDS_UNAVAILABLE";
      case XR_FRAME_DISCARDED: return "XR_FRAME_DISCARDED";
      // You can add many more — this is just a subset.
      default: return "Unknown XrResult";
    }
  }

  #ifdef __ANDROID__
    #define CHECK_XR(expr)                                                      \
      do {                                                                      \
        XrResult result = (expr);                                               \
        if (XR_FAILED(result)) {                                                \
          /* stderr */                                                          \
          fprintf(stderr,                                                       \
                  "[OpenXR] %s failed with error: %d (%s)\n",                  \
                  #expr, result, getXrResultString(result));                    \
          /* logcat */                                                          \
          LOGI("[OpenXR] %s failed with error: %d (%s)",                        \
              #expr, result, getXrResultString(result));                      \
          std::exit(EXIT_FAILURE);                                              \
        }                                                                       \
      } while (0)
  #else
    #define CHECK_XR(x) \
        { \
            XrResult res = (x); \
            if (res != XR_SUCCESS) { \
                SDL_Log("OpenXR call failed: %d at %s:%d", res, __FILE__, __LINE__); \
                std::exit(EXIT_FAILURE);   \
            } \
        }
  #endif

  inline void poseToViewMatrix(const XrPosef& pose, float* out) {
    bx::Vec3 position = { pose.position.x, pose.position.y, pose.position.z };
    bx::Quaternion orientation = { pose.orientation.x, pose.orientation.y, pose.orientation.z, pose.orientation.w };

    float rot[16];
    bx::mtxFromQuaternion(rot, orientation);
    rot[12] = -position.x;
    rot[13] = -position.y;
    rot[14] = -position.z;

    // Output = inverse of pose = view matrix
    bx::mtxInverse(out, rot);
  }

    
#endif

class App {
  std::chrono::time_point<std::chrono::steady_clock> lastTime; // Last time the frame was rendered
  int frames = 0; // Frame counter
  float fps = 0.0f; // Frames per second
  
  const char* renderBackendStr = nullptr; // String representation of the rendering backend
  bgfx::VertexLayout m_layout;
  bgfx::VertexBufferHandle vbh;
  bgfx::IndexBufferHandle ibh;
  bgfx::ProgramHandle program;
  bgfx::PlatformData pd;
  
  int w_width = 1280;
  int w_height = 720;
  
  SDL_Window *window = nullptr;
  SDL_Event event;
  bool initializeFlatContext();
  bool initializeBGFX();

#ifdef ILUMA_VR
  bool initializeVRContext();
  XrInstance xrInstance = XR_NULL_HANDLE;
  XrSystemId xrSystemId = 0;
  XrSession xrSession = XR_NULL_HANDLE;
  XrSpace xrAppSpace = XR_NULL_HANDLE;
  std::vector<XrSwapchain> swapchains;
  std::vector<std::vector<XrSwapchainImageVulkanKHR>> swapchainImages;
  uint32_t swapchainWidth = 0;
  uint32_t swapchainHeight = 0;

#endif

  public:
    // Constructor
    App();
  
    bool running = true; // Flag to control the main loop
    bool initialize();
    bool mainLoop();
    #ifndef ILUMA_VR
      void renderFlat();
    #else
      void renderVR();
    #endif
    void draw(bgfx::ViewId viewId, float *view_matrix, float *projection_matrix);
    void shutdown();

};

struct PosColorVertex {
  float x, y, z;
  uint32_t abgr;
};

static PosColorVertex s_cubeVertices[] =
{
	{-1.0f,  1.0f,  1.0f, 0xff000000 },
	{ 1.0f,  1.0f,  1.0f, 0xff0000ff },
	{-1.0f, -1.0f,  1.0f, 0xff00ff00 },
	{ 1.0f, -1.0f,  1.0f, 0xff00ffff },
	{-1.0f,  1.0f, -1.0f, 0xffff0000 },
	{ 1.0f,  1.0f, -1.0f, 0xffff00ff },
	{-1.0f, -1.0f, -1.0f, 0xffffff00 },
	{ 1.0f, -1.0f, -1.0f, 0xffffffff }
};

static const uint16_t s_cubeTriangleList[] =
{
	0, 1, 2, // 0
	1, 3, 2,
	4, 6, 5, // 2
	5, 6, 7,
	0, 2, 4, // 4
	4, 2, 6,
	1, 5, 3, // 6
	5, 7, 3,
	0, 4, 1, // 8
	4, 5, 1,
	2, 3, 6, // 10
	6, 3, 7,
};

extern unsigned char vs_shader[];
extern unsigned int vs_shader_len;

extern unsigned char fs_shader[];
extern unsigned int fs_shader_len;
