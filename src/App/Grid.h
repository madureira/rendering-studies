#pragma once

#include <RenderingStudies/GL.h>
#include <RenderingStudies/Types.h>

#include "../Shader/Shader.h"

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

public:
    Grid()
    {
        m_Shader = new Shader("assets/shaders/grid.vert", "assets/shaders/grid.frag");

        // Generate Objects
        GL(glGenVertexArrays(1, &m_VAO));
        GL(glGenBuffers(1, &m_VBO));
        GL(glGenBuffers(1, &m_EBO));

        // Bind Vertex Array Object
        GL(glBindVertexArray(m_VAO));

        // Bind Vertex Buffer Object
        GL(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
        GL(glBufferData(GL_ARRAY_BUFFER, sizeof(m_Vertices), m_Vertices, GL_STATIC_DRAW));

        // Bind Element Buffer Object
        GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO));
        GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_Indices), m_Indices, GL_STATIC_DRAW));

        // Set vertex attributes
        GL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float32), (void*)0));
        GL(glEnableVertexAttribArray(0));

        // Unbind objects
        GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
        GL(glBindVertexArray(0));
    }
    ~Grid()
    {
        delete m_Shader;
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);
        glDeleteBuffers(1, &m_EBO);
    }

    auto Draw(glm::mat4 const& view, glm::mat4 const& projection, glm::vec3 const& cameraPos) const -> void
    {
        m_Shader->Bind();

        m_Shader->SetMat4("u_View", view);
        m_Shader->SetMat4("u_Projection", projection);
        m_Shader->SetMat4("u_ViewInv", glm::inverse(view));
        m_Shader->SetMat4("u_ProjectionInv", glm::inverse(projection));
        m_Shader->SetVec3("u_CameraPos", cameraPos);

        glBindVertexArray(m_VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
        m_Shader->Unbind();
    }
};
