#include "./Cube.h"

#include "../../FileManager/FileManager.h"

Cube::Cube()
    : m_Model(1.f),
      m_View(1.f),
      m_Shader(Shader(
          FileManager::Read("shaders/simple.vs"), // Vertex shader source
          FileManager::Read("shaders/simple.fs")  // Fragment shader source
          ))
{
    CreateMesh();

    // Model matrix
    m_Model = glm::mat4(1.f); // Identity matrix, meaning no transformation on the model

    m_Camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
    m_LastX = 1200 / 2.0f;
    m_LastY = 900 / 2.0f;
}

Cube::~Cube()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
}

void Cube::Update(Window *window)
{
    float64 xpos = window->GetMouseX();
    float64 ypos = window->GetMouseY();

    if (m_FirstMouse)
    {
        m_LastX = xpos;
        m_LastY = ypos;
        m_FirstMouse = false;
    }

    float xoffset = xpos - m_LastX;
    float yoffset = m_LastY - ypos; // Reversed since Y-coordinates go from bottom to top
    m_LastX = xpos;
    m_LastY = ypos;

    m_Camera->ProcessMouseMovement(xoffset, yoffset);

    // Get the currentFrame in seconds
    float32 currentFrame = window->GetTime();
    m_DeltaTime = currentFrame - m_LastFrame;
    m_LastFrame = currentFrame;

    // Get the window dimensions
    uint32 windowWidth = window->GetWidth();
    uint32 windowHeight = window->GetHeight();

    {
        m_Shader.Use();

        glm::mat4 view = m_Camera->GetViewMatrix();

        glm::mat4 projection = glm::perspective(
            glm::radians(m_Camera->GetZoom()),            // Field of view (90 degrees in radians)
            (float32)windowWidth / (float32)windowHeight, // Aspect ratio (width / height)
            0.01f,                                        // Near clipping plane
            100.0f                                        // Far clipping plane
        );

        if (window->IsKeyPressed(KeyToken::Up) || window->IsKeyPressed(KeyToken::W))
        {
            m_Camera->ProcessKeyboard(CameraMovement::FORWARD, m_DeltaTime);
        }

        if (window->IsKeyPressed(KeyToken::Down) || window->IsKeyPressed(KeyToken::S))
        {
            m_Camera->ProcessKeyboard(CameraMovement::BACKWARD, m_DeltaTime);
        }

        if (window->IsKeyPressed(KeyToken::Left) || window->IsKeyPressed(KeyToken::A))
        {
            m_Camera->ProcessKeyboard(CameraMovement::LEFT, m_DeltaTime);
        }

        if (window->IsKeyPressed(KeyToken::Right) || window->IsKeyPressed(KeyToken::D))
        {
            m_Camera->ProcessKeyboard(CameraMovement::RIGHT, m_DeltaTime);
        }

        m_Shader.SetMat4("u_model", m_Model);
        m_Shader.SetMat4("u_view", view);
        m_Shader.SetMat4("u_projection", projection);

        glBindVertexArray(m_VAO);

        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // Unbind the VAO
        glBindVertexArray(0);
    }
}

void Cube::CreateMesh()
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
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    // Bind and set vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Bind and set index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
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
