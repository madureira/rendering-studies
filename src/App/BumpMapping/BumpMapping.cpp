#include "BumpMapping.h"

#include <RenderingStudies/GL.h>
#include <RenderingStudies/RegisterApp.h>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

#include "../../Engine/Camera/Camera.h"
#include "../../Engine/Shader/Shader.h"
#include "../../Engine/Window/Window.h"

REGISTER_APP(BumpMapping, true)

BumpMapping::BumpMapping(const Window& window, const Camera& camera)
    : m_Window(window)
    , m_Camera(camera)
    , m_LightPos(2.0f, 4.0f, 2.0f)
    , m_LightColor(1.0f, 1.0f, 1.0f)
    , m_RippleCenterXZ(0.0f, 0.0f)
    , m_Time(0.0f)
    , m_Amp(0.08f)
    , m_K(18.0f)
    , m_Omega(8.0f)
    , m_Damping(1.2f)
    , m_NormalStrength(2.0f)
{
    m_Shader = new Shader("assets/shaders/bump_mapping.vert", "assets/shaders/bump_mapping.frag");

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

BumpMapping::~BumpMapping()
{
    if (m_Shader)
    {
        m_Shader->Unbind();
        delete m_Shader;
    }

    GL(glDeleteBuffers(1, &m_EBO));
    GL(glDeleteBuffers(1, &m_VBO));
    GL(glDeleteVertexArrays(1, &m_VAO));
}

void BumpMapping::Update(float32 deltaTime)
{
    m_Time += deltaTime;

    ImGui::Begin("Bump Mapping");

    ImGui::TextUnformatted("Light");
    ImGui::SliderFloat3("Light position", &m_LightPos.x, -10.0f, 10.0f, "%.2f");
    ImGui::ColorEdit3("Light color", &m_LightColor.x);

    ImGui::Separator();
    ImGui::TextUnformatted("Ripple (height field)");
    ImGui::SliderFloat2("Ripple center (XZ)", &m_RippleCenterXZ.x, -2.0f, 2.0f, "%.2f");
    ImGui::SliderFloat("Amplitude (u_Amp)", &m_Amp, 0.0f, 0.5f, "%.3f");
    ImGui::SliderFloat("Wave number (u_K)", &m_K, 1.0f, 40.0f, "%.1f");
    ImGui::SliderFloat("Speed (u_Omega)", &m_Omega, 0.0f, 20.0f, "%.1f");
    ImGui::SliderFloat("Damping (u_Damping)", &m_Damping, 0.0f, 5.0f, "%.2f");

    ImGui::Separator();
    ImGui::SliderFloat("Normal strength (u_NormalStrength)", &m_NormalStrength, 0.0f, 6.0f, "%.2f");

    ImGui::End();
}

void BumpMapping::Render()
{
    glm::mat4 model = glm::mat4(1.0);
    model = glm::scale(model, glm::vec3(15.0f, 1.0f, 15.0f));
    glm::mat4 view = m_Camera.GetViewMatrix();
    glm::mat4 projection = m_Camera.GetProjectionMatrix(m_Window.GetWidth(), m_Window.GetHeight());

    // Move model above the x-axis origin
    float distanceX = 1.0f;
    model = glm::translate(model, glm::vec3(0.0f, distanceX, 0.0f));

    glm::vec3 cameraPos = m_Camera.GetPosition();

    m_Shader->Bind();
    m_Shader->SetMat4("u_Model", model);
    m_Shader->SetMat4("u_VP", projection * view);
    m_Shader->SetVec3("u_CameraPos", cameraPos);
    m_Shader->SetVec3("u_LightPos", m_LightPos);
    m_Shader->SetVec3("u_LightColor", m_LightColor);
    m_Shader->SetVec2("u_RippleCenterXZ", m_RippleCenterXZ);
    m_Shader->SetFloat("u_Time", m_Time);
    m_Shader->SetFloat("u_Amp", m_Amp);
    m_Shader->SetFloat("u_K", m_K);
    m_Shader->SetFloat("u_Omega", m_Omega);
    m_Shader->SetFloat("u_Damping", m_Damping);
    m_Shader->SetFloat("u_NormalStrength", m_NormalStrength);

    GL(glBindVertexArray(m_VAO));
    GL(glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, (void*)0));

    GL(glBindVertexArray(0));
    m_Shader->Unbind();
}

void BumpMapping::CreateMesh()
{
    // A unit plane centered at origin, on XZ plane (y = 0)
    // CCW (Counter-Clockwise) winding when looking from +y.
    const float32 vertices[] = {
        // position             // normal               // uv
        -0.5f, 0.0f, -0.5f,     0.0f, 1.0f, 0.0f,       0.0f, 0.0f, // 0
         0.5f, 0.0f, -0.5f,     0.0f, 1.0f, 0.0f,       1.0f, 0.0f, // 1
         0.5f, 0.0f,  0.5f,     0.0f, 1.0f, 0.0f,       1.0f, 1.0f, // 2
        -0.5f, 0.0f,  0.5f,     0.0f, 1.0f, 0.0f,       0.0f, 1.0f  // 3
    };

    const uint32 indices[] = {
        0, 2, 1,
        0, 3, 2
    };

    m_IndexCount = (size_t)(sizeof(indices) / sizeof(indices[0]));

    GL(glGenVertexArrays(1, &m_VAO));
    GL(glGenBuffers(1, &m_VBO));
    GL(glGenBuffers(1, &m_EBO));

    GL(glBindVertexArray(m_VAO));

    // VBO
    GL(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
    GL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

    // EBO
    GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO));
    GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

    const size_t stride = (3 + 3 + 2) * (size_t)sizeof(float32);

    // location 0: vertex position (vec3)
    GL(glEnableVertexAttribArray(0));
    GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0));

    // location 1: normal (vec3)
    GL(glEnableVertexAttribArray(1));
    GL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float32))));

    // location 2: uv (vec2)
    GL(glEnableVertexAttribArray(2));
    GL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)((3 + 3) * sizeof(float32))));

    GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GL(glBindVertexArray(0));
}
