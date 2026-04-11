#pragma once

#include <RenderingStudies/Demo.h>
#include <RenderingStudies/Types.h>
#include <glm/glm.hpp>

class Camera;
class Model;
class Shader;
class Window;

class TeapotShading final : public Demo
{
private:
    const Window& m_Window;
    const Camera& m_Camera;
    Shader* m_Shader[3];
    Model* m_Model;

    int32 m_CurrentShader;
    glm::vec3 m_LightDir;
    glm::vec3 m_RotDeg;
    glm::vec3 m_Scale;
    bool m_LockScale;
    float32 m_ScaleAll;

    static const char* const s_ShaderOptions[3];

public:
    TeapotShading(const Window& window, const Camera& camera);
    ~TeapotShading();

    virtual void Update(float32 deltaTime) override;
    virtual void Render() override;
};
