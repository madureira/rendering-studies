#pragma once

#include <RenderingStudies/App.h>
#include <RenderingStudies/Types.h>

class Camera;
class Grid;
class Shader;
class Window;

class CubeMap final : public App
{
private:
    Window* m_Window;
    Shader* m_Shader;
    Camera* m_Camera;
    Grid* m_Grid;

    uint32 m_VAO;
    uint32 m_VBO;
    uint32 m_CubeMapTexture;

public:
    CubeMap(Window* window);
    ~CubeMap();

    virtual void Update(float32 deltaTime) override;
    virtual void Render() override;

private:
    void CreateMesh();
    bool LoadCubeMap(const char* basePath);
};
