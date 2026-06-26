#pragma once

#include <glm/glm.hpp>

#include <RenderingStudies/Demo.h>
#include <RenderingStudies/Types.h>

class Camera;
class Model;
class Shader;
class Texture;
class Window;

class NormalMapping final : public Demo
{
private:
    const Window& m_Window;
    const Camera& m_Camera;
    Shader* m_Shader;
    Model* m_Model;
    Texture* m_DiffuseMap;
    Texture* m_NormalMap;
    glm::vec3 m_LightPos;
    float32 m_NormalStrength = 2.0f;
    float32 m_TexScale = 4.0f;

public:
    NormalMapping(const Window& window, const Camera& camera);
    ~NormalMapping();

    virtual void Update(float32 deltaTime) override;
    virtual void Render() override;
};
