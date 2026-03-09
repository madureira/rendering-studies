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
    Skybox(std::string& skyboxTexturesBasePath);
    ~Skybox();

    void Render(const Camera& camera, const uint32 windowWidth, const uint32 windowHeight, const float32 exposure = 1.0f) const;

private:
    void CreateMesh();
    void LoadSkybox(const std::string& basePath);
};
