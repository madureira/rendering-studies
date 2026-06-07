#include "ShadowMapping.h"

#include <imgui.h>

#include <RenderingStudies/GL.h>
#include <RenderingStudies/RegisterDemo.h>
#include <glm/gtc/matrix_transform.hpp>

#include "../../Engine/Camera/Camera.h"
#include "../../Engine/Shader/Shader.h"
#include "../../Engine/Texture/Texture.h"
#include "../../Engine/Window/Window.h"

REGISTER_DEMO(ShadowMapping, true)

ShadowMapping::ShadowMapping(const Window& window, const Camera& camera)
    : m_Window(window)
    , m_Camera(camera)
    , m_LightPos(2.0f, 4.0f, 2.0f)
    , m_CubePos(0.0f, 3.0f, 0.0f)
{
    m_ShadowMapShader = new Shader("assets/shaders/shadow_mapping.vert", "assets/shaders/shadow_mapping.frag");
    m_DepthShader = new Shader("assets/shaders/depth.vert", "assets/shaders/depth.frag");

    TextureParams texParams{};
    texParams.srgb = false;
    texParams.generateMipmaps = true;
    texParams.flipY = true;
    m_Texture = new Texture("assets/images/container.jpg", texParams);
    m_MarbleTexture = new Texture("assets/images/marble.jpg", texParams);

    CreateMesh(PLANE_INDEX);
    CreateMesh(CUBE_INDEX);
    CreateFrameBuffer();
}

ShadowMapping::~ShadowMapping()
{
    if (m_ShadowMapShader)
    {
        m_ShadowMapShader->Unbind();
        delete m_ShadowMapShader;
    }
    if (m_DepthShader)
    {
        m_DepthShader->Unbind();
        delete m_DepthShader;
    }
    if (m_Texture)
    {
        delete m_Texture;
    }
    if (m_MarbleTexture)
    {
        delete m_MarbleTexture;
    }
    for (uint32 i = 0; i <= 1; ++i)
    {
        GL(glDeleteVertexArrays(1, &m_VAO[i]));
        GL(glDeleteBuffers(1, &m_VBO[i]));
        GL(glDeleteBuffers(1, &m_EBO[i]));
    }
    if (m_DepthMapTexture != 0)
    {
        GL(glDeleteTextures(1, &m_DepthMapTexture));
    }
    GL(glDeleteFramebuffers(1, &m_DepthMapFBO));
}

void ShadowMapping::Update(float32 /*unused: deltaTime*/)
{
    ImGui::Begin("Shadow Mapping");

    const float32 itemWidth = ImGui::CalcItemWidth() / 3.0f;

    auto DragVec3 = [&](const char* label, glm::vec3& v) {
        ImGui::PushID(label);
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(label);
        ImGui::SameLine();

        ImGui::TextColored(ImVec4(1, 0, 0, 1), "X");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(itemWidth - 20.0f);
        ImGui::DragFloat("##x", &v.x, 0.1f);
        ImGui::SameLine();

        ImGui::TextColored(ImVec4(0, 1, 0, 1), "Y");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(itemWidth - 20.0f);
        ImGui::DragFloat("##y", &v.y, 0.1f);
        ImGui::SameLine();

        ImGui::TextColored(ImVec4(0, 0.5f, 1, 1), "Z");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(itemWidth - 20.0f);
        ImGui::DragFloat("##z", &v.z, 0.1f);
        ImGui::PopID();
    };

    DragVec3("Light", m_LightPos);
    ImGui::Separator();
    DragVec3("Cube", m_CubePos);

    ImGui::End();
}

void ShadowMapping::Render()
{
    // Render the depth map first
    RenderDepthMap();

    glm::mat4 view = m_Camera.GetViewMatrix();
    glm::mat4 projection = m_Camera.GetProjectionMatrix(m_Window.GetWidth(), m_Window.GetHeight());

    m_ShadowMapShader->Bind();
    m_ShadowMapShader->SetMat4("u_LightSpaceMatrix", ComputeLightSpaceMatrix());
    m_ShadowMapShader->SetVec3("u_LightPos", m_LightPos);
    m_ShadowMapShader->SetVec3("u_ViewPos", m_Camera.GetPosition());
    m_ShadowMapShader->SetMat4("u_View", view);
    m_ShadowMapShader->SetMat4("u_Projection", projection);

    GL(glActiveTexture(GL_TEXTURE1));
    GL(glBindTexture(GL_TEXTURE_2D, m_DepthMapTexture));

    for (uint32 i = 0; i < 2; ++i)
    {
        GL(glActiveTexture(GL_TEXTURE0));
        GL(glBindTexture(GL_TEXTURE_2D, i == PLANE_INDEX ? m_MarbleTexture->GetID() : m_Texture->GetID()));

        glm::mat4 model = glm::mat4(1.0f);
        if (i == PLANE_INDEX)
        {
            model = glm::scale(model, glm::vec3(20.0f, 0.1f, 20.0f));
            model = glm::translate(model, glm::vec3(0.0f, 0.05f, 0.0f));
        }
        else
        {
            model = glm::translate(model, m_CubePos);
        }

        m_ShadowMapShader->SetMat4("u_Model", model);
        m_ShadowMapShader->SetMat3("u_NormalMatrix", glm::mat3(glm::transpose(glm::inverse(model))));

        GL(glBindVertexArray(m_VAO[i]));
        GL(glDisable(GL_CULL_FACE));
        GL(glDrawElements(GL_TRIANGLES, INDEX_COUNT, GL_UNSIGNED_INT, 0));
        GL(glEnable(GL_CULL_FACE));
        GL(glBindVertexArray(0));
    }

    m_ShadowMapShader->Unbind();
}

