#pragma once

#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <RenderingStudies/Types.h>

class Grid final
{
private:
    uint32 m_VAO;
    uint32 m_VBO;
    std::vector<float32> m_Vertices;

public:
    Grid()
    {
        int32 gridSize = 10;    // Half of the grid size
        float32 spacing = 1.0f; // Distance between lines

        for (int32 i = -gridSize; i <= gridSize; i++)
        {
            // Vertical lines (constant x, varying z)
            m_Vertices.push_back(i * spacing);
            m_Vertices.push_back(0.0f);
            m_Vertices.push_back(-gridSize * spacing);
            m_Vertices.push_back(i * spacing);
            m_Vertices.push_back(0.0f);
            m_Vertices.push_back(gridSize * spacing);

            // Horizontal lines (constant z, varying x)
            m_Vertices.push_back(-gridSize * spacing);
            m_Vertices.push_back(0.0f);
            m_Vertices.push_back(i * spacing);
            m_Vertices.push_back(gridSize * spacing);
            m_Vertices.push_back(0.0f);
            m_Vertices.push_back(i * spacing);
        }

        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);

        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(float32), m_Vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float32), (void *)0);
        glEnableVertexAttribArray(0);

        // Unbind objects
        glBindVertexArray(0);
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    ~Grid()
    {
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);
    }

    void Draw()
    {
        glBindVertexArray(m_VAO);
        glDrawArrays(GL_LINES, 0, m_Vertices.size() / 3);
        glBindVertexArray(0);
    }
};
