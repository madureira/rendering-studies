#include "TexturedCube.h"
#include <RenderingStudies/RegisterApp.h>
#include "../../Shader/Shader.h"
#include "../../Texture/Texture.h"
#include "../../Window/Window.h"
#include "../Camera.h"

REGISTER_APP(TexturedCube)

TexturedCube::TexturedCube(Window* window)
    : m_Window(window)
{
    m_Shader = new Shader("assets/shaders/texture.vert", "assets/shaders/texture.frag");
    m_Camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
    CreateMesh();
}

TexturedCube::~TexturedCube()
{
    delete m_Texture;
    delete m_Camera;
    delete m_Shader;

    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
}

void TexturedCube::Update(float32 deltaTime)
{
    const MouseState& mouse = m_Window->GetMouse();

    if (mouse.LeftDown())
    {
        m_Camera->ProcessMouseDelta((float32)mouse.dx, (float32)mouse.dy);
    }

    float32 speed = 1.0f;

    if (m_Window->IsKeyPressed(KeyToken::LeftShift))
    {
        speed = 3.f;
    }

    if (m_Window->IsKeyPressed(KeyToken::Up) || m_Window->IsKeyPressed(KeyToken::W))
    {
        m_Camera->ProcessKeyboard(CameraMove::FORWARD, deltaTime, speed);
    }

    if (m_Window->IsKeyPressed(KeyToken::Down) || m_Window->IsKeyPressed(KeyToken::S))
    {
        m_Camera->ProcessKeyboard(CameraMove::BACKWARD, deltaTime, speed);
    }

    if (m_Window->IsKeyPressed(KeyToken::Left) || m_Window->IsKeyPressed(KeyToken::A))
    {
        m_Camera->ProcessKeyboard(CameraMove::LEFT, deltaTime, speed);
    }

    if (m_Window->IsKeyPressed(KeyToken::Right) || m_Window->IsKeyPressed(KeyToken::D))
    {
        m_Camera->ProcessKeyboard(CameraMove::RIGHT, deltaTime, speed);
    }
}

void TexturedCube::Render()
{
    glm::mat4 view = m_Camera->GetViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 projection = m_Camera->GetProjectionMatrix(m_Window->GetWidth(), m_Window->GetHeight());

    m_Shader->Bind();
    m_Texture->Bind(0);

    m_Shader->SetMat4("u_Model", model);
    m_Shader->SetMat4("u_View", view);
    m_Shader->SetMat4("u_Projection", projection);

    glBindVertexArray(m_VAO);

    glDisable(GL_CULL_FACE);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glEnable(GL_CULL_FACE);

    // Unbind
    glBindVertexArray(0);
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

    // Load and create a texture
    TextureParams texParams{};
    texParams.srgb = false;
    texParams.generateMipmaps = true;
    texParams.flipY = true;

    m_Texture = new Texture("assets/images/container.jpg", texParams);

    m_Shader->Bind();
    m_Shader->SetInt("u_Texture", 0);
    //m_Shader->SetInt("u_UseTint", 1);
    //m_Shader->SetVec4("u_Tint", 2,2,2,2);
    m_Shader->Unbind();

    GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GL(glBindVertexArray(0));
}
