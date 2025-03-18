#pragma once

class Window;

class App
{
public:
    virtual ~App() = default;
    virtual void update(Window *window) = 0;
};
