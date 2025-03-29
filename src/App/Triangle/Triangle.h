#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <RenderingStudies/App.h>
#include <RenderingStudies/Types.h>

#include "../../Shader/Shader.h"
#include "../../Window/Window.h"

class Triangle : public App
{
private:
    Window *m_Window;
    Shader *m_Shader;

    uint32 m_VAO;
    uint32 m_VBO;
    uint32 m_EBO;

    glm::mat4 m_View;

public:
    Triangle(Window *window);
    ~Triangle();

    virtual void Update() override;

private:
    void CreateMesh();
};
