#include "Cube.h"
#include <RenderingStudies/RegisterApp.h>
#include <RenderingStudies/GL.h>
#include "../../Engine/Shader/Shader.h"
#include "../../Engine/Window/Window.h"
#include "../../Engine/Camera/Camera.h"
#include "../../Engine/Utils/InputProcessorUtil.h"

REGISTER_APP(Cube)

Cube::Cube(Window* window)
    : m_Window(window)
{
    m_Shader = new Shader("assets/shaders/simple.vert", "assets/shaders/simple.frag");
    m_Camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
    CreateMesh();
}

Cube::~Cube()
{
    delete m_Camera;
    delete m_Shader;
    GL(glDeleteVertexArrays(1, &m_VAO));
    GL(glDeleteBuffers(1, &m_VBO));
    GL(glDeleteBuffers(1, &m_EBO));
}

void Cube::Update(float32 deltaTime)
{
    InputProcessorUtil::moveCamera(m_Camera, m_Window, deltaTime);
}

void Cube::Render()
{
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = m_Camera->GetViewMatrix();
    glm::mat4 projection = m_Camera->GetProjectionMatrix(m_Window->GetWidth(), m_Window->GetHeight());

    m_Shader->Bind();

    m_Shader->SetMat4("u_MVP", projection * view * model);

    GL(glBindVertexArray(m_VAO));

    GL(glDisable(GL_CULL_FACE));
    GL(glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0));
    GL(glEnable(GL_CULL_FACE));

    // Unbind
    GL(glBindVertexArray(0));
    m_Shader->Unbind();
}

void Cube::CreateMesh()
{
    float32 vertices[] = {
        // Positions        // Colors
        // Front face
        -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, // Bottom-left
         0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, // Bottom-right
         0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f, // Top-right
        -0.5f,  0.5f, 0.5f, 1.0f, 1.0f, 0.0f, // Top-left

        // Back face
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, // Bottom-left
         0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, // Bottom-right
         0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, // Top-right
        -0.5f,  0.5f, -0.5f, 0.5f, 0.5f, 0.5f, // Top-left
    };

    uint32 indices[] = {
        // Front face
        0, 1, 2, 2, 3, 0,
        // Back face
        4, 5, 6, 6, 7, 4,
        // Left face
        4, 0, 3, 3, 7, 4,
        // Right face
        1, 5, 6, 6, 2, 1,
        // Top face
        3, 2, 6, 6, 7, 3,
        // Bottom face
        4, 5, 1, 1, 0, 4,
    };

    // Generate Objects
    GL(glGenVertexArrays(1, &m_VAO));
    GL(glGenBuffers(1, &m_VBO));
    GL(glGenBuffers(1, &m_EBO));

    // Bind Vertex Array Object
    GL(glBindVertexArray(m_VAO));

    // Bind and set vertex buffer
    GL(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
    GL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

    // Bind and set index buffer
    GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO));
    GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

    // Position attribute (location = 0 in shader)
    GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float32), (void*)0));
    GL(glEnableVertexAttribArray(0));

    // Color attribute (location = 1 in shader)
    GL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float32), (void*)(3 * sizeof(float32))));
    GL(glEnableVertexAttribArray(1));

    // Unbind objects
    GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GL(glBindVertexArray(0));
}
