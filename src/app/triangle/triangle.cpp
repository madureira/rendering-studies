#include "./triangle.h"

#include <iostream>

#include "../../file_manager/file_manager.h"

Triangle::Triangle()
    : m_model(1.f),
      m_view(1.f),
      m_shader(Shader(
          FileManager::read("shaders/simple.vs"), // Vertex shader source
          FileManager::read("shaders/simple.fs")  // Fragment shader source
          ))
{
    createMesh();

    // Camera setup
    glm::vec3 cameraPosition(0.f, 0.f, 2.f);       // Camera is placed at (0, 0, 2)
    glm::vec3 cameraViewDirection(0.f, 0.f, -1.f); // Camera looks towards the negative Z-axis

    // Model matrix
    m_model = glm::mat4(1.f); // Identity matrix, meaning no transformation on the model

    // View matrix
    m_view = glm::lookAt(
        cameraPosition,                       // Camera position
        cameraPosition + cameraViewDirection, // Target position (camera looks here)
        glm::vec3(0.f, 1.f, 0.f)              // Up vector (positive Y-axis)
    );
}

Triangle::~Triangle()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
}

void Triangle::update(Window *window)
{
    float32 time = window->getTime();
    uint32 windowWidth = window->getWidth();
    uint32 windowHeight = window->getHeight();

    // Vary color based on time
    float32 red = (std::sin(time * 0.5f) + 1.0f) / 4.0f;
    float32 green = (std::sin(time * 0.3f) + 1.0f) / 4.0f;
    float32 blue = (std::sin(time * 0.7f) + 1.0f) / 4.0f;

    glClearColor(red, green, blue, 1.0f); // Use oscillating colors

    {
        m_shader.use();

        glm::mat4 projection = glm::perspective(
            (float32)M_PI_2,                              // Field of view (90 degrees in radians)
            (float32)windowWidth / (float32)windowHeight, // Aspect ratio (width / height)
            0.01f,                                        // Near clipping plane
            100.0f                                        // Far clipping plane
        );

        // Update model matrix for rotating the triangle on Z-axis
        m_model = glm::rotate(
            glm::mat4(1.f),
            std::sin(time * 0.8f) / 4.f,
            glm::vec3(0.f, 0.f, -1.f));

        m_shader.setMat4("u_model", m_model);
        m_shader.setMat4("u_view", m_view);
        m_shader.setMat4("u_projection", projection);

        glBindVertexArray(m_vao);

        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        // Unbind the VAO
        glBindVertexArray(0);
    }
}

void Triangle::createMesh()
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
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    // Bind the VAO to start recording the OpenGL state for this triangle
    glBindVertexArray(m_vao);

    // Bind the VBO to the GL_ARRAY_BUFFER target to store vertex data
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    // Upload the vertex data to the GPU
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Bind the EBO to the GL_ELEMENT_ARRAY_BUFFER target to store index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    // Upload the index data to the GPU
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Specify the layout of the vertex data:
    // Attribute 0: Position (x, y, z) - 3 floats, starting at offset 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float32), (void *)0);
    glEnableVertexAttribArray(0); // Enable attribute 0

    // Attribute 1: Color (r, g, b) - 3 floats, starting at offset 3 * sizeof(float)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float32), (void *)(3 * sizeof(float32)));
    glEnableVertexAttribArray(1); // Enable attribute 1

    // Unbind the VBO (optional, to prevent accidental modification of VBO data)
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Unbind the VAO (optional, to prevent accidental modification of VAO state)
    glBindVertexArray(0);
}
