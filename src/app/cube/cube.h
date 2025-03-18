#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <RenderingStudies/App.h>
#include <RenderingStudies/Types.h>

#include "../../Shader/Shader.h"
#include "../../Window/Window.h"
#include "../Camera.h"

class Cube : public App
{
private:
    uint32 m_VAO;
    uint32 m_VBO;
    uint32 m_EBO;

    glm::mat4 m_Model;
    glm::mat4 m_View;

    bool m_FirstMouse = true;
    float32 m_DeltaTime = 0.0f;
    float32 m_LastFrame = 0.0f;
    float32 m_LastX;
    float32 m_LastY;

    Shader m_Shader;
    Camera *m_Camera;

public:
    Cube();
    ~Cube();

    virtual void Update(Window *window) override;

private:
    void CreateMesh();
};
