#include <app.hpp>


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
	#if defined(__ANDROID__) || defined(TARGET_OS_IPHONE)
		const SDL_DisplayMode* displayMode = SDL_GetCurrentDisplayMode(SDL_GetPrimaryDisplay());
		if (!displayMode) {
			SDL_Log("Couldnt get screen size");
		} else {
			w_width = displayMode->w;
			w_height = displayMode->h;
			LOGI("Display Height: %d, Display Width: %d", w_height, w_width);
		}
	#endif

	SDL_Window *window = SDL_CreateWindow("BGFX + SDL3 Window",
																				w_width, w_height, 0);
	if (!window)
	{
		SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
		SDL_Quit();
		return false;
	}
	SDL_Log("SDL_CreateWindow succeded");

	

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

	// Validate values before using them
	if (!pd.nwh || !pd.ndt) {
			SDL_Log("Invalid X11 window handle or display pointer");
			return false;
	}

// I cannot test if it works on macOS, so I will comment it out for now. UPDATE: I can build, but not run it
#elif defined(__APPLE__)

	#if defined(TARGET_OS_OSX)
		pd.nwh = (void *)SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, NULL);
		pd.ndt = NULL;
	#elif defined(TARGET_OS_IPHONE)
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
#if defined(__ANDROID__) || TARGET_OS_IPHONE
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
	init.resolution.width = w_width;
	init.resolution.height = w_height;
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

	m_layout.begin()
		.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
		.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
		.end();

	vbh = bgfx::createVertexBuffer(bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices)), m_layout);
	ibh = bgfx::createIndexBuffer(bgfx::makeRef(s_cubeTriangleList, sizeof(s_cubeTriangleList)));

	bgfx::ShaderHandle vertexShader = bgfx::createShader(bgfx::makeRef(vs_shader, vs_shader_len));
	bgfx::ShaderHandle fragmentShader = bgfx::createShader(bgfx::makeRef(fs_shader, fs_shader_len));

	program = bgfx::createProgram(vertexShader, fragmentShader, true);

	if (!bgfx::isValid(vbh)) {
    SDL_Log("VertexBufferHandle is invalid!");
	}

	if (!bgfx::isValid(ibh)) {
    SDL_Log("IndexBufferHandle is invalid!");
	}

	if (!bgfx::isValid(program)) {
		SDL_Log("Program is invalid!");
	}

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
	const bx::Vec3 at  = { 0.0f, 0.0f,   0.0f };
	const bx::Vec3 eye = { 0.0f, 0.0f, -10.0f };


	float view_matrix[16];
	bx::mtxLookAt(view_matrix, eye, at);

	float projection_matrix[16];
	bx::mtxProj(projection_matrix, 60.0f, float(w_width)/float(w_height), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
	bgfx::setViewTransform(0, view_matrix, projection_matrix);


	bgfx::setViewRect(0, 0, 0, w_width, w_height);
	bgfx::touch(0);
	bgfx::dbgTextClear();
	bgfx::dbgTextPrintf(0, 1, 0x4f, "fps: %.2f", fps);
	bgfx::dbgTextPrintf(0, 2, 0x2f, "Render Backend: %s", renderBackendStr);
	bgfx::dbgTextPrintf(0, 4, 0x3f, "View size: %dx%d", w_width, w_height);

	float mtx[16];
	float time = SDL_GetTicks() / 1000.0f;
	bx::mtxRotateXY(mtx, 45.0f-fmodf(time * 2.0f * bx::kPi * 0.1f, 2.0f * bx::kPi), -45.0f-fmodf(time * 1.5f * bx::kPi * 0.1f, 2.0f * bx::kPi));
	bgfx::setTransform(mtx);

	bgfx::setVertexBuffer(0, vbh);
	bgfx::setIndexBuffer(ibh);
	bgfx::setState(BGFX_STATE_DEFAULT);
	bgfx::submit(0, program);

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

	bgfx::destroy(vbh);
	bgfx::destroy(ibh);
	bgfx::destroy(program);
	bgfx::shutdown();

	SDL_DestroyWindow(window);
	SDL_Quit();

	SDL_Log("shutdown succeded");
#ifdef __ANDROID__
	LOGI("shutdown succeded");
#endif
}
