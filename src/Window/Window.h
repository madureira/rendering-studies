#pragma once

// Include OpenGL (glad) before GLFW; GLFW may pull system OpenGL otherwise.
#include <RenderingStudies/GL.h>
#include <GLFW/glfw3.h>

#include <RenderingStudies/Types.h>

#include "Keyboard.h"
#include "Mouse.h"

struct Config;
class Shader;
class TextRenderer;

class Window final
{
private:
    GLFWwindow* m_Window;
    Shader* m_TextShader;
    TextRenderer* m_TextRenderer;
    uint32 m_InitialWidth;
    uint32 m_InitialHeight;
    uint32 m_Width;
    uint32 m_Height;
    bool m_ShowFPS;
    bool m_VSyncOn;
    MouseState m_Mouse;
    mutable bool m_FullScreen;
    mutable float64 m_LastTime = 0.0f;
    mutable float64 m_FpsLastTime = 0.0;
    mutable int32 m_FpsNbFrames = 0;

public:
    Window(const Config& config);
    ~Window();

    bool IsOpen() const;
    void BeginFrame();
    void Clear() const;
    void SwapBuffers() const;
    void PollEvents() const;
    uint32 GetWidth() const;
    uint32 GetHeight() const;
    float64 GetTime() const;
    float64 GetDeltaTime() const;
    bool IsKeyPressed(KeyToken key) const;
    bool IsKeyReleased(KeyToken key) const;
    const MouseState& GetMouse() const;


private:
    void ShowHardwareInfo() const;
    void Shutdown() const;
    void RenderFPS() const;
    void Fullscreen() const;
    void SetPolygonMode() const;
    void OnCursorPos(float64 x, float64 y);
    void OnMouseButton(int32 button, int32 action, int32 mods);
};
