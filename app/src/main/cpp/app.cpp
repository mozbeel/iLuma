#include <app.hpp>


App::App() {
  return;
}

bool App::initializeFlatContext() {
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
		}
	#endif


	window = SDL_CreateWindow("BGFX + SDL3 Window",
																				w_width, w_height, 
	#if defined(__ANDROID__) || defined(TARGET_OS_IPHONE) 
		SDL_WINDOW_FULLSCREEN
	#else 
		0
	#endif
	);
																	
	if (!window)
	{
		SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
		SDL_Quit();
		return false;
	}
	SDL_Log("SDL_CreateWindow succeded");	

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
			if(!pd.nwh) {
				SDL_Log("Couldn't get nwh");
			}
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

	return true;
}
bool App::initializeBGFX() {
	SDL_Log("Setting PlatformData for bgfx...");
		
		bgfx::setPlatformData(pd);
		SDL_Log("Setting PlatformData for bgfx succeded..\nInitializing bgfx...");
		#ifdef __ANDROID__
		LOGI("Setting PlatformData for bgfx succeded..\nInitializing bgfx...");
		#endif
		
		bgfx::Init init;
	#ifndef ILUMA_VR
		init.type = bgfx::RendererType::Count;
	#else
		init.type = bgfx::RendererType::Vulkan;
	#endif
		init.platformData.nwh = pd.nwh;
		init.platformData.ndt = pd.ndt;
		init.resolution.width = w_width;
		init.resolution.height = w_height;
		init.resolution.reset = BGFX_RESET_VSYNC;
		init.debug = true;  // Enable debug mode
		
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
}

