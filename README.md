# Rendering Studies

> Real-time rendering demos written in C++11 — runs natively on desktop and in the browser via WebAssembly.

![C++11](https://img.shields.io/badge/C%2B%2B-11-00599C?style=flat-square&logo=c%2B%2B&logoColor=white)
![OpenGL 4.1](https://img.shields.io/badge/OpenGL-4.1-5586A4?style=flat-square&logo=opengl&logoColor=white)
![WebGL 2.0](https://img.shields.io/badge/WebGL-2.0-990000?style=flat-square&logo=webgl&logoColor=white)
![WebAssembly](https://img.shields.io/badge/WebAssembly-Emscripten-654FF0?style=flat-square&logo=webassembly&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-3.24+-064F8C?style=flat-square&logo=cmake&logoColor=white)
![macOS](https://img.shields.io/badge/macOS-000000?style=flat-square&logo=apple&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?style=flat-square&logo=linux&logoColor=black)
![Windows](https://img.shields.io/badge/Windows-0078D4?style=flat-square&logo=windows&logoColor=white)

---

## Overview

Each demo is selectable at runtime from a settings panel — no restart needed. The same codebase compiles to a native binary (OpenGL 4.1 Core) and to a static web page (WebGL 2.0 via Emscripten), deployable on GitHub Pages.

[![Live Demo](https://img.shields.io/badge/Live_Demo-madureira.github.io/rendering--studies-654FF0?style=for-the-badge&logo=webassembly&logoColor=white)](https://madureira.github.io/rendering-studies)

![Demo](docs/video_demo.gif)

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

## Requirements

### Desktop — macOS · Linux · Windows

- [CMake](https://cmake.org/download/) 3.24+
- [Conan 2](https://conan.io/)

Set up your Conan profile once:

```sh
conan profile detect
```

### Web

- [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html)

---

## Building

### Desktop

```sh
# Debug
./debug_build.sh

# Release
./release_build.sh
```

### Web (WebAssembly + WebGL 2.0)

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

## Configuration

Edit `config.ini` to change the window size, monitor, and VSync:

```ini
window_title=Rendering Studies
window_width=1200
window_height=900
vsync_on=true
monitor_index=0
```

---

## Dependencies

| Library | Version | Role | Vendored as |
| --- | --- | --- | --- |
| [GLFW](https://github.com/glfw/glfw) | 3.4 | Window, OpenGL context, input | Conan |
| [Freetype](https://freetype.org) | 2.13.3 | Font rasterization | Conan |
| [Assimp](https://github.com/assimp/assimp) | 5.4.3 | 3D model loading (OBJ, FBX) | Git submodule |
| [GLM](https://github.com/g-truc/glm) | 1.0.1 | Math (vectors, matrices) | Git submodule |
| [ImGui](https://github.com/ocornut/imgui) | 1.91.8 | Immediate-mode UI | Git submodule |
| [spdlog](https://github.com/gabime/spdlog) | 1.15.1 | Logging | Git submodule |
| [glad](https://github.com/Dav1dde/glad) | gl=4.1 core | OpenGL loader *(desktop only)* | Vendored |
