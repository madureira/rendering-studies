#include "Window.h"

#include <iomanip>
#include <sstream>

#include <glm/gtc/matrix_transform.hpp>

#include "../Shader/Shader.h"
#include "../TextRenderer/TextRenderer.h"
#include "../Utils/HardwareUtil.h"
#include <RenderingStudies/Config.h>

Window::Window(const Config& config)
{
    m_InitialWidth = config.window_width;
    m_InitialHeight = config.window_height;
    m_Width = config.window_width;
    m_Height = config.window_height;
    m_FullScreen = config.window_fullscreen;
    m_ShowFPS = config.show_fps;
    m_VSyncOn = config.vsync_on;

    glfwSetErrorCallback([](int32 error, const char* description) {
        LOG_ERROR("GLFW ERROR: code: {}, message: {}", error, description);
    });

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
    glfwWindowHint(GLFW_SAMPLES, 4);

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
    // glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval(m_VSyncOn ? 1 : 0);
    glfwFocusWindow(m_Window);

    double mx, my;
    glfwGetCursorPos(m_Window, &mx, &my);
    m_Mouse.x = mx;
    m_Mouse.y = my;

    glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* pNativeWindow, int32 width, int32 height) {
        Window& window = *(Window*)glfwGetWindowUserPointer(pNativeWindow);
        window.m_Width = width;
        window.m_Height = height;
    });

    glfwSetFramebufferSizeCallback(m_Window, []([[maybe_unused]] GLFWwindow* pNativeWindow, int32 width, int32 height) {
        GL(glViewport(0, 0, width, height));
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

        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        {
            window.m_WireframeMode = !window.m_WireframeMode;
        }
    });

    glfwSetCursorPosCallback(m_Window, [](GLFWwindow* pNativeWindow, float64 xpos, float64 ypos) {
        Window& window = *(Window*)glfwGetWindowUserPointer(pNativeWindow);
        window.OnCursorPos(xpos, ypos);
    });

    glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* pNativeWindow, int32 button, int32 action, int32 mods) {
        Window& window = *(Window*)glfwGetWindowUserPointer(pNativeWindow);
        window.OnMouseButton(button, action, mods);
    });

    glfwSetScrollCallback(m_Window, [](GLFWwindow* pNativeWindow, float64 xoffset, float64 yoffset) {
        Window& window = *(Window*)glfwGetWindowUserPointer(pNativeWindow);
        window.OnScroll(xoffset, yoffset);
    });

    // Initialize GLAD
    if (!gladLoadGL())
    {
        LOG_ERROR("Window: error initializing GLAD");
        Shutdown();
        return;
    }

    ShowHardwareInfo();

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

void Window::BeginFrame()
{
    m_Mouse.BeginFrame();
}

void Window::Clear() const
{
    // Gray background
    GL(glClearColor(0.2f, 0.2f, 0.2f, 1.0f));
    // Clear color buffer and depth buffer
    GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    SetPolygonMode();
}

void Window::SwapBuffers() const
{
    if (m_ShowFPS)
    {
        RenderFPS();
    }
    glfwSwapBuffers(m_Window);
}

void Window::PollEvents() const
{
    glfwPollEvents();
}

uint32 Window::GetWidth() const
{
    return m_Width;
}

uint32 Window::GetHeight() const
{
    return m_Height;
}

float64 Window::GetTime() const
{
    return glfwGetTime();
}

float64 Window::GetDeltaTime() const
{
    float64 currentTime = glfwGetTime();
    float64 deltaTime = currentTime - m_LastTime;
    m_LastTime = currentTime;
    return deltaTime;
}

bool Window::IsKeyPressed(KeyToken key) const
{
    return glfwGetKey(m_Window, (int32)key) == (int32)KeyAction::Press;
}

bool Window::IsKeyReleased(KeyToken key) const
{
    return glfwGetKey(m_Window, (int32)key) == (int32)KeyAction::Release;
}

const MouseState& Window::GetMouse() const
{
    return m_Mouse;
}

void Window::Shutdown() const
{
    if (m_Window)
    {
        glfwDestroyWindow(m_Window);
    }

    glfwTerminate();
}

void Window::RenderFPS() const
{
    float64 currentTime = glfwGetTime();
    m_FpsNbFrames++;

    static std::string fpsText;
    if (currentTime - m_FpsLastTime >= 1.0) // If 1 second has passed
    {
        float64 fps = float64(m_FpsNbFrames) / (currentTime - m_FpsLastTime);

        // Convert FPS to string with 2 decimal places
        std::stringstream fpsStream;
        fpsStream << std::fixed << std::setprecision(2) << fps;
        fpsText = "FPS: " + fpsStream.str();

        m_FpsNbFrames = 0;
        m_FpsLastTime = currentTime;
    }

    static const float32 scale = 0.3f;
    static const float32 originX = 5.0f;
    static const float32 rowHeight = 20.0f;
    const float32 fpsTextPosY = m_InitialHeight - rowHeight;

    if (!m_TextRenderer)
    {
        return;
    }

    m_TextRenderer->Render(*m_TextShader, fpsText, originX, fpsTextPosY, scale, glm::vec3(0.5f, 0.8f, 0.2f));
}

