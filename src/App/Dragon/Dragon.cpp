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
    m_Camera = new Camera(glm::vec3(0.0f, 10.0f, 20.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -20.0f);
    m_Grid = new Grid();
    m_Model = new Model("assets/models/dragon.obj");
}

Dragon::~Dragon()
{
    delete m_Model;
    delete m_Grid;
    delete m_Camera;
    delete m_Shader;
}

void Dragon::Update(float32 deltaTime)
{
    InputProcessorUtil::moveCamera(m_Camera, m_Window, deltaTime, 5.0f, 10.0f);
}

void Dragon::Render()
{
    glm::mat4 view = m_Camera->GetViewMatrix();
    glm::mat4 projection = m_Camera->GetProjectionMatrix(m_Window->GetWidth(), m_Window->GetHeight());

    m_Grid->Draw(*m_Camera, projection);

    m_Shader->Bind();

    m_Shader->SetMat4("u_Model", glm::mat4(1.0f));
    m_Shader->SetMat4("u_View", view);
    m_Shader->SetMat4("u_Projection", projection);

    m_Model->Draw();

    m_Shader->Unbind();
}
