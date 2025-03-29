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

void Cube::Update()
{
    float64 xpos = m_Window->GetMouseX();
    float64 ypos = m_Window->GetMouseY();

    if (m_FirstMouse)
    {
        m_LastX = xpos;
        m_LastY = ypos;
        m_FirstMouse = false;
    }

    float32 xoffset = xpos - m_LastX;
    float32 yoffset = m_LastY - ypos; // Reversed since Y-coordinates go from bottom to top
    m_LastX = xpos;
    m_LastY = ypos;

    m_Camera->ProcessMouseMovement(xoffset, yoffset);

    // Get the currentFrame in seconds
    float32 currentFrame = m_Window->GetTime();
    m_DeltaTime = currentFrame - m_LastFrame;
    m_LastFrame = currentFrame;

    // Get the window dimensions
    uint32 windowWidth = m_Window->GetWidth();
    uint32 windowHeight = m_Window->GetHeight();

    m_Shader->Bind();

    if (m_Window->IsKeyPressed(KeyToken::Up) || m_Window->IsKeyPressed(KeyToken::W))
    {
        m_Camera->ProcessKeyboard(CameraMove::FORWARD, m_DeltaTime);
    }

    if (m_Window->IsKeyPressed(KeyToken::Down) || m_Window->IsKeyPressed(KeyToken::S))
    {
        m_Camera->ProcessKeyboard(CameraMove::BACKWARD, m_DeltaTime);
    }

    if (m_Window->IsKeyPressed(KeyToken::Left) || m_Window->IsKeyPressed(KeyToken::A))
    {
        m_Camera->ProcessKeyboard(CameraMove::LEFT, m_DeltaTime);
    }

    if (m_Window->IsKeyPressed(KeyToken::Right) || m_Window->IsKeyPressed(KeyToken::D))
    {
        m_Camera->ProcessKeyboard(CameraMove::RIGHT, m_DeltaTime);
    }

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = m_Camera->GetViewMatrix();
    glm::mat4 projection = glm::perspective(
        glm::radians(m_Camera->GetZoom()),            // Field of view (90 degrees in radians)
        (float32)windowWidth / (float32)windowHeight, // Aspect ratio (width / height)
        0.01f,                                        // Near clipping plane
        100.0f                                        // Far clipping plane
    );

    m_Shader->SetMat4("uModel", model);
    m_Shader->SetMat4("uView", view);
    m_Shader->SetMat4("uProjection", projection);

    if (m_Window->IsKeyPressed(KeyToken::Space))
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Unbind the VAO
    glBindVertexArray(0);

    m_Shader->Unbind();
}

void Cube::CreateMesh()
{
    float32 vertices[] = {
        // Positions          // Colors
        // Front face
        -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, // Bottom-left
        0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f,  // Bottom-right
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,   // Top-right
        -0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f,  // Top-left

        // Back face
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, // Bottom-left
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,  // Bottom-right
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f,   // Top-right
        -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  // Top-left
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
