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

int main(int argc, char *argv[])
{

	if (SDL_Init(SDL_INIT_VIDEO) == false)
	{
		SDL_Log("SDL_Init failed: %s", SDL_GetError());
		return 1;
	}
	SDL_Log("SDL_Init succeded");

	SDL_Window *window = SDL_CreateWindow("BGFX + SDL3 Window",
																				SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	if (!window)
	{
		SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
		SDL_Quit();
		return 1;
	}
	SDL_Log("SDL_CreateWindow succeded");

	bgfx::PlatformData pd;

#if defined(__ANDROID__)
	pd.nwh = app->window; // Use the native window from the Android app
	pd.ndt = NULL;
#elif defined(_WIN32)

	pd.nwh = (void *)SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
	pd.ndt = NULL;
#elif defined(__linux__)
	pd.nwh = (void *)(uintptr_t)SDL_GetNumberProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
	pd.ndt = (void *)SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_X11_DISPLAY_POINTER, NULL);

// I cannot test if it works on macOS, so I will comment it out for now. UPDATE: I can build, but not run it
#elif defined(__APPLE__)

#if TARGET_OS_OSX
	pd.nwh = (void *)SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, NULL);
	pd.ndt = NULL;
#endif

#else
	SDL_Log("Unsupported platform for bgfx initialization.");
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 1;
#endif

	SDL_Log("Setting PlatformData for bgfx...");
	bgfx::setPlatformData(pd);
	SDL_Log("Setting PlatformData for bgfx succeded..\nInitializing bgfx...");

	bgfx::Init init;
	init.type = bgfx::RendererType::Count;
	init.platformData.nwh = pd.nwh;
	init.platformData.ndt = pd.ndt;
	init.resolution.width = SCREEN_WIDTH;
	init.resolution.height = SCREEN_HEIGHT;
	init.resolution.reset = BGFX_RESET_VSYNC;
	// init.debug = true;  // Enable debug mode
	SDL_Log("Initializing bgfx succeded...");

	if (!bgfx::init(init))
	{
		SDL_Log("bgfx::init unsuccessful");
		return 1;
	}

	SDL_Log("bgfx::init succeded");

	bgfx::setDebug(BGFX_DEBUG_TEXT);
	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030FF, 1.0f, 0);

	bool running = true;
	SDL_Event event;

	auto lastTime = std::chrono::steady_clock::now();
	int frames = 0;
	float fps = 0.0f;

	bgfx::RendererType::Enum renderBackend = bgfx::getRendererType();

	const char *renderBackendStr = bgfx::getRendererName(renderBackend);

	auto start = std::chrono::steady_clock::now();
	SDL_Log("Main loop incomig, render backend: %s", renderBackendStr);

	while (running)
	{
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
		if (!running)
			break; // break the frame loop before calling bgfx stuff

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
		SDL_Delay(16);  // Simulate frame delay (60 FPS)

		if (std::chrono::duration_cast<std::chrono::seconds>(now - start).count() > 20) {
			running = false; // Stop after 20 seconds for upload
		}
	}
	bgfx::frame();
	bgfx::shutdown();

	SDL_DestroyWindow(window);
	SDL_Quit();

	SDL_Log("shutdown succeded");

	return 0;
}