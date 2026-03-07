#include "CubeMap.h"

#include <RenderingStudies/GL.h>
#include <RenderingStudies/RegisterApp.h>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

#include "../../Engine/Camera/Camera.h"
#include "../../Engine/FileManager/FileManager.h"
#include "../../Engine/Grid/Grid.h"
#include "../../Engine/Shader/Shader.h"
#include "../../Engine/Utils/InputProcessorUtil.h"
#include "../../Engine/Window/Window.h"

REGISTER_APP(CubeMap)

// Face names: p = positive, n = negative.
// OpenGL cubemap targets are sequential: GL_TEXTURE_CUBE_MAP_POSITIVE_X + 0..5
//   +X(px), -X(nx), +Y(py), -Y(ny), +Z(pz), -Z(nz)
static const char* const s_CubeMapFaces[] = {
    "px", "nx", "py", "ny", "pz", "nz"
};

CubeMap::CubeMap(Window* window)
    : m_Window(window)
    , m_VAO(0)
    , m_VBO(0)
    , m_EBO(0)
    , m_CubeMapTexture(0)
{
    m_Shader = new Shader("assets/shaders/cube_map.vert", "assets/shaders/cube_map.frag");

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

    CreateMesh();
    LoadCubeMap("assets/images/cube_map");
}

CubeMap::~CubeMap()
{
    delete m_Grid;
    delete m_Camera;

    if (m_Shader)
    {
        m_Shader->Unbind();
        delete m_Shader;
    }

    GL(glDeleteVertexArrays(1, &m_VAO));
    GL(glDeleteBuffers(1, &m_VBO));
    GL(glDeleteBuffers(1, &m_EBO));
    GL(glDeleteTextures(1, &m_CubeMapTexture));

    // Restore OpenGL state so switching to another app doesn't inherit skybox settings.
    // Match Window's default: depth LEQUAL, cull face enabled (CubeMap disables it for skybox).
    GL(glDepthFunc(GL_LEQUAL));
    GL(glEnable(GL_CULL_FACE));
    GL(glBindVertexArray(0));
    GL(glActiveTexture(GL_TEXTURE0));
    GL(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
}

void CubeMap::Update(float32 deltaTime)
{
    InputProcessorUtil::moveCamera(m_Camera, m_Window, deltaTime, 5.0f, 30.f);
}

void CubeMap::Render()
{
    glm::mat4 projection = m_Camera->GetProjectionMatrix(m_Window->GetWidth(), m_Window->GetHeight());

    glm::dvec3 origin = m_Camera->GetPositionHP();
    origin.y = 0.0;

    // Relative view matrix (computed in double, result in float with small values)
    glm::mat4 viewRel = m_Camera->GetViewMatrixRelative(origin);

    // Grid: use the same viewRel as the model (full depth buffer coherence)
    m_Grid->Draw(*m_Camera, viewRel, projection, origin, false);

    glm::mat4 view = m_Camera->GetViewMatrix();
    // Strip translation from the view matrix so the skybox appears infinitely far away.
    // Only the upper-left 3x3 (rotation) matters; the skybox cube stays centered on the camera.
    glm::mat4 viewRotOnly = glm::mat4(glm::mat3(view));

    m_Shader->Bind();
    m_Shader->SetMat4("u_VP", projection * viewRotOnly);

    if (m_CubeMapTexture)
    {
        GL(glActiveTexture(GL_TEXTURE0));
        GL(glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubeMapTexture));
        m_Shader->SetInt("u_CubeMap", 0);
    }

    // Skybox: depth LEQUAL so far-plane fragments pass; no cull so interior is visible.
    GL(glDepthFunc(GL_LEQUAL));
    GL(glDisable(GL_CULL_FACE));
    GL(glBindVertexArray(m_VAO));
    GL(glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0));
    GL(glBindVertexArray(0));
    // Restore state so other examples (and their grids) are unaffected.
    GL(glEnable(GL_CULL_FACE));
    GL(glDepthFunc(GL_LESS));

    if (m_CubeMapTexture)
    {
        GL(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
    }
    m_Shader->Unbind();
}

void CubeMap::CreateMesh()
{
    // Skybox cube: 8 unique corners, indexed by EBO (6 faces × 2 triangles × 3 verts = 36 indices).
    // Vertex layout: 0=(-1,-1,-1), 1=(+1,-1,-1), 2=(+1,+1,-1), 3=(-1,+1,-1), 4=(-1,-1,+1), 5=(+1,-1,+1), 6=(+1,+1,+1), 7=(-1,+1,+1).
    const float32 vertices[] = {
        -1.0f, -1.0f, -1.0f,  // 0 left bottom back
         1.0f, -1.0f, -1.0f,  // 1 right bottom back
         1.0f,  1.0f, -1.0f,  // 2 right top back
        -1.0f,  1.0f, -1.0f,  // 3 left top back
        -1.0f, -1.0f,  1.0f,  // 4 left bottom front
         1.0f, -1.0f,  1.0f,  // 5 right bottom front
         1.0f,  1.0f,  1.0f,  // 6 right top front
        -1.0f,  1.0f,  1.0f,  // 7 left top front
    };
    const uint32 indices[] = {
        // Back (-Z), Left (-X), Right (+X), Front (+Z), Top (+Y), Bottom (-Y)
        3, 0, 1, 1, 2, 3,
        4, 0, 3, 3, 7, 4,
        1, 5, 6, 6, 2, 1,
        4, 7, 6, 6, 5, 4,
        3, 2, 6, 6, 7, 3,
        0, 4, 1, 1, 4, 5,
    };

    GL(glGenVertexArrays(1, &m_VAO));
    GL(glGenBuffers(1, &m_VBO));
    GL(glGenBuffers(1, &m_EBO));
    GL(glBindVertexArray(m_VAO));
    GL(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
    GL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
    GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO));
    GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));
    GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float32), (void*)0));
    GL(glEnableVertexAttribArray(0));
    GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GL(glBindVertexArray(0));
}

void CubeMap::LoadCubeMap(const char* basePath)
{
    GL(glGenTextures(1, &m_CubeMapTexture));
    GL(glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubeMapTexture));

    for (int i = 0; i < 6; ++i)
    {
        int32 width = 0;
        int32 height = 0;
        int32 channels = 0;

        std::string path = std::string(basePath) + "/" + s_CubeMapFaces[i] + ".png";
        uchar* data = FileManager::LoadTexture(path, width, height, channels, false);
        if (!data)
        {
            LOG_ERROR("CubeMap: failed to load face '{}'", s_CubeMapFaces[i]);
            GL(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
            GL(glDeleteTextures(1, &m_CubeMapTexture));
            m_CubeMapTexture = 0;
            return;
        }

        GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
        GLenum internalFormat = (channels == 4) ? GL_RGBA8 : GL_RGB8;
        GLenum target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
        GL(glTexImage2D(target, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data));
        FileManager::FreeTexture(data);
    }

    GL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

    GL(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
}
