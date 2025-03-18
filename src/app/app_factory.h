#pragma once

#include <rendering_studies/app.h>

#include "./cube/cube.h"
#include "./triangle/triangle.h"

enum class AppType
{
    Cube,
    Triangle
};

class AppFactory
{
public:
    App *generate(AppType type)
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
