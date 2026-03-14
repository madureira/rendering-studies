#pragma once

class Window;

class UI final
{
public:
    UI(const Window& window);
    ~UI();

    static void NewFrame();
    static void Render();
};
