#pragma once

#include <RenderingStudies/App.h>
#include <RenderingStudies/Types.h>
#include <glm/glm.hpp>

class Camera;
class Grid;
class Model;
class Shader;
class Window;

class Anisotropic final : public App
{
private:
    Window* m_Window;
    Shader* m_Shader;
    Camera* m_Camera;
    Grid* m_Grid;
    Model* m_Model;

    glm::vec3 m_LightDir;
    glm::vec3 m_RotDeg;
    float32 m_Roughness;
    float32 m_Anisotropy;
    float32 m_Metallic;
    glm::vec3 m_Albedo;

public:
    Anisotropic(Window* window);
    ~Anisotropic();

    virtual void Update(float32 deltaTime) override;
    virtual void Render() override;
};
