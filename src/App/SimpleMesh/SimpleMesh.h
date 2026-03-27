#pragma once

#include <RenderingStudies/App.h>
#include <RenderingStudies/Types.h>
#include <glm/glm.hpp>

class Camera;
class Model;
class Shader;
class Window;

class SimpleMesh final : public App
{
private:
    const Window& m_Window;
    const Camera& m_Camera;
    Shader* m_Shader;
    Model* m_Model[7];
    int32 m_CurrentMesh;
    glm::vec3 m_ModelPos;

    static const char* const s_MeshOptions[7];

public:
    SimpleMesh(const Window& window, const Camera& camera);
    ~SimpleMesh();

    virtual void Update(float32 deltaTime) override;
    virtual void Render() override;
};
