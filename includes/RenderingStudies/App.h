#pragma once

class Window;

class App
{
public:
    virtual ~App() = default;
    virtual void Update(Window *window) = 0;
};
