#include "Triangle.h"

#include <RenderingStudies/GL.h>
#include <RenderingStudies/RegisterApp.h>
#include <glm/gtc/matrix_transform.hpp>

#include "../../Engine/Camera/Camera.h"
#include "../../Engine/Shader/Shader.h"
#include "../../Engine/Utils/InputProcessorUtil.h"
#include "../../Engine/Window/Window.h"

REGISTER_APP(Triangle)

Triangle::Triangle(Window* window)
    : m_Window(window)
{
    m_Shader = new Shader("assets/shaders/simple.vert", "assets/shaders/simple.frag");
    m_Camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
    CreateMesh();
}

Triangle::~Triangle()
{
    delete m_Camera;
    if (m_Shader)
    {
        m_Shader->Unbind();
        delete m_Shader;
    }
    GL(glDeleteVertexArrays(1, &m_VAO));
    GL(glDeleteBuffers(1, &m_VBO));
    GL(glDeleteBuffers(1, &m_EBO));
}

void Triangle::Update([[maybe_unused]] float32 deltaTime)
{
    InputProcessorUtil::moveCamera(m_Camera, m_Window, deltaTime);
}

void Triangle::Render()
{
    float32 time = m_Window->GetTime();

    // Vary color based on time
    float32 red = (std::sin(time * 0.5f) + 1.0f) / 4.0f;
    float32 green = (std::sin(time * 0.3f) + 1.0f) / 4.0f;
    float32 blue = (std::sin(time * 0.7f) + 1.0f) / 4.0f;

    GL(glClearColor(red, green, blue, 1.0f)); // Use oscillating colors
    GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    // Update model matrix for rotating the triangle on Z-axis
    glm::mat4 model = glm::rotate(glm::mat4(1.f), std::sin(time * 0.8f) / 4.f, glm::vec3(0.f, 0.f, -1.f));
    glm::mat4 view = m_Camera->GetViewMatrix();
    glm::mat4 projection = m_Camera->GetProjectionMatrix(m_Window->GetWidth(), m_Window->GetHeight());

    m_Shader->Bind();

    m_Shader->SetMat4("u_MVP", projection * view * model);

    GL(glBindVertexArray(m_VAO));
    GL(glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0));

    // Unbind
    GL(glBindVertexArray(0));
    m_Shader->Unbind();
}

void Triangle::CreateMesh()
{
    // Define the vertices and their associated colors for the triangle
    // Each vertex has a position (x, y, z) and a color (r, g, b)
    float32 vertices[] = {
        0.0f, 1.0f, 0.0f,  // Position of vertex 1
        1.0f, 0.0f, 0.0f,  // Color of vertex 1 (Red)
        -1.f, -0.5f, 0.0f, // Position of vertex 2
        0.0f, 1.0f, 0.0f,  // Color of vertex 2 (Green)
        1.0f, -0.5f, 0.0f, // Position of vertex 3
        0.0f, 0.0f, 1.0f   // Color of vertex 3 (Blue)
    };

    // Define the indices that represent how to draw the triangle using the vertices
    uint32 indices[] = {
        0, 1, 2 // The triangle consists of vertices 0, 1, and 2
    };

    // Generate a Vertex Array Object (VAO), Vertex Buffer Object (VBO),
    // and Element Buffer Object (EBO) to manage and store OpenGL state
    GL(glGenVertexArrays(1, &m_VAO));
    GL(glGenBuffers(1, &m_VBO));
    GL(glGenBuffers(1, &m_EBO));

    // Bind the VAO to start recording the OpenGL state for this triangle
    GL(glBindVertexArray(m_VAO));

    // Bind the VBO to the GL_ARRAY_BUFFER target to store vertex data
    GL(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
    // Upload the vertex data to the GPU
    GL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

    // Bind the EBO to the GL_ELEMENT_ARRAY_BUFFER target to store index data
    GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO));
    // Upload the index data to the GPU
    GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

    // Specify the layout of the vertex data:
    // Attribute 0: Position (x, y, z) - 3 floats, starting at offset 0
    GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float32), (void*)0));
    GL(glEnableVertexAttribArray(0)); // Enable attribute 0

    // Attribute 1: Color (r, g, b) - 3 floats, starting at offset 3 * sizeof(float32)
    GL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float32), (void*)(3 * sizeof(float32))));
    GL(glEnableVertexAttribArray(1)); // Enable attribute 1

    // Unbind objects
    GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GL(glBindVertexArray(0));
}
