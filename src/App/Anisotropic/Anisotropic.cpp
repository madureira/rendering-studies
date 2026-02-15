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

// Light direction (directional light; the vector points toward the light)
static glm::vec3 lightDir = { 0.5f, 0.6f, 0.5f };
static glm::vec3 rotDeg   = { 0.0f, 0.0f, 0.0f };

// Material parameters
static float s_roughness  = 1.0f;
static float s_anisotropy = 0.75f;
static float s_metallic   = 1.0f;

// Steel base reflectance (F0 for metal)
static glm::vec3 s_albedo = { 0.56f, 0.57f, 0.58f };

Anisotropic::Anisotropic(Window* window)
    : m_Window(window)
{
    m_Shader = new Shader("assets/shaders/anisotropic.vert", "assets/shaders/anisotropic.frag");
    m_Model  = new Model("assets/models/sphere.obj");
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

    ImGui::TextUnformatted("Light direction");
    ImGui::SameLine();
    ImGui::SliderFloat3("##Light direction", &lightDir.x, -10.0f, 10.0f);

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

    // Floating-origin: all positions are relative to 'origin' to preserve
    // floating-point precision far from the world origin.
    glm::dvec3 origin = m_Camera->GetPositionHP();
    origin.y = 0.0;

    glm::mat4 viewRel = m_Camera->GetViewMatrixRelative(origin);

    m_Grid->Draw(*m_Camera, viewRel, projection, origin);

    m_Shader->Bind();

    // Model position in origin-relative space
    glm::vec3 modelPosRel = glm::vec3(glm::dvec3(0.0) - origin);
    glm::mat4 modelRel = glm::translate(glm::mat4(1.0f), modelPosRel);

    float radius = 1.0f;
    float gap    = 0.01f;
    modelRel = glm::translate(modelRel, glm::vec3(0.0f, radius + gap, 0.0f));

    glm::vec3 r = glm::radians(rotDeg);

    // Rotation: yaw (Y), pitch (X), roll (Z)
    modelRel = glm::rotate(modelRel, r.y, glm::vec3(0, 1, 0));
    modelRel = glm::rotate(modelRel, r.x, glm::vec3(1, 0, 0));
    modelRel = glm::rotate(modelRel, r.z, glm::vec3(0, 0, 1));

    m_Shader->SetMat4("u_Model",      modelRel);
    m_Shader->SetMat4("u_VP", projection * viewRel);

    // Camera position in origin-relative space (same space as WorldPos in shaders)
    glm::vec3 cameraPosRel = glm::vec3(m_Camera->GetPositionHP() - origin);
    m_Shader->SetVec3("u_CameraPosition", cameraPosRel);

    // Directional light: direction vector (not a position), independent of origin
    m_Shader->SetVec3 ("u_LightDir",   glm::normalize(lightDir));
    m_Shader->SetVec3 ("u_LightColor", glm::vec3(3.5f, 3.4f, 3.2f));
    m_Shader->SetVec3 ("u_Albedo",     s_albedo);
    m_Shader->SetFloat("u_Roughness",  s_roughness);
    m_Shader->SetFloat("u_Anisotropy", s_anisotropy);
    m_Shader->SetFloat("u_Metallic",   s_metallic);

    m_Model->Draw();
}
