#include "Renderer.h"

#include <iomanip>
#include <sstream>
#include <glm/gtc/matrix_transform.hpp>

#include <RenderingStudies/GL.h>
#include "../Camera/Camera.h"
#include "../Grid/Grid.h"
#include "../Shader/Shader.h"
#include "../TextRenderer/TextRenderer.h"

const glm::vec3 CAMERA_POSITION = glm::vec3(0.0f, 10.0f, 20.0f);
const glm::vec3 CAMERA_UP = glm::vec3(0.0f, 1.0f, 0.0f);
const float32 CAMERA_YAW = -90.0f;
const float32 CAMERA_PITCH = -20.0f;

Renderer::Renderer(const uint32 windowInitialWidth, const uint32 windowInitialHeight)
    : m_WindowInitialWidth(windowInitialWidth)
    , m_WindowInitialHeight(windowInitialHeight)
{
    m_TextShader = new Shader("assets/shaders/text.vert", "assets/shaders/text.frag");
    m_TextRenderer = new TextRenderer("assets/fonts/roboto-regular.ttf");

    glm::mat4 projection = glm::ortho(0.0f, (float32)m_WindowInitialWidth, 0.0f, (float32)m_WindowInitialHeight);
    m_TextShader->Bind();
    m_TextShader->SetMat4("u_Projection", projection);
    m_TextShader->Unbind();

    m_Camera = new Camera(CAMERA_POSITION, CAMERA_UP, CAMERA_YAW, CAMERA_PITCH);
    m_Grid = new Grid();

    GL(glEnable(GL_MULTISAMPLE));
    GL(glFrontFace(GL_CCW));
    GL(glEnable(GL_BLEND));
    GL(glDepthFunc(GL_LEQUAL));
    GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
}

Renderer::~Renderer()
{
    if (m_TextRenderer) delete m_TextRenderer;
    if (m_TextShader) delete m_TextShader;
    if (m_Camera) delete m_Camera;
    if (m_Grid) delete m_Grid;
}

void Renderer::Clear(const float32 r, const float32 g, const float32 b) const
{
    // Background color
    GL(glClearColor(r, g, b, 1.0f));
    GL(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::RenderFPS(const float64 currentTime, const bool display) const
{
    m_FpsNbFrames++;

    static std::string fpsText;
    if (currentTime - m_FpsLastTime >= 1.0) // If 1 second has passed
    {
        float64 fps = float64(m_FpsNbFrames) / (currentTime - m_FpsLastTime);

        // Convert FPS to string with 2 decimal places
        std::stringstream fpsStream;
        fpsStream << std::fixed << std::setprecision(2) << fps;
        fpsText = "FPS: " + fpsStream.str();

        m_FpsNbFrames = 0;
        m_FpsLastTime = currentTime;
    }

    static const float32 scale = 0.3f;
    static const float32 originX = 5.0f;
    static const float32 rowHeight = 20.0f;
    const float32 fpsTextPosY = m_WindowInitialHeight - rowHeight;

    if (!display || !m_TextRenderer)
    {
        return;
    }

    m_TextRenderer->Render(*m_TextShader, fpsText, originX, fpsTextPosY, scale, glm::vec3(0.5f, 0.8f, 0.2f));
}

void Renderer::SetPolygonMode(const bool enabled)
{
    if (enabled)
    {
        GL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
    }
    else
    {
        GL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
    }
}

void Renderer::SetZBuffer(const bool enabled)
{
    if (enabled)
    {
        GL(glEnable(GL_DEPTH_TEST));
        GL(glDepthFunc(GL_LEQUAL));
        GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    }
    else
    {
        GL(glDisable(GL_DEPTH_TEST));
        GL(glClear(GL_COLOR_BUFFER_BIT));
    }
}

void Renderer::SetCullFace(const bool enabled)
{
    if (enabled)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }
    else
    {
        glDisable(GL_CULL_FACE);
    }
}

void Renderer::RenderGrid(const uint32 winWidth, const uint32 winHeight, const bool enabled) const
{
    if (enabled) {
        m_Grid->Render(*m_Camera, winWidth, winHeight);
    }
}

void Renderer::ResetCameraPosition() const
{
    m_Camera->OverrideInitialPosition(CAMERA_POSITION, CAMERA_UP, CAMERA_YAW, CAMERA_PITCH);
}

Camera* Renderer::GetCamera() const
{
    return m_Camera;
}
