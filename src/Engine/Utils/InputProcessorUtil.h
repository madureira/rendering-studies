#pragma once

#include <RenderingStudies/Types.h>

class Camera;
class Window;

class InputProcessorUtil final
{
public:
    InputProcessorUtil() = delete;
    InputProcessorUtil(const InputProcessorUtil&) = delete;
    InputProcessorUtil(InputProcessorUtil&&) = delete;
    InputProcessorUtil& operator=(const InputProcessorUtil&) = delete;
    InputProcessorUtil& operator=(InputProcessorUtil&&) = delete;
    ~InputProcessorUtil() = delete;

    static void moveCamera(Camera* camera, Window* window, float32 deltaTime, float32 speed = 1.0f, float32 acceleratedSpeed = 3.0f);
};
