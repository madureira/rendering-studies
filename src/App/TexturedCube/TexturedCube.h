#pragma once

#include <RenderingStudies/App.h>
#include <RenderingStudies/Types.h>

class Camera;
class Shader;
class Texture;
class Window;

class TexturedCube final : public App
{
private:
    Window* m_Window;
    Shader* m_Shader;
    Camera* m_Camera;
    Texture* m_Texture;

    uint32 m_VAO;
    uint32 m_VBO;
    uint32 m_EBO;

public:
    TexturedCube(Window* window);
    ~TexturedCube();

    virtual void Update(float32 deltaTime) override;

private:
    void CreateMesh();
};
