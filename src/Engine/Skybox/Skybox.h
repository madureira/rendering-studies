#pragma once

#include <RenderingStudies/Types.h>
#include <string>
#include <glm/glm.hpp>

class Camera;
class Shader;

class Skybox final
{
private:
    Shader* m_Shader;

    uint32 m_VAO;
    uint32 m_VBO;
    uint32 m_EBO;
    uint32 m_Texture;

public:
    Skybox(const std::string& skyboxTexturesBasePath);
    ~Skybox();

    void Render(const Camera& camera, uint32 windowWidth, uint32 windowHeight, float32 exposure = 1.0f) const;

private:
    void CreateMesh();
    void LoadSkybox(const std::string& basePath);
};
