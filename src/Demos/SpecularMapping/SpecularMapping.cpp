#include "SpecularMapping.h"

#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

#include <RenderingStudies/GL.h>
#include <RenderingStudies/RegisterDemo.h>

#include "../../Engine/Camera/Camera.h"
#include "../../Engine/Model/Model.h"
#include "../../Engine/Shader/Shader.h"
#include "../../Engine/Texture/Texture.h"
#include "../../Engine/Window/Window.h"

REGISTER_DEMO(SpecularMapping, true)

SpecularMapping::SpecularMapping(const Window& window, const Camera& camera)
    : m_Window(window)
    , m_Camera(camera)
    , m_LightPos(0.5f, 1.0f, 0.3f)
{
    m_Shader = new Shader("assets/shaders/specular_mapping.vert", "assets/shaders/specular_mapping.frag");
    m_Model = new Model("assets/models/backpack.obj");

    TextureParams texParams{};
    texParams.srgb = false;
    texParams.generateMipmaps = true;
    texParams.flipY = true;

    m_DiffuseMap = new Texture("assets/images/backpack/diffuse.jpg", texParams);
    m_NormalMap = new Texture("assets/images/backpack/normal.jpg", texParams);
    m_SpecularMap = new Texture("assets/images/backpack/specular.jpg", texParams);

    m_Camera.OverrideInitialPosition(
        glm::vec3(0.0f, 5.0f, 15.0f),
        -90.0f,
        -10.0f);
}

SpecularMapping::~SpecularMapping()
{
    delete m_Model;
    if (m_Shader)
    {
        m_Shader->Unbind();
        delete m_Shader;
    }
    if (m_DiffuseMap)
    {
        delete m_DiffuseMap;
    }
    if (m_NormalMap)
    {
        delete m_NormalMap;
    }
    if (m_SpecularMap)
    {
        delete m_SpecularMap;
    }
}

void SpecularMapping::Update(float32 /*unused: deltaTime*/)
{
    ImGui::Begin("Specular Mapping");

    const float32 itemWidth = ImGui::CalcItemWidth() / 3.0f;

    auto DragVec3 = [&](const char* label, glm::vec3& v) {
        ImGui::PushID(label);
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(label);
        ImGui::SameLine();

        ImGui::TextColored(ImVec4(1, 0, 0, 1), "X");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(itemWidth - 20.0f);
        ImGui::DragFloat("##x", &v.x, 0.1f);
        ImGui::SameLine();

        ImGui::TextColored(ImVec4(0, 1, 0, 1), "Y");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(itemWidth - 20.0f);
        ImGui::DragFloat("##y", &v.y, 0.1f);
        ImGui::SameLine();

        ImGui::TextColored(ImVec4(0, 0.5f, 1, 1), "Z");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(itemWidth - 20.0f);
        ImGui::DragFloat("##z", &v.z, 0.1f);
        ImGui::PopID();
    };

    DragVec3("Light", m_LightPos);

    ImGui::TextUnformatted("Normal Strength");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(itemWidth + 70.0f);
    ImGui::SliderFloat("##Normal Strength", &m_NormalStrength, 0.0f, 5.0f);
    ImGui::TextUnformatted("Specular Strength");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(itemWidth + 70.0f);
    ImGui::SliderFloat("##Specular Strength", &m_SpecularStrength, 0.0f, 5.0f);

    ImGui::End();
}

void SpecularMapping::Render()
{
    uint32 winWidth = m_Window.GetWidth();
    uint32 winHeight = m_Window.GetHeight();

    glm::mat4 projection = m_Camera.GetProjectionMatrix(winWidth, winHeight);
    glm::mat4 viewRel = m_Camera.GetViewMatrixRelative();

    glm::dvec3 origin = m_Camera.GetPositionHP();
    origin.y = 0.0;

    // Move model
    const float32 distanceY = 2.0f;
    const float32 distanceZ = 0.5f;

    glm::vec3 modelPosRel = glm::vec3(glm::dvec3(0.0f) - origin);
    modelPosRel += glm::vec3(0.0f, distanceY, distanceZ);

    glm::mat4 modelRel = glm::translate(glm::mat4(1.0f), modelPosRel);
    glm::mat4 modelView = viewRel * modelRel;

    glm::vec3 lightPosView = glm::vec3(viewRel * glm::vec4(m_LightPos, 1.0f));

    m_Shader->Bind();
    m_DiffuseMap->Bind(0);
    m_NormalMap->Bind(1);
    m_SpecularMap->Bind(2);

    m_Shader->SetMat4("u_MVP", projection * modelView);
    m_Shader->SetMat4("u_ModelView", modelView);
    m_Shader->SetVec3("u_LightPos", lightPosView);
    m_Shader->SetInt("u_DiffuseMap", 0);
    m_Shader->SetInt("u_NormalMap", 1);
    m_Shader->SetInt("u_SpecularMap", 2);
    m_Shader->SetFloat("u_NormalStrength", m_NormalStrength);
    m_Shader->SetFloat("u_SpecularStrength", m_SpecularStrength);

    m_Model->Draw();

    m_Shader->Unbind();
    m_SpecularMap->Unbind();
    m_NormalMap->Unbind();
    m_DiffuseMap->Unbind();
}
