#include "SimpleMesh.h"

#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

#include <RenderingStudies/RegisterDemo.h>

#include "../../Engine/Camera/Camera.h"
#include "../../Engine/Grid/Grid.h"
#include "../../Engine/Shader/Shader.h"
#include "../../Engine/Utils/ModelSelector.h"
#include "../../Engine/Utils/UIComponents.h"
#include "../../Engine/Window/Window.h"

REGISTER_DEMO(SimpleMesh, true)

SimpleMesh::SimpleMesh(const Window& window, const Camera& camera)
    : m_Window(window)
    , m_Camera(camera)
    , m_ModelPos(0.0f, 0.0f, 0.0f)
{
    m_Shader = new Shader("assets/shaders/simple.vert", "assets/shaders/simple.frag");
    m_ModelSelector = new ModelSelector();
}

SimpleMesh::~SimpleMesh()
{
    delete m_ModelSelector;

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

    m_ModelSelector->Render();

    UIComponent::Vector3Sliders("Position", m_ModelPos[0], m_ModelPos[1], m_ModelPos[2]);

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

    glm::vec3 modelPosRel = glm::vec3(glm::dvec3(m_ModelPos) - origin);
    glm::mat4 modelRel = glm::translate(glm::mat4(1.0f), modelPosRel);

    m_Shader->Bind();

    m_Shader->SetMat4("u_MVP", projection * viewRel * modelRel);

    m_ModelSelector->GetSelectedModel().Draw();

    m_Shader->Unbind();
}
