#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <RenderingStudies/App.h>
#include <RenderingStudies/Types.h>

#include "../../Shader/Shader.h"
#include "../../Texture/Texture.h"
#include "../../Window/Window.h"
#include "../Camera.h"

class TexturedCube : public App
{
private:
    Window *m_Window;
    Shader *m_Shader;
    Camera *m_Camera;
    Texture *m_Texture;

    uint32 m_VAO;
    uint32 m_VBO;
    uint32 m_EBO;

    float32 m_DeltaTime = 0.0f;
    float32 m_LastFrame = 0.0f;
    float32 m_LastX;
    float32 m_LastY;
    bool m_FirstMouse = true;

public:
    TexturedCube(Window *window);
    ~TexturedCube();

    virtual void Update() override;

private:
    void CreateMesh();
};
