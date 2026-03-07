#include "Engine/UI/UI.h"
#include "Engine/Utils/AppSelector.h"
#include "Engine/Window/Window.h"
#include <RenderingStudies/Config.h>
#include <RenderingStudies/Types.h>

int main()
{
    Config cfg = loadConfig("config.ini");

    Window window(cfg);
    UI ui(&window);

    App* app = nullptr;
    AppSelector appSelector;
    int32 lastSelectedAppIndex = -1;

    while (window.IsOpen())
    {
        window.BeginFrame();
        window.PollEvents();
        window.Clear();

        UI::NewFrame();
        appSelector.Render();

        int32 currentAppIndex = appSelector.GetSelectedIndex();
        if (currentAppIndex != lastSelectedAppIndex)
        {
            delete app;
            app = appSelector.GetSelectedApp(&window);
            lastSelectedAppIndex = currentAppIndex;
        }

        if (app)
        {
            app->Update(window.GetDeltaTime());
            app->Render();
        }
        UI::Render();

        window.SwapBuffers();
    }

    delete app;

    return EXIT_SUCCESS;
}
