#pragma once

#include <RenderingStudies/App.h>
#include <RenderingStudies/Types.h>

class Camera;
class Grid;
class Model;
class Shader;
class Window;

class ColorCoord final : public App
{
private:
    Window* m_Window;
    Shader* m_Shader;
    Camera* m_Camera;
    Grid* m_Grid;
    Model* m_Model;

public:
    ColorCoord(Window* window);
    ~ColorCoord();

    virtual void Update(float32 deltaTime) override;
    virtual void Render() override;
};
