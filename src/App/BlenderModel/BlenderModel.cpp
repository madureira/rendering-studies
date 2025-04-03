#include "BlenderModel.h"

#include "../../FileManager/FileManager.h"
#include "../../Model/Model.h"
#include "../../Shader/Shader.h"
#include "../../Window/Window.h"
#include "../Camera.h"
#include "../Grid.h"

BlenderModel::BlenderModel(Window* window)
    : m_Window(window)
{
    m_Shader = new Shader("assets/shaders/simple.vert", "assets/shaders/simple.frag");
    m_Camera = new Camera(glm::vec3(0.0f, 10.0f, 20.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -20.0f);
    m_Grid = new Grid();
    m_Model = new Model("assets/models/dragon.obj");
    m_Model2 = new Model("assets/models/apple.fbx");
}

BlenderModel::~BlenderModel()
{
    delete m_Model2;
    delete m_Model;
    delete m_Grid;
    delete m_Camera;
    delete m_Shader;
}

void BlenderModel::Update(float32 deltaTime)
{
    m_Camera->ProcessMouseMovement(m_Window->GetMouseX(), m_Window->GetMouseY());

    float32 speed = 5.0f;

    if (m_Window->IsKeyPressed(KeyToken::LeftShift))
    {
        speed = 15.f;
    }

    if (m_Window->IsKeyPressed(KeyToken::Up) || m_Window->IsKeyPressed(KeyToken::W))
    {
        m_Camera->ProcessKeyboard(CameraMove::FORWARD, deltaTime, speed);
    }

    if (m_Window->IsKeyPressed(KeyToken::Down) || m_Window->IsKeyPressed(KeyToken::S))
    {
        m_Camera->ProcessKeyboard(CameraMove::BACKWARD, deltaTime, speed);
    }

    if (m_Window->IsKeyPressed(KeyToken::Left) || m_Window->IsKeyPressed(KeyToken::A))
    {
        m_Camera->ProcessKeyboard(CameraMove::LEFT, deltaTime, speed);
    }

    if (m_Window->IsKeyPressed(KeyToken::Right) || m_Window->IsKeyPressed(KeyToken::D))
    {
        m_Camera->ProcessKeyboard(CameraMove::RIGHT, deltaTime, speed);
    }

    glm::mat4 view = m_Camera->GetViewMatrix();
    glm::mat4 projection = m_Camera->GetProjectionMatrix(m_Window->GetWidth(), m_Window->GetHeight());

    m_Grid->Draw(view, projection);

    m_Shader->Bind();

    m_Shader->SetMat4("uView", view);
    m_Shader->SetMat4("uProjection", projection);

    m_Shader->SetMat4("uModel", glm::mat4(1.0f));
    m_Model->Draw();

    glm::mat4 appleModel = glm::mat4(1.0f);
    appleModel = glm::translate(appleModel, glm::vec3(-6.0f, 7.3f, 0.7f));
    appleModel = glm::scale(appleModel, glm::vec3(0.5f, 0.5f, 0.5f));
    appleModel = glm::rotate(appleModel, glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));

    m_Shader->SetMat4("uModel", appleModel);
    m_Model2->Draw();

    m_Shader->Unbind();
}