#ifdef ILUMA_VR
bool App::initializeVRContext() {
	const char* extensions[] = { 
		XR_KHR_VULKAN_ENABLE_EXTENSION_NAME
	};


	#ifdef __ANDROID__
		LOGI("Checkpoint 1");
	#endif
	SDL_Log("Checkpoint 1");


	XrApplicationInfo appInfo = {};
	strncpy(appInfo.applicationName, "OpenXR + bgfx", XR_MAX_APPLICATION_NAME_SIZE);
	appInfo.applicationVersion = 1;
	strncpy(appInfo.engineName, "OpenXR Engine + bgfx (iLuma)", XR_MAX_ENGINE_NAME_SIZE);
	appInfo.engineVersion = 1;
	appInfo.apiVersion = XR_MAKE_VERSION(1, 0, 0);
	
	XrInstanceCreateInfo instanceCreateInfo{XR_TYPE_INSTANCE_CREATE_INFO};
	instanceCreateInfo.applicationInfo        = appInfo;
	instanceCreateInfo.enabledExtensionCount  = 1;
	instanceCreateInfo.enabledExtensionNames  = extensions;
	instanceCreateInfo.enabledApiLayerCount   = 0;
	instanceCreateInfo.enabledApiLayerNames   = nullptr;
	instanceCreateInfo.next                   = nullptr;

	#ifdef __ANDROID__
		LOGI("Checkpoint 2");
	#endif
	SDL_Log("Checkpoint 2");

	CHECK_XR(xrCreateInstance(&instanceCreateInfo, &xrInstance));

	#ifdef __ANDROID__
		LOGI("Created XR Instance");
	#endif
	SDL_Log("Created XR Instance");
	
	XrSystemGetInfo systemInfo{ XR_TYPE_SYSTEM_GET_INFO };
	systemInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
	CHECK_XR(xrGetSystem(xrInstance, &systemInfo, &xrSystemId));
	#ifdef __ANDROID__
		LOGI("Got for XR SystemInfo");
	#endif
	SDL_Log("Got for XR SystemInfo");

	PFN_xrGetVulkanGraphicsRequirementsKHR fnGetReqs = nullptr;
	xrGetInstanceProcAddr(
			xrInstance,
			"xrGetVulkanGraphicsRequirementsKHR",
			(PFN_xrVoidFunction*)&fnGetReqs
	);

	// Query the requirements
	XrGraphicsRequirementsVulkanKHR requirements{XR_TYPE_GRAPHICS_REQUIREMENTS_VULKAN_KHR};
	CHECK_XR(fnGetReqs(xrInstance, xrSystemId, &requirements));

// Now requirements.minApiVersionSupported and maxApiVersionSupported are set
			// Query the requirements
	XrGraphicsRequirementsVulkanKHR requirements{XR_TYPE_GRAPHICS_REQUIREMENTS_VULKAN_KHR};
	CHECK_XR(fnGetReqs(xrInstance, xrSystemId, &requirements));

	VkApplicationInfo vkAppInfo {VK_STRUCTURE_TYPE_APPLICATION_INFO};
	appInfo.apiVersion = requirements.minApiVersionSupported;

	VkInstanceCreateInfo vkInstInfo {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
	vkInstInfo.pApplicationInfo = &vkAppInfo;
	
	VkInstance vkInstance;
	vkCreateInstance(&vkInstInfo, nullptr, &vkInstance);

	uint32_t gpuCount = 0;
	vkEnumeratePhysicalDevices(vkInstance, &gpuCount, nullptr);
	std::vector<VkPhysicalDevice> gpus(gpuCount);
	vkEnumeratePhysicalDevices(vkInstance, &gpuCount, gpus.data());
	VkPhysicalDevice vkPhysicalDevice = gpus[0]; // pick first or test features

	// Ensure Vulkan handles are initialized and compatible with OpenXR requirements

	// XrGraphicsBindingVulkanKHR graphicsBinding = {XR_TYPE_GRAPHICS_BINDING_VULKAN_KHR};
	// // graphicsBinding.next = nullptr;
	// graphicsBinding.device = vulkanHandles.device;
	// graphicsBinding.physicalDevice = vulkanHandles.physicalDevice;
	// graphicsBinding.instance = vulkanHandles.instance;
	// graphicsBinding.queueFamilyIndex = vulkanHandles.queueFamilyIndex;
	// graphicsBinding.queueIndex = 0;

	// XrSessionCreateInfo sessionCreateInfo{XR_TYPE_SESSION_CREATE_INFO};
	// sessionCreateInfo.next = &graphicsBinding; // graphicsBinding must be XrGraphicsBindingVulkanKHR and fully filled
	// sessionCreateInfo.systemId = xrSystemId;
	// sessionCreateInfo.createFlags = 0;
	
	
	// CHECK_XR(xrCreateSession(xrInstance, &sessionCreateInfo, &xrSession));
	#ifdef __ANDROID__
		LOGI("Created XrSession");
	#endif
	SDL_Log("Created XrSession");

	XrReferenceSpaceCreateInfo spaceCreateInfo = {XR_TYPE_REFERENCE_SPACE_CREATE_INFO};
	spaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
	spaceCreateInfo.poseInReferenceSpace = { {0,0,0,1}, {0,0,0}};

	CHECK_XR(xrCreateReferenceSpace(xrSession, &spaceCreateInfo, &xrAppSpace));
	#ifdef __ANDROID__
		LOGI("Created XrSpace");
	#endif
	SDL_Log("Created XrSpace");

	uint32_t viewCount;
	CHECK_XR(xrEnumerateViewConfigurationViews(xrInstance, xrSystemId, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, 0, &viewCount, nullptr));

	std::vector<XrViewConfigurationView> configViews(viewCount, {XR_TYPE_VIEW_CONFIGURATION_VIEW});
	CHECK_XR(xrEnumerateViewConfigurationViews(xrInstance, xrSystemId,
			XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, viewCount, &viewCount, configViews.data()));

	swapchainWidth = configViews[0].recommendedImageRectWidth;
	swapchainHeight = configViews[0].recommendedImageRectHeight;

	swapchains.resize(viewCount);
	swapchainImages.resize(viewCount);

	for (uint32_t i = 0; i < viewCount; ++i) {
			XrSwapchainCreateInfo sci{XR_TYPE_SWAPCHAIN_CREATE_INFO};
			sci.arraySize = 1;
			sci.format = VK_FORMAT_R8G8B8A8_SRGB;
			sci.width = configViews[i].recommendedImageRectWidth;
			sci.height = configViews[i].recommendedImageRectHeight;
			sci.mipCount = 1;
			sci.faceCount = 1;
			sci.sampleCount = 1;
			sci.usageFlags = XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;

			CHECK_XR(xrCreateSwapchain(xrSession, &sci, &swapchains[i]));

			// Get images
			uint32_t imageCount;
			CHECK_XR(xrEnumerateSwapchainImages(swapchains[i], 0, &imageCount, nullptr));

			swapchainImages[i].resize(imageCount, {XR_TYPE_SWAPCHAIN_IMAGE_VULKAN_KHR});
			CHECK_XR(xrEnumerateSwapchainImages(swapchains[i], imageCount,
					&imageCount, (XrSwapchainImageBaseHeader*)swapchainImages[i].data()));
	}
	#ifdef __ANDROID__
		LOGI("Created Swapchains");
	#endif
	SDL_Log("Created Swapchains");
	return true;
}

#endif

bool App::initialize() {
	if (!initializeFlatContext()) {
		return false;
	}

	#ifdef ILUMA_VR
		if (!initializeVRContext()) {
			return false;
		}
		
	#endif


  return true;
}

void App::draw(bgfx::ViewId viewId, float* view_matrix, float* projection_matrix) {
	bgfx::setViewTransform(viewId, view_matrix, projection_matrix);
	bgfx::setViewRect(viewId, 0, 0, w_width, w_height);
	bgfx::touch(viewId);
	bgfx::dbgTextClear();
	bgfx::dbgTextPrintf(0, 1, 0x4f, "fps: %.2f", fps);
	bgfx::dbgTextPrintf(0, 2, 0x2f, "Render Backend: %s", renderBackendStr);
	bgfx::dbgTextPrintf(0, 4, 0x3f, "View size: %dx%d", w_width, w_height);

	float mtx[16];
	float time = SDL_GetTicks() / 1000.0f;
	bx::mtxRotateXY(mtx,
									45.0f - fmodf(time * 2.0f * bx::kPi * 0.1f, 2.0f * bx::kPi),
									-45.0f - fmodf(time * 1.5f * bx::kPi * 0.1f, 2.0f * bx::kPi));
	bgfx::setTransform(mtx);

	bgfx::setVertexBuffer(0, vbh);
	bgfx::setIndexBuffer(ibh);
	bgfx::setState(BGFX_STATE_DEFAULT);
	bgfx::submit(viewId, program);
}

#ifndef ILUMA_VR
	void App::renderFlat() {
		const bx::Vec3 at = {0.0f, 0.0f, 0.0f};
		const bx::Vec3 eye = {0.0f, 0.0f, -10.0f};

		float view_matrix[16];
		bx::mtxLookAt(view_matrix, eye, at);

		float projection_matrix[16];
		bx::mtxProj(projection_matrix, 60.0f, float(w_width) / float(w_height), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
		draw(0, view_matrix, projection_matrix)
	}
#else
	void App::renderVR() {
		XrFrameState frameState{XR_TYPE_FRAME_STATE};
		XrFrameWaitInfo waitInfo{XR_TYPE_FRAME_WAIT_INFO};
		CHECK_XR(xrWaitFrame(xrSession, &waitInfo, &frameState));

		XrFrameBeginInfo beginInfo{XR_TYPE_FRAME_BEGIN_INFO};
		CHECK_XR(xrBeginFrame(xrSession, &beginInfo));

		uint32_t viewCount = (uint32_t)swapchains.size();
		std::vector<XrView> views(viewCount, {XR_TYPE_VIEW});

		XrViewLocateInfo locateInfo{XR_TYPE_VIEW_LOCATE_INFO};
		locateInfo.viewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
		locateInfo.displayTime = frameState.predictedDisplayTime; // Should be set to predicted display time
		locateInfo.space = xrAppSpace;

		XrViewState viewState{XR_TYPE_VIEW_STATE};
		uint32_t viewCountOutput;
		CHECK_XR(xrLocateViews(xrSession, &locateInfo, &viewState, viewCount, &viewCountOutput, views.data()));

		// Render for each eye
		std::vector<XrCompositionLayerProjectionView> projectionLayerViews(viewCount);
		
		std::vector<XrSwapchainImageAcquireInfo> acquireInfos(viewCount);
		std::vector<XrSwapchainImageWaitInfo> waitInfos(viewCount);
		std::vector<XrSwapchainImageReleaseInfo> releaseInfos(viewCount);

		for (uint32_t i = 0; i < viewCount; ++i) {
			// Acquire swapchain image for this eye
			uint32_t imageIndex;
			XrSwapchainImageAcquireInfo swapchainImageAcquireInfo {XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO};
			CHECK_XR(xrAcquireSwapchainImage(swapchains[i], &swapchainImageAcquireInfo, &imageIndex));

			XrSwapchainImageWaitInfo swapchainImageWaitInfo = { XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO };
			swapchainImageWaitInfo.timeout = 1000000000;
			CHECK_XR(xrWaitSwapchainImage(swapchains[i], &swapchainImageWaitInfo));

			projectionLayerViews[i].type = XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW;
			projectionLayerViews[i].pose = views[i].pose;
			projectionLayerViews[i].fov  = views[i].fov;
			projectionLayerViews[i].subImage.swapchain = swapchains[i];
			projectionLayerViews[i].subImage.imageRect.offset = { 0, 0 };
			projectionLayerViews[i].subImage.imageRect.extent = { static_cast<int>(swapchainWidth), static_cast<int>(swapchainHeight) };
			projectionLayerViews[i].subImage.imageArrayIndex = 0;
			// Setup view & projection matrices from OpenXR data
			float viewMatrix[16];
			float projMatrix[16];

			bx::mtxProj(
				projMatrix,
				90.0f,
				float(swapchainWidth) / float(swapchainHeight),
				0.1f,
				100.0f,
				false
			);

			// Convert OpenXR pose to bx::mtxLookAt or manually copy OpenXR pose matrix here
			// You can use a helper function to convert XrPosef to matrix
			// Here we just set identity for simplicity
			const bx::Vec3 at = {0.0f, 0.0f, 0.0f};
			const bx::Vec3 eye = {0.0f, 0.0f, -10.0f};

			poseToViewMatrix(views[i].pose, viewMatrix);

			// 1. Create a dummy bgfx texture handle (any format is fine; we override it)
			bgfx::TextureHandle colorTex = bgfx::createTexture2D(
				swapchainWidth,
				swapchainHeight,
				false,
				1,
				bgfx::TextureFormat::RGBA8,
				BGFX_TEXTURE_RT
			);
			
			// 2. Replace its internal image with the one OpenXR gave you
			bgfx::overrideInternal(colorTex,  (uintptr_t)swapchainImages[i][imageIndex].image);
			bgfx::FrameBufferHandle fb = bgfx::createFrameBuffer(1, &colorTex, true);
			bgfx::setViewFrameBuffer(i, fb);
			draw(i, viewMatrix, projMatrix);
			bgfx::destroy(fb);
			bgfx::destroy(colorTex);

			// Release image
			XrSwapchainImageReleaseInfo swapchainImageReleaseInfo = {XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};
			CHECK_XR(xrReleaseSwapchainImage(swapchains[i], &swapchainImageReleaseInfo));
		}
		bgfx::frame();
		XrCompositionLayerProjection layer{XR_TYPE_COMPOSITION_LAYER_PROJECTION};
		layer.space = xrAppSpace;
		layer.viewCount = projectionLayerViews.size();
		layer.views = projectionLayerViews.data();

		XrCompositionLayerBaseHeader* layers[] = {(XrCompositionLayerBaseHeader*)&layer};

		XrFrameEndInfo frameEndInfo{XR_TYPE_FRAME_END_INFO};
		frameEndInfo.displayTime = frameState.predictedDisplayTime;
		frameEndInfo.environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
		frameEndInfo.layerCount = 1;
		frameEndInfo.layers = layers;

		CHECK_XR(xrEndFrame(xrSession, &frameEndInfo));
	}
#endif

bool App::mainLoop() {
	// Non-VR SDL + bgfx render path
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_EVENT_QUIT:
			case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
			running = false;
			break;
		}
	}

	#ifdef ILUMA_VR
		renderVR();
	#else
		renderFlat();
	#endif

    frames++;
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<float> elapsed = now - lastTime;

    if (elapsed.count() >= 1.0f) {
        fps = frames / elapsed.count();
        frames = 0;
        lastTime = now;
    }

    return running;
}

void App::shutdown() {
	bgfx::frame();

	bgfx::destroy(vbh);
	bgfx::destroy(ibh);
	bgfx::destroy(program);
	bgfx::shutdown();

	SDL_Quit();
	SDL_DestroyWindow(window);

#ifdef ILUMA_VR
    if (xrSession != XR_NULL_HANDLE) {
        xrDestroySession(xrSession);
    }
    if (xrInstance != XR_NULL_HANDLE) {
        xrDestroyInstance(xrInstance);
    }
#endif
	SDL_Log("shutdown succeded");
#ifdef __ANDROID__
	LOGI("shutdown succeded");
#endif
}
