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
    delete m_Shader;
    if (m_VAO)
        GL(glDeleteVertexArrays(1, &m_VAO));
    if (m_VBO)
        GL(glDeleteBuffers(1, &m_VBO));
    if (m_CubeMapTexture)
        GL(glDeleteTextures(1, &m_CubeMapTexture));
    m_VAO = 0;
    m_VBO = 0;
    m_CubeMapTexture = 0;
}

void CubeMap::Update(float32 deltaTime)
{
    InputProcessorUtil::moveCamera(m_Camera, m_Window, deltaTime);
}

void CubeMap::Render()
{
    glm::mat4 view = m_Camera->GetViewMatrix();
    glm::mat4 projection = m_Camera->GetProjectionMatrix(m_Window->GetWidth(), m_Window->GetHeight());

    // Strip translation from the view matrix so the skybox appears infinitely far away.
    // Only the upper-left 3x3 (rotation) matters; the skybox cube stays centered on the camera.
    glm::mat4 viewRotOnly = glm::mat4(glm::mat3(view));
    glm::mat4 vp = projection * viewRotOnly;

    glm::dvec3 origin = m_Camera->GetPositionHP();
    origin.y = 0.0;

    // Relative view matrix (computed in double, result in float with small values)
    glm::mat4 viewRel = m_Camera->GetViewMatrixRelative(origin);

    // Grid: use the same viewRel as the model (full depth buffer coherence)
    m_Grid->Draw(*m_Camera, viewRel, projection, origin, false);

    m_Shader->Bind();
    m_Shader->SetMat4("u_VP", vp);

    if (m_CubeMapTexture)
    {
        GL(glActiveTexture(GL_TEXTURE0));
        GL(glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubeMapTexture));
        m_Shader->SetInt("u_CubeMap", 0);
    }

    GL(glDepthFunc(GL_LEQUAL));
    GL(glDisable(GL_CULL_FACE));
    GL(glBindVertexArray(m_VAO));
    GL(glDrawArrays(GL_TRIANGLES, 0, 36));
    GL(glBindVertexArray(0));
    GL(glEnable(GL_CULL_FACE));
    GL(glDepthFunc(GL_LESS));

    if (m_CubeMapTexture)
        GL(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
    m_Shader->Unbind();
}

void CubeMap::CreateMesh()
{
    // Skybox cube: ±1, one vertex per line for clarity. 6 faces × 2 triangles × 3 verts = 36.
    const float32 vertices[] = {
        // Back face (-Z)
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        // Left face (-X)
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        // Right face (+X)
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        // Front face (+Z)
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        // Top face (+Y)
        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        // Bottom face (-Y)
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    GL(glGenVertexArrays(1, &m_VAO));
    GL(glGenBuffers(1, &m_VBO));
    GL(glBindVertexArray(m_VAO));
    GL(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
    GL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
    GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float32), (void*)0));
    GL(glEnableVertexAttribArray(0));
    GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GL(glBindVertexArray(0));
}

bool CubeMap::LoadCubeMap(const char* basePath)
{
    GL(glGenTextures(1, &m_CubeMapTexture));
    GL(glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubeMapTexture));

    for (int i = 0; i < 6; ++i)
    {
        int32 width = 0, height = 0, channels = 0;
        std::string path = std::string(basePath) + "/" + s_CubeMapFaces[i] + ".png";
        uchar* data = FileManager::LoadTexture(path, width, height, channels, false);
        if (!data)
        {
            path = std::string(basePath) + "/" + s_CubeMapFaces[i] + ".jpg";
            data = FileManager::LoadTexture(path, width, height, channels, false);
        }
        if (!data)
        {
            LOG_ERROR("CubeMap: failed to load face '{}'", s_CubeMapFaces[i]);
            GL(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
            GL(glDeleteTextures(1, &m_CubeMapTexture));
            m_CubeMapTexture = 0;
            return false;
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
    return true;
}
