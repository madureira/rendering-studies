#pragma once

#include <RenderingStudies/App.h>

#include "./BlenderModel/BlenderModel.h"
#include "./Cube/Cube.h"
#include "./TexturedCube/TexturedCube.h"
#include "./Triangle/Triangle.h"

class Window;

enum class AppType
{
    BlenderModel,
    Cube,
    TexturedCube,
    Triangle,
};

class AppFactory
{
public:
    App *Generate(AppType type, Window *window)
    {
        switch (type)
        {
        case AppType::BlenderModel:
            LOG_INFO("AppFactory: instantiating BlenderModel app");
            return new BlenderModel(window);
        case AppType::Cube:
            LOG_INFO("AppFactory: instantiating Cube app");
            return new Cube(window);
        case AppType::TexturedCube:
            LOG_INFO("AppFactory: instantiating TexturedCube app");
            return new TexturedCube(window);
        case AppType::Triangle:
            LOG_INFO("AppFactory: instantiating Triangle app");
            return new Triangle(window);
        }

        LOG_ERROR("AppFactory: unknown app type");

        return nullptr;
    }
};
