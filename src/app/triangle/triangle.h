#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <headers/types.h>
#include "../app.h"
#include "../../shader/shader.h"

class Triangle : public App
{
private:
    uint32 m_vao;
    uint32 m_vbo;
    uint32 m_ebo;

    glm::mat4 m_model;
    glm::mat4 m_view;

    Shader m_shader;

public:
    Triangle();
    ~Triangle();

    virtual void update(float32 time, uint32 windowWidth, uint32 windowHeight) override;

private:
    void createMesh();
};
