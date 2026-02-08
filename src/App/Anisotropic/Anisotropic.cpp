#include "Anisotropic.h"
#include <RenderingStudies/RegisterApp.h>
#include <glm/gtc/matrix_transform.hpp>
#include "../../Engine/Model/Model.h"
#include "../../Engine/Shader/Shader.h"
#include "../../Engine/Window/Window.h"
#include "../../Engine/Camera/Camera.h"
#include "../../Engine/Grid/Grid.h"
#include "../../Engine/Utils/InputProcessorUtil.h"
#include <imgui.h>

REGISTER_APP(Anisotropic)

static glm::vec3 lightPosition = { 0.5f, 0.6f, 0.5f };
static glm::vec3 rotDeg        = { 0.0f, 0.0f, 0.0f };

// Material parameters
static float s_roughness  = 0.45f;   // perceptual roughness [0..1]
static float s_anisotropy = 0.85f;   // anisotropy [-1..1]; positive = streak along T
static float s_metallic   = 1.0f;    // 1.0 = full metal

// Steel base reflectance (F0 for metal)
static glm::vec3 s_albedo = { 0.56f, 0.57f, 0.58f };

Anisotropic::Anisotropic(Window* window)
    : m_Window(window)
{
    m_Shader = new Shader("assets/shaders/anisotropic.vert", "assets/shaders/anisotropic.frag");
    m_Model  = new Model("assets/models/sphere_smooth.obj");
    m_Camera = new Camera(glm::vec3(0.0f, 10.0f, 20.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -20.0f);
    m_Grid   = new Grid();
}

Anisotropic::~Anisotropic()
{
    delete m_Grid;
    delete m_Camera;
    delete m_Model;
    delete m_Shader;
}

void Anisotropic::Update(float32 deltaTime)
{
    InputProcessorUtil::moveCamera(m_Camera, m_Window, deltaTime, 5.0f, 30.f);

    ImGui::Begin("Anisotropic Shading");
    ImGui::AlignTextToFramePadding();

    ImGui::TextUnformatted("Light position");
    ImGui::SameLine();
    ImGui::SliderFloat3("##Light position", &lightPosition.x, -10.0f, 10.0f);

    ImGui::TextUnformatted("Rotation (deg)");
    ImGui::SameLine();
    ImGui::SliderFloat3("##Rotation (deg)", &rotDeg.x, -180.0f, 180.0f);

    ImGui::Separator();
    ImGui::TextUnformatted("Material");
    ImGui::SliderFloat("Roughness",  &s_roughness,  0.05f, 1.0f, "%.2f");
    ImGui::SliderFloat("Anisotropy", &s_anisotropy, -1.0f, 1.0f, "%.2f");
    ImGui::SliderFloat("Metallic",   &s_metallic,   0.0f,  1.0f, "%.2f");
    ImGui::ColorEdit3("Base color",  &s_albedo.x);

    ImGui::End();
}

void Anisotropic::Render()
{
    glm::mat4 projection = m_Camera->GetProjectionMatrix(m_Window->GetWidth(), m_Window->GetHeight());

    glm::dvec3 origin = m_Camera->GetPositionHP();
    origin.y = 0.0;

    glm::mat4 viewRel = m_Camera->GetViewMatrixRelative(origin);

    m_Grid->Draw(*m_Camera, viewRel, projection, origin);

    m_Shader->Bind();

    glm::vec3 modelPosRel = glm::vec3(glm::dvec3(0.0) - origin);
    glm::mat4 modelRel = glm::translate(glm::mat4(1.0f), modelPosRel);

    float radius = 1.0f;
    float gap    = 0.01f;
    glm::vec3 spherePos = glm::vec3(0.0f, radius + gap, 0.0f);

    modelRel = glm::translate(modelRel, spherePos);

    glm::vec3 r = glm::radians(rotDeg);

    // Rotation: yaw (Y), pitch (X), roll (Z)
    modelRel = glm::rotate(modelRel, r.y, glm::vec3(0, 1, 0));
    modelRel = glm::rotate(modelRel, r.x, glm::vec3(1, 0, 0));
    modelRel = glm::rotate(modelRel, r.z, glm::vec3(0, 0, 1));

    m_Shader->SetMat4("u_Model",      modelRel);
    m_Shader->SetMat4("u_View",       viewRel);
    m_Shader->SetMat4("u_Projection", projection);
    m_Shader->SetVec3("u_CameraPosition", glm::vec3(m_Camera->GetPositionHP()));

    // Slightly warm directional light (treated as direction, like Phong shader)
    glm::vec3 lightColor = glm::vec3(3.5f, 3.4f, 3.2f);

    m_Shader->SetVec3 ("u_LightPosition", lightPosition);
    m_Shader->SetVec3 ("u_LightColor", lightColor);
    m_Shader->SetVec3 ("u_Albedo",     s_albedo);
    m_Shader->SetFloat("u_Roughness",  s_roughness);
    m_Shader->SetFloat("u_Anisotropy", s_anisotropy);
    m_Shader->SetFloat("u_Metallic",   s_metallic);

    m_Model->Draw();
}
