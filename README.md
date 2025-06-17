#<p align="center">
  # iLuma
  ### A powerful cross-platform game framework
</p>

## About the Project

iLuma is a modern, high-performance game framework (3D and 2D) designed for rapid development of games and interactive applications on all major desktop and mobile platforms (very soon). It's designed to be used very close to functionality of a full game engine like Unity, but also gives you the control to dive into the nitty-gritty low-level code. Built on top of bgfx and SDL3, iLuma offers a robust, flexible, and modular foundation for creating games that run seamlessly across Windows, Linux, macOS, iOS (very soon), and Android (very soon).

## Features

- ✨ Cross-platform support: Build games for desktop (Windows, Linux, macOS) and mobile (iOS, Android; no support yet but soon) from a single codebase.
- 🚀 Powered by bgfx for advanced rendering and SDL3 for input, audio, and window management.
- 🎮 Modular architecture: Plug in only the components you need.
- 📱 Touch and sensor input support for mobile devices (not yet).
- 🛠️ Easy integration with popular game development tools and workflows.
- 📂 Open source and actively developed.

## Supported Platforms

- Windows
- Linux
- macOS
- iOS (soon)
- Android (soon)

## Getting Started

1. Clone the repository:
   ```bash
   git clone https://github.com/mozbeel/iLuma.git
   ```
2. Follow the platform-specific setup instructions in the `docs/` folder. (not available yet)
3. Build and run the sample projects to see iLuma in action.

## Example (soon going to be implemented)

```cpp
#include <iLuma/Game.h>

class MyGame : public iLuma::Game {
    void onInit() override {
        // Initialize your game here
    }
    void onUpdate(float dt) override {
        // Game update logic
    }
};

int main() {
    MyGame game;
    game.run();
    return 0;
}
```

## Contributing

Contributions are welcome! Please see the CONTRIBUTING.md for guidelines. (not yet)

## License

This project is licensed under the MIT License. (not yet)

---
