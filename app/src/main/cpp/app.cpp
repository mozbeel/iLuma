#include <app.hpp>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720


App::App() {
  return;
}

bool App::initialize() {
if (SDL_Init(SDL_INIT_VIDEO) == false)
	{
		SDL_Log("SDL_Init failed: %s", SDL_GetError());
#ifdef __ANDROID__
		LOGI("SDL_Init failed: %s", SDL_GetError());
#endif
		return false;
	}
	SDL_Log("SDL_Init succeded");
#ifdef __ANDROID__
	LOGI("SDL_Init succeded");
#endif

	SDL_Window *window = SDL_CreateWindow("BGFX + SDL3 Window",
																				SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	if (!window)
	{
		SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
#ifdef __ANDROID__
		LOGI("SDL_CreateWindow failed: %s", SDL_GetError());
#endif
		SDL_Quit();
		return false;
	}
	SDL_Log("SDL_CreateWindow succeded");
#ifdef __ANDROID__
	LOGI("SDL_CreateWindow succeded");
#endif

	bgfx::PlatformData pd;

#if defined(__ANDROID__)
	pd.nwh = (void*)SDL_GetPointerProperty(SDL_GetWindowProperties(window),SDL_PROP_WINDOW_ANDROID_WINDOW_POINTER, NULL); // Use the native window from the Android app
	pd.ndt = NULL;
#elif defined(_WIN32)

	pd.nwh = (void *)SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
	pd.ndt = NULL;
#elif defined(__linux__)
	pd.nwh = (void *)(uintptr_t)SDL_GetNumberProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_X11_WINDOW_NUMBER, NULL);
	pd.ndt = (void *)SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_X11_DISPLAY_POINTER, NULL);

// I cannot test if it works on macOS, so I will comment it out for now. UPDATE: I can build, but not run it
#elif defined(__APPLE__)

	#if TARGET_OS_OSX
		pd.nwh = (void *)SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, NULL);
		pd.ndt = NULL;
	#elif TARGET_OS_IPHONE
		// iOS does not use bgfx in the same way, so we don't set nwh or ndt here.
		pd.nwh = (void*)SDL_GetPointerProperty(SDL_GetWindowProperties(window),SDL_PROP_WINDOW_UIKIT_WINDOW_POINTER, NULL);
		pd.ndt = NULL; // No native display type in iOS

	#endif

#elif defined(__EMSCRIPTEN__)
	const char* canvasSelector = SDL_GetStringProperty(
      SDL_GetWindowProperties(window),
      SDL_PROP_WINDOW_EMSCRIPTEN_CANVAS_ID_STRING,
      NULL
  );
  
  if (!canvasSelector) {
      SDL_Log("Failed to get canvas ID: %s", SDL_GetError());
#ifdef __ANDROID__
      LOGI("Failed to get canvas ID: %s", SDL_GetError());
#endif
      return false;
  }
  SDL_Log("Canvas selector: %s", canvasSelector);
#ifdef __ANDROID__
  LOGI("Canvas selector: %s", canvasSelector);
#endif

  pd.nwh = (void*)canvasSelector;
  pd.ndt = NULL;

#else
	SDL_Log("Unsupported platform for bgfx initialization.");
#ifdef __ANDROID__
	LOGI("Unsupported platform for bgfx initialization.");
#endif
	SDL_DestroyWindow(window);
	SDL_Quit();
	return false;
#endif

	SDL_Log("Setting PlatformData for bgfx...");
#ifdef __ANDROID__
	LOGI("Setting PlatformData for bgfx...");
#endif
	bgfx::setPlatformData(pd);
	SDL_Log("Setting PlatformData for bgfx succeded..\nInitializing bgfx...");
#ifdef __ANDROID__
	LOGI("Setting PlatformData for bgfx succeded..\nInitializing bgfx...");
#endif
  
	bgfx::Init init;
	init.type = bgfx::RendererType::Count;
	init.platformData.nwh = pd.nwh;
	init.platformData.ndt = pd.ndt;
	init.resolution.width = SCREEN_WIDTH;
	init.resolution.height = SCREEN_HEIGHT;
	init.resolution.reset = BGFX_RESET_VSYNC;
	init.debug = true;  // Enable debug mode

	SDL_Log("Initializing bgfx succeded...");
#ifdef __ANDROID__
	LOGI("Initializing bgfx succeded...");
#endif

	if (!bgfx::init(init))
	{
		SDL_Log("bgfx::init unsuccessful");
#ifdef __ANDROID__
		LOGI("bgfx::init unsuccessful");
#endif
		return false;
	}

	SDL_Log("bgfx::init succeded");
#ifdef __ANDROID__
	LOGI("bgfx::init succeded");
#endif
	SDL_Log("Setting up bgfx debug and clear view...");
#ifdef __ANDROID__
	LOGI("Setting up bgfx debug and clear view...");
#endif

	bgfx::setDebug(BGFX_DEBUG_TEXT);
	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x87CEFAFF, 1.0f, 0);

  lastTime = std::chrono::steady_clock::now();
	int frames = 0;
	float fps = 0.0f;

	bgfx::RendererType::Enum renderBackend = bgfx::getRendererType();

	renderBackendStr = bgfx::getRendererName(renderBackend);
  
  SDL_Log("Main loop incomig, render backend: %s", renderBackendStr);
#ifdef __ANDROID__
  LOGI("Main loop incomig, render backend: %s", renderBackendStr);
#endif
  return true;
}

bool App::mainLoop() {

  while (SDL_PollEvent(&event))
  {
    switch (event.type)
    {
    case SDL_EVENT_QUIT:
    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
      running = false;
      break;
    }
  }

  bgfx::setViewRect(0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  bgfx::touch(0);
  bgfx::dbgTextClear();
  bgfx::dbgTextPrintf(0, 1, 0x4f, "fps: %.2f", fps);
  bgfx::dbgTextPrintf(0, 2, 0x2f, "Render Backend: %s", renderBackendStr);
  bgfx::frame();

  frames++;
  auto now = std::chrono::steady_clock::now();
  std::chrono::duration<float> elapsed = now - lastTime;

  if (elapsed.count() >= 1.0f)
  {
    fps = frames / elapsed.count();
    frames = 0;
    lastTime = now;
  }
  // SDL_Delay(16);  // Simulate frame delay (60 FPS)


  return running;
	
}

void App::shutdown() {
	bgfx::frame();
	bgfx::shutdown();

	SDL_DestroyWindow(window);
	SDL_Quit();

	SDL_Log("shutdown succeded");
#ifdef __ANDROID__
	LOGI("shutdown succeded");
#endif
}