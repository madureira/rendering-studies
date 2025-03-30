#pragma once

#include <RenderingStudies/App.h>

#include "./BlenderModel/BlenderModel.h"
#include "./Cube/Cube.h"
#include "./RippleEffect/RippleEffect.h"
#include "./TexturedCube/TexturedCube.h"
#include "./Triangle/Triangle.h"

class Window;

enum class AppName
{
    BlenderModel,
    Cube,
    RippleEffect,
    TexturedCube,
    Triangle,
};

class AppFactory
{
public:
    App *Generate(AppName type, Window *window)
    {
        switch (type)
        {
        case AppName::BlenderModel:
            LOG_INFO("AppFactory: instantiating BlenderModel app");
            return new BlenderModel(window);
        case AppName::Cube:
            LOG_INFO("AppFactory: instantiating Cube app");
            return new Cube(window);
        case AppName::RippleEffect:
            LOG_INFO("AppFactory: instantiating RippleEffect app");
            return new RippleEffect(window);
        case AppName::TexturedCube:
            LOG_INFO("AppFactory: instantiating TexturedCube app");
            return new TexturedCube(window);
        case AppName::Triangle:
            LOG_INFO("AppFactory: instantiating Triangle app");
            return new Triangle(window);
        }

        LOG_ERROR("AppFactory: unknown app type");

        return nullptr;
    }
};
