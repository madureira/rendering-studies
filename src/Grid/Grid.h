#pragma once

#include <RenderingStudies/Types.h>
#include <glm/glm.hpp>

class Camera;
class Shader;

class Grid
{
private:
    Shader* m_Shader;
    uint32 m_VAO;
    uint32 m_VBO;
    uint32 m_EBO;

    static const float32 m_Vertices[8];
    static const uint32 m_Indices[6];

public:
    Grid();
    ~Grid();

    void Draw(const Camera& camera, const glm::mat4& projection) const;
};
