#include <RenderingStudies/Config.h>
#include <RenderingStudies/Types.h>

#include "Engine/Renderer/Renderer.h"
#include "Engine/UI/UI.h"
#include "Engine/Utils/DemoSelector.h"
#include "Engine/Utils/InputProcessorUtil.h"
#include "Engine/Window/Window.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

struct LoopState
{
    Window* window;
    Renderer* renderer;
    DemoSelector* demoSelector;
    Demo* demo = nullptr;
    int32 lastDemoIndex = -1;
};

static void runFrame(LoopState& s)
{
    s.window->BeginFrame();
    s.window->PollEvents();
    s.renderer->Clear(0.2f, 0.2f, 0.2f);

    UI::NewFrame();
    s.demoSelector->RenderSettings();

    s.renderer->SetZBuffer(s.demoSelector->IsZBufferEnabled());
    s.renderer->SetCullFace(s.demoSelector->IsCullFaceEnabled());

    int32 currentDemoIndex = s.demoSelector->GetSelectedIndex();
    if (currentDemoIndex != s.lastDemoIndex)
    {
        delete s.demo;
        s.renderer->ResetCameraPosition();
        s.demo = s.demoSelector->GetSelectedDemo(*s.window, *s.renderer->GetCamera());
        s.lastDemoIndex = currentDemoIndex;
    }

    s.renderer->RenderGrid(s.window->GetWidth(), s.window->GetHeight(), s.demoSelector->IsGridEnabled());

    float32 deltaTime = s.window->GetDeltaTime();
    InputProcessorUtil::moveCamera(*s.renderer->GetCamera(), *s.window, deltaTime, s.demoSelector->GetCameraSpeed(), s.demoSelector->GetCameraAcceleratedSpeed());

    if (s.demo)
    {
        ImGui::SetNextWindowPos(ImVec2(10.0f, s.demoSelector->GetPanelBottom() + 10.0f), ImGuiCond_Appearing);
        s.demo->Update(deltaTime);
        s.demo->Render();
    }

    s.demoSelector->RenderControls(s.window->GetWidth());

    UI::Render();

    s.renderer->RenderFPS(s.window->GetTime(), s.demoSelector->IsFpsEnabled(), s.window->GetWidth(), s.window->GetHeight());
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
