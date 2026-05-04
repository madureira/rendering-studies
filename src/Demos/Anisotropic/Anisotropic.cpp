#include "Anisotropic.h"

#include <RenderingStudies/RegisterDemo.h>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

#include "../../Engine/Camera/Camera.h"
#include "../../Engine/Grid/Grid.h"
#include "../../Engine/Model/Model.h"
#include "../../Engine/Shader/Shader.h"
#include "../../Engine/Window/Window.h"

REGISTER_DEMO(Anisotropic, true)

Anisotropic::Anisotropic(const Window& window, const Camera& camera)
    : m_Window(window)
    , m_Camera(camera)
    , m_LightDir(0.5f, 0.6f, 0.5f)
    , m_RotDeg(0.0f, 0.0f, 0.0f)
    , m_Roughness(1.0f)
    , m_Anisotropy(0.75f)
    , m_Metallic(1.0f)
    , m_Albedo(0.56f, 0.57f, 0.58f)
{
    m_Shader = new Shader("assets/shaders/anisotropic.vert", "assets/shaders/anisotropic.frag");
    m_Model = new Model("assets/models/sphere.obj");
}

Anisotropic::~Anisotropic()
{
    delete m_Model;
    if (m_Shader)
    {
        m_Shader->Unbind();
        delete m_Shader;
    }
}

void Anisotropic::Update(float32 /*unused: deltaTime*/)
{
    ImGui::Begin("Anisotropic Shading");
    ImGui::AlignTextToFramePadding();

    ImGui::TextUnformatted("Light direction");
    ImGui::SameLine();
    ImGui::SliderFloat3("##Light direction", &m_LightDir.x, -10.0f, 10.0f);

    ImGui::TextUnformatted("Rotation (deg)");
    ImGui::SameLine();
    ImGui::SliderFloat3("##Rotation (deg)", &m_RotDeg.x, -180.0f, 180.0f);

    ImGui::Separator();
    ImGui::TextUnformatted("Material");
    ImGui::SliderFloat("Roughness", &m_Roughness, 0.05f, 1.0f, "%.2f");
    ImGui::SliderFloat("Anisotropy", &m_Anisotropy, -1.0f, 1.0f, "%.2f");
    ImGui::SliderFloat("Metallic", &m_Metallic, 0.0f, 1.0f, "%.2f");
    ImGui::ColorEdit3("Base color", &m_Albedo.x);

    ImGui::End();
}

void Anisotropic::Render()
{
    uint32 winWidth = m_Window.GetWidth();
    uint32 winHeight = m_Window.GetHeight();

    glm::mat4 projection = m_Camera.GetProjectionMatrix(winWidth, winHeight);

    glm::mat4 viewRel = m_Camera.GetViewMatrixRelative();

    glm::dvec3 cameraPos = m_Camera.GetPositionHP();

    glm::dvec3 origin = cameraPos;
    origin.y = 0.0;

    // Model position in origin-relative space
    glm::vec3 modelPosRel = glm::vec3(glm::dvec3(0.0) - origin);
    glm::mat4 modelRel = glm::translate(glm::mat4(1.0f), modelPosRel);

    float32 radius = 1.0f;
    float32 gap = 0.01f;
    modelRel = glm::translate(modelRel, glm::vec3(0.0f, radius + gap, 0.0f));

    glm::vec3 r = glm::radians(m_RotDeg);

    // Rotation: yaw (Y), pitch (X), roll (Z)
    modelRel = glm::rotate(modelRel, r.y, glm::vec3(0, 1, 0));
    modelRel = glm::rotate(modelRel, r.x, glm::vec3(1, 0, 0));
    modelRel = glm::rotate(modelRel, r.z, glm::vec3(0, 0, 1));

    // Camera position in origin-relative space (same space as WorldPos in shaders)
    glm::vec3 cameraPosRel = glm::vec3(cameraPos - origin);

    m_Shader->Bind();

    m_Shader->SetMat4("u_Model", modelRel);
    m_Shader->SetMat4("u_VP", projection * viewRel);
    m_Shader->SetVec3("u_CameraPosition", cameraPosRel);

    // Directional light: direction vector (not a position), independent of origin
    m_Shader->SetVec3("u_LightDir", glm::normalize(m_LightDir));
    m_Shader->SetVec3("u_LightColor", glm::vec3(3.5f, 3.4f, 3.2f));
    m_Shader->SetVec3("u_Albedo", m_Albedo);
    m_Shader->SetFloat("u_Roughness", m_Roughness);
    m_Shader->SetFloat("u_Anisotropy", m_Anisotropy);
    m_Shader->SetFloat("u_Metallic", m_Metallic);

    m_Model->Draw();

    m_Shader->Unbind();
}
