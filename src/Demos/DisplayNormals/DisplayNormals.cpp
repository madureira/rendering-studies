#include "DisplayNormals.h"

#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

#include <RenderingStudies/GL.h>
#include <RenderingStudies/RegisterDemo.h>

#include "../../Engine/Camera/Camera.h"
#include "../../Engine/Model/Model.h"
#include "../../Engine/Shader/Shader.h"
#include "../../Engine/Window/Window.h"

REGISTER_DEMO(DisplayNormals, false)

DisplayNormals::DisplayNormals(const Window& window, const Camera& camera)
    : m_Window(window)
    , m_Camera(camera)
{
    m_Shader = new Shader("assets/shaders/per_face_gray.vert", "assets/shaders/per_face_gray.frag");
    m_NormalShader = new Shader(
        "assets/shaders/display_normals.vert",
        "assets/shaders/display_normals.geom",
        "assets/shaders/display_normals.frag");
    m_Model = new Model("assets/models/cube.obj");
}

DisplayNormals::~DisplayNormals()
{
    delete m_Model;
    if (m_Shader)
    {
        m_Shader->Unbind();
        delete m_Shader;
    }
    if (m_NormalShader)
    {
        m_NormalShader->Unbind();
        delete m_NormalShader;
    }
}

void DisplayNormals::Update(float32 /*unused: deltaTime*/)
{
    ImGui::Begin("Display Normals");
    ImGui::AlignTextToFramePadding();

    ImGui::Checkbox("View normals?", &m_ShowNormals);
    ImGui::Checkbox("Face normal?", &m_IsFaceNormal);

    ImGui::End();
}

void DisplayNormals::Render()
{
    uint32 winWidth = m_Window.GetWidth();
    uint32 winHeight = m_Window.GetHeight();

    glm::mat4 projection = m_Camera.GetProjectionMatrix(winWidth, winHeight);
    glm::mat4 viewRel = m_Camera.GetViewMatrixRelative();

    glm::dvec3 origin = m_Camera.GetPositionHP();
    origin.y = 0.0f;

    // Move model along the y-axis origin
    float32 distanceY = 1.45f;

    glm::vec3 modelPosRel = glm::vec3(glm::dvec3(0.0f) - origin);
    modelPosRel += glm::vec3(0.0f, distanceY, 0.0f);

    glm::mat4 modelRel = glm::translate(glm::mat4(1.0f), modelPosRel);
    glm::mat4 modelView = viewRel * modelRel;

    m_Shader->Bind();
    m_Shader->SetMat4("u_MVP", projection * modelView);
    m_Model->Draw();
    m_Shader->Unbind();

    if (m_ShowNormals)
    {
        m_NormalShader->Bind();
        m_NormalShader->SetMat4("u_Projection", projection);
        m_NormalShader->SetMat4("u_ModelView", modelView);
        m_NormalShader->SetBool("u_FaceNormal", m_IsFaceNormal);
        m_Model->Draw();
        m_NormalShader->Unbind();
    }
}
