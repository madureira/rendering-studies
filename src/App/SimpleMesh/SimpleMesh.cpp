#include "SimpleMesh.h"

#include <RenderingStudies/RegisterApp.h>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

#include "../../Engine/Camera/Camera.h"
#include "../../Engine/Grid/Grid.h"
#include "../../Engine/Model/Model.h"
#include "../../Engine/Shader/Shader.h"
#include "../../Engine/Window/Window.h"

REGISTER_APP(SimpleMesh)

const char* const SimpleMesh::s_MeshOptions[7] = {
    "apple",
    "bunny",
    "cube",
    "dragon",
    "monkey",
    "sphere",
    "teapot"
};

SimpleMesh::SimpleMesh(const Window& window, const Camera& camera)
    : m_Window(window)
    , m_Camera(camera)
    , m_ModelPos(0.0f, 0.0f, 0.0f)
{
    m_Shader = new Shader("assets/shaders/simple.vert", "assets/shaders/simple.frag");
    m_Model[0] = new Model("assets/models/apple.fbx");
    m_Model[1] = new Model("assets/models/bunny.obj");
    m_Model[2] = new Model("assets/models/cube.obj");
    m_Model[3] = new Model("assets/models/dragon.obj");
    m_Model[4] = new Model("assets/models/monkey.obj");
    m_Model[5] = new Model("assets/models/sphere.obj");
    m_Model[6] = new Model("assets/models/teapot.obj");
}

SimpleMesh::~SimpleMesh()
{
    for (uint32 i = 0; i < 7; i++)
    {
        if (m_Model[i])
        {
            delete m_Model[i];
        }
    }

    if (m_Shader)
    {
        m_Shader->Unbind();
        delete m_Shader;
    }
}

void SimpleMesh::Update(float32 /*unused: deltaTime*/)
{
    ImGui::Begin("Simple Mesh");
    ImGui::AlignTextToFramePadding();

    ImGui::TextUnformatted("Meshes");
    ImGui::SameLine();
    ImGui::Combo("##Meshes", &m_CurrentMesh, s_MeshOptions, IM_ARRAYSIZE(s_MeshOptions));

    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted("Position");
    ImGui::SameLine();

    float32 itemWidth = ImGui::CalcItemWidth() / 3.0f;

    ImGui::AlignTextToFramePadding();
    ImGui::TextColored(ImVec4(1, 0, 0, 1), "X");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(itemWidth - 20.0f);
    ImGui::DragFloat("##x", &m_ModelPos[0], 0.1f);
    ImGui::SameLine();

    ImGui::TextColored(ImVec4(0, 1, 0, 1), "Y");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(itemWidth - 20.0f);
    ImGui::DragFloat("##y", &m_ModelPos[1], 0.1f);
    ImGui::SameLine();

    ImGui::TextColored(ImVec4(0, 0.5f, 1, 1), "Z");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(itemWidth - 20.0f);
    ImGui::DragFloat("##z", &m_ModelPos[2], 0.1f);

    ImGui::End();
}

void SimpleMesh::Render()
{
    uint32 winWidth = m_Window.GetWidth();
    uint32 winHeight = m_Window.GetHeight();

    glm::mat4 projection = m_Camera.GetProjectionMatrix(winWidth, winHeight);

    glm::mat4 viewRel = m_Camera.GetViewMatrixRelative();

    glm::dvec3 origin = m_Camera.GetPositionHP();
    origin.y = 0.0;

    // m_ModelPos is a world-space position. Convert to camera-relative space by
    // subtracting the camera origin (origin.y is always 0, so world Y == relative Y).
    glm::vec3 modelPosRel = glm::vec3(glm::dvec3(m_ModelPos) - origin);
    glm::mat4 modelRel = glm::translate(glm::mat4(1.0f), modelPosRel);

    m_Shader->Bind();

    m_Shader->SetMat4("u_MVP", projection * viewRel * modelRel);

    m_Model[m_CurrentMesh]->Draw();

    m_Shader->Unbind();
}
