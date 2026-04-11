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
    Shader* m_Shader = nullptr;
    Model* m_Model = nullptr;
    int32 m_CurrentMesh = 0;
    int32 m_LoadedMeshIndex = -1;
    glm::vec3 m_ModelPos;

    static const char* const s_MeshOptions[7];
    static const char* const s_MeshPaths[7];

    void LoadCurrentModel();

public:
    SimpleMesh(const Window& window, const Camera& camera);
    ~SimpleMesh();

    virtual void Update(float32 deltaTime) override;
    virtual void Render() override;
};