void Window::Fullscreen() const
{
    m_FullScreen = !m_FullScreen;

    const int32 MONITOR_INDEX = 0;
    int32 monitors;
    GLFWmonitor* pMonitor = glfwGetMonitors(&monitors)[MONITOR_INDEX];
    const GLFWvidmode* pMode = glfwGetVideoMode(pMonitor);

    if (m_FullScreen)
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
    if (m_WireframeMode)
    {
        GL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
    }
    else
    {
        GL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
    }
}

void Window::OnCursorPos(float64 x, float64 y)
{
    m_Mouse.dx += (x - m_Mouse.x);
    m_Mouse.dy += (y - m_Mouse.y);
    m_Mouse.x = x;
    m_Mouse.y = y;
}

void Window::OnMouseButton(int32 button, int32 action, [[maybe_unused]] int32 mods)
{
    if (button < 0 || button > Input::kMouseButtonLast)
    {
        return;
    }

    auto& b = m_Mouse.Button(button);

    if (action == GLFW_PRESS)
    {
        if (!b.down)
        {
            b.down = true;
            b.pressed = true;
        }
    }
    else if (action == GLFW_RELEASE)
    {
        if (b.down)
        {
            b.down = false;
            b.released = true;
        }
    }
}

void Window::OnScroll([[maybe_unused]] float64 xoffset, float64 yoffset)
{
    m_Mouse.scrollY += yoffset;
}

void Window::ShowHardwareInfo() const
{
    LOG_INFO("================================================");
    LOG_INFO("SYSTEM");
    LOG_INFO("OS: {}", HardwareUtil::GetOS());
    LOG_INFO("CPU Model: {}", HardwareUtil::GetCPUModel());
    LOG_INFO("CPU Architecture: {}", HardwareUtil::GetCPUArchitecture());
    LOG_INFO("CPU Cores: {}", HardwareUtil::GetCPUCores());

    const double totalMem = HardwareUtil::GetTotalMemory();
    const double freeMem = HardwareUtil::GetFreeMemory();
    const double usedMem = HardwareUtil::GetUsedMemory();

    if (totalMem > 0.0)
    {
        LOG_INFO("Physical Memory: {:.2f} GB", totalMem);
        if (usedMem >= 0.0)
            LOG_INFO("Used Memory: {:.2f} GB", usedMem);
        if (freeMem >= 0.0)
            LOG_INFO("Free Memory: {:.2f} GB", freeMem);
    }
    else
    {
        LOG_INFO("Physical Memory: Unknown");
    }

    LOG_INFO("================================================");
    LOG_INFO("GRAPHICS");
    const auto gfx = HardwareUtil::QueryGraphicsInfo();
    LOG_INFO("GPU Vendor: {}", gfx.vendor);
    LOG_INFO("GPU Renderer: {}", gfx.renderer);
    LOG_INFO("OpenGL Version: {}", gfx.glVersion);
    LOG_INFO("GLSL Version: {}", gfx.glslVersion);

    if (gfx.dedicatedVramMiB != 0)
    {
        LOG_INFO("GPU Dedicated VRAM: {} MiB", gfx.dedicatedVramMiB);
    }
    else if (gfx.totalAvailableVramMiB != 0)
    {
        LOG_INFO("GPU Available VRAM: {} MiB", gfx.totalAvailableVramMiB);
    }
    else
    {
        // fallback (your older heuristic)
        const double est = HardwareUtil::GetEstimateGPUMemory();
        if (est > 0.0)
            LOG_INFO("GPU Memory (estimated): {:.2f} GB", est);
        else
            LOG_INFO("GPU Memory: Unknown (integrated / not exposed)");
    }

    LOG_INFO("Max Texture Size: {}", gfx.maxTextureSize);
    LOG_INFO("Max Samples: {}", gfx.maxSamples);
    LOG_INFO("Max Anisotropy: {:.2f}", gfx.maxAnisotropy);

    LOG_INFO("Debug Context: {}", gfx.debugContext ? "true" : "false");
    LOG_INFO("Compute Supported: {}", gfx.supportsCompute ? "true" : "false");
    if (gfx.supportsCompute)
    {
        LOG_INFO("Max Compute Work Group Count: [{}, {}, {}]",
            gfx.maxComputeWorkGroupCount[0],
            gfx.maxComputeWorkGroupCount[1],
            gfx.maxComputeWorkGroupCount[2]);
        LOG_INFO("Max Compute Work Group Size:  [{}, {}, {}]",
            gfx.maxComputeWorkGroupSize[0],
            gfx.maxComputeWorkGroupSize[1],
            gfx.maxComputeWorkGroupSize[2]);
    }

    if (gfx.maxTessGenLevel > 0)
    {
        LOG_INFO("GL_MAX_TESS_GEN_LEVEL: {}", gfx.maxTessGenLevel);
        LOG_INFO("GL_MAX_PATCH_VERTICES: {}", gfx.maxPatchVertices);
    }

    LOG_INFO("Max Vertex Attribs: {}", gfx.maxVertexAttribs);
    LOG_INFO("Max Combined Texture Image Units: {}", gfx.maxCombinedTextureUnits);
    LOG_INFO("Max Uniform Block Size: {} bytes", gfx.maxUniformBlockSizeBytes);

    LOG_INFO("================================================");

}
