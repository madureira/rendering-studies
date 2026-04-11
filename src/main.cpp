#include <RenderingStudies/Config.h>
#include <RenderingStudies/Types.h>

#include "Engine/Renderer/Renderer.h"
#include "Engine/UI/UI.h"
#include "Engine/Utils/AppSelector.h"
#include "Engine/Utils/InputProcessorUtil.h"
#include "Engine/Window/Window.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

struct LoopState
{
    Window* window;
    Renderer* renderer;
    AppSelector* appSelector;
    App* app = nullptr;
    int32 lastAppIndex = -1;
};

static void runFrame(LoopState& s)
{
    s.window->BeginFrame();
    s.window->PollEvents();
    s.renderer->Clear(0.2f, 0.2f, 0.2f);

    UI::NewFrame();
    s.appSelector->RenderSettings();

    s.renderer->SetZBuffer(s.appSelector->IsZBufferEnabled());
    s.renderer->SetCullFace(s.appSelector->IsCullFaceEnabled());

    int32 currentAppIndex = s.appSelector->GetSelectedIndex();
    if (currentAppIndex != s.lastAppIndex)
    {
        delete s.app;
        s.renderer->ResetCameraPosition();
        s.app = s.appSelector->GetSelectedApp(*s.window, *s.renderer->GetCamera());
        s.lastAppIndex = currentAppIndex;
    }

    s.renderer->RenderGrid(s.window->GetWidth(), s.window->GetHeight(), s.appSelector->IsGridEnabled());

    float32 deltaTime = s.window->GetDeltaTime();
    InputProcessorUtil::moveCamera(*s.renderer->GetCamera(), *s.window, deltaTime, s.appSelector->GetCameraSpeed(), s.appSelector->GetCameraAcceleratedSpeed());

    if (s.app)
    {
        ImGui::SetNextWindowPos(ImVec2(10.0f, s.appSelector->GetPanelBottom() + 10.0f), ImGuiCond_Appearing);
        s.app->Update(deltaTime);
        s.app->Render();
    }

    s.appSelector->RenderControls(s.window->GetWidth());

    UI::Render();

    s.renderer->RenderFPS(s.window->GetTime(), s.appSelector->IsFpsEnabled(), s.window->GetWidth(), s.window->GetHeight());
    s.renderer->SetPolygonMode(s.appSelector->IsPolygonModeEnabled());

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
    AppSelector appSelector;

    LoopState state;
    state.window = &window;
    state.renderer = &renderer;
    state.appSelector = &appSelector;

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(emscriptenCallback, &state, 0, 1);
#else
    while (window.IsOpened())
    {
        runFrame(state);
    }

    delete state.app;
#endif

    return EXIT_SUCCESS;
}
