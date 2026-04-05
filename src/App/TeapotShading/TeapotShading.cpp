#include "TeapotShading.h"

#include <RenderingStudies/RegisterApp.h>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

#include "../../Engine/Camera/Camera.h"
#include "../../Engine/Grid/Grid.h"
#include "../../Engine/Model/Model.h"
#include "../../Engine/Shader/Shader.h"
#include "../../Engine/Window/Window.h"


REGISTER_APP(TeapotShading)

const char* const TeapotShading::s_ShaderOptions[3] = {
    "flat",
    "gouraud",
    "phong"
};

TeapotShading::TeapotShading(const Window& window, const Camera& camera)
    : m_Window(window)
    , m_Camera(camera)
    , m_CurrentShader(0)
    , m_LightDir(0.5f, 0.6f, 0.5f)
    , m_RotDeg(0.0f, 0.0f, 0.0f)
    , m_Scale(1.0f, 1.0f, 1.0f)
    , m_LockScale(true)
    , m_ScaleAll(1.0f)
{
    m_Shader[0] = new Shader("assets/shaders/flat_shading.vert", "assets/shaders/flat_shading.frag");
    m_Shader[1] = new Shader("assets/shaders/gouraud_shading.vert", "assets/shaders/gouraud_shading.frag");
    m_Shader[2] = new Shader("assets/shaders/phong_shading.vert", "assets/shaders/phong_shading.frag");
    m_Model = new Model("assets/models/teapot.obj");
}

TeapotShading::~TeapotShading()
{
    delete m_Model;
    for (uint32 i = 0; i < 3; i++)
    {
        if (m_Shader[i])
        {
            m_Shader[i]->Unbind();
            delete m_Shader[i];
        }
    }
}

void TeapotShading::Update(float32 /*unused: deltaTime*/)
{
    ImGui::Begin("Teapot Shading");
    ImGui::AlignTextToFramePadding();

    ImGui::TextUnformatted("Shading type");
    ImGui::SameLine();
    ImGui::Combo("##Shading type", &m_CurrentShader, s_ShaderOptions, IM_ARRAYSIZE(s_ShaderOptions));

    ImGui::TextUnformatted("Light direction");
    ImGui::SameLine();
    ImGui::SliderFloat3("##Light direction", &m_LightDir.x, -10.0f, 10.0f);

    ImGui::TextUnformatted("Rotation (deg)");
    ImGui::SameLine();
    ImGui::SliderFloat3("##Rotation (deg)", &m_RotDeg.x, -180.0f, 180.0f);

    ImGui::TextUnformatted("Scale");
    ImGui::SameLine();
    if (m_LockScale)
    {
        if (ImGui::SliderFloat("##ScaleAll", &m_ScaleAll, 0.1f, 10.0f))
        {
            m_Scale = glm::vec3(m_ScaleAll);
        }
    }
    else
    {
        ImGui::SliderFloat3("##Scale", &m_Scale.x, 0.1f, 10.0f);
    }

    ImGui::SameLine();

    if (ImGui::Checkbox("Lock", &m_LockScale))
    {
        if (m_LockScale)
        {
            m_ScaleAll = m_Scale.x;
            m_Scale = glm::vec3(m_ScaleAll);
        }
    }

    ImGui::End();
}

void TeapotShading::Render()
{
    uint32 winWidth = m_Window.GetWidth();
    uint32 winHeight = m_Window.GetHeight();

    glm::mat4 projection = m_Camera.GetProjectionMatrix(winWidth, winHeight);

    glm::mat4 viewRel = m_Camera.GetViewMatrixRelative();

    glm::dvec3 cameraPos = m_Camera.GetPositionHP();

    glm::dvec3 origin = cameraPos;
    origin.y = 0.0;

    m_Shader[m_CurrentShader]->Bind();

    // Model position in origin-relative space
    glm::vec3 modelPosRel = glm::vec3(glm::dvec3(0.0) - origin);
    glm::mat4 modelRel = glm::translate(glm::mat4(1.0f), modelPosRel);

    glm::vec3 r = glm::radians(m_RotDeg);

    // Rotation order: yaw (Y), pitch (X), roll (Z)
    modelRel = glm::rotate(modelRel, r.y, glm::vec3(0, 1, 0));
    modelRel = glm::rotate(modelRel, r.x, glm::vec3(1, 0, 0));
    modelRel = glm::rotate(modelRel, r.z, glm::vec3(0, 0, 1));

    modelRel = glm::scale(modelRel, m_Scale);

    m_Shader[m_CurrentShader]->SetMat4("u_Model", modelRel);
    m_Shader[m_CurrentShader]->SetMat4("u_VP", projection * viewRel);

    // Camera position in origin-relative space (only used by gouraud and phong for view vector)
    if (m_CurrentShader != 0)
    {
        glm::vec3 cameraPosRel = glm::vec3(cameraPos - origin);
        m_Shader[m_CurrentShader]->SetVec3("u_CameraPosition", cameraPosRel);
    }

    // Directional light: direction vector (not a position), independent of origin
    m_Shader[m_CurrentShader]->SetVec3("u_LightDir", glm::normalize(m_LightDir));

    m_Model->Draw();

    m_Shader[m_CurrentShader]->Unbind();
}
