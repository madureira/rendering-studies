#include "SkyboxDemo.h"

#include <RenderingStudies/GL.h>
#include <RenderingStudies/RegisterDemo.h>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <string>

#include "../../Engine/Camera/Camera.h"
#include "../../Engine/FileManager/FileManager.h"
#include "../../Engine/Grid/Grid.h"
#include "../../Engine/Shader/Shader.h"
#include "../../Engine/Skybox/Skybox.h"
#include "../../Engine/Window/Window.h"

REGISTER_DEMO(SkyboxDemo, true)

const char* SKYBOX_BASE_PATH = "assets/images/skybox";

const char* const SkyboxDemo::s_SkyboxOptions[2] = {
    "day",
    "night"
};

static float32 exposure = 1.0f;

SkyboxDemo::SkyboxDemo(const Window& window, const Camera& camera)
    : m_Window(window)
    , m_Camera(camera)
{
    // Isometric-style: elevated, diagonal, looking at origin (not straight top-down).
    // Position in +X,+Y,+Z octant; yaw 225deg + pitch ~-10deg so front points at (0,0,0).
    const float32 isoDist = 14.0f; // distance in XZ
    const float32 isoHeight = 12.0f;
    m_Camera.OverrideInitialPosition(
        glm::vec3(isoDist, isoHeight, isoDist),
        225.0f,  // yaw: look from (+X,+Z) back toward origin
        -10.264f // pitch: ~10deg down from horizontal (classic isometric)
    );

    CreateSkybox();
}

SkyboxDemo::~SkyboxDemo()
{
    delete m_Skybox;
}

void SkyboxDemo::Update(float32 /*unused: deltaTime*/)
{
    ImGui::Begin("Skybox");
    ImGui::AlignTextToFramePadding();

    ImGui::TextUnformatted("Select the texture");
    ImGui::SameLine();
    if (ImGui::Combo("##Skybox", &m_CurrentSkybox, s_SkyboxOptions, IM_ARRAYSIZE(s_SkyboxOptions)))
    {
        CreateSkybox();
        exposure = (m_CurrentSkybox == 1) ? 0.5f : 1.0f;
    }

    ImGui::TextUnformatted("Exposure");
    ImGui::SameLine();
    ImGui::SliderFloat("##Exposure", &exposure, 0.1f, 2.0f);

    ImGui::End();
}

void SkyboxDemo::Render()
{
    if (!m_Skybox)
    {
        return;
    }

    uint32 winWidth = m_Window.GetWidth();
    uint32 winHeight = m_Window.GetHeight();

    m_Skybox->Render(m_Camera, winWidth, winHeight, exposure);
}

void SkyboxDemo::CreateSkybox()
{
    delete m_Skybox;

    std::string skyboxPath = std::string(SKYBOX_BASE_PATH) + "/" + std::string(s_SkyboxOptions[m_CurrentSkybox]);

    m_Skybox = new Skybox(skyboxPath);
}
