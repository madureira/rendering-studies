#include "NormalMapping.h"

#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

#include <RenderingStudies/GL.h>
#include <RenderingStudies/RegisterDemo.h>

#include "../../Engine/Camera/Camera.h"
#include "../../Engine/Model/Model.h"
#include "../../Engine/Shader/Shader.h"
#include "../../Engine/Texture/Texture.h"
#include "../../Engine/Utils/UIComponents.h"
#include "../../Engine/Window/Window.h"

REGISTER_DEMO(NormalMapping, true)

NormalMapping::NormalMapping(const Window& window, const Camera& camera)
    : m_Window(window)
    , m_Camera(camera)
    , m_LightPos(0.5f, 1.0f, 0.3f)
{
    m_Shader = new Shader("assets/shaders/normal_mapping.vert", "assets/shaders/normal_mapping.frag");
    m_Model = new Model("assets/models/cylinder.obj");

    TextureParams texParams{};
    texParams.srgb = false;
    texParams.generateMipmaps = true;
    texParams.flipY = true;

    m_DiffuseMap = new Texture("assets/images/brickwall/diffuse.png", texParams);
    m_NormalMap = new Texture("assets/images/brickwall/normal.png", texParams);
}

NormalMapping::~NormalMapping()
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
}

void NormalMapping::Update(float32 /*unused: deltaTime*/)
{
    ImGui::Begin("Normal Mapping");

    const float32 itemWidth = ImGui::CalcItemWidth() / 3.0f;

    UIComponent::Vector3Sliders("Light", m_LightPos.x, m_LightPos.y, m_LightPos.z);

    ImGui::TextUnformatted("Normal Strength");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(itemWidth + 70.0f);
    ImGui::SliderFloat("##Normal Strength", &m_NormalStrength, 0.0f, 5.0f);
    ImGui::TextUnformatted("Texture Scale");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(itemWidth + 84.0f);
    ImGui::SliderFloat("##Texture Scale", &m_TexScale, 0.1f, 10.0f);

    ImGui::End();
}

void NormalMapping::Render()
{
    uint32 winWidth = m_Window.GetWidth();
    uint32 winHeight = m_Window.GetHeight();

    glm::mat4 projection = m_Camera.GetProjectionMatrix(winWidth, winHeight);
    glm::mat4 viewRel = m_Camera.GetViewMatrixRelative();

    glm::dvec3 origin = m_Camera.GetPositionHP();
    origin.y = 0.0;

    // Move model along the y-axis origin
    float32 distanceY = 1.45f;

    glm::vec3 modelPosRel = glm::vec3(glm::dvec3(0.0f) - origin);
    modelPosRel += glm::vec3(0.0f, distanceY, 0.0f);

    glm::mat4 modelRel = glm::translate(glm::mat4(1.0f), modelPosRel);
    glm::mat4 modelView = viewRel * modelRel;

    glm::vec3 lightPosView = glm::vec3(viewRel * glm::vec4(m_LightPos, 1.0f));

    m_Shader->Bind();
    m_DiffuseMap->Bind(0);
    m_NormalMap->Bind(1);

    m_Shader->SetMat4("u_MVP", projection * modelView);
    m_Shader->SetMat4("u_ModelView", modelView);
    m_Shader->SetVec3("u_LightPos", lightPosView);
    m_Shader->SetInt("u_DiffuseMap", 0);
    m_Shader->SetInt("u_NormalMap", 1);
    m_Shader->SetFloat("u_NormalStrength", m_NormalStrength);
    m_Shader->SetFloat("u_TexScale", m_TexScale);

    m_Model->Draw();

    m_Shader->Unbind();
    m_NormalMap->Unbind();
    m_DiffuseMap->Unbind();
}
