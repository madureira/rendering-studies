#pragma once

#include <RenderingStudies/Demo.h>
#include <RenderingStudies/Types.h>
#include <glm/glm.hpp>

class Camera;
class Model;
class Shader;
class Window;

class Anisotropic final : public Demo
{
private:
    const Window& m_Window;
    const Camera& m_Camera;
    Shader* m_Shader;
    Model* m_Model;

    glm::vec3 m_LightDir;
    glm::vec3 m_RotDeg;
    float32 m_Roughness;
    float32 m_Anisotropy;
    float32 m_Metallic;
    glm::vec3 m_Albedo;

public:
    Anisotropic(const Window& window, const Camera& camera);
    ~Anisotropic();

    virtual void Update(float32 deltaTime) override;
    virtual void Render() override;
};
