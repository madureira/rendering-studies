#include "Dragon.h"

#include <RenderingStudies/RegisterApp.h>
#include <glm/gtc/matrix_transform.hpp>

#include "../../Engine/Camera/Camera.h"
#include "../../Engine/Grid/Grid.h"
#include "../../Engine/Model/Model.h"
#include "../../Engine/Shader/Shader.h"
#include "../../Engine/Utils/InputProcessorUtil.h"
#include "../../Engine/Window/Window.h"

REGISTER_APP(Dragon)

Dragon::Dragon(const Window& window, const Camera& camera)
    : m_Window(window)
    , m_Camera(camera)
{
    m_Shader = new Shader("assets/shaders/simple.vert", "assets/shaders/simple.frag");
    m_Model = new Model("assets/models/dragon.obj");
}

Dragon::~Dragon()
{
    delete m_Model;
    if (m_Shader)
    {
        m_Shader->Unbind();
        delete m_Shader;
    }
}

void Dragon::Update(float32 deltaTime)
{
    InputProcessorUtil::moveCamera(m_Camera, m_Window, deltaTime, 5.0f, 30.0f);
}

void Dragon::Render()
{
    uint32 winWidth = m_Window.GetWidth();
    uint32 winHeight = m_Window.GetHeight();

    glm::mat4 projection = m_Camera.GetProjectionMatrix(winWidth, winHeight);

    glm::mat4 viewRel = m_Camera.GetViewMatrixRelative();

    glm::dvec3 origin = m_Camera.GetPositionHP();
    origin.y = 0.0;

    // Model is at (0,0,0) in world space. In relative space it needs to be offset.
    glm::vec3 modelPosRel = glm::vec3(glm::dvec3(0.0) - origin);
    glm::mat4 modelRel = glm::translate(glm::mat4(1.0f), modelPosRel);

    m_Shader->Bind();

    m_Shader->SetMat4("u_MVP", projection * viewRel * modelRel);

    m_Model->Draw();

    m_Shader->Unbind();
}
