#include "App/AppFactory.h"
#include "Window/Window.h"

int main()
{
    Window window("Rendering Studies", 1200, 900);
    App *app = AppFactory().Generate(AppName::Triangle, &window);

    if (app == nullptr)
    {
        return EXIT_FAILURE;
    }

    while (window.IsOpen())
    {
        window.Clear();
        app->Update(window.GetDeltaTime());
        window.SwapBuffers();
        window.PollEvents();
    }

    delete app;

    return EXIT_SUCCESS;
}
