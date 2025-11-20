#include "Window.h"

#include <iomanip>
#include <sstream>

#include <glm/gtc/matrix_transform.hpp>

#include <RenderingStudies/Config.h>
#include "../FileManager/FileManager.h"
#include "../Shader/Shader.h"
#include "../TextRenderer/TextRenderer.h"
#include "../Utils/HardwareUtil.h"

static float64 s_LastTime;
static int32 s_NbFrames;
static bool s_Fullscreen;

Window::Window(const Config& config)
{
    m_InitialWidth = config.window_width;
    m_InitialHeight = config.window_height;
    m_Width = config.window_width;
    m_Height = config.window_height;
    m_FullScreen = config.window_fullscreen;
    m_ShowFPS = config.show_fps;
    m_VSyncOn = config.vsync_on;

    if (!glfwInit())
    {
        LOG_ERROR("Window: error initializing GLFW");
        return;
    }

    const int32 MONITOR_INDEX = 0;
    int32 monitors;
    GLFWmonitor* pMonitor = glfwGetMonitors(&monitors)[MONITOR_INDEX];
    const GLFWvidmode* pMode = glfwGetVideoMode(pMonitor);

    // Set OpenGL version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if defined(__APPLE__)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
    glfwWindowHint(GLFW_RED_BITS, pMode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, pMode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, pMode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, 60);

    m_Window = glfwCreateWindow(m_Width, m_Height, config.window_title.c_str(), m_FullScreen ? pMonitor : NULL, NULL);
    if (!m_Window)
    {
        LOG_ERROR("Window: error creating window");
        Shutdown();
        return;
    }

    // Make OpenGL context current
    glfwMakeContextCurrent(m_Window);
    glfwSetWindowUserPointer(m_Window, this);
    glfwSetWindowPos(m_Window, (pMode->width - m_Width) / 2, (pMode->height - m_Height) / 2);
    glfwSetWindowSizeLimits(m_Window, 800, 600, 3840, 2160);
    glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval(m_VSyncOn ? 1 : 0);
    glfwFocusWindow(m_Window);
    glfwWindowHint(GLFW_SAMPLES, 4);

    glfwSetErrorCallback([](int32 error, const char* description) {
        LOG_ERROR("GLFW ERROR: code: {}, message: {}", error, description);
    });

    glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* pNativeWindow, int32 width, int32 height) {
        Window& window = *(Window*)glfwGetWindowUserPointer(pNativeWindow);
        window.m_Width = width;
        window.m_Height = height;
    });

    glfwSetFramebufferSizeCallback(m_Window, []([[maybe_unused]] GLFWwindow* pNativeWindow, int32 width, int32 height) {
        glViewport(0, 0, width, height);
    });

    glfwSetCursorPosCallback(m_Window, [](GLFWwindow* pNativeWindow, float64 xpos, float64 ypos) {
        Window& window = *(Window*)glfwGetWindowUserPointer(pNativeWindow);
        window.m_MouseX = xpos;
        window.m_MouseY = ypos;
    });

    glfwSetScrollCallback(m_Window, [](GLFWwindow* pNativeWindow, float64 xoffset, float64 yoffset) {
        Window& window = *(Window*)glfwGetWindowUserPointer(pNativeWindow);
        window.m_OffsetX = xoffset;
        window.m_OffsetY = yoffset;
    });

    glfwSetKeyCallback(m_Window, [](GLFWwindow* pNativeWindow, int32 key, [[maybe_unused]] int32 scancode, int32 action, [[maybe_unused]] int32 mods) {
        Window& window = *(Window*)glfwGetWindowUserPointer(pNativeWindow);

        if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
        {
            window.Fullscreen();
        }

        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(pNativeWindow, GLFW_TRUE);
        }
    });

    // Initialize GLAD
    if (!gladLoadGL())
    {
        LOG_ERROR("Window: error initializing GLAD");
        Shutdown();
        return;
    }

    LOG_INFO("================================================");
    LOG_INFO("OS: {}", HardwareUtil::GetOS());
    LOG_INFO("================================================");
    LOG_INFO("CPU Model: {}", HardwareUtil::GetCPUModel());
    LOG_INFO("CPU Architecture: {}", HardwareUtil::GetCPUArchitecture());
    LOG_INFO("CPU Cores: {}", HardwareUtil::GetCPUCores());
    LOG_INFO("================================================");
    LOG_INFO("Physical Memory: {} GB", HardwareUtil::GetTotalMemory());
    LOG_INFO("Used Memory: {} GB", HardwareUtil::GetUsedMemory());
    LOG_INFO("Free Memory: {} GB", HardwareUtil::GetFreeMemory());
    LOG_INFO("================================================");
    LOG_INFO("GPU Model: {}", HardwareUtil::GetGPUModel());
    LOG_INFO("GPU Vendor: {}", HardwareUtil::GetGPUVendor());
    LOG_INFO("GPU Memory: {} GB", HardwareUtil::GetEstimateGPUMemory());
    LOG_INFO("================================================");
    LOG_INFO("OpenGL Version: {}", HardwareUtil::GetGLVersion());
    LOG_INFO("GLSL Version: {}", HardwareUtil::GetGLSLVersion());
    LOG_INFO("================================================");

    if (!glfwGetCurrentContext())
    {
        LOG_ERROR("No OpenGL context is active!");
        return;
    }

    GL(glEnable(GL_DEPTH_TEST));
    GL(glEnable(GL_MULTISAMPLE));
    GL(glEnable(GL_CULL_FACE));
    GL(glCullFace(GL_BACK));
    GL(glFrontFace(GL_CCW));
    GL(glEnable(GL_BLEND));
    GL(glDepthFunc(GL_LEQUAL));
    GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    m_TextShader = new Shader("assets/shaders/text.vert", "assets/shaders/text.frag");

    if (m_ShowFPS)
    {
        m_TextRenderer = new TextRenderer("assets/fonts/roboto-regular.ttf");
    }

    glm::mat4 projection = glm::ortho(0.0f, (float32)m_InitialWidth, 0.0f, (float32)m_InitialHeight);
    m_TextShader->Bind();
    m_TextShader->SetMat4("u_Projection", projection);
    m_TextShader->Unbind();
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
    // Gray background
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    // Clear color buffer and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_ShowFPS)
    {
        RenderFPS();
    }

    SetPolygonMode();
}

