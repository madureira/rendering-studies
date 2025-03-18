#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <rendering_studies/app.h>
#include <rendering_studies/types.h>

#include "../../shader/shader.h"
#include "../../window/window.h"
#include "../camera.h"

class Cube : public App
{
private:
    uint32 m_vao;
    uint32 m_vbo;
    uint32 m_ebo;

    glm::mat4 m_model;
    glm::mat4 m_view;

    Shader m_shader;

    float32 m_positionX;
    float32 m_positionY;

    Camera *m_camera;

    // Delta time
    bool m_firstMouse = true;
    float32 m_deltaTime = 0.0f;
    float32 m_lastFrame = 0.0f;
    float32 m_lastX;
    float32 m_lastY;

public:
    Cube();
    ~Cube();

    virtual void update(Window *window) override;

private:
    void createMesh();
};
