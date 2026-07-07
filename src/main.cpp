#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include <RenderingStudies/Config.h>

#include "Engine/Renderer/Renderer.h"
#include "Engine/UI/CameraGizmo.h"
#include "Engine/UI/UI.h"
#include "Engine/Utils/DemoSelector.h"
#include "Engine/Utils/InputProcessorUtil.h"
#include "Engine/Window/Window.h"

struct LoopState
{
    Window* window;
    Renderer* renderer;
    DemoSelector* demoSelector;
    Demo* demo = nullptr;
    int32 lastDemoIndex = -1;
    bool initialized = false;
};

static void runFrame(LoopState& s)
{
    uint32 winWidth = s.window->GetWidth();
    uint32 winHeight = s.window->GetHeight();
    Camera& camera = *s.renderer->GetCamera();

    s.window->BeginFrame();
    float32 deltaTime = s.window->GetDeltaTime();

    s.window->PollEvents();
    s.renderer->Clear();

    UI::NewFrame();
    s.demoSelector->RenderSettings();

    s.renderer->SetZBuffer(s.demoSelector->IsZBufferEnabled());
    s.renderer->SetCullFace(s.demoSelector->IsCullFaceEnabled());

    InputProcessorUtil::moveCamera(camera, *s.window, deltaTime, s.demoSelector->GetCameraSpeed(), s.demoSelector->GetCameraAcceleratedSpeed());

    s.renderer->RenderGrid(winWidth, winHeight, s.demoSelector->IsGridEnabled());
    CameraGizmo::Render(camera, winWidth);

    if (!s.initialized)
    {
        s.initialized = true;
    }
    else
    {
        int32 currentDemoIndex = s.demoSelector->GetSelectedIndex();
        if (currentDemoIndex != s.lastDemoIndex)
        {
            delete s.demo;
            s.renderer->ResetCameraPosition();
            s.demo = s.demoSelector->GetSelectedDemo(*s.window, camera);
            s.lastDemoIndex = currentDemoIndex;
        }

        if (s.demo)
        {
            static constexpr float32 offset = 10.0f;
            ImGui::SetNextWindowPos(ImVec2(offset, s.demoSelector->GetPanelBottom() + offset), ImGuiCond_Appearing);
            s.demo->Update(deltaTime);
            s.demo->Render();
        }
    }

    s.demoSelector->RenderControls(winWidth);

    UI::Render();

    s.renderer->RenderFPS(s.window->GetTime(), s.demoSelector->IsFpsEnabled(), winWidth, winHeight);
    s.renderer->RenderCameraInfo(winWidth, winHeight, s.demoSelector->IsCameraInfoEnabled());
    s.renderer->SetPolygonMode(s.demoSelector->IsPolygonModeEnabled());

    s.window->SwapBuffers();
}

#ifdef __EMSCRIPTEN__
static void emscriptenCallback(void* arg)
{
    runFrame(*static_cast<LoopState*>(arg));
}
#endif

int main()
{
    Config cfg = loadConfig("config.ini");

    Window window(cfg);
    Renderer renderer(window.GetWidth(), window.GetHeight());
    UI ui(window);
    DemoSelector demoSelector;

    LoopState state;
    state.window = &window;
    state.renderer = &renderer;
    state.demoSelector = &demoSelector;

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(emscriptenCallback, &state, 0, 1);
#else
    while (window.IsOpened())
    {
        runFrame(state);
    }
    delete state.demo;
#endif

    return EXIT_SUCCESS;
}
