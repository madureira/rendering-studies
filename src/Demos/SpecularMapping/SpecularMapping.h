#pragma once

#include <glm/glm.hpp>

#include <RenderingStudies/Demo.h>

class Camera;
class Model;
class Shader;
class Texture;
class Window;

class SpecularMapping final : public Demo
{
private:
    const Window& m_Window;
    const Camera& m_Camera;
    Shader* m_Shader;
    Model* m_Model;
    Texture* m_DiffuseMap;
    Texture* m_NormalMap;
    Texture* m_SpecularMap;
    glm::vec3 m_LightPos;
    float32 m_NormalStrength = 1.0f;
    float32 m_SpecularStrength = 1.0f;

public:
    SpecularMapping(const Window& window, const Camera& camera);
    ~SpecularMapping();

    virtual void Update(float32 deltaTime) override;
    virtual void Render() override;
};
