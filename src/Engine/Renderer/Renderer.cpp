#include "Renderer.h"

#include <cstdio>
#include <iomanip>
#include <sstream>

#include <glm/gtc/matrix_transform.hpp>

#include <RenderingStudies/GL.h>

#include "../Camera/Camera.h"
#include "../Grid/Grid.h"
#include "../Shader/Shader.h"
#include "../TextRenderer/TextRenderer.h"

const glm::vec4 BACKGROUND_COLOR = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
const glm::vec3 CAMERA_POSITION = glm::vec3(0.0f, 10.0f, 20.0f);
const glm::vec3 CAMERA_UP = glm::vec3(0.0f, 1.0f, 0.0f);
const float32 CAMERA_YAW = -90.0f;
const float32 CAMERA_PITCH = -20.0f;
const glm::vec3 FPS_COLOR = glm::vec3(0.5f, 0.8f, 0.2f);
const glm::vec3 X_AXIS_COLOR = glm::vec3(1.0f, 0.35f, 0.35f);
const glm::vec3 Y_AXIS_COLOR = glm::vec3(0.35f, 1.0f, 0.35f);
const glm::vec3 Z_AXIS_COLOR = glm::vec3(0.4f, 0.65f, 1.0f);
const glm::vec3 PITCH_COLOR = glm::vec3(1.0f, 0.55f, 0.0f);
const glm::vec3 YAW_COLOR = glm::vec3(0.75f, 0.4f, 1.0f);
const glm::vec3 ROLL_COLOR = glm::vec3(1.0f, 0.85f, 0.2f);
const glm::vec3 HUD_VALUE_WHITE = glm::vec3(1.0f, 1.0f, 1.0f);

Renderer::Renderer(uint32 windowInitialWidth, uint32 windowInitialHeight)
    : m_WindowInitialWidth(windowInitialWidth)
    , m_WindowInitialHeight(windowInitialHeight)
{
    m_TextShader = new Shader("assets/shaders/text.vert", "assets/shaders/text.frag");
    m_TextRenderer = new TextRenderer("assets/fonts/roboto-regular.ttf");
    m_BoldTextRenderer = new TextRenderer("assets/fonts/roboto-bold.ttf");

    glm::mat4 projection = glm::ortho(0.0f, (float32)m_WindowInitialWidth, 0.0f, (float32)m_WindowInitialHeight);
    m_TextShader->Bind();
    m_TextShader->SetMat4("u_Projection", projection);
    m_TextShader->Unbind();

    GL(glClearColor(BACKGROUND_COLOR.r, BACKGROUND_COLOR.g, BACKGROUND_COLOR.b, BACKGROUND_COLOR.a));

    static constexpr float32 kCamInfoScale = 0.45f;
    m_HudLabelWidths.x = m_BoldTextRenderer->MeasureText("X:", kCamInfoScale);
    m_HudLabelWidths.y = m_BoldTextRenderer->MeasureText("  Y:", kCamInfoScale);
    m_HudLabelWidths.z = m_BoldTextRenderer->MeasureText("  Z:", kCamInfoScale);
    m_HudLabelWidths.pitch = m_BoldTextRenderer->MeasureText("Pitch:", kCamInfoScale);
    m_HudLabelWidths.yaw = m_BoldTextRenderer->MeasureText("  Yaw:", kCamInfoScale);
    m_HudLabelWidths.roll = m_BoldTextRenderer->MeasureText("  Roll:", kCamInfoScale);

    m_Camera = new Camera(CAMERA_POSITION, CAMERA_UP, CAMERA_YAW, CAMERA_PITCH);
    m_Grid = new Grid();

#ifndef __EMSCRIPTEN__
    GL(glEnable(GL_MULTISAMPLE));
#endif
    GL(glFrontFace(GL_CCW));
    GL(glEnable(GL_BLEND));
    GL(glDepthFunc(GL_LEQUAL));
    GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
}

Renderer::~Renderer()
{
    if (m_TextRenderer)
    {
        delete m_TextRenderer;
    }

    if (m_BoldTextRenderer)
    {
        delete m_BoldTextRenderer;
    }

    if (m_TextShader)
    {
        delete m_TextShader;
    }

    if (m_Camera)
    {
        delete m_Camera;
    }

    if (m_Grid)
    {
        delete m_Grid;
    }
}

void Renderer::UpdateTextProjectionIfNeeded(uint32 winWidth, uint32 winHeight) const
{
    if (winWidth == m_LastTextWidth && winHeight == m_LastTextHeight)
        return;
    m_TextProjection = glm::ortho(0.0f, (float32)winWidth, 0.0f, (float32)winHeight);
    m_LastTextWidth = winWidth;
    m_LastTextHeight = winHeight;
    m_TextShader->Bind();
    m_TextShader->SetMat4("u_Projection", m_TextProjection);
    m_TextShader->Unbind();
}

