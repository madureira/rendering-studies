#pragma once

#include <glm/glm.hpp>

#include <RenderingStudies/Demo.h>

class Camera;
class ModelSelector;
class Shader;
class Window;

class SimpleMesh final : public Demo
{
private:
    const Window& m_Window;
    const Camera& m_Camera;
    Shader* m_Shader = nullptr;
    ModelSelector* m_ModelSelector = nullptr;
    glm::vec3 m_ModelPos;

public:
    SimpleMesh(const Window& window, const Camera& camera);
    ~SimpleMesh();

    virtual void Update(float32 deltaTime) override;
    virtual void Render() override;
};
