#include "Window.h"

#include <iomanip>
#include <sstream>

#include <glm/gtc/matrix_transform.hpp>

#include "../Utils/HardwareUtil.h"
#include <RenderingStudies/Config.h>

Window::Window(const Config& config)
{
    m_InitialWidth = config.window_width;
    m_InitialHeight = config.window_height;
    m_Width = config.window_width;
    m_Height = config.window_height;
    m_VSyncOn = config.vsync_on;
    m_MonitorIndex = config.monitor_index;

    glfwSetErrorCallback([](int32 error, const char* description) {
        LOG_ERROR("GLFW ERROR: code: {}, message: {}", error, description);
    });

    if (!glfwInit())
    {
        LOG_ERROR("Window: error initializing GLFW");
        return;
    }

    int32 monitorCount;

    GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);

    m_Monitor = (monitorCount > 1) ? monitors[0] : glfwGetPrimaryMonitor();

    if (m_MonitorIndex >= 0 && m_MonitorIndex < monitorCount)
    {
        m_Monitor = monitors[m_MonitorIndex];
    }
    else
    {
        m_Monitor = glfwGetPrimaryMonitor();
    }

    const GLFWvidmode* pVideoMode = glfwGetVideoMode(m_Monitor);

    // Set OpenGL version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if defined(__APPLE__)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // Hide initially to prevent flicker during move
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
    glfwWindowHint(GLFW_RED_BITS, pVideoMode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, pVideoMode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, pVideoMode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, 60);
    glfwWindowHint(GLFW_SAMPLES, 4);
#if defined(DEBUG) && defined(GL_DEBUG_SOURCE_API)
    // Required for reliable debug callback coverage on all drivers.
    // On macOS (4.1 + ARB_debug_output): registering our callback via
    // glDebugMessageCallbackARB replaces Apple's terse stderr output with
    // our detailed spdlog messages.
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

    // Create the window
    m_Window = glfwCreateWindow(m_Width, m_Height, config.window_title.c_str(), m_FullScreen ? m_Monitor : NULL, NULL);

    if (!m_Window)
    {
        LOG_ERROR("Window: error creating window");
        Shutdown();
        return;
    }

    // Make OpenGL context current
    glfwMakeContextCurrent(m_Window);

    glfwSetWindowSizeLimits(m_Window, 800, 600, 3840, 2160);

    WindowPosition winPosition = CenterWindow(pVideoMode, m_Width, m_Height);
    glfwSetWindowPos(m_Window, winPosition.CenterX, winPosition.CenterY);

    glfwSetWindowUserPointer(m_Window, this);
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

    glfwShowWindow(m_Window);
}

Window::~Window()
{
    Shutdown();
}

bool Window::IsOpened() const
{
    return !glfwWindowShouldClose(m_Window);
}

void Window::BeginFrame()
{
    m_Mouse.BeginFrame();
}

void Window::SwapBuffers() const
{
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

GLFWwindow* Window::getNativeWindow() const
{
    return m_Window;
}

void Window::Shutdown() const
{
    if (m_Window)
    {
        glfwDestroyWindow(m_Window);
    }

    glfwTerminate();
}

void Window::Fullscreen() const
{
    m_FullScreen = !m_FullScreen;

    const GLFWvidmode* pVideoMode = glfwGetVideoMode(m_Monitor);

    if (m_FullScreen)
    {
        glfwSetWindowMonitor(m_Window, m_Monitor, 0, 0, pVideoMode->width, pVideoMode->height, pVideoMode->refreshRate);
    }
    else
    {
        WindowPosition winPos = CenterWindow(pVideoMode, m_InitialWidth, m_InitialHeight);
        glfwSetWindowMonitor(m_Window, NULL, winPos.CenterX, winPos.CenterY, m_InitialWidth, m_InitialHeight, GLFW_DONT_CARE);
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

WindowPosition Window::CenterWindow(const GLFWvidmode* pVideoMode, const uint32 winWidth, const uint32 winHeight) const
{
    int monitorX, monitorY;
    glfwGetMonitorPos(m_Monitor, &monitorX, &monitorY);

    // Calculate the centered position relative to the monitor's start position
    int32 centerX = monitorX + (pVideoMode->width - winWidth) / 2;
    int32 centerY = monitorY + (pVideoMode ->height - winHeight) / 2;

    WindowPosition winPos;
    winPos.CenterX = centerX;
    winPos.CenterY = centerY;

    return winPos;
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
