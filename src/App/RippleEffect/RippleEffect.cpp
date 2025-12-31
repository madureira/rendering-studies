#include "RippleEffect.h"
#include <RenderingStudies/RegisterApp.h>
#include "../../Shader/Shader.h"
#include "../../Window/Window.h"
#include "../Camera.h"

REGISTER_APP(RippleEffect)

RippleEffect::RippleEffect(Window* window)
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

void RippleEffect::Update(float32 deltaTime)
{
    const MouseState& mouse = m_Window->GetMouse();

    if (mouse.LeftDown())
    {
        m_Camera->ProcessMouseDelta((float32)mouse.dx, (float32)mouse.dy);
    }

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
}

void RippleEffect::Render()
{
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = m_Camera->GetViewMatrix();
    glm::mat4 projection = m_Camera->GetProjectionMatrix(m_Window->GetWidth(), m_Window->GetHeight());

    m_Shader->Bind();

    m_Shader->SetMat4("u_Model", model);
    m_Shader->SetMat4("u_View", view);
    m_Shader->SetMat4("u_Projection", projection);

    m_Shader->SetFloat("u_Time", m_Window->GetTime());
    m_Shader->SetFloat("u_Amplitude", 0.2f);
    m_Shader->SetFloat("u_Frequency", 5.0f);

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
    GL(glGenVertexArrays(1, &m_VAO));
    GL(glBindVertexArray(m_VAO));

    // Generate and bind the VBO
    GL(glGenBuffers(1, &m_VBO));
    GL(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
    GL(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float32), vertices.data(), GL_STATIC_DRAW));

    // Generate and bind the EBO
    GL(glGenBuffers(1, &m_EBO));
    GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO));
    GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32), indices.data(), GL_STATIC_DRAW));

    // Set vertex attribute pointers
    GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float32), (void*)0));
    GL(glEnableVertexAttribArray(0)); // Position attribute

    // Unbind objects
    GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GL(glBindVertexArray(0));
}
