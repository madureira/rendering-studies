#pragma once

#include <RenderingStudies/App.h>
#include <RenderingStudies/Types.h>

class Camera;
class Grid;
class Shader;
class Window;

class Skybox final : public App
{
private:
    Window* m_Window;
    Shader* m_Shader;
    Camera* m_Camera;
    Grid* m_Grid;

    uint32 m_VAO;
    uint32 m_VBO;
    uint32 m_EBO;
    uint32 m_SkyboxTexture;

public:
    Skybox(Window* window);
    ~Skybox();

    virtual void Update(float32 deltaTime) override;
    virtual void Render() override;

private:
    void CreateMesh();
    void LoadSkybox(const char* basePath);
};
