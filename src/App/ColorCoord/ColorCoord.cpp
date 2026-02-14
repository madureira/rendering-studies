#include "ColorCoord.h"

#include <RenderingStudies/RegisterApp.h>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

#include "../../Engine/Camera/Camera.h"
#include "../../Engine/Grid/Grid.h"
#include "../../Engine/Model/Model.h"
#include "../../Engine/Shader/Shader.h"
#include "../../Engine/Utils/InputProcessorUtil.h"
#include "../../Engine/Window/Window.h"

REGISTER_APP(ColorCoord)

static bool isUsingModelCoords = false;

ColorCoord::ColorCoord(Window* window)
    : m_Window(window)
{
    m_Shader = new Shader("assets/shaders/color_coord.vert", "assets/shaders/color_coord.frag");
    m_Model = new Model("assets/models/teapot.obj");
    m_Camera = new Camera(glm::vec3(0.0f, 10.0f, 20.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -20.0f);
    m_Grid = new Grid();
}

ColorCoord::~ColorCoord()
{
    delete m_Grid;
    delete m_Camera;
    delete m_Model;
    delete m_Shader;
}

void ColorCoord::Update(float32 deltaTime)
{
    InputProcessorUtil::moveCamera(m_Camera, m_Window, deltaTime, 5.0f, 30.f);

    ImGui::Begin("Color by coordinates");
    ImGui::AlignTextToFramePadding();

    ImGui::TextUnformatted("Use model coords?");
    ImGui::SameLine();

    ImGui::Checkbox("##Use model coords?", &isUsingModelCoords);

    ImGui::End();
}

void ColorCoord::Render()
{
    glm::mat4 projection = m_Camera->GetProjectionMatrix(m_Window->GetWidth(), m_Window->GetHeight());

    glm::dvec3 origin = m_Camera->GetPositionHP();
    origin.y = 0.0;

    glm::mat4 viewRel = m_Camera->GetViewMatrixRelative(origin);

    m_Grid->Draw(*m_Camera, viewRel, projection, origin);

    m_Shader->Bind();

    // Model position in origin-relative space
    glm::vec3 modelPosRel = glm::vec3(glm::dvec3(0.0) - origin);
    glm::mat4 modelRel = glm::translate(glm::mat4(1.0f), modelPosRel);

    m_Shader->SetMat4("u_Model", modelRel);
    m_Shader->SetMat4("u_View", viewRel);
    m_Shader->SetMat4("u_Projection", projection);

    m_Shader->SetBool("u_UseModelCoords", isUsingModelCoords);

    m_Model->Draw();
}
