#include "Dragon.h"
#include <RenderingStudies/RegisterApp.h>
#include <glm/gtc/matrix_transform.hpp>
#include "../../Engine/Model/Model.h"
#include "../../Engine/Shader/Shader.h"
#include "../../Engine/Window/Window.h"
#include "../../Engine/Camera/Camera.h"
#include "../../Engine/Grid/Grid.h"
#include "../../Engine/Utils/InputProcessorUtil.h"

REGISTER_APP(Dragon)

Dragon::Dragon(Window* window)
    : m_Window(window)
{
    m_Shader = new Shader("assets/shaders/simple.vert", "assets/shaders/simple.frag");
    m_Model = new Model("assets/models/dragon.obj");
    m_Camera = new Camera(glm::vec3(0.0f, 10.0f, 20.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -20.0f);
    m_Grid = new Grid();
}

Dragon::~Dragon()
{
    delete m_Grid;
    delete m_Camera;
    delete m_Model;
    delete m_Shader;
}

void Dragon::Update(float32 deltaTime)
{
    InputProcessorUtil::moveCamera(m_Camera, m_Window, deltaTime, 5.0f, 30.0f);
}

void Dragon::Render()
{
    glm::mat4 projection = m_Camera->GetProjectionMatrix(m_Window->GetWidth(), m_Window->GetHeight());

    // Floating origin: camera position with Y=0 to keep the ground stable.
    // All rendering happens relative to this origin, keeping float values small.
    glm::dvec3 origin = m_Camera->GetPositionHP();
    origin.y = 0.0;

    // Relative view matrix (computed in double, result in float with small values)
    glm::mat4 viewRel = m_Camera->GetViewMatrixRelative(origin);

    // Grid: use the same viewRel as the model (full depth buffer coherence)
    m_Grid->Draw(*m_Camera, viewRel, projection, origin);

    // Model: world position (0,0,0) converted to relative space
    m_Shader->Bind();

    // Model is at (0,0,0) in world space. In relative space it needs to be offset.
    glm::vec3 modelPosRel = glm::vec3(glm::dvec3(0.0) - origin);
    glm::mat4 modelRel = glm::translate(glm::mat4(1.0f), modelPosRel);

    m_Shader->SetMat4("u_MVP", projection * viewRel * modelRel);

    m_Model->Draw();

    m_Shader->Unbind();
}
