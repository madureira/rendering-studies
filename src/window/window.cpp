#include "window.h"

Window::Window(const std::string &title, uint32 width, uint32 height)
{
    m_width = width;
    m_height = height;

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

    m_window = glfwCreateWindow(width, height, title.c_str(), FULLSCREEN ? pMonitor : NULL, NULL);
    if (!m_window)
    {
        LOG_ERROR("Window: error creating window");
        shutdown();
        return;
    }

    // Make OpenGL context current
    glfwMakeContextCurrent(m_window);
    glfwSetWindowUserPointer(m_window, this);
    glfwSetWindowPos(m_window, (pMode->width - width) / 2, (pMode->height - height) / 2);
    glfwSetWindowSizeLimits(m_window, 800, 600, 3840, 2160);
    glfwSwapInterval(VSYNC_ON ? 1 : 0);
    glfwFocusWindow(m_window);

    glfwSetErrorCallback([](int error, const char *description) {
        LOG_ERROR("GLFW ERROR: code: {}, message: {}", error, description);
    });

    glfwSetWindowSizeCallback(m_window, [](GLFWwindow *pNativeWindow, int32 width, int32 height) {
        Window &window = *(Window *)glfwGetWindowUserPointer(pNativeWindow);
        window.m_width = width;
        window.m_height = height;
        glViewport(0, 0, width, height);
    });

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        LOG_ERROR("Window: error initializing GLEW");
        shutdown();
        return;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

Window::~Window()
{
    shutdown();
}

bool Window::isOpen() const
{
    return !glfwWindowShouldClose(m_window);
}

void Window::clear() const
{
    // Clear color buffer and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Black background
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void Window::swapBuffers() const
{
    glfwSwapBuffers(m_window);
}

void Window::pollEvents() const
{
    glfwPollEvents();
}

float Window::getTime() const
{
    return (float)glfwGetTime();
}

uint32 Window::getWidth() const
{
    return m_width;
}

uint32 Window::getHeight() const
{
    return m_height;
}

void Window::shutdown() const
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}
