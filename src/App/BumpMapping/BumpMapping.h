#pragma once

#include <RenderingStudies/App.h>
#include <RenderingStudies/Types.h>

class Camera;
class Shader;
class Window;

class BumpMapping final : public App
{
private:
    Window* m_Window;
    Shader* m_Shader;
    Camera* m_Camera;

    uint32 m_VAO;
    uint32 m_VBO;
    uint32 m_EBO;
    size_t m_IndexCount;

public:
    BumpMapping(Window* window);
    ~BumpMapping();

    virtual void Update(float32 deltaTime) override;
    virtual void Render() override;

private:
    void CreateMesh();
};
