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
    Window *m_Window;
    Camera *m_Camera;
    Shader *m_Shader;

    uint32 m_VAO;
    uint32 m_VBO;
    uint32 m_EBO;

    float32 m_DeltaTime = 0.0f;
    float32 m_LastFrame = 0.0f;
    float32 m_LastX;
    float32 m_LastY;
    bool m_FirstMouse = true;

public:
    Cube(Window *window);
    ~Cube();

    virtual void Update() override;

private:
    void CreateMesh();
};
