#include "ChromaDepth.h"

#include <RenderingStudies/RegisterApp.h>
#include <glm/gtc/matrix_transform.hpp>

#include "../../Engine/Camera/Camera.h"
#include "../../Engine/Grid/Grid.h"
#include "../../Engine/Model/Model.h"
#include "../../Engine/Shader/Shader.h"
#include "../../Engine/Window/Window.h"

REGISTER_APP(ChromaDepth, true)

ChromaDepth::ChromaDepth(const Window& window, const Camera& camera)
    : m_Window(window)
    , m_Camera(camera)
{
    m_Shader = new Shader("assets/shaders/chroma_depth.vert", "assets/shaders/chroma_depth.frag");
    m_Model = new Model("assets/models/dragon.obj");
}

ChromaDepth::~ChromaDepth()
{
    delete m_Model;
    if (m_Shader)
    {
        m_Shader->Unbind();
        delete m_Shader;
    }
}

void ChromaDepth::Update(float32 /*unused: deltaTime*/)
{
}

void ChromaDepth::Render()
{
    uint32 winWidth = m_Window.GetWidth();
    uint32 winHeight = m_Window.GetHeight();

    glm::mat4 projection = m_Camera.GetProjectionMatrix(winWidth, winHeight);

    glm::mat4 viewRel = m_Camera.GetViewMatrixRelative();

    // Floating origin: camera position with Y=0 to keep the ground stable.
    // All rendering happens relative to this origin, keeping float values small.
    glm::dvec3 origin = m_Camera.GetPositionHP();
    origin.y = 0.0;

    // Model is at (0,0,0) in world space. In relative space it needs to be offset.
    glm::vec3 modelPosRel = glm::vec3(glm::dvec3(0.0) - origin);
    glm::mat4 modelRel = glm::translate(glm::mat4(1.0f), modelPosRel);
    glm::mat4 modelView = viewRel * modelRel;

    m_Shader->Bind();

    m_Shader->SetMat4("u_MVP", projection * modelView);
    m_Shader->SetMat4("u_ModelView", modelView);
    m_Shader->SetFloat("u_ChromaBlue", -5.0f);
    m_Shader->SetFloat("u_ChromaRed", -80.0f);

    m_Model->Draw();

    m_Shader->Unbind();
}
