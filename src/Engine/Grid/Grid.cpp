#include "Grid.h"

#include <RenderingStudies/GL.h>
#include <glm/gtc/matrix_transform.hpp>

#include "../Camera/Camera.h"
#include "../Shader/Shader.h"

const float32 Grid::m_Vertices[8] = {
    -1.0f, -1.0f,
    -1.0f,  1.0f,
     1.0f,  1.0f,
     1.0f, -1.0f,
};
const uint32 Grid::m_Indices[6] = {
    0, 2, 1,
    0, 3, 2,
};

static glm::vec2 CalculateFract(const glm::dvec3& position, double scale)
{
    double scaledX = position.x / scale;
    double scaledZ = position.z / scale;
    return glm::vec2(
        static_cast<float>(scaledX - glm::floor(scaledX)),
        static_cast<float>(scaledZ - glm::floor(scaledZ)));
}

Grid::Grid()
{
    m_Shader = new Shader("assets/shaders/grid.vert", "assets/shaders/grid.frag");

    GL(glGenVertexArrays(1, &m_VAO));
    GL(glGenBuffers(1, &m_VBO));
    GL(glGenBuffers(1, &m_EBO));

    GL(glBindVertexArray(m_VAO));

    GL(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
    GL(glBufferData(GL_ARRAY_BUFFER, sizeof(m_Vertices), m_Vertices, GL_STATIC_DRAW));

    GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO));
    GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_Indices), m_Indices, GL_STATIC_DRAW));

    GL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float32), (void*)0));
    GL(glEnableVertexAttribArray(0));

    GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GL(glBindVertexArray(0));
}

Grid::~Grid()
{
    delete m_Shader;
    GL(glDeleteVertexArrays(1, &m_VAO));
    GL(glDeleteBuffers(1, &m_VBO));
    GL(glDeleteBuffers(1, &m_EBO));
}

void Grid::Draw(const Camera& camera, const glm::mat4& viewRel, const glm::mat4& projection, const glm::dvec3& origin, bool showYAxis) const
{
    glm::dvec3 positionHP = camera.GetPositionHP();

    // Camera position relative to the floating origin
    glm::vec3 relativePos = glm::vec3(positionHP - origin);

    // Compute fractions in double precision for precise grid alignment
    glm::vec2 gridFract1 = CalculateFract(positionHP, 1.0);
    glm::vec2 gridFract10 = CalculateFract(positionHP, 10.0);
    glm::vec2 gridFract100 = CalculateFract(positionHP, 100.0);

    m_Shader->Bind();

    // Use the same viewRel as the rest of the scene (depth coherence)
    m_Shader->SetMat4("u_View", viewRel);
    m_Shader->SetMat4("u_Projection", projection);
    m_Shader->SetMat4("u_ViewInv", glm::inverse(viewRel));
    m_Shader->SetMat4("u_ProjectionInv", glm::inverse(projection));
    m_Shader->SetVec3("u_CameraPos", relativePos);
    m_Shader->SetVec3("u_GridOrigin", glm::vec3(origin));
    m_Shader->SetVec2("u_GridFract1", gridFract1);
    m_Shader->SetVec2("u_GridFract10", gridFract10);
    m_Shader->SetVec2("u_GridFract100", gridFract100);
    m_Shader->SetBool("u_ShowYAxis", showYAxis);

    GL(glBindVertexArray(m_VAO));
    GL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

    GL(glBindVertexArray(0));
    m_Shader->Unbind();
}
