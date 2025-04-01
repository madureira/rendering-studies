#pragma once

#include <magic_enum.hpp>

#include <RenderingStudies/App.h>
#include <RenderingStudies/Types.h>

#include "../Window/Window.h"
#include "./BlenderModel/BlenderModel.h"
#include "./Cube/Cube.h"
#include "./RippleEffect/RippleEffect.h"
#include "./TexturedCube/TexturedCube.h"
#include "./Triangle/Triangle.h"

enum class AppName
{
    BlenderModel,
    Cube,
    RippleEffect,
    TexturedCube,
    Triangle,
};

class AppFactory final
{
public:
    App* Generate(AppName appName, Window* window)
    {
        auto name = magic_enum::enum_name(appName);

        LOG_INFO("AppFactory: instantiating app {}", name);

        switch (appName)
        {
            case AppName::BlenderModel:
                return new BlenderModel(window);
            case AppName::Cube:
                return new Cube(window);
            case AppName::RippleEffect:
                return new RippleEffect(window);
            case AppName::TexturedCube:
                return new TexturedCube(window);
            case AppName::Triangle:
                return new Triangle(window);
        }

        LOG_ERROR("AppFactory: unknown app {}", name);

        return nullptr;
    }
};
