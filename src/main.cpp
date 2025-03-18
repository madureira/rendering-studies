#include "app/app_factory.h"
#include "window/window.h"

int main()
{
    Window window("Rendering Studies", 1200, 900);
    App *app = AppFactory().generate(AppType::Cube);

    while (window.isOpen())
    {
        window.clear();
        app->update(&window);
        window.swapBuffers();
        window.pollEvents();
    }

    delete app;

    return EXIT_SUCCESS;
}
