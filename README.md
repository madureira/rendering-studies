# Rendering Studies

> Real-time rendering demos written in C++11 and OpenGL — runs natively on desktop and in the browser via WebAssembly.

![C++11](https://img.shields.io/badge/C%2B%2B-11-00599C?style=flat-square&logo=c%2B%2B&logoColor=white)
![OpenGL 4.1](https://img.shields.io/badge/OpenGL-4.1-5586A4?style=flat-square&logo=opengl&logoColor=white)
![WebGL 2.0](https://img.shields.io/badge/WebGL-2.0-990000?style=flat-square&logo=webgl&logoColor=white)
![WebAssembly](https://img.shields.io/badge/WebAssembly-Emscripten-654FF0?style=flat-square&logo=webassembly&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-3.24+-064F8C?style=flat-square&logo=cmake&logoColor=white)
![macOS](https://img.shields.io/badge/macOS-000000?style=flat-square&logo=apple&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?style=flat-square&logo=linux&logoColor=black)
![Windows](https://img.shields.io/badge/Windows-0078D4?style=flat-square&logo=data:image/svg+xml;base64,PHN2ZyByb2xlPSJpbWciIHZpZXdCb3g9IjAgMCAyNCAyNCIgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIj48cGF0aCBkPSJNMCAwaDExLjM3N3YxMS4zNzJIMHptMTIuNjIzIDBIMjR2MTEuMzcySDEyLjYyM3pNMCAxMi42MjNoMTEuMzc3VjI0SDB6bTEyLjYyMyAwSDI0VjI0SDEyLjYyM3oiIGZpbGw9IndoaXRlIi8+PC9zdmc+)

---

## Overview

A collection of real-time 3D rendering demos built from scratch in C++11 and OpenGL, covering foundational techniques in computer graphics — from basic geometry and lighting models to normal mapping, environment mapping, and tessellation shaders.

Each demo is selectable at runtime from a settings panel with no restart needed. The same codebase compiles to a native binary using OpenGL 4.1 Core, and to a static web page using WebGL 2.0 via Emscripten, making every demo instantly accessible in the browser without any installation.

![Demo](docs/video_demo.gif)

[![Live Demo](docs/live_demo_badge.svg)](https://madureira.github.io/rendering-studies)

### Demos

| Demo | Technique | Platform |
| --- | --- | --- |
| **Triangle** | Hello world — basic vertex pipeline | ![Desktop](https://img.shields.io/badge/Desktop-555?style=flat-square&logo=data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNCAyNCIgZmlsbD0id2hpdGUiPjxwYXRoIGQ9Ik0yMSAySDNjLTEuMSAwLTIgLjktMiAydjEyYzAgMS4xLjkgMiAyIDJoN2wtMiAzdjFoOHYtMWwtMi0zaDdjMS4xIDAgMi0uOSAyLTJWNGMwLTEuMS0uOS0yLTItMnptMCAxMkgzVjRoMTh2MTB6Ii8+PC9zdmc+) ![Web](https://img.shields.io/badge/Web-654FF0?style=flat-square&logo=webassembly&logoColor=white) |
| **Cube** | Indexed geometry, depth testing | ![Desktop](https://img.shields.io/badge/Desktop-555?style=flat-square&logo=data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNCAyNCIgZmlsbD0id2hpdGUiPjxwYXRoIGQ9Ik0yMSAySDNjLTEuMSAwLTIgLjktMiAydjEyYzAgMS4xLjkgMiAyIDJoN2wtMiAzdjFoOHYtMWwtMi0zaDdjMS4xIDAgMi0uOSAyLTJWNGMwLTEuMS0uOS0yLTItMnptMCAxMkgzVjRoMTh2MTB6Ii8+PC9zdmc+) ![Web](https://img.shields.io/badge/Web-654FF0?style=flat-square&logo=webassembly&logoColor=white) |
| **Textured Cube** | UV mapping, texture sampling | ![Desktop](https://img.shields.io/badge/Desktop-555?style=flat-square&logo=data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNCAyNCIgZmlsbD0id2hpdGUiPjxwYXRoIGQ9Ik0yMSAySDNjLTEuMSAwLTIgLjktMiAydjEyYzAgMS4xLjkgMiAyIDJoN2wtMiAzdjFoOHYtMWwtMi0zaDdjMS4xIDAgMi0uOSAyLTJWNGMwLTEuMS0uOS0yLTItMnptMCAxMkgzVjRoMTh2MTB6Ii8+PC9zdmc+) ![Web](https://img.shields.io/badge/Web-654FF0?style=flat-square&logo=webassembly&logoColor=white) |
| **Simple Mesh** | OBJ / FBX model loading via Assimp | ![Desktop](https://img.shields.io/badge/Desktop-555?style=flat-square&logo=data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNCAyNCIgZmlsbD0id2hpdGUiPjxwYXRoIGQ9Ik0yMSAySDNjLTEuMSAwLTIgLjktMiAydjEyYzAgMS4xLjkgMiAyIDJoN2wtMiAzdjFoOHYtMWwtMi0zaDdjMS4xIDAgMi0uOSAyLTJWNGMwLTEuMS0uOS0yLTItMnptMCAxMkgzVjRoMTh2MTB6Ii8+PC9zdmc+) ![Web](https://img.shields.io/badge/Web-654FF0?style=flat-square&logo=webassembly&logoColor=white) |
| **Gouraud Shading** | Per-vertex lighting | ![Desktop](https://img.shields.io/badge/Desktop-555?style=flat-square&logo=data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNCAyNCIgZmlsbD0id2hpdGUiPjxwYXRoIGQ9Ik0yMSAySDNjLTEuMSAwLTIgLjktMiAydjEyYzAgMS4xLjkgMiAyIDJoN2wtMiAzdjFoOHYtMWwtMi0zaDdjMS4xIDAgMi0uOSAyLTJWNGMwLTEuMS0uOS0yLTItMnptMCAxMkgzVjRoMTh2MTB6Ii8+PC9zdmc+) ![Web](https://img.shields.io/badge/Web-654FF0?style=flat-square&logo=webassembly&logoColor=white) |
| **Phong Shading** | Per-fragment lighting | ![Desktop](https://img.shields.io/badge/Desktop-555?style=flat-square&logo=data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNCAyNCIgZmlsbD0id2hpdGUiPjxwYXRoIGQ9Ik0yMSAySDNjLTEuMSAwLTIgLjktMiAydjEyYzAgMS4xLjkgMiAyIDJoN2wtMiAzdjFoOHYtMWwtMi0zaDdjMS4xIDAgMi0uOSAyLTJWNGMwLTEuMS0uOS0yLTItMnptMCAxMkgzVjRoMTh2MTB6Ii8+PC9zdmc+) ![Web](https://img.shields.io/badge/Web-654FF0?style=flat-square&logo=webassembly&logoColor=white) |
| **Flat Shading** | Face normals, no geometry shader | ![Desktop](https://img.shields.io/badge/Desktop-555?style=flat-square&logo=data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNCAyNCIgZmlsbD0id2hpdGUiPjxwYXRoIGQ9Ik0yMSAySDNjLTEuMSAwLTIgLjktMiAydjEyYzAgMS4xLjkgMiAyIDJoN2wtMiAzdjFoOHYtMWwtMi0zaDdjMS4xIDAgMi0uOSAyLTJWNGMwLTEuMS0uOS0yLTItMnptMCAxMkgzVjRoMTh2MTB6Ii8+PC9zdmc+) ![Web](https://img.shields.io/badge/Web-654FF0?style=flat-square&logo=webassembly&logoColor=white) |
| **Color Coord** | Visualizing UV / position coordinates | ![Desktop](https://img.shields.io/badge/Desktop-555?style=flat-square&logo=data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNCAyNCIgZmlsbD0id2hpdGUiPjxwYXRoIGQ9Ik0yMSAySDNjLTEuMSAwLTIgLjktMiAydjEyYzAgMS4xLjkgMiAyIDJoN2wtMiAzdjFoOHYtMWwtMi0zaDdjMS4xIDAgMi0uOSAyLTJWNGMwLTEuMS0uOS0yLTItMnptMCAxMkgzVjRoMTh2MTB6Ii8+PC9zdmc+) ![Web](https://img.shields.io/badge/Web-654FF0?style=flat-square&logo=webassembly&logoColor=white) |
| **Chroma Depth** | Depth-based color grading | ![Desktop](https://img.shields.io/badge/Desktop-555?style=flat-square&logo=data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNCAyNCIgZmlsbD0id2hpdGUiPjxwYXRoIGQ9Ik0yMSAySDNjLTEuMSAwLTIgLjktMiAydjEyYzAgMS4xLjkgMiAyIDJoN2wtMiAzdjFoOHYtMWwtMi0zaDdjMS4xIDAgMi0uOSAyLTJWNGMwLTEuMS0uOS0yLTItMnptMCAxMkgzVjRoMTh2MTB6Ii8+PC9zdmc+) ![Web](https://img.shields.io/badge/Web-654FF0?style=flat-square&logo=webassembly&logoColor=white) |
| **Bump Mapping** | Normal maps in tangent space | ![Desktop](https://img.shields.io/badge/Desktop-555?style=flat-square&logo=data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNCAyNCIgZmlsbD0id2hpdGUiPjxwYXRoIGQ9Ik0yMSAySDNjLTEuMSAwLTIgLjktMiAydjEyYzAgMS4xLjkgMiAyIDJoN2wtMiAzdjFoOHYtMWwtMi0zaDdjMS4xIDAgMi0uOSAyLTJWNGMwLTEuMS0uOS0yLTItMnptMCAxMkgzVjRoMTh2MTB6Ii8+PC9zdmc+) ![Web](https://img.shields.io/badge/Web-654FF0?style=flat-square&logo=webassembly&logoColor=white) |
| **Anisotropic** | Anisotropic highlight model | ![Desktop](https://img.shields.io/badge/Desktop-555?style=flat-square&logo=data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNCAyNCIgZmlsbD0id2hpdGUiPjxwYXRoIGQ9Ik0yMSAySDNjLTEuMSAwLTIgLjktMiAydjEyYzAgMS4xLjkgMiAyIDJoN2wtMiAzdjFoOHYtMWwtMi0zaDdjMS4xIDAgMi0uOSAyLTJWNGMwLTEuMS0uOS0yLTItMnptMCAxMkgzVjRoMTh2MTB6Ii8+PC9zdmc+) ![Web](https://img.shields.io/badge/Web-654FF0?style=flat-square&logo=webassembly&logoColor=white) |
| **Skybox** | Cubemap environment mapping | ![Desktop](https://img.shields.io/badge/Desktop-555?style=flat-square&logo=data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNCAyNCIgZmlsbD0id2hpdGUiPjxwYXRoIGQ9Ik0yMSAySDNjLTEuMSAwLTIgLjktMiAydjEyYzAgMS4xLjkgMiAyIDJoN2wtMiAzdjFoOHYtMWwtMi0zaDdjMS4xIDAgMi0uOSAyLTJWNGMwLTEuMS0uOS0yLTItMnptMCAxMkgzVjRoMTh2MTB6Ii8+PC9zdmc+) ![Web](https://img.shields.io/badge/Web-654FF0?style=flat-square&logo=webassembly&logoColor=white) |
| **Ripple Effect** | Tessellation shaders (tesc / tese) | ![Desktop](https://img.shields.io/badge/Desktop-555?style=flat-square&logo=data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNCAyNCIgZmlsbD0id2hpdGUiPjxwYXRoIGQ9Ik0yMSAySDNjLTEuMSAwLTIgLjktMiAydjEyYzAgMS4xLjkgMiAyIDJoN2wtMiAzdjFoOHYtMWwtMi0zaDdjMS4xIDAgMi0uOSAyLTJWNGMwLTEuMS0uOS0yLTItMnptMCAxMkgzVjRoMTh2MTB6Ii8+PC9zdmc+) |

---

## Architecture

### Engine

The engine lives in `src/Engine/` and is split into focused subsystems:

| Subsystem | Responsibility |
| --- | --- |
| **Renderer** | Frame orchestration — clears buffers, renders FPS counter and reference grid, controls polygon mode and depth test |
| **Camera** | Euler-angle FPS camera with 6-DOF movement, mouse look, and zoom. Uses `glm::dvec3` for high-precision positioning |
| **Shader** | Compiles and links GLSL programs (vertex + fragment, or full tessellation pipeline). Caches uniform locations to avoid redundant lookups |
| **Texture** | Loads images to the GPU with configurable wrapping, mipmaps, sRGB, and Y-flip |
| **Model** | Loads OBJ and FBX files via Assimp, walks the node hierarchy, and generates a VAO/VBO/EBO per mesh |
| **TextRenderer** | Rasterizes TrueType fonts via Freetype and renders them with a dedicated shader |
| **Window** | GLFW wrapper — multi-monitor support, HiDPI awareness, fullscreen toggle, mouse and keyboard state |
| **UI** | Dear ImGui context and backend lifecycle |
| **FileManager** | Static helpers for reading shader source files and loading/freeing image data via stb_image |

### Demo system

Each demo focuses on a single rendering technique — from basic geometry to tessellation shaders. Every demo is self-contained in `src/Demos/` and ships with its own GLSL shaders, which live alongside the project assets in `assets/shaders/`.

### Single codebase, two platforms

The same source compiles to a native binary (OpenGL 4.1 Core) and to a WebAssembly page (WebGL 2.0) via Emscripten.

| | Desktop | Web |
| --- | --- | --- |
| GL version | OpenGL 4.1 Core | OpenGL ES 3.0 |
| GLSL | `#version 410 core` | `#version 300 es` |
| Event loop | `while (window.IsOpened())` | `emscripten_set_main_loop_arg` |
| Assets | Copied to build directory | Preloaded into the WASM virtual filesystem |
| GLFW / Freetype | Compiled from submodules | Emscripten built-in ports |
| Wireframe mode | Supported | Disabled (not in WebGL 2) |

All vendored dependencies are compiled as **static libraries** (`BUILD_SHARED_LIBS=OFF`), producing a single self-contained binary on desktop and a single `.wasm` bundle on the web.

---

## Requirements

### Desktop — macOS · Linux · Windows

- [CMake](https://cmake.org/download/) 3.24+
- zlib (available by default on macOS and most Linux distros; on Windows install via your package manager)

### Web

- [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html)

---

## 🔨 Building

### 1. Clone the repository

```sh
git clone --recurse-submodules https://github.com/madureira/rendering-studies.git
```

If you already cloned without submodules, initialize them manually:

```sh
git submodule update --init --recursive
```

> **Note:** The first clone may take a few minutes — Assimp is a large dependency (~300 MB).

### 🖥️ Desktop

```sh
# Debug
./debug_build.sh

# Release
./release_build.sh
```

> No package manager required — all dependencies are compiled from source via git submodules.

### 🌍 Web (WebAssembly + WebGL 2.0)

```sh
source ~/emsdk/emsdk_env.sh
./web_build.sh
```

Preview locally:

```sh
python3 -m http.server 8080 --directory build/Web
# → http://localhost:8080
```

Deploy to GitHub Pages:

```sh
./web_deploy.sh
# Copies output to docs/ — commit and push to activate Pages
```

---

## ⚙️ Configuration

Edit `config.ini` to change the window size, monitor, and VSync:

```ini
window_title=Rendering Studies
window_width=1200
window_height=900
vsync_on=true
monitor_index=0
```

---

## 📦 Dependencies

| Library | Version | Role | Vendored as |
| --- | --- | --- | --- |
| [GLFW](https://github.com/glfw/glfw) | 3.4 | Window, OpenGL context, input | Git submodule |
| [Freetype](https://gitlab.freedesktop.org/freetype/freetype) | 2.13.3 | Font rasterization | Git submodule |
| [Assimp](https://github.com/assimp/assimp) | 5.4.3 | 3D model loading (OBJ, FBX) | Git submodule |
| [GLM](https://github.com/g-truc/glm) | 1.0.1 | Math (vectors, matrices) | Git submodule |
| [ImGui](https://github.com/ocornut/imgui) | 1.91.8 | Immediate-mode UI | Git submodule |
| [spdlog](https://github.com/gabime/spdlog) | 1.15.1 | Logging | Git submodule |
| [stb](https://github.com/nothings/stb) | 2.30 | Image loading (stb_image) | Git submodule |
| [glad](https://github.com/Dav1dde/glad) | gl=4.1 core | OpenGL loader *(desktop only)* | Vendored |
