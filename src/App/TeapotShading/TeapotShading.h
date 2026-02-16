#pragma once

#include <RenderingStudies/App.h>
#include <RenderingStudies/Types.h>
#include <glm/glm.hpp>

class Camera;
class Grid;
class Model;
class Shader;
class Window;

class TeapotShading final : public App
{
private:
    Window* m_Window;
    Shader* m_Shader[3];
    Camera* m_Camera;
    Grid* m_Grid;
    Model* m_Model;

    int32 m_CurrentShader;
    glm::vec3 m_LightDir;
    glm::vec3 m_RotDeg;
    glm::vec3 m_Scale;
    bool m_LockScale;
    float32 m_ScaleAll;

    static const char* const s_ShaderOptions[3];

public:
    TeapotShading(Window* window);
    ~TeapotShading();

    virtual void Update(float32 deltaTime) override;
    virtual void Render() override;
};
