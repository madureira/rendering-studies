#pragma once

#include <RenderingStudies/App.h>
#include <RenderingStudies/Types.h>

class Camera;
class Grid;
class Model;
class Shader;
class Window;

class BlenderModel final : public App
{
private:
    Window* m_Window;
    Shader* m_Shader;
    Camera* m_Camera;
    Grid* m_Grid;
    Model* m_Model;
    Model* m_Model2;

public:
    BlenderModel(Window* window);
    ~BlenderModel();

    virtual void Update(float32 deltaTime) override;
};
