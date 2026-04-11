#pragma once

#include <RenderingStudies/Demo.h>
#include <RenderingStudies/Types.h>
#include <glm/glm.hpp>

class Camera;
class Shader;
class Window;

class BumpMapping final : public Demo
{
private:
    const Window& m_Window;
    const Camera& m_Camera;
    Shader* m_Shader;

    uint32 m_VAO;
    uint32 m_VBO;
    uint32 m_EBO;
    size_t m_IndexCount;

    glm::vec3 m_LightPos;
    glm::vec3 m_LightColor;
    glm::vec2 m_RippleCenterXZ;
    float32 m_Time;
    float32 m_Amp;
    float32 m_K;
    float32 m_Omega;
    float32 m_Damping;
    float32 m_NormalStrength;

public:
    BumpMapping(const Window& window, const Camera& camera);
    ~BumpMapping();

    virtual void Update(float32 deltaTime) override;
    virtual void Render() override;

private:
    void CreateMesh();
};
