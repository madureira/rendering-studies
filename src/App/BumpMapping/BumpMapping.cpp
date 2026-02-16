#include "BumpMapping.h"

#include <RenderingStudies/GL.h>
#include <RenderingStudies/RegisterApp.h>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

#include "../../Engine/Camera/Camera.h"
#include "../../Engine/Shader/Shader.h"
#include "../../Engine/Utils/InputProcessorUtil.h"
#include "../../Engine/Window/Window.h"

REGISTER_APP(BumpMapping)

static glm::vec3 s_LightPos(2.0f, 4.0f, 2.0f);
static glm::vec3 s_LightColor(1.0f, 1.0f, 1.0f);
static glm::vec2 s_RippleCenterXZ(0.0f, 0.0f);
static float32 s_Time = 0.0f;
static float32 s_Amp = 0.08f;
static float32 s_K = 18.0f;
static float32 s_Omega = 8.0f;
static float32 s_Damping = 1.2f;
static float32 s_NormalStrength = 2.0f;

BumpMapping::BumpMapping(Window* window)
    : m_Window(window)
{
    m_Shader = new Shader("assets/shaders/bump_mapping.vert", "assets/shaders/bump_mapping.frag");

    // Isometric-style: elevated, diagonal, looking at origin (not straight top-down).
    // Position in +X,+Y,+Z octant; yaw 225° + pitch ~-35° so front points at (0,0,0).
    const float32 isoDist = 14.0f; // distance in XZ
    const float32 isoHeight = 12.0f;
    m_Camera = new Camera(
        glm::vec3(isoDist, isoHeight, isoDist),
        glm::vec3(0.0f, 1.0f, 0.0f),
        225.0f,  // yaw: look from (+X,+Z) back toward origin
        -35.264f // pitch: ~35° down from horizontal (classic isometric)
    );

    CreateMesh();
}

BumpMapping::~BumpMapping()
{
    delete m_Camera;
    delete m_Shader;
    if (m_EBO)
        GL(glDeleteBuffers(1, &m_EBO));
    if (m_VBO)
        GL(glDeleteBuffers(1, &m_VBO));
    if (m_VAO)
        GL(glDeleteVertexArrays(1, &m_VAO));
    m_EBO = 0;
    m_VBO = 0;
    m_VAO = 0;
    m_IndexCount = 0;
}

void BumpMapping::Update(float32 deltaTime)
{
    InputProcessorUtil::moveCamera(m_Camera, m_Window, deltaTime);

    s_Time += deltaTime;

    ImGui::Begin("Bump Mapping");

    ImGui::TextUnformatted("Light");
    ImGui::SliderFloat3("Light position", &s_LightPos.x, -10.0f, 10.0f, "%.2f");
    ImGui::ColorEdit3("Light color", &s_LightColor.x);

    ImGui::Separator();
    ImGui::TextUnformatted("Ripple (height field)");
    ImGui::SliderFloat2("Ripple center (XZ)", &s_RippleCenterXZ.x, -2.0f, 2.0f, "%.2f");
    ImGui::SliderFloat("Amplitude (u_Amp)", &s_Amp, 0.0f, 0.5f, "%.3f");
    ImGui::SliderFloat("Wave number (u_K)", &s_K, 1.0f, 40.0f, "%.1f");
    ImGui::SliderFloat("Speed (u_Omega)", &s_Omega, 0.0f, 20.0f, "%.1f");
    ImGui::SliderFloat("Damping (u_Damping)", &s_Damping, 0.0f, 5.0f, "%.2f");

    ImGui::Separator();
    ImGui::SliderFloat("Normal strength (u_NormalStrength)", &s_NormalStrength, 0.0f, 6.0f, "%.2f");

    ImGui::End();
}

void BumpMapping::Render()
{
    glm::mat4 model = glm::mat4(1.0);
    model = glm::scale(model, glm::vec3(15.0f, 1.0f, 15.0f));
    glm::mat4 view = m_Camera->GetViewMatrix();
    glm::mat4 projection = m_Camera->GetProjectionMatrix(m_Window->GetWidth(), m_Window->GetHeight());

    m_Shader->Bind();

    glm::vec3 cameraPos = m_Camera->GetPosition();

    m_Shader->SetMat4("u_Model", model);
    m_Shader->SetMat4("u_VP", projection * view);
    m_Shader->SetVec3("u_CameraPos", cameraPos);
    m_Shader->SetVec3("u_LightPos", s_LightPos);
    m_Shader->SetVec3("u_LightColor", s_LightColor);
    m_Shader->SetVec2("u_RippleCenterXZ", s_RippleCenterXZ);
    m_Shader->SetFloat("u_Time", s_Time);
    m_Shader->SetFloat("u_Amp", s_Amp);
    m_Shader->SetFloat("u_K", s_K);
    m_Shader->SetFloat("u_Omega", s_Omega);
    m_Shader->SetFloat("u_Damping", s_Damping);
    m_Shader->SetFloat("u_NormalStrength", s_NormalStrength);

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
