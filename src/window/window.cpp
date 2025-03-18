#include "Window.h"

Window::Window(const std::string &title, uint32 width, uint32 height)
{
    m_Width = width;
    m_Height = height;

    if (!glfwInit())
    {
        LOG_ERROR("Window: error initializing GLFW");
        return;
    }

    const int32 MONITOR_INDEX = 0;
    int32 monitors;
    GLFWmonitor *pMonitor = glfwGetMonitors(&monitors)[MONITOR_INDEX];
    const GLFWvidmode *pMode = glfwGetVideoMode(pMonitor);

    // Set OpenGL version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // OpenGL 4.1
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1); // OpenGL 4.1
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on Mac
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
    glfwWindowHint(GLFW_RED_BITS, pMode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, pMode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, pMode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, 60);

    const bool FULLSCREEN = false;
    const bool VSYNC_ON = true;

    m_Window = glfwCreateWindow(width, height, title.c_str(), FULLSCREEN ? pMonitor : NULL, NULL);
    if (!m_Window)
    {
        LOG_ERROR("Window: error creating window");
        Shutdown();
        return;
    }

    // Make OpenGL context current
    glfwMakeContextCurrent(m_Window);
    glfwSetWindowUserPointer(m_Window, this);
    glfwSetWindowPos(m_Window, (pMode->width - width) / 2, (pMode->height - height) / 2);
    glfwSetWindowSizeLimits(m_Window, 800, 600, 3840, 2160);
    glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval(VSYNC_ON ? 1 : 0);
    glfwFocusWindow(m_Window);

    glfwSetErrorCallback([](int error, const char *description) {
        LOG_ERROR("GLFW ERROR: code: {}, message: {}", error, description);
    });

    glfwSetWindowSizeCallback(m_Window, [](GLFWwindow *pNativeWindow, int32 width, int32 height) {
        Window &window = *(Window *)glfwGetWindowUserPointer(pNativeWindow);
        window.m_Width = width;
        window.m_Height = height;
        glViewport(0, 0, width, height);
    });

    glfwSetCursorPosCallback(m_Window, [](GLFWwindow *pNativeWindow, float64 xpos, float64 ypos) {
        Window &window = *(Window *)glfwGetWindowUserPointer(pNativeWindow);
        window.m_MouseX = xpos;
        window.m_MouseY = ypos;
    });

    glfwSetScrollCallback(m_Window, [](GLFWwindow *pNativeWindow, float64 xoffset, float64 yoffset) {
        Window &window = *(Window *)glfwGetWindowUserPointer(pNativeWindow);
        window.m_OffsetX = xoffset;
        window.m_OffsetY = yoffset;
    });

    glfwSetKeyCallback(m_Window, [](GLFWwindow *pNativeWindow, int32 key, int32 scancode, int32 action, int32 mods) {
        Window &window = *(Window *)glfwGetWindowUserPointer(pNativeWindow);
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(pNativeWindow, GLFW_TRUE);
        }
    });

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        LOG_ERROR("Window: error initializing GLEW");
        Shutdown();
        return;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

Window::~Window()
{
    Shutdown();
}

bool Window::IsOpen() const
{
    return !glfwWindowShouldClose(m_Window);
}

void Window::Clear() const
{
    // Clear color buffer and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Black background
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void Window::SwapBuffers() const
{
    glfwSwapBuffers(m_Window);
}

void Window::PollEvents() const
{
    glfwPollEvents();
}

float Window::GetTime() const
{
    return (float)glfwGetTime();
}

uint32 Window::GetWidth() const
{
    return m_Width;
}

uint32 Window::GetHeight() const
{
    return m_Height;
}

bool Window::IsKeyPressed(KeyToken key) const
{
    return glfwGetKey(m_Window, (int32)key) == (int32)KeyAction::Press;
}

bool Window::isKeyReleased(KeyToken key) const
{
    return glfwGetKey(m_Window, (int32)key) == (int32)KeyAction::Release;
}

float64 Window::GetMouseX() const
{
    return m_MouseX;
}

float64 Window::GetMouseY() const
{
    return m_MouseY;
}

float64 Window::GetOffsetX() const
{
    return m_OffsetX;
}

float64 Window::GetOffsetY() const
{
    return m_OffsetY;
}

void Window::Shutdown() const
{
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}