void Window::SwapBuffers() const
{
    glfwSwapBuffers(m_Window);
}

void Window::PollEvents() const
{
    glfwPollEvents();
}

float32 Window::GetTime() const
{
    return (float32)glfwGetTime();
}

uint32 Window::GetWidth() const
{
    return m_Width;
}

uint32 Window::GetHeight() const
{
    return m_Height;
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

float32 Window::GetDeltaTime()
{
    float32 currentTime = (float32)glfwGetTime();
    float32 deltaTime = currentTime - m_LastTime;
    m_LastTime = currentTime;
    return deltaTime;
}

bool Window::IsKeyPressed(KeyToken key) const
{
    return glfwGetKey(m_Window, (int32)key) == (int32)KeyAction::Press;
}

bool Window::isKeyReleased(KeyToken key) const
{
    return glfwGetKey(m_Window, (int32)key) == (int32)KeyAction::Release;
}

void Window::Shutdown() const
{
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

void Window::RenderFPS() const
{
    float64 currentTime = glfwGetTime();
    s_NbFrames++;

    static std::string fpsText;
    if (currentTime - s_LastTime >= 1.0) // If 1 second has passed
    {
        float64 fps = float64(s_NbFrames) / (currentTime - s_LastTime);

        // Convert FPS to string with 2 decimal places
        std::stringstream fpsStream;
        fpsStream << std::fixed << std::setprecision(2) << fps;
        fpsText = "FPS: " + fpsStream.str();

        s_NbFrames = 0;
        s_LastTime = currentTime;
    }

    static const float32 scale = 0.3f;
    static const float32 originX = 5.0f;
    static const float32 rowHeight = 20.0f;
    const float32 fpsTextPosY = m_InitialHeight - rowHeight;

    m_TextRenderer->Render(*m_TextShader, fpsText, originX, fpsTextPosY, scale, glm::vec3(0.5f, 0.8f, 0.2f));
}

void Window::Fullscreen() const
{
    s_Fullscreen = !s_Fullscreen;

    const int32 MONITOR_INDEX = 0;
    int32 monitors;
    GLFWmonitor* pMonitor = glfwGetMonitors(&monitors)[MONITOR_INDEX];
    const GLFWvidmode* pMode = glfwGetVideoMode(pMonitor);

    if (s_Fullscreen)
    {
        glfwSetWindowMonitor(m_Window, pMonitor, 0, 0, pMode->width, pMode->height, pMode->refreshRate);
    }
    else
    {
        glfwSetWindowMonitor(m_Window, NULL, 0, 0, m_InitialWidth, m_InitialHeight, 0);
        glfwSetWindowPos(m_Window, (pMode->width - m_InitialWidth) / 2, (pMode->height - m_InitialHeight) / 2);
    }
}

void Window::SetPolygonMode() const
{
    if (IsKeyPressed(KeyToken::Space))
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}
