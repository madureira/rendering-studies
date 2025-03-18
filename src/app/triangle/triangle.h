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
    uint32 m_VAO;
    uint32 m_VBO;
    uint32 m_EBO;

    glm::mat4 m_Model;
    glm::mat4 m_View;

    Shader m_Shader;

public:
    Triangle();
    ~Triangle();

    virtual void Update(Window *window) override;

private:
    void CreateMesh();
};
