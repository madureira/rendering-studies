#include "InputProcessorUtil.h"

#include "../Camera/Camera.h"
#include "../Window/Window.h"
#include <imgui.h>

#ifdef __EMSCRIPTEN__
#include <emscripten/html5.h>

static float32 s_ScrollY = 0.0f;

static EM_BOOL onWheel(int32 /*eventType*/, const EmscriptenWheelEvent* e, void* /*userData*/)
{
    s_ScrollY -= static_cast<float>(e->deltaY) * 0.05f;
    return EM_TRUE;
}
#endif

void InputProcessorUtil::moveCamera(const Camera& camera, const Window& window, float32 deltaTime, float32 speed, float32 acceleratedSpeed)
{
    ImGuiIO& io = ImGui::GetIO();

#ifdef __EMSCRIPTEN__
    static bool s_WheelRegistered = false;
    if (!s_WheelRegistered)
    {
        emscripten_set_wheel_callback("#canvas", nullptr, EM_FALSE, onWheel);
        s_WheelRegistered = true;
    }
#endif

    if (io.WantCaptureMouse || io.WantCaptureKeyboard)
    {
        return;
    }

    if (window.IsKeyPressed(KeyToken::LeftShift))
    {
        speed = acceleratedSpeed;
    }

    if (window.IsKeyPressed(KeyToken::Up) || window.IsKeyPressed(KeyToken::W))
    {
        camera.Move(CameraMove::FORWARD, deltaTime, speed);
    }

    if (window.IsKeyPressed(KeyToken::Down) || window.IsKeyPressed(KeyToken::S))
    {
        camera.Move(CameraMove::BACKWARD, deltaTime, speed);
    }

    if (window.IsKeyPressed(KeyToken::Left) || window.IsKeyPressed(KeyToken::A))
    {
        camera.Move(CameraMove::LEFT, deltaTime, speed);
    }

    if (window.IsKeyPressed(KeyToken::Right) || window.IsKeyPressed(KeyToken::D))
    {
        camera.Move(CameraMove::RIGHT, deltaTime, speed);
    }

#ifdef __EMSCRIPTEN__
    if (io.MouseDown[0])
    {
        camera.Look(io.MouseDelta.x, io.MouseDelta.y);
    }

    const float32 scrollY = s_ScrollY;
    s_ScrollY = 0.0f;
    if (scrollY > 0.0f)
    {
        camera.Move(CameraMove::FORWARD, 0.15f, scrollY * 2.0f);
    }
    else if (scrollY < 0.0f)
    {
        camera.Move(CameraMove::BACKWARD, 0.15f, -scrollY * 2.0f);
    }
#else
    const MouseState& mouse = window.GetMouse();

    if (mouse.LeftDown())
    {
        camera.Look((float32)mouse.dx, (float32)mouse.dy);
    }

    const float64 scrollY = mouse.scrollY;
    if (scrollY > 0.0)
    {
        camera.Move(CameraMove::FORWARD, 0.15f, (float32)scrollY * 2.0f);
    }
    else if (scrollY < 0.0)
    {
        camera.Move(CameraMove::BACKWARD, 0.15f, (float32)(-scrollY) * 2.0f);
    }
#endif
}
