#include "BlenderModel.h"

#include "../../FileManager/FileManager.h"

BlenderModel::BlenderModel(Window* window)
    : m_Window(window)
{
    m_Shader = new Shader("assets/shaders/simple.vert", "assets/shaders/simple.frag");
    m_Camera = new Camera(glm::vec3(0.0f, 10.0f, 20.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -20.0f);
    m_Grid = new Grid();
    m_Model = new Model("assets/models/dragon.obj");
}

BlenderModel::~BlenderModel()
{
    delete m_Model;
    delete m_Grid;
    delete m_Camera;
    delete m_Shader;
}

void BlenderModel::Update(float32 deltaTime)
{
    m_Camera->ProcessMouseMovement(m_Window->GetMouseX(), m_Window->GetMouseY());

    float32 speed = 1.0f;

    if (m_Window->IsKeyPressed(KeyToken::LeftShift))
    {
        speed = 10.f;
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

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = m_Camera->GetViewMatrix();
    glm::mat4 projection = m_Camera->GetProjectionMatrix(m_Window->GetWidth(), m_Window->GetHeight());

    m_Grid->Draw(view, projection);

    m_Shader->Bind();

    m_Shader->SetMat4("uModel", model);
    m_Shader->SetMat4("uView", view);
    m_Shader->SetMat4("uProjection", projection);

    m_Model->Draw();

    m_Shader->Unbind();
}
