# GameCosmo 🚀

> High-performance 2D Space Arcade & Custom Game Engine written in Modern C++17 with OpenGL, GLSL Shaders, and FreeType.

[![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![OpenGL](https://img.shields.io/badge/OpenGL-3.3%2B-green.svg)](https://www.opengl.org/)
[![CMake](https://img.shields.io/badge/Build-CMake-orange.svg)](https://cmake.org/)
[![License](https://img.shields.io/badge/License-MIT-purple.svg)](LICENSE)

---

## 🎮 Overview

**GameCosmo** is a dynamic 2D retro space shooter developed from the ground up using custom graphics subsystems. Instead of relying on ready-made game engines (Unity/Unreal/Godot), the core rendering pipeline, shader management, resource caches, collision detection, and entity lifecycle were designed and implemented directly using C++ and OpenGL.

### Key Gameplay Features
- **Player Mechanics**: Smooth vertical movement, standard blaster shots, and secondary spread-shot power-ups.
- **Dynamic Shield System**: Timed energy shield with visual feedback protecting against enemy fire.
- **Enemy AI & Waves**: Distinct enemy unit archetypes with automated bullet firing and movement routines.
- **Boss Fight & State Machine**: Multi-stage boss encounters powered by an internal finite state machine (Entering, Attack Phases, Enrage Mode).
- **Power-Up Drops**: Dynamic drop system for shields, rapid fire, and extra lives.
- **Score & HUD Rendering**: On-screen dynamic text and status display using FreeType-rasterized TrueType fonts (`Nasalization.otf`).

---

## 🏗 Engine Architecture & Technical Highlights

```
GameCosmo/
├── CMakeLists.txt              # Cross-platform build script with automated asset copying
├── libs/                       # Bundled third-party libraries (GLFW, GLAD, GLM, FreeType, stb)
├── fonts/                      # TrueType / OpenType font assets
├── shaders/                    # GLSL Vertex & Fragment shaders
│   ├── sprite.vert / .frag     # 2D Orthographic batched sprite pipeline
│   └── text.vert / .frag       # FreeType glyph texture coordinate rendering
├── textures/                   # Sprite atlases and background artwork
└── src/                        # Core Engine Architecture
    ├── main.cpp                # Application entry point & game loop
    ├── game.h / game.cpp       # Main Game class: state machine, input handling, update/render cycle
    ├── game_object.h / .cpp    # Base entity class (transform, velocity, color, collider)
    ├── player.h / player.cpp   # Player entity with shield & spread-shot mechanics
    ├── enemy.h / boss.h / .cpp # Enemy hierarchy & Boss state machine
    ├── bullet.h / enemy_bullet.h# Projectile subsystems & trajectory logic
    ├── power_up.h              # Power-up items and modifier timers
    ├── sprite_renderer.h / .cpp# Quad VAO/VBO batching with GLSL matrix transforms
    ├── text_renderer.h / .cpp  # Dynamic font glyph rasterization via FreeType 2
    ├── resource_manager.h / .cpp# Centralized cache for shaders, textures, and assets
    └── shader.h / shader.cpp   # Modern shader compilation, linking & uniform setters
```

### Architectural Highlights
- **SpriteRenderer**: Uses normalized unit quad geometry `(0..1, 0..1)` transformed via an orthographic projection and model matrices in GLSL, minimizing dynamic buffer uploads.
- **ResourceManager**: Static cache pattern preventing redundant GPU texture/shader uploads across frames.
- **TextRenderer**: Pre-generates character glyph textures into OpenGL 8-bit alpha textures using the FreeType library and renders them with sub-pixel alignment.
- **Separation of Concerns**: Entity updates (`DoCollisions`, `Update`) are decoupled from rendering passes (`Render`).

---

## 🕹 Controls

| Action | Primary Key | Alternative Key |
| :--- | :--- | :--- |
| **Move Up** | `W` | `Up Arrow` |
| **Move Down** | `S` | `Down Arrow` |
| **Fire Blaster** | `Space` | `Right Shift` |
| **Select Menu Option** | `1`, `2`, `3` | `Enter` |
| **Pause / Return** | `Escape` | — |

---

## ⚙️ Build & Installation

### Requirements
- **OS**: Windows 10/11 (MSVC 2022 or MinGW-w64)
- **Compiler**: C++17 compatible (`MSVC >= 19.28`, `GCC >= 9`, or `Clang >= 10`)
- **Build System**: CMake 3.10+
- **Graphics**: OpenGL 3.3 compatible GPU

### Compiling with CMake (Command Line)

```bash
# 1. Clone repository
git clone https://github.com/YOUR_USERNAME/GameCosmo.git
cd GameCosmo

# 2. Generate build directory
cmake -B build -S .

# 3. Build project
cmake --build build --config Release

# 4. Run executable
./build/Release/GameCosmo.exe
```

*Note: Shaders, textures, fonts, and required `.dll` files are automatically copied to the executable directory upon build completion.*

### Opening in Visual Studio 2022
1. Open Visual Studio $\rightarrow$ **Open a Local Folder** $\rightarrow$ select `GameCosmo`.
2. Let CMake configure the target.
3. Select `GameCosmo.exe` in the launch target dropdown and press **F5**.

---

## 📜 License
This project is licensed under the MIT License.
