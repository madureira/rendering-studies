#include "app/app_factory.h"
#include "window/window.h"

int main()
{
    // Initialize window
    Window window("Rendering Studies", 1200, 900);

    // Initialize app
    App *app = AppFactory().generate(AppType::Triangle);

    // Main loop
    while (window.isOpen())
    {
        window.clear();

        app->update(window.getTime(), window.getWidth(), window.getHeight());

        // Swap buffers and poll events
        window.swapBuffers();
        window.pollEvents();
    }

    delete app;

    return 0;
}
