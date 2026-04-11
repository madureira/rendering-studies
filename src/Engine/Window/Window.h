#pragma once

// Include OpenGL (glad) before GLFW; GLFW may pull system OpenGL otherwise.
#include <RenderingStudies/GL.h>
#include <GLFW/glfw3.h>

#include <RenderingStudies/Types.h>

#include "Keyboard.h"
#include "Mouse.h"

struct Config;

struct WindowPosition
{
    int32 CenterX;
    int32 CenterY;
};

class Window final
{
private:
    GLFWwindow* m_Window;
    GLFWmonitor* m_Monitor;

    uint32 m_InitialWidth;
    uint32 m_InitialHeight;
    uint32 m_Width;
    uint32 m_Height;
    bool m_VSyncOn;
    MouseState m_Mouse;
    mutable bool m_FullScreen = false;
    mutable float64 m_LastTime = 0.0f;
    int32 m_MonitorIndex;

public:
    Window(const Config& config);
    ~Window();

    bool IsOpened() const;
    void BeginFrame();
    void SwapBuffers() const;
    void PollEvents() const;
    uint32 GetWidth() const;
    uint32 GetHeight() const;
    float64 GetTime() const;
    float64 GetDeltaTime() const;
    bool IsKeyPressed(KeyToken key) const;
    bool IsKeyReleased(KeyToken key) const;
    const MouseState& GetMouse() const;
    GLFWwindow* getNativeWindow() const;


private:
    void Shutdown() const;
    void Fullscreen() const;
    void OnCursorPos(float64 x, float64 y);
    void OnMouseButton(int32 button, int32 action, int32 mods);
    void OnScroll(float64 xoffset, float64 yoffset);
    WindowPosition CenterWindow(const GLFWvidmode* pVideoMode, const uint32 winWidth, const uint32 winHeight) const;
    void ShowHardwareInfo() const;
};
