#include <SDL3/SDL_main.h>
#include <app.hpp>

App app;

#ifdef __EMSCRIPTEN__
  void emscriptenMainLoop() {
    if (!app.running) {
      emscripten_cancel_main_loop();
      app.shutdown();
      return;
    }
    app.mainLoop();
  } 
#endif

int main(int argc, char *argv[])
{
	app = App();
	if (!app.initialize())
	{
		SDL_Log("App initialization failed");
		return 1;
	}

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(emscriptenMainLoop, 0, 1);
#else
	while (app.running)
	{
		app.mainLoop();
	}
#endif
	app.shutdown();

	return 0;
}