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
            return new Triangle();
        }

        return nullptr;
    }
};
