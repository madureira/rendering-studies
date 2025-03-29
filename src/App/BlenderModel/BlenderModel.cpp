#include "BlenderModel.h"

#include <glm/gtc/matrix_transform.hpp>

#include "../../FileManager/FileManager.h"

BlenderModel::BlenderModel(Window *window)
    : m_Window(window)
{
    m_Shader = new Shader("assets/shaders/simple.vs", "assets/shaders/simple.fs");
    m_Camera = new Camera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
    m_Grid = new Grid();
    m_Model = new Model("assets/models/cube.obj");
}

BlenderModel::~BlenderModel()
{
    delete m_Model;
    delete m_Grid;
    delete m_Camera;
    delete m_Shader;
}

void BlenderModel::Update()
{
    float64 xpos = m_Window->GetMouseX();
    float64 ypos = m_Window->GetMouseY();

    if (m_FirstMouse)
    {
        m_LastX = xpos;
        m_LastY = ypos;
        m_FirstMouse = false;
    }

    float32 xoffset = xpos - m_LastX;
    float32 yoffset = m_LastY - ypos; // Reversed since Y-coordinates go from bottom to top
    m_LastX = xpos;
    m_LastY = ypos;

    m_Camera->ProcessMouseMovement(xoffset, yoffset);

    // Get the currentFrame in seconds
    float32 currentFrame = m_Window->GetTime();
    m_DeltaTime = currentFrame - m_LastFrame;
    m_LastFrame = currentFrame;

    float32 speed = 1.0f;

    if (m_Window->IsKeyPressed(KeyToken::LeftShift))
    {
        speed = 3.f;
    }

    if (m_Window->IsKeyPressed(KeyToken::Up) || m_Window->IsKeyPressed(KeyToken::W))
    {
        m_Camera->ProcessKeyboard(CameraMove::FORWARD, m_DeltaTime, speed);
    }

    if (m_Window->IsKeyPressed(KeyToken::Down) || m_Window->IsKeyPressed(KeyToken::S))
    {
        m_Camera->ProcessKeyboard(CameraMove::BACKWARD, m_DeltaTime, speed);
    }

    if (m_Window->IsKeyPressed(KeyToken::Left) || m_Window->IsKeyPressed(KeyToken::A))
    {
        m_Camera->ProcessKeyboard(CameraMove::LEFT, m_DeltaTime, speed);
    }

    if (m_Window->IsKeyPressed(KeyToken::Right) || m_Window->IsKeyPressed(KeyToken::D))
    {
        m_Camera->ProcessKeyboard(CameraMove::RIGHT, m_DeltaTime, speed);
    }

    if (m_Window->IsKeyPressed(KeyToken::Space))
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // Get the window dimensions
    uint32 windowWidth = m_Window->GetWidth();
    uint32 windowHeight = m_Window->GetHeight();

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = m_Camera->GetViewMatrix();
    glm::mat4 projection = glm::perspective(
        glm::radians(m_Camera->GetZoom()),            // Field of view (90 degrees in radians)
        (float32)windowWidth / (float32)windowHeight, // Aspect ratio (width / height)
        0.01f,                                        // Near clipping plane
        100.0f                                        // Far clipping plane
    );

    m_Shader->Bind();

    m_Shader->SetMat4("uModel", model);
    m_Shader->SetMat4("uView", view);
    m_Shader->SetMat4("uProjection", projection);

    m_Grid->Draw();
    m_Model->Draw();

    m_Shader->Unbind();
}
