#include "SkyboxExample.h"

#include <RenderingStudies/GL.h>
#include <RenderingStudies/RegisterApp.h>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

#include "../../Engine/Camera/Camera.h"
#include "../../Engine/FileManager/FileManager.h"
#include "../../Engine/Grid/Grid.h"
#include "../../Engine/Shader/Shader.h"
#include "../../Engine/Skybox/Skybox.h"
#include "../../Engine/Utils/InputProcessorUtil.h"
#include "../../Engine/Window/Window.h"

REGISTER_APP(SkyboxExample)

SkyboxExample::SkyboxExample(Window* window)
    : m_Window(window)
{
    // Isometric-style: elevated, diagonal, looking at origin (not straight top-down).
    // Position in +X,+Y,+Z octant; yaw 225° + pitch ~-10° so front points at (0,0,0).
    const float32 isoDist = 14.0f; // distance in XZ
    const float32 isoHeight = 12.0f;
    m_Camera = new Camera(
        glm::vec3(isoDist, isoHeight, isoDist),
        glm::vec3(0.0f, 1.0f, 0.0f),
        225.0f,  // yaw: look from (+X,+Z) back toward origin
        -10.264f // pitch: ~10° down from horizontal (classic isometric)
    );
    m_Grid = new Grid();

    std::string skyboxBasePath = "assets/images/skybox/blue_sky";
    m_Skybox = new Skybox(skyboxBasePath);
}

SkyboxExample::~SkyboxExample()
{
    delete m_Grid;
    delete m_Skybox;
    delete m_Camera;
}

void SkyboxExample::Update(float32 deltaTime)
{
    InputProcessorUtil::moveCamera(m_Camera, m_Window, deltaTime, 5.0f, 30.f);
}

void SkyboxExample::Render()
{
    uint32 winWidth = m_Window->GetWidth();
    uint32 winHeight = m_Window->GetHeight();

    m_Grid->Render(*m_Camera, winWidth, winHeight, false);

    m_Skybox->Render(*m_Camera, winWidth, winHeight);
}
