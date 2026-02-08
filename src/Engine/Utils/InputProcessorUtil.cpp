#include "InputProcessorUtil.h"

#include "../Camera/Camera.h"
#include "../Window/Window.h"
#include <imgui.h>

void InputProcessorUtil::moveCamera(Camera* camera, Window* window, float32 deltaTime, float32 speed, float32 acceleratedSpeed)
{
    ImGuiIO& io = ImGui::GetIO();

    if (io.WantCaptureMouse || io.WantCaptureKeyboard)
    {
        return;
    }

    const MouseState& mouse = window->GetMouse();

    if (mouse.LeftDown())
    {
        camera->Look((float32)mouse.dx, (float32)mouse.dy);
    }

    if (window->IsKeyPressed(KeyToken::LeftShift))
    {
        speed = acceleratedSpeed;
    }

    if (window->IsKeyPressed(KeyToken::Up) || window->IsKeyPressed(KeyToken::W))
    {
        camera->Move(CameraMove::FORWARD, deltaTime, speed);
    }

    if (window->IsKeyPressed(KeyToken::Down) || window->IsKeyPressed(KeyToken::S))
    {
        camera->Move(CameraMove::BACKWARD, deltaTime, speed);
    }

    if (window->IsKeyPressed(KeyToken::Left) || window->IsKeyPressed(KeyToken::A))
    {
        camera->Move(CameraMove::LEFT, deltaTime, speed);
    }

    if (window->IsKeyPressed(KeyToken::Right) || window->IsKeyPressed(KeyToken::D))
    {
        camera->Move(CameraMove::RIGHT, deltaTime, speed);
    }
}
