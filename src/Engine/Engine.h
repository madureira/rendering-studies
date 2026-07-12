#pragma once

class Window;
class Renderer;
class DemoSelector;
class Demo;

struct LoopState
{
    Window* window;
    Renderer* renderer;
    DemoSelector* demoSelector;
    Demo* demo = nullptr;
    int32 lastDemoIndex = -1;
    bool initialized = false;
};

class Engine final
{
public:
    Engine() = default;
    ~Engine() = default;
    void Start() const;
};
