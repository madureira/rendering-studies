#include "./RippleEffect.h"

#include "../../FileManager/FileManager.h"

RippleEffect::RippleEffect(Window *window)
    : m_Window(window)
{
    m_Shader = new Shader("assets/shaders/ripple_effect.vert", "assets/shaders/ripple_effect.frag");
    m_Camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
    CreateMesh();
}

RippleEffect::~RippleEffect()
{
    delete m_Camera;
    delete m_Shader;
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
}

void RippleEffect::Update()
{
    float32 time = m_Window->GetTime();
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

    float32 speed = 1.0f;

    if (m_Window->IsKeyPressed(KeyToken::LeftShift))
    {
        speed = 3.f;
    }

    if (m_Window->IsKeyPressed(KeyToken::Up) || m_Window->IsKeyPressed(KeyToken::W))
    {
        m_Camera->ProcessKeyboard(CameraMove::FORWARD, m_DeltaTime, speed);
    }

    if (m_Window->IsKeyPressed(KeyToken::Down) || m_Window->IsKeyPressed(KeyToken::S))
    {
        m_Camera->ProcessKeyboard(CameraMove::BACKWARD, m_DeltaTime, speed);
    }

    if (m_Window->IsKeyPressed(KeyToken::Left) || m_Window->IsKeyPressed(KeyToken::A))
    {
        m_Camera->ProcessKeyboard(CameraMove::LEFT, m_DeltaTime, speed);
    }

    if (m_Window->IsKeyPressed(KeyToken::Right) || m_Window->IsKeyPressed(KeyToken::D))
    {
        m_Camera->ProcessKeyboard(CameraMove::RIGHT, m_DeltaTime, speed);
    }

    if (m_Window->IsKeyPressed(KeyToken::Space))
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = m_Camera->GetViewMatrix();
    glm::mat4 projection = glm::perspective(
        glm::radians(m_Camera->GetZoom()),            // Field of view (90 degrees in radians)
        (float32)windowWidth / (float32)windowHeight, // Aspect ratio (width / height)
        0.01f,                                        // Near clipping plane
        100.0f                                        // Far clipping plane
    );

    m_Shader->Bind();

    m_Shader->SetMat4("uModel", model);
    m_Shader->SetMat4("uView", view);
    m_Shader->SetMat4("uProjection", projection);

    m_Shader->SetFloat("uTime", time);
    m_Shader->SetFloat("uAmplitude", 0.2f);
    m_Shader->SetFloat("uFrequency", 5.0f);

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    // Unbind the VAO
    glBindVertexArray(0);
    m_Shader->Unbind();
}

void RippleEffect::CreateMesh()
{
    uint8 gridSize = 128; // Number of subdivisions
    float32 size = 20.0f; // Size of the plane

    for (uint8 y = 0; y <= gridSize; ++y)
    {
        for (uint8 x = 0; x <= gridSize; ++x)
        {
            float32 xPos = (x / (float32)gridSize) * size - size / 2.0f;
            float32 yPos = 0.0f; // Initial height
            float32 zPos = (y / (float32)gridSize) * size - size / 2.0f;

            vertices.push_back(xPos);
            vertices.push_back(yPos);
            vertices.push_back(zPos);
        }
    }

    for (uint8 y = 0; y < gridSize; ++y)
    {
        for (uint8 x = 0; x < gridSize; ++x)
        {
            uint32 topLeft = y * (gridSize + 1) + x;
            uint32 topRight = topLeft + 1;
            uint32 bottomLeft = (y + 1) * (gridSize + 1) + x;
            uint32 bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    // Generate and bind the VAO
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    // Generate and bind the VBO
    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float32), vertices.data(), GL_STATIC_DRAW);

    // Generate and bind the EBO
    glGenBuffers(1, &m_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32), indices.data(), GL_STATIC_DRAW);

    // Set vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float32), (void *)0);
    glEnableVertexAttribArray(0); // Position attribute

    // Unbind objects
    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
