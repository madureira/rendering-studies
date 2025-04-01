#pragma once

#include "Types.h"

class App
{
public:
    virtual ~App() = default;
    virtual void Update(float32 deltaTime) = 0;
};
