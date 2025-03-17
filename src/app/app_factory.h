#pragma once

#include <rendering_studies/app.h>

#include "./triangle/triangle.h"

class AppFactory
{
public:
    App *generate(AppType type)
    {
        switch (type)
        {
        case AppType::Triangle:
            LOG_INFO("AppFactory: instantiating Triangle app");
            return new Triangle();
        }

        return nullptr;
    }
};