void ShadowMapping::CreateMesh(const uint32 index)
{
    // clang-format off
    float32 vertices[] = {
        // Positions          // Normals           // TexCoords
        // Front face (0, 0, 1)
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
        // Back face (0, 0, -1)
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        // Left face (-1, 0, 0)
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        // Right face (1, 0, 0)
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        // Top face (0, 1, 0)
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        // Bottom face (0, -1, 0)
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
    };

    uint32 indices[] = {
         0,  1,  2,  2,  3,  0, // Front
         4,  5,  6,  6,  7,  4, // Back
         8,  9, 10, 10, 11,  8, // Left
        12, 13, 14, 14, 15, 12, // Right
        16, 17, 18, 18, 19, 16, // Top
        20, 21, 22, 22, 23, 20, // Bottom
    };
    // clang-format on

    // Generate Objects
    GL(glGenVertexArrays(1, &m_VAO[index]));
    GL(glGenBuffers(1, &m_VBO[index]));
    GL(glGenBuffers(1, &m_EBO[index]));

    // Bind Vertex Array Object
    GL(glBindVertexArray(m_VAO[index]));

    // Bind and set vertex buffer
    GL(glBindBuffer(GL_ARRAY_BUFFER, m_VBO[index]));
    GL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

    // Bind and set index buffer
    GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO[index]));
    GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

    // Position attribute (location = 0)
    GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float32), (void*)0));
    GL(glEnableVertexAttribArray(0));

    // Normal attribute (location = 1)
    GL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float32), (void*)(3 * sizeof(float32))));
    GL(glEnableVertexAttribArray(1));

    // TexCoord attribute (location = 2)
    GL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float32), (void*)(6 * sizeof(float32))));
    GL(glEnableVertexAttribArray(2));

    m_ShadowMapShader->Bind();
    m_ShadowMapShader->SetInt("u_DiffuseTexture", 0);
    m_ShadowMapShader->SetInt("u_ShadowMap", 1);
    m_ShadowMapShader->Unbind();

    // Unbind objects
    GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GL(glBindVertexArray(0));
}

void ShadowMapping::CreateFrameBuffer()
{
    GL(glGenFramebuffers(1, &m_DepthMapFBO));

    GL(glGenTextures(1, &m_DepthMapTexture));
    GL(glBindTexture(GL_TEXTURE_2D, m_DepthMapTexture));
    GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    GL(glBindFramebuffer(GL_FRAMEBUFFER, m_DepthMapFBO));
    GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthMapTexture, 0));
    GL(glReadBuffer(GL_NONE));
    GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

glm::mat4 ShadowMapping::ComputeLightSpaceMatrix() const
{
    glm::mat4 lightProjection = glm::ortho(-25.0f, 25.0f, -25.0f, 25.0f, 0.1f, 100.0f);
    glm::mat4 lightView = glm::lookAt(m_LightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    return lightProjection * lightView;
}

void ShadowMapping::RenderDepthMap()
{
    m_DepthShader->Bind();
    m_DepthShader->SetMat4("u_LightSpaceMatrix", ComputeLightSpaceMatrix());

    GL(glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT));
    GL(glBindFramebuffer(GL_FRAMEBUFFER, m_DepthMapFBO));
    GL(glClear(GL_DEPTH_BUFFER_BIT));

    glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(20.0f, 0.1f, 20.0f));
    model = glm::translate(model, glm::vec3(0.0f, 0.05f, 0.0f));
    m_DepthShader->SetMat4("u_Model", model);
    GL(glBindVertexArray(m_VAO[PLANE_INDEX]));
    GL(glDrawElements(GL_TRIANGLES, INDEX_COUNT, GL_UNSIGNED_INT, 0));

    model = glm::translate(glm::mat4(1.0f), m_CubePos);
    m_DepthShader->SetMat4("u_Model", model);
    GL(glBindVertexArray(m_VAO[CUBE_INDEX]));
    GL(glDrawElements(GL_TRIANGLES, INDEX_COUNT, GL_UNSIGNED_INT, 0));

    GL(glBindVertexArray(0));
    GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GL(glViewport(0, 0, m_Window.GetWidth(), m_Window.GetHeight()));
}
