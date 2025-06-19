#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <iostream>

#include <chrono>
#if defined(__APPLE__)
	#include "TargetConditionals.h"
#endif


#ifdef __ANDROID__
	#include <android_native_app_glue.h>
	#include <android/log.h>
#endif


#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

int AppMain() {
	
#ifdef __ANDROID__
	__android_log_print(ANDROID_LOG_INFO, "iLuma", "AppMain() starting...");
#endif

#ifdef __ANDROID__
	__android_log_print(ANDROID_LOG_INFO, "iLuma", "Init SDL starting...");
#endif

	if (SDL_Init(SDL_INIT_VIDEO) == false) {
			SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
			#ifdef __ANDROID__
			__android_log_print(ANDROID_LOG_INFO, "iLuma", "SDL initialized successfully...");
			#endif
			return 1;
	}

#ifdef __ANDROID__
	__android_log_print(ANDROID_LOG_INFO, "iLuma", "SDL initialized successfully...");
#endif
	

	
	SDL_Window* window = SDL_CreateWindow("BGFX + SDL3 Window",
		SCREEN_WIDTH, SCREEN_HEIGHT, 0);

	if (!window) {
		SDL_Log("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	} else {
		SDL_Log("Window created successfully.");
	} 
#ifdef __ANDROID__
	__android_log_print(ANDROID_LOG_INFO, "iLuma", "Init Window sucessful...");
#endif

	bgfx::PlatformData pd;

#if defined(_WIN32)

	pd.nwh = (void*)SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
	pd.ndt = NULL;
#elif defined(__linux__)
	pd.nwh = (void*)(uintptr_t)SDL_GetNumberProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
	pd.ndt = (void*)SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_X11_DISPLAY_POINTER, NULL);

// I cannot test if it works on macOS, so I will comment it out for now. UPDATE: I can build, but not run it
#elif defined(__APPLE__) 

	#if TARGET_OS_OSX
		pd.nwh = (void*)SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, NULL);
		pd.ndt = NULL;
		std::cout << "macOS detected, using Cocoa window." << std::endl;
	#endif

#elif defined(__ANDROID__)
	pd.nwh = (void*)SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_ANDROID_WINDOW_POINTER, NULL);
	pd.ndt = NULL;
#else
	SDL_Log("Unsupported platform for bgfx initialization.");
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 1;
#endif

	bgfx::setPlatformData(pd);

	bgfx::Init init;
	init.type = bgfx::RendererType::Count;
	init.platformData.nwh = pd.nwh;
	init.platformData.ndt = pd.ndt;
	init.resolution.width = SCREEN_WIDTH;
	init.resolution.height = SCREEN_HEIGHT;
	init.resolution.reset = BGFX_RESET_VSYNC;
	// init.debug = true;  // Enable debug mode


	if (!bgfx::init(init)) {
		std::cout << "Couldn't initialize bgfx" << std::endl;
		return 1;

	} else {
		std::cout << "bgfx initialized successfully." << std::endl;
		
	}
#ifdef __ANDROID__
	__android_log_print(ANDROID_LOG_INFO, "iLuma", "Init bgfx sucessful...");
#endif

	bgfx::setDebug(BGFX_DEBUG_TEXT);
	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030FF, 1.0f, 0);

	bool running = true;
	SDL_Event event;

	auto lastTime = std::chrono::steady_clock::now();
	int frames = 0;
	float fps = 0.0f;

	bgfx::RendererType::Enum renderBackend = bgfx::getRendererType();

	const char* renderBackendStr = bgfx::getRendererName(renderBackend);

	while (running) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_EVENT_QUIT:
			case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
				running = false;
				break;
			}
		}
		if (!running)
			break;  // break the frame loop before calling bgfx stuff
		
		bgfx::setViewRect(0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		bgfx::touch(0);
		bgfx::dbgTextClear();
		bgfx::dbgTextPrintf(0, 1, 0x4f, "fps: %.2f" , fps);
		bgfx::dbgTextPrintf(0, 2, 0x2f, "Render Backend: %s" , renderBackendStr);
		bgfx::frame();

		frames++;
		auto now = std::chrono::steady_clock::now();
		std::chrono::duration<float> elapsed = now - lastTime;

		if (elapsed.count() >= 1.0f) {
				fps = frames / elapsed.count();
				frames = 0;
				lastTime = now;
		}
		// SDL_Delay(16);  // Simulate frame delay (60 FPS)

	}
	bgfx::frame();
	bgfx::shutdown();

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

#ifndef __ANDROID__
int main(int argc, char* argv[]) {
	return AppMain();
}
#else

extern "C" void android_main(struct android_app* app) {
    __android_log_print(ANDROID_LOG_INFO, "iLuma", "android_main entered!");

    SDL_SetMainReady();

    int events;
    struct android_poll_source* source;

    while (true) {
        while (ALooper_pollOnce(-1, NULL, &events, (void**)&source) >= 0) {
            if (source != NULL) {
                source->process(app, source);
            }
            if (app->destroyRequested) {
                __android_log_print(ANDROID_LOG_INFO, "iLuma", "App destroy requested. Exiting.");
                return;
            }
        }
        if (app->window != nullptr) {
            __android_log_print(ANDROID_LOG_INFO, "iLuma", "Window is ready, starting AppMain");
            break;
        }
    }

    int ret = AppMain();
    __android_log_print(ANDROID_LOG_INFO, "iLuma", "AppMain() returned with %d", ret);
}


// END_INCLUDE(all)
#endif