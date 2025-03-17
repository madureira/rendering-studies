#pragma once

#include <headers/types.h>

enum class AppType
{
    Triangle
};

class App
{
public:
    virtual ~App() = default;
    virtual void update(float32 time, uint32 windowWidth, uint32 windowHeight) = 0;
};
