#include "./cube.h"

#include <iostream>

#include "../../file_manager/file_manager.h"

Cube::Cube()
    : m_model(1.f),
      m_view(1.f),
      m_shader(Shader(
          FileManager::read("shaders/simple.vs"), // Vertex shader source
          FileManager::read("shaders/simple.fs")  // Fragment shader source
          ))
{
    createMesh();

    // Model matrix
    m_model = glm::mat4(1.f); // Identity matrix, meaning no transformation on the model

    m_camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
    m_lastX = 1200 / 2.0f;
    m_lastY = 900 / 2.0f;
}

Cube::~Cube()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
}

void Cube::update(Window *window)
{
    float64 xpos = window->getMouseX();
    float64 ypos = window->getMouseY();

    if (m_firstMouse)
    {
        m_lastX = xpos;
        m_lastY = ypos;
        m_firstMouse = false;
    }

    float xoffset = xpos - m_lastX;
    float yoffset = m_lastY - ypos; // Reversed since Y-coordinates go from bottom to top
    m_lastX = xpos;
    m_lastY = ypos;

    m_camera->processMouseMovement(xoffset, yoffset);

    // Get the currentFrame in seconds
    float32 currentFrame = window->getTime();
    m_deltaTime = currentFrame - m_lastFrame;
    m_lastFrame = currentFrame;

    // Get the window dimensions
    uint32 windowWidth = window->getWidth();
    uint32 windowHeight = window->getHeight();

    {
        m_shader.use();

        glm::mat4 view = m_camera->getViewMatrix();

        glm::mat4 projection = glm::perspective(
            glm::radians(m_camera->Zoom),                 // Field of view (90 degrees in radians)
            (float32)windowWidth / (float32)windowHeight, // Aspect ratio (width / height)
            0.01f,                                        // Near clipping plane
            100.0f                                        // Far clipping plane
        );

        if (window->isKeyPressed(KeyToken::Up) || window->isKeyPressed(KeyToken::W))
        {
            m_camera->processKeyboard(FORWARD, m_deltaTime);
        }

        if (window->isKeyPressed(KeyToken::Down) || window->isKeyPressed(KeyToken::S))
        {
            m_camera->processKeyboard(BACKWARD, m_deltaTime);
        }

        if (window->isKeyPressed(KeyToken::Left) || window->isKeyPressed(KeyToken::A))
        {
            m_camera->processKeyboard(LEFT, m_deltaTime);
        }

        if (window->isKeyPressed(KeyToken::Right) || window->isKeyPressed(KeyToken::D))
        {
            m_camera->processKeyboard(RIGHT, m_deltaTime);
        }

        m_shader.setMat4("u_model", m_model);
        m_shader.setMat4("u_view", view);
        m_shader.setMat4("u_projection", projection);

        glBindVertexArray(m_vao);

        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // Unbind the VAO
        glBindVertexArray(0);
    }
}

void Cube::createMesh()
{
    float32 vertices[] = {
        // Positions          // Colors
        // Front face
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  // Bottom-left
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  // Bottom-right
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  // Top-right
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  // Top-left

        // Back face
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.5f, 0.5f, 0.5f,
    };

    uint32 indices[] = {
        // Front face
        0, 1, 2,  2, 3, 0,
        // Back face
        4, 5, 6,  6, 7, 4,
        // Left face
        4, 0, 3,  3, 7, 4,
        // Right face
        1, 5, 6,  6, 2, 1,
        // Top face
        3, 2, 6,  6, 7, 3,
        // Bottom face
        4, 5, 1,  1, 0, 4
    };

    // Generate and bind VAO
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    // Bind and set vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Bind and set index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute (location = 0 in shader)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Color attribute (location = 1 in shader)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind VAO
    glBindVertexArray(0);
}
