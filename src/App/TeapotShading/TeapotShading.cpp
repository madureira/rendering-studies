#include "TeapotShading.h"
#include <RenderingStudies/RegisterApp.h>
#include <glm/gtc/matrix_transform.hpp>
#include "../../Engine/Model/Model.h"
#include "../../Engine/Shader/Shader.h"
#include "../../Engine/Window/Window.h"
#include "../../Engine/Camera/Camera.h"
#include "../../Engine/Grid/Grid.h"
#include "../../Engine/Utils/InputProcessorUtil.h"
#include <imgui.h>

REGISTER_APP(TeapotShading)

static int currentShader = 0;
static const char* shaderOptions[] = {
    "flat",
    "gouraud",
    "phong"
};

TeapotShading::TeapotShading(Window* window)
    : m_Window(window)
{
    m_Shader[0] = new Shader("assets/shaders/flat_shading.vert", "assets/shaders/flat_shading.frag");
    m_Shader[1] = new Shader("assets/shaders/gouraud_shading.vert", "assets/shaders/gouraud_shading.frag");
    m_Shader[2] = new Shader("assets/shaders/phong_shading.vert", "assets/shaders/phong_shading.frag");
    m_Model = new Model("assets/models/teapot.obj");
    m_Camera = new Camera(glm::vec3(0.0f, 10.0f, 20.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -20.0f);
    m_Grid = new Grid();
}

TeapotShading::~TeapotShading()
{
    delete m_Grid;
    delete m_Camera;
    delete m_Model;
    delete m_Shader[0];
    delete m_Shader[1];
    delete m_Shader[2];
}

void TeapotShading::Update(float32 deltaTime)
{
    InputProcessorUtil::moveCamera(m_Camera, m_Window, deltaTime, 5.0f, 30.f);

    // UI:
    ImGui::Begin("Teapot Shading");
    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted("Shading type");
    ImGui::SameLine();
    // Control the combo width
    ImGui::SetNextItemWidth(120);
    ImGui::Combo("##Shading type", &currentShader, shaderOptions, IM_ARRAYSIZE(shaderOptions));
    ImGui::End();
}

void TeapotShading::Render()
{
    glm::mat4 projection = m_Camera->GetProjectionMatrix(m_Window->GetWidth(), m_Window->GetHeight());

    glm::dvec3 origin = m_Camera->GetPositionHP();
    origin.y = 0.0;

    glm::mat4 viewRel = m_Camera->GetViewMatrixRelative(origin);

    m_Grid->Draw(*m_Camera, viewRel, projection, origin);

    m_Shader[currentShader]->Bind();

    glm::vec3 modelPosRel = glm::vec3(glm::dvec3(0.0) - origin);
    glm::mat4 modelRel = glm::translate(glm::mat4(1.0f), modelPosRel);

    m_Shader[currentShader]->SetMat4("u_Model", modelRel);
    m_Shader[currentShader]->SetMat4("u_View", viewRel);
    m_Shader[currentShader]->SetMat4("u_Projection", projection);

    m_Model->Draw();
}
