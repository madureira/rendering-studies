#include "RippleEffect.h"

#include <RenderingStudies/GL.h>
#include <RenderingStudies/RegisterApp.h>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

#include "../../Engine/Camera/Camera.h"
#include "../../Engine/Shader/Shader.h"
#include "../../Engine/Window/Window.h"

REGISTER_APP(RippleEffect)

RippleEffect::RippleEffect(const Window& window, const Camera& camera)
    : m_Window(window)
    , m_Camera(camera)
    , m_IndexCount(0)
    , m_TessLevel(20)
    , m_Amplitude(0.2f)
    , m_Frequency(5.0f)
{
    // Tessellation pipeline (vert + TCS + TES + frag): grid from CreateMesh(), each cell drawn as a patch.
    m_Shader = new Shader(
        "assets/shaders/ripple_effect.vert",
        "assets/shaders/ripple_effect.tesc",
        "assets/shaders/ripple_effect.tese",
        "assets/shaders/ripple_effect.frag");

    // Isometric-style: elevated, diagonal, looking at origin (not straight top-down).
    // Position in +X,+Y,+Z octant; yaw 225° + pitch ~-35° so front points at (0,0,0).
    const float32 isoDist = 14.0f; // distance in XZ
    const float32 isoHeight = 12.0f;

    m_Camera.OverrideInitialPosition(
        glm::vec3(isoDist, isoHeight, isoDist),
        glm::vec3(0.0f, 1.0f, 0.0f),
        225.0f,  // yaw: look from (+X,+Z) back toward origin
        -35.264f // pitch: ~35° down from horizontal (classic isometric)
    );

    CreateMesh();
}

RippleEffect::~RippleEffect()
{
    if (m_Shader)
    {
        m_Shader->Unbind();
        delete m_Shader;
    }
    GL(glDeleteVertexArrays(1, &m_VAO));
    GL(glDeleteBuffers(1, &m_VBO));
    GL(glDeleteBuffers(1, &m_EBO));
}

void RippleEffect::Update(float32 /*unused: deltaTime*/)
{
    ImGui::Begin("Ripple Effect");
    ImGui::SliderInt("Tessellation level", &m_TessLevel, 1, 64, "%d");
    ImGui::SliderFloat("Amplitude", &m_Amplitude, 0.01f, 1.0f, "%.2f");
    ImGui::SliderFloat("Frequency", &m_Frequency, 0.5f, 20.0f, "%.1f");
    ImGui::End();
}

void RippleEffect::Render()
{
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = m_Camera.GetViewMatrix();
    glm::mat4 projection = m_Camera.GetProjectionMatrix(m_Window.GetWidth(), m_Window.GetHeight());

    // Move model above the x-axis origin
    float distanceX = 1.0f;
    model = glm::translate(model, glm::vec3(0.0f, distanceX, 0.0f));

    m_Shader->Bind();

    m_Shader->SetMat4("u_Model", model);
    m_Shader->SetMat4("u_VP", projection * view);
    m_Shader->SetFloat("u_Time", m_Window.GetTime());
    m_Shader->SetInt("u_TessLevel", m_TessLevel);
    m_Shader->SetFloat("u_Amplitude", m_Amplitude);
    m_Shader->SetFloat("u_Frequency", m_Frequency);

    GL(glPatchParameteri(GL_PATCH_VERTICES, 4));
    GL(glBindVertexArray(m_VAO));
    GL(glDrawElements(GL_PATCHES, m_IndexCount, GL_UNSIGNED_INT, 0));

    // Unbind the VAO
    GL(glBindVertexArray(0));
    m_Shader->Unbind();
}

void RippleEffect::CreateMesh()
{
    uint8 gridSize = 4;  // Number of subdivisions
    float32 size = 20.0f; // Size of the plane

    const uint32 vertCount = (gridSize + 1) * (gridSize + 1);
    m_Vertices.reserve(vertCount * 3);

    const uint32 quadCount = gridSize * gridSize;
    // 4 indices per quad (one patch per cell for tessellation)
    m_Indices.reserve(quadCount * 4);

    for (uint8 y = 0; y <= gridSize; ++y)
    {
        for (uint8 x = 0; x <= gridSize; ++x)
        {
            float32 xPos = (x / (float32)gridSize) * size - size / 2.0f;
            float32 yPos = 0.0f; // Initial height
            float32 zPos = (y / (float32)gridSize) * size - size / 2.0f;

            m_Vertices.push_back(xPos);
            m_Vertices.push_back(yPos);
            m_Vertices.push_back(zPos);
        }
    }

    // Patch order for TES bilinear interp: p0=(0,0), p1=(1,0), p2=(1,1), p3=(0,1) -> bottomLeft, bottomRight, topRight, topLeft
    for (uint8 y = 0; y < gridSize; ++y)
    {
        for (uint8 x = 0; x < gridSize; ++x)
        {
            uint32 topLeft = y * (gridSize + 1) + x;
            uint32 topRight = topLeft + 1;
            uint32 bottomLeft = (y + 1) * (gridSize + 1) + x;
            uint32 bottomRight = bottomLeft + 1;

            m_Indices.push_back(bottomLeft);
            m_Indices.push_back(bottomRight);
            m_Indices.push_back(topRight);
            m_Indices.push_back(topLeft);
        }
    }

    m_IndexCount = static_cast<uint32>(m_Indices.size());

    // Generate and bind the VAO
    GL(glGenVertexArrays(1, &m_VAO));
    GL(glBindVertexArray(m_VAO));

    // Generate and bind the VBO
    GL(glGenBuffers(1, &m_VBO));
    GL(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
    GL(glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(float32), m_Vertices.data(), GL_STATIC_DRAW));

    // Generate and bind the EBO
    GL(glGenBuffers(1, &m_EBO));
    GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO));
    GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(uint32), m_Indices.data(), GL_STATIC_DRAW));

    // Set vertex attribute pointers
    GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float32), (void*)0));
    GL(glEnableVertexAttribArray(0)); // Position attribute

    // Unbind objects
    GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GL(glBindVertexArray(0));
}
