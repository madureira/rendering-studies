#pragma once

#include <RenderingStudies/App.h>
#include <RenderingStudies/Types.h>

class Camera;
class Shader;
class Window;

class Triangle final : public App
{
private:
    Window* m_Window;
    Camera* m_Camera;
    Shader* m_Shader;

    uint32 m_VAO;
    uint32 m_VBO;
    uint32 m_EBO;

public:
    Triangle(Window* window);
    ~Triangle();

    virtual void Update(float32 deltaTime) override;
    virtual void Render() override;

private:
    void CreateMesh();
};
