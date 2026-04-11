#pragma once

#include "Types.h"

class Demo
{
public:
    virtual ~Demo() = default;
    virtual void Update(float32 deltaTime) = 0;
    virtual void Render() = 0;
};
