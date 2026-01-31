#include "InputProcessorUtil.h"

#include "../Camera/Camera.h"
#include "../Window/Window.h"

void InputProcessorUtil::moveCamera(Camera* camera, Window* window, float32 deltaTime, float32 speed, float32 acceleratedSpeed)
{
    const MouseState& mouse = window->GetMouse();

    if (mouse.LeftDown())
    {
        camera->ProcessMouseDelta((float32)mouse.dx, (float32)mouse.dy);
    }

    if (window->IsKeyPressed(KeyToken::LeftShift))
    {
        speed = acceleratedSpeed;
    }

    if (window->IsKeyPressed(KeyToken::Up) || window->IsKeyPressed(KeyToken::W))
    {
        camera->ProcessKeyboard(CameraMove::FORWARD, deltaTime, speed);
    }

    if (window->IsKeyPressed(KeyToken::Down) || window->IsKeyPressed(KeyToken::S))
    {
        camera->ProcessKeyboard(CameraMove::BACKWARD, deltaTime, speed);
    }

    if (window->IsKeyPressed(KeyToken::Left) || window->IsKeyPressed(KeyToken::A))
    {
        camera->ProcessKeyboard(CameraMove::LEFT, deltaTime, speed);
    }

    if (window->IsKeyPressed(KeyToken::Right) || window->IsKeyPressed(KeyToken::D))
    {
        camera->ProcessKeyboard(CameraMove::RIGHT, deltaTime, speed);
    }
}
