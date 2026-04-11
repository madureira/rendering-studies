#pragma once

#include <cassert>
#include <RenderingStudies/Types.h>

namespace Input
{
    constexpr int32 kMouseButtonLeft = 0; // equivalent to GLFW_MOUSE_BUTTON_LEFT
    constexpr int32 kMouseButtonLast = 7; // equivalent to GLFW_MOUSE_BUTTON_LAST
}

struct MouseButtonState
{
    bool down = false;
    bool pressed = false;
    bool released = false;
};

struct MouseState
{
    float64 x = 0.0;
    float64 y = 0.0;
    float64 dx = 0.0;
    float64 dy = 0.0;
    float64 scrollY = 0.0;

    MouseButtonState buttons[Input::kMouseButtonLast + 1]{};

    void BeginFrame()
    {
        dx = 0.0;
        dy = 0.0;
        scrollY = 0.0;

        for (auto& button : buttons)
        {
            button.pressed = false;
            button.released = false;
        }
    }

    MouseButtonState& Button(int32 button)
    {
        assert(button >= 0 && button <= Input::kMouseButtonLast);
        return buttons[button];
    }

    const MouseButtonState& Button(int32 button) const
    {
        assert(button >= 0 && button <= Input::kMouseButtonLast);
        return buttons[button];
    }

    bool ButtonDown(int32 button) const
    {
        return Button(button).down;
    }

    bool ButtonPressed(int32 button) const
    {
        return Button(button).pressed;
    }

    bool ButtonReleased(int32 button) const
    {
        return Button(button).released;
    }

    bool LeftDown() const
    {
        return ButtonDown(Input::kMouseButtonLeft);
    }

    bool LeftPressed() const
    {
        return ButtonPressed(Input::kMouseButtonLeft);
    }

    bool LeftReleased() const
    {
        return ButtonReleased(Input::kMouseButtonLeft);
    }
};
