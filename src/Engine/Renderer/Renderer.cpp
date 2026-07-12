#include "Renderer.h"

#include <cstdio>

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
    {
        return;
    }
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

    static float64 lastTime = 0.0;
    static std::string fpsText;
    static std::string ftText;
    static float32 fpsTextWidth = 0.0f;
    static bool fpsTextWidthDirty = true;

    if (currentTime - lastTime >= 1.0)
    {
        float64 elapsed = currentTime - lastTime;
        float64 fps = static_cast<float64>(m_FpsNbFrames) / elapsed;
        float64 avgFrameMs = (elapsed / static_cast<float64>(m_FpsNbFrames)) * 1000.0;

        char buf[64];
        snprintf(buf, sizeof(buf), "FPS: %.2f", fps);
        fpsText = buf;
        snprintf(buf, sizeof(buf), "  %.2fms", avgFrameMs);
        ftText = buf;

        m_FpsNbFrames = 0;
        lastTime = currentTime;
        fpsTextWidthDirty = true;
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

    if (fpsTextWidthDirty)
    {
        fpsTextWidth = m_TextRenderer->MeasureText(fpsText, scale);
        fpsTextWidthDirty = false;
    }

    m_TextShader->Bind();
    m_TextRenderer->BeginBatch(*m_TextShader);
    m_TextRenderer->SubmitText(fpsText, FPS_COLOR, originX, fpsTextPosY, scale);
    m_TextRenderer->SubmitText(ftText, FPS_COLOR, originX + fpsTextWidth, fpsTextPosY, scale);
    m_TextRenderer->EndBatch();
    m_TextShader->Unbind();
}

void Renderer::SetPolygonMode(bool enabled)
{
    m_WireframeMode = enabled;
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

    // Refresh cached formatted strings and widths only when values change
    {
        char buf[32];
        snprintf(buf, sizeof(buf), " %.1f", pos.x);
        if (m_CamCache.xVal != buf)
        {
            m_CamCache.xVal = buf;
            m_CamCache.xW = m_TextRenderer->MeasureText(m_CamCache.xVal, scale);
        }
        snprintf(buf, sizeof(buf), " %.1f", pos.y);
        if (m_CamCache.yVal != buf)
        {
            m_CamCache.yVal = buf;
            m_CamCache.yW = m_TextRenderer->MeasureText(m_CamCache.yVal, scale);
        }
        snprintf(buf, sizeof(buf), " %.1f", pos.z);
        if (m_CamCache.zVal != buf)
        {
            m_CamCache.zVal = buf;
        }
        snprintf(buf, sizeof(buf), " %.2f", pitch);
        if (m_CamCache.pitchVal != buf)
        {
            m_CamCache.pitchVal = buf;
            m_CamCache.pitchW = m_TextRenderer->MeasureText(m_CamCache.pitchVal, scale);
        }
        snprintf(buf, sizeof(buf), " %.2f", yaw);
        if (m_CamCache.yawVal != buf)
        {
            m_CamCache.yawVal = buf;
            m_CamCache.yawW = m_TextRenderer->MeasureText(m_CamCache.yawVal, scale);
        }
        snprintf(buf, sizeof(buf), " %.2f", roll);
        if (m_CamCache.rollVal != buf)
        {
            m_CamCache.rollVal = buf;
        }
    }

    // Pre-compute all X positions from cached widths
    const float32 posY = 2.0f * rowHeight;
    const float32 rotY = rowHeight;

    const float32 xLabelX = originX;
    const float32 xValX = xLabelX + m_HudLabelWidths.x;
    const float32 yLabelX = xValX + m_CamCache.xW;
    const float32 yValX = yLabelX + m_HudLabelWidths.y;
    const float32 zLabelX = yValX + m_CamCache.yW;
    const float32 zValX = zLabelX + m_HudLabelWidths.z;

    const float32 pitchLabelX = originX;
    const float32 pitchValX = pitchLabelX + m_HudLabelWidths.pitch;
    const float32 yawLabelX = pitchValX + m_CamCache.pitchW;
    const float32 yawValX = yawLabelX + m_HudLabelWidths.yaw;
    const float32 rollLabelX = yawValX + m_CamCache.yawW;
    const float32 rollValX = rollLabelX + m_HudLabelWidths.roll;

    m_TextShader->Bind();

    // Bold batch: each label has a unique color — SubmitText flushes on each color change
    m_BoldTextRenderer->BeginBatch(*m_TextShader);
    m_BoldTextRenderer->SubmitText("X:", X_AXIS_COLOR, xLabelX, posY, scale);
    m_BoldTextRenderer->SubmitText("  Y:", Y_AXIS_COLOR, yLabelX, posY, scale);
    m_BoldTextRenderer->SubmitText("  Z:", Z_AXIS_COLOR, zLabelX, posY, scale);
    m_BoldTextRenderer->SubmitText("Pitch:", PITCH_COLOR, pitchLabelX, rotY, scale);
    m_BoldTextRenderer->SubmitText("  Yaw:", YAW_COLOR, yawLabelX, rotY, scale);
    m_BoldTextRenderer->SubmitText("  Roll:", ROLL_COLOR, rollLabelX, rotY, scale);
    m_BoldTextRenderer->EndBatch();

    // Regular batch: xVal/yVal/zVal share WHITE (batched together), colored values flush individually
    m_TextRenderer->BeginBatch(*m_TextShader);
    m_TextRenderer->SubmitText(m_CamCache.xVal, HUD_VALUE_WHITE, xValX, posY, scale);
    m_TextRenderer->SubmitText(m_CamCache.yVal, HUD_VALUE_WHITE, yValX, posY, scale);
    m_TextRenderer->SubmitText(m_CamCache.zVal, HUD_VALUE_WHITE, zValX, posY, scale);
    m_TextRenderer->SubmitText(m_CamCache.pitchVal, PITCH_COLOR, pitchValX, rotY, scale);
    m_TextRenderer->SubmitText(m_CamCache.yawVal, YAW_COLOR, yawValX, rotY, scale);
    m_TextRenderer->SubmitText(m_CamCache.rollVal, ROLL_COLOR, rollValX, rotY, scale);
    m_TextRenderer->EndBatch();

    m_TextShader->Unbind();
}
