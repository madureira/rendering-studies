#include <RenderingStudies/AppRegistry.h>
#include <RenderingStudies/Config.h>
#include "Window/Window.h"

int main()
{
    Config cfg = loadConfig("config.ini");

    Window window(cfg);

    auto app = AppRegistry::Instance().Create(cfg.app_name, &window);

    if (app == nullptr)
    {
        return EXIT_FAILURE;
    }

    while (window.IsOpen())
    {
        window.Clear();
        app->Update(window.GetDeltaTime());
        app->Render();
        window.SwapBuffers();
        window.PollEvents();
    }

    return EXIT_SUCCESS;
}
