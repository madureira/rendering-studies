#pragma once

#include <RenderingStudies/App.h>
#include <RenderingStudies/Types.h>

class Camera;
class Shader;
class Window;

class Cube final : public App
{
private:
    const Window& m_Window;
    const Camera& m_Camera;
    Shader* m_Shader;

    uint32 m_VAO;
    uint32 m_VBO;
    uint32 m_EBO;

public:
    Cube(const Window& window, const Camera& camera);
    ~Cube();

    virtual void Update(float32 deltaTime) override;
    virtual void Render() override;

private:
    void CreateMesh();
};
