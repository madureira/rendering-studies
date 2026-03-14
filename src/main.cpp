#include "Engine/UI/UI.h"
#include "Engine/Utils/AppSelector.h"
#include "Engine/Window/Window.h"
#include "Engine/Renderer/Renderer.h"
#include <RenderingStudies/Config.h>
#include <RenderingStudies/Types.h>

int main()
{
    Config cfg = loadConfig("config.ini");

    Window window(cfg);
    Renderer renderer(window.GetWidth(), window.GetHeight());
    UI ui(window);

    App* app = nullptr;
    AppSelector appSelector;
    int32 lastSelectedAppIndex = -1;

    while (window.IsOpened())
    {
        window.BeginFrame();
        window.PollEvents();
        renderer.Clear(0.2f, 0.2f, 0.2f);

        UI::NewFrame();
        appSelector.Render();

        renderer.SetZBuffer(appSelector.IsZBufferEnabled());
        renderer.SetCullFace(appSelector.IsCullFaceEnabled());

        int32 currentAppIndex = appSelector.GetSelectedIndex();
        if (currentAppIndex != lastSelectedAppIndex)
        {
            delete app;
            renderer.ResetCameraPosition();
            app = appSelector.GetSelectedApp(window, *renderer.GetCamera());
            lastSelectedAppIndex = currentAppIndex;
        }

        renderer.RenderGrid(window.GetWidth(), window.GetHeight(), appSelector.IsGridEnabled());

        if (app)
        {
            app->Update(window.GetDeltaTime());
            app->Render();
        }
        UI::Render();

        renderer.RenderFPS(window.GetTime(), appSelector.IsFpsEnabled());

        renderer.SetPolygonMode(appSelector.IsPolygonModeEnabled());

        window.SwapBuffers();
    }

    delete app;

    return EXIT_SUCCESS;
}
