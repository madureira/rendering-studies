#include <RenderingStudies/AppRegistry.h>
#include <RenderingStudies/Config.h>
#include "Engine/Window/Window.h"
#include "Engine/UI/UI.h"

int main()
{
    Config cfg = loadConfig("config.ini");

    Window window(cfg);
    UI ui(&window);

    auto app = AppRegistry::Instance().Create(cfg.app_name, &window);

    if (app == nullptr)
    {
        return EXIT_FAILURE;
    }

    while (window.IsOpen())
    {
        window.BeginFrame();
        window.PollEvents();
        window.Clear();

        UI::NewFrame();
        app->Update(window.GetDeltaTime());

        app->Render();
        UI::Render();

        window.SwapBuffers();
    }

    return EXIT_SUCCESS;
}
