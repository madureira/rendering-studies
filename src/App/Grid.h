#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <RenderingStudies/Types.h>

#include "../Shader/Shader.h"

class Grid
{
private:
    const float32 NEAR_CLIP = 0.01f;
    const float32 FAR_CLIP = 10000.0f;

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
    enum class plane
    {
        xz, xy,
        yz, yx,
    };

    Grid()
    {
        m_Shader = new Shader("assets/shaders/grid.vert", "assets/shaders/grid.frag");

        // Generate Objects
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glGenBuffers(1, &m_EBO);

        // Bind Vertex Array Object
        glBindVertexArray(m_VAO);

        // Bind Vertex Buffer Object
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_Vertices), m_Vertices, GL_STATIC_DRAW);

        // Bind Element Buffer Object
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_Indices), m_Indices, GL_STATIC_DRAW);

        // Set vertex attributes
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float32), (void*)0);
        glEnableVertexAttribArray(0);

        // Unbind objects
        glBindVertexArray(0);
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    ~Grid()
    {
        delete m_Shader;
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);
        glDeleteBuffers(1, &m_EBO);
    }

    auto Draw(glm::mat4 const& view, glm::mat4 const& projection) const -> void
    {
        m_Shader->Bind();

        m_Shader->SetFloat("uNear", NEAR_CLIP);
        m_Shader->SetFloat("uFar", FAR_CLIP);
        m_Shader->SetMat4("uView", view);
        m_Shader->SetMat4("uProjection", projection);

        glBindVertexArray(m_VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
        m_Shader->Unbind();
    }
};