void Renderer::Clear() const
{
    GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Renderer::RenderFPS(float64 currentTime, bool display, uint32 winWidth, uint32 winHeight) const
{
    m_FpsNbFrames++;

    static std::string fpsText;
    static std::string ftText;
    if (currentTime - m_FpsLastTime >= 1.0)
    {
        float64 elapsed = currentTime - m_FpsLastTime;
        float64 fps = float64(m_FpsNbFrames) / elapsed;
        float64 avgFrameMs = elapsed / float64(m_FpsNbFrames) * 1000.0;

        std::stringstream fpsStream;
        fpsStream << std::fixed << std::setprecision(2) << fps;
        fpsText = "FPS: " + fpsStream.str();

        std::stringstream ftStream;
        ftStream << std::fixed << std::setprecision(2) << "  " << avgFrameMs << "ms";
        ftText = ftStream.str();

        m_FpsNbFrames = 0;
        m_FpsLastTime = currentTime;
    }

    static const float32 rowHeight = 32.0f;
    static const float32 scale = 0.55f;
    static const float32 originX = 5.0f;
    const float32 fpsTextPosY = winHeight - rowHeight;

    if (!display || !m_TextRenderer)
    {
        return;
    }

    UpdateTextProjectionIfNeeded(winWidth, winHeight);

    m_TextRenderer->Render(*m_TextShader, fpsText, FPS_COLOR, originX, fpsTextPosY, scale);

    const float32 fpsTextWidth = m_TextRenderer->MeasureText(fpsText, scale);
    m_TextRenderer->Render(*m_TextShader, ftText, FPS_COLOR, originX + fpsTextWidth, fpsTextPosY, scale);
}

void Renderer::SetPolygonMode(bool enabled)
{
#ifndef __EMSCRIPTEN__
    if (enabled)
    {
        GL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
    }
    else
    {
        GL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
    }
#endif
}

void Renderer::SetZBuffer(bool enabled)
{
    if (enabled == m_LastZBuffer)
    {
        return;
    }
    m_LastZBuffer = enabled;
    if (enabled)
    {
        GL(glEnable(GL_DEPTH_TEST));
    }
    else
    {
        GL(glDisable(GL_DEPTH_TEST));
    }
}

void Renderer::SetCullFace(bool enabled)
{
    if (enabled == m_LastCullFace)
    {
        return;
    }
    m_LastCullFace = enabled;
    if (enabled)
    {
        GL(glEnable(GL_CULL_FACE));
        GL(glCullFace(GL_BACK));
    }
    else
    {
        GL(glDisable(GL_CULL_FACE));
    }
}

void Renderer::RenderGrid(uint32 winWidth, uint32 winHeight, bool enabled) const
{
    if (enabled)
    {
        m_Grid->Render(*m_Camera, winWidth, winHeight);
    }
}

void Renderer::ResetCameraPosition() const
{
    m_Camera->OverrideInitialPosition(CAMERA_POSITION, CAMERA_YAW, CAMERA_PITCH);
}

Camera* Renderer::GetCamera() const
{
    return m_Camera;
}

void Renderer::RenderCameraInfo(uint32 winWidth, uint32 winHeight, bool display) const
{
    if (!display || !m_TextRenderer)
    {
        return;
    }

    glm::vec3 pos = m_Camera->GetPosition();
    float32 pitch = m_Camera->GetPitch();
    float32 yaw = m_Camera->GetYaw();
    float32 roll = m_Camera->GetRoll();

    static const float32 rowHeight = 32.0f;
    static const float32 scale = 0.45f;
    static const float32 maxLineWidth = 410.0f;
    static const float32 rightPad = 10.0f;
    const float32 originX = static_cast<float32>(winWidth) - maxLineWidth - rightPad;

    UpdateTextProjectionIfNeeded(winWidth, winHeight);

    char buf[32];
    std::string v;

    // Position line: bold colored labels, white values
    const float32 posY = 2.0f * rowHeight;
    float32 x = originX;

    RenderBold("X:", X_AXIS_COLOR, x, posY, scale);
    x += m_HudLabelWidths.x;
    snprintf(buf, sizeof(buf), " %.1f", pos.x);
    v = buf;
    m_TextRenderer->Render(*m_TextShader, v, HUD_VALUE_WHITE, x, posY, scale);
    x += m_TextRenderer->MeasureText(v, scale);

    RenderBold("  Y:", Y_AXIS_COLOR, x, posY, scale);
    x += m_HudLabelWidths.y;
    snprintf(buf, sizeof(buf), " %.1f", pos.y);
    v = buf;
    m_TextRenderer->Render(*m_TextShader, v, HUD_VALUE_WHITE, x, posY, scale);
    x += m_TextRenderer->MeasureText(v, scale);

    RenderBold("  Z:", Z_AXIS_COLOR, x, posY, scale);
    x += m_HudLabelWidths.z;
    snprintf(buf, sizeof(buf), " %.1f", pos.z);
    m_TextRenderer->Render(*m_TextShader, buf, HUD_VALUE_WHITE, x, posY, scale);

    // Rotation line: bold labels, colored values
    const float32 rotY = rowHeight;
    float32 rx = originX;

    RenderBold("Pitch:", PITCH_COLOR, rx, rotY, scale);
    rx += m_HudLabelWidths.pitch;
    snprintf(buf, sizeof(buf), " %.2f", pitch);
    v = buf;
    m_TextRenderer->Render(*m_TextShader, v, PITCH_COLOR, rx, rotY, scale);
    rx += m_TextRenderer->MeasureText(v, scale);

    RenderBold("  Yaw:", YAW_COLOR, rx, rotY, scale);
    rx += m_HudLabelWidths.yaw;
    snprintf(buf, sizeof(buf), " %.2f", yaw);
    v = buf;
    m_TextRenderer->Render(*m_TextShader, v, YAW_COLOR, rx, rotY, scale);
    rx += m_TextRenderer->MeasureText(v, scale);

    RenderBold("  Roll:", ROLL_COLOR, rx, rotY, scale);
    rx += m_HudLabelWidths.roll;
    snprintf(buf, sizeof(buf), " %.2f", roll);
    m_TextRenderer->Render(*m_TextShader, buf, ROLL_COLOR, rx, rotY, scale);
}

void Renderer::RenderBold(const std::string& text, const glm::vec3& color, float32 x, float32 y, float32 scale) const
{
    m_BoldTextRenderer->Render(*m_TextShader, text, color, x, y, scale);
}
