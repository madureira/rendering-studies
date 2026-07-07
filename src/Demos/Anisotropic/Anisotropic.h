#pragma once

#include <glm/glm.hpp>

#include <RenderingStudies/Demo.h>

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
