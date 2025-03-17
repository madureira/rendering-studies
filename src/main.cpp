#include <headers/types.h>
#include "window/window.h"
#include "app/app_factory.h"

int main()
{
    // Initialize window
    Window window("C++ Project Template", 1200, 900);

    // Initialize app
    App* app = AppFactory().generate(AppType::Triangle);

    // Main loop
    while (window.isOpen())
    {
        window.clear();

        app->update(window.getTime(), window.getWidth(), window.getHeight());

        // Swap buffers and poll events
        window.swapBuffers();
        window.pollEvents();
    }

    return 0;
}
