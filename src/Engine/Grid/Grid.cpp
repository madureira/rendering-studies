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

static glm::vec2 CalculateFract(const glm::dvec3& position, float64 scale)
{
    float64 scaledX = position.x / scale;
    float64 scaledZ = position.z / scale;
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

void Grid::Render(const Camera& camera, uint32 windowWidth, uint32 windowHeight, bool showYAxis) const
{
    glm::dvec3 cameraPosHP = camera.GetPositionHP();
    glm::mat4 projection = camera.GetProjectionMatrix(windowWidth, windowHeight);

    glm::mat4 viewRel = camera.GetViewMatrixRelative();

    // Camera position relative to the floating origin
    glm::dvec3 origin = cameraPosHP;
    origin.y = 0.0;

    glm::vec3 relativePos = glm::vec3(cameraPosHP - origin);

    // Compute fractions in double precision for precise grid alignment
    glm::vec2 gridFract1 = CalculateFract(cameraPosHP, 1.0);
    glm::vec2 gridFract10 = CalculateFract(cameraPosHP, 10.0);
    glm::vec2 gridFract100 = CalculateFract(cameraPosHP, 100.0);

    // Disable wireframe mode
#ifndef __EMSCRIPTEN__
    GLint oldPolygonMode[2];
    GL(glGetIntegerv(GL_POLYGON_MODE, oldPolygonMode));
    GL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
#endif

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

    // Restore previous state
#ifndef __EMSCRIPTEN__
    assert(oldPolygonMode[0] == oldPolygonMode[1]);
    GL(glPolygonMode(GL_FRONT_AND_BACK, oldPolygonMode[0]));
#endif
}
