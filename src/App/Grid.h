#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <RenderingStudies/GL.h>
#include <RenderingStudies/Types.h>

#include "../Shader/Shader.h"
#include "Camera.h"

class Grid
{
private:
    Shader* m_Shader;
    uint32 m_VAO;
    uint32 m_VBO;
    uint32 m_EBO;

    const float32 m_Vertices[8] = {
        -1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f,
         1.0f, -1.0f,
    };
    const uint32 m_Indices[6] = {
        0, 2, 1,
        0, 3, 2,
    };

    // Calculate fractional part with double precision
    static glm::vec2 CalculateFract(const glm::dvec3& position, double scale)
    {
        double scaledX = position.x / scale;
        double scaledZ = position.z / scale;
        return glm::vec2(
            static_cast<float>(scaledX - glm::floor(scaledX)),
            static_cast<float>(scaledZ - glm::floor(scaledZ)));
    }

public:
    Grid()
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

    ~Grid()
    {
        delete m_Shader;
        GL(glDeleteVertexArrays(1, &m_VAO));
        GL(glDeleteBuffers(1, &m_VBO));
        GL(glDeleteBuffers(1, &m_EBO));
    }

    auto Draw(const Camera& camera, const glm::mat4& projection) const -> void
    {
        glm::dvec3 positionHP = camera.GetPositionHP();

        // Grid origin (camera position with Y=0)
        glm::dvec3 gridOrigin = positionHP;
        gridOrigin.y = 0.0;

        // Relative camera position (small values for shader precision)
        glm::vec3 relativePos = glm::vec3(positionHP - gridOrigin);

        // Relative view matrix
        glm::mat4 relativeView = glm::lookAt(relativePos, relativePos + camera.GetFront(), camera.GetUp());

        // Fractional parts for grid alignment (using double precision)
        glm::vec2 gridFract1 = CalculateFract(positionHP, 1.0);
        glm::vec2 gridFract10 = CalculateFract(positionHP, 10.0);
        glm::vec2 gridFract100 = CalculateFract(positionHP, 100.0);

        m_Shader->Bind();

        m_Shader->SetMat4("u_View", relativeView);
        m_Shader->SetMat4("u_Projection", projection);
        m_Shader->SetMat4("u_ViewInv", glm::inverse(relativeView));
        m_Shader->SetMat4("u_ProjectionInv", glm::inverse(projection));
        m_Shader->SetVec3("u_CameraPos", relativePos);
        m_Shader->SetVec3("u_GridOrigin", glm::vec3(gridOrigin));
        m_Shader->SetVec2("u_GridFract1", gridFract1);
        m_Shader->SetVec2("u_GridFract10", gridFract10);
        m_Shader->SetVec2("u_GridFract100", gridFract100);

        GL(glBindVertexArray(m_VAO));
        GL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

        GL(glBindVertexArray(0));
        m_Shader->Unbind();
    }
};
