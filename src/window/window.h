#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>

#include <rendering_studies/types.h>

class Window
{
private:
    GLFWwindow *m_window;
    uint32 m_width;
    uint32 m_height;

public:
    Window(const std::string &title, uint32 width, uint32 height);
    ~Window();

    bool isOpen() const;
    void clear() const;
    void swapBuffers() const;
    void pollEvents() const;
    float32 getTime() const;
    uint32 getWidth() const;
    uint32 getHeight() const;

private:
    void shutdown() const;
};
