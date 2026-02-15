#include "TexturedCube.h"
#include <RenderingStudies/RegisterApp.h>
#include <RenderingStudies/GL.h>
#include "../../Engine/Shader/Shader.h"
#include "../../Engine/Texture/Texture.h"
#include "../../Engine/Window/Window.h"
#include "../../Engine/Camera/Camera.h"
#include "../../Engine/Utils/InputProcessorUtil.h"

REGISTER_APP(TexturedCube)

TexturedCube::TexturedCube(Window* window)
    : m_Window(window)
{
    m_Shader = new Shader("assets/shaders/texture.vert", "assets/shaders/texture.frag");
    // Load and create a texture
    TextureParams texParams{};
    texParams.srgb = false;
    texParams.generateMipmaps = true;
    texParams.flipY = true;
    m_Texture = new Texture("assets/images/container.jpg", texParams);
    m_Camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
    CreateMesh();
}

TexturedCube::~TexturedCube()
{
    delete m_Camera;
    delete m_Texture;
    delete m_Shader;

    GL(glDeleteVertexArrays(1, &m_VAO));
    GL(glDeleteBuffers(1, &m_VBO));
    GL(glDeleteBuffers(1, &m_EBO));
}

void TexturedCube::Update(float32 deltaTime)
{
    InputProcessorUtil::moveCamera(m_Camera, m_Window, deltaTime);
}

void TexturedCube::Render()
{
    glm::mat4 view = m_Camera->GetViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 projection = m_Camera->GetProjectionMatrix(m_Window->GetWidth(), m_Window->GetHeight());

    m_Shader->Bind();
    m_Texture->Bind(0);

    m_Shader->SetMat4("u_MVP", projection * view * model);

    GL(glBindVertexArray(m_VAO));

    GL(glDisable(GL_CULL_FACE));
    GL(glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0));
    GL(glEnable(GL_CULL_FACE));

    // Unbind
    GL(glBindVertexArray(0));
    m_Shader->Unbind();
    m_Texture->Unbind();
}

void TexturedCube::CreateMesh()
{
    float32 vertices[] = {
        // Positions           // TexCoords
        // Front face
        -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,   0.0f, 1.0f,

        // Back face
        -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,   1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,   1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,

        // Left face
        -0.5f,  0.5f,  0.5f,   0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,   1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,   1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,   0.0f, 1.0f,

        // Right face
         0.5f,  0.5f,  0.5f,   0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,   1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,   1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,   0.0f, 1.0f,

        // Bottom face
        -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,   1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,   1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,   0.0f, 1.0f,

        // Top face
        -0.5f,  0.5f, -0.5f,   0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,   0.0f, 1.0f,
    };


    uint32 indices[] = {
        // Front face
        0, 1, 2, 2, 3, 0,
        // Back face
        4, 5, 6, 6, 7, 4,
        // Left face
        8, 9, 10, 10, 11, 8,
        // Right face
        12, 13, 14, 14, 15, 12,
        // Bottom face
        16, 17, 18, 18, 19, 16,
        // Top face
        20, 21, 22, 22, 23, 20,
    };

    // Generate Objects
    GL(glGenVertexArrays(1, &m_VAO));
    GL(glGenBuffers(1, &m_VBO));
    GL(glGenBuffers(1, &m_EBO));

    // Bind the Vertex Array Object
    GL(glBindVertexArray(m_VAO));

    // Bind and set vertex buffer
    GL(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
    GL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

    // Bind and set index buffer
    GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO));
    GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

    const int32 stride = 5 * sizeof(float32);

    // Position attribute (location = 0 in shader)
    GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0));
    GL(glEnableVertexAttribArray(0));

    // Texture coordinate attribute (location = 1 in shader)
    GL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float32))));
    GL(glEnableVertexAttribArray(1));

    m_Shader->Bind();
    m_Shader->SetInt("u_Texture", 0);
    //m_Shader->SetInt("u_UseTint", 1);
    //m_Shader->SetVec4("u_Tint", 2,2,2,2);
    m_Shader->Unbind();

    GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GL(glBindVertexArray(0));
}
