#include "Renderer.h"

#include <iomanip>
#include <sstream>
#include <glm/gtc/matrix_transform.hpp>

#include <RenderingStudies/GL.h>
#include "../Shader/Shader.h"
#include "../TextRenderer/TextRenderer.h"

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
}

Renderer::~Renderer()
{
    if (m_TextRenderer) delete m_TextRenderer;
    if (m_TextShader) delete m_TextShader;
}

void Renderer::Clear(const glm::vec4& color) const
{
    // Background color
    GL(glClearColor(color.r, color.g, color.b, color.a));
    // Clear color buffer and depth buffer
    GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
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

void Renderer::SetPolygonMode(const bool enabled) const
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

