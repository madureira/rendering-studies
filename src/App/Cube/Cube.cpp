#include "./Cube.h"

#include "../../FileManager/FileManager.h"

Cube::Cube(Window *window)
    : m_Window(window)
{
    m_Shader = new Shader("assets/shaders/simple.vert", "assets/shaders/simple.frag");
    m_Camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
    CreateMesh();
}

Cube::~Cube()
{
    delete m_Camera;
    delete m_Shader;
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
}

void Cube::Update(float32 deltaTime)
{
    m_Camera->ProcessMouseMovement(m_Window->GetMouseX(), m_Window->GetMouseY());

    float32 speed = 1.0f;

    if (m_Window->IsKeyPressed(KeyToken::LeftShift))
    {
        speed = 3.f;
    }

    if (m_Window->IsKeyPressed(KeyToken::Up) || m_Window->IsKeyPressed(KeyToken::W))
    {
        m_Camera->ProcessKeyboard(CameraMove::FORWARD, deltaTime, speed);
    }

    if (m_Window->IsKeyPressed(KeyToken::Down) || m_Window->IsKeyPressed(KeyToken::S))
    {
        m_Camera->ProcessKeyboard(CameraMove::BACKWARD, deltaTime, speed);
    }

    if (m_Window->IsKeyPressed(KeyToken::Left) || m_Window->IsKeyPressed(KeyToken::A))
    {
        m_Camera->ProcessKeyboard(CameraMove::LEFT, deltaTime, speed);
    }

    if (m_Window->IsKeyPressed(KeyToken::Right) || m_Window->IsKeyPressed(KeyToken::D))
    {
        m_Camera->ProcessKeyboard(CameraMove::RIGHT, deltaTime, speed);
    }

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = m_Camera->GetViewMatrix();
    glm::mat4 projection = m_Camera->GetProjectionMatrix(m_Window->GetWidth(), m_Window->GetHeight());

    m_Shader->Bind();

    m_Shader->SetMat4("uModel", model);
    m_Shader->SetMat4("uView", view);
    m_Shader->SetMat4("uProjection", projection);

    glBindVertexArray(m_VAO);

    glCullFace(GL_BACK);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glCullFace(GL_FRONT);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Undind
    glCullFace(GL_BACK);
    glBindVertexArray(0);
    m_Shader->Unbind();
}

void Cube::CreateMesh()
{
    float32 vertices[] = {
        // Positions        // Colors
        // Front face
        -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, // Bottom-left
         0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, // Bottom-right
         0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f, // Top-right
        -0.5f,  0.5f, 0.5f, 1.0f, 1.0f, 0.0f, // Top-left

        // Back face
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, // Bottom-left
         0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, // Bottom-right
         0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, // Top-right
        -0.5f,  0.5f, -0.5f, 0.5f, 0.5f, 0.5f, // Top-left
    };

    uint32 indices[] = {
        // Front face
        0, 1, 2, 2, 3, 0,
        // Back face
        4, 5, 6, 6, 7, 4,
        // Left face
        4, 0, 3, 3, 7, 4,
        // Right face
        1, 5, 6, 6, 2, 1,
        // Top face
        3, 2, 6, 6, 7, 3,
        // Bottom face
        4, 5, 1, 1, 0, 4,
    };

    // Generate Objects
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    // Bind Vertex Array Object
    glBindVertexArray(m_VAO);

    // Bind and set vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Bind and set index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute (location = 0 in shader)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float32), (void *)0);
    glEnableVertexAttribArray(0);

    // Color attribute (location = 1 in shader)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float32), (void *)(3 * sizeof(float32)));
    glEnableVertexAttribArray(1);

    // Unbind objects
    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
