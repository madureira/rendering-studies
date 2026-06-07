#pragma once

#include <RenderingStudies/Demo.h>
#include <RenderingStudies/Types.h>
#include <glm/glm.hpp>

class Camera;
class Shader;
class Window;
class Texture;

class ShadowMapping final : public Demo
{
private:
    const uint32 SHADOW_WIDTH = 1024;
    const uint32 SHADOW_HEIGHT = 1024;
    const uint32 PLANE_INDEX = 0;
    const uint32 CUBE_INDEX = 1;
    const uint32 INDEX_COUNT = 36;

    const Window& m_Window;
    const Camera& m_Camera;
    Shader* m_ShadowMapShader;
    Shader* m_DepthShader;
    Texture* m_Texture;
    Texture* m_MarbleTexture;

    uint32 m_DepthMapFBO;
    uint32 m_DepthMapTexture;

    uint32 m_VAO[2];
    uint32 m_VBO[2];
    uint32 m_EBO[2];

    glm::vec3 m_LightPos;
    glm::vec3 m_CubePos;

public:
    ShadowMapping(const Window& window, const Camera& camera);
    ~ShadowMapping();

    virtual void Update(float32 deltaTime) override;
    virtual void Render() override;

private:
    glm::mat4 ComputeLightSpaceMatrix() const;
    void CreateMesh(const uint32 index);
    void CreateFrameBuffer();
    void RenderDepthMap();
};
