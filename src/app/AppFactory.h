#pragma once

#include <RenderingStudies/App.h>

#include "./Cube/Cube.h"
#include "./Triangle/Triangle.h"

enum class AppType
{
    Cube,
    Triangle
};

class AppFactory
{
public:
    App *Generate(AppType type)
    {
        switch (type)
        {
        case AppType::Cube:
            LOG_INFO("AppFactory: instantiating Cube app");
            return new Cube();
        case AppType::Triangle:
            LOG_INFO("AppFactory: instantiating Triangle app");
            return new Triangle();
        }

        return nullptr;
    }
};
