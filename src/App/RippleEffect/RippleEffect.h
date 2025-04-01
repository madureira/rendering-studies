#pragma once

#include <vector>

#include <RenderingStudies/App.h>
#include <RenderingStudies/Types.h>

#include "../../Shader/Shader.h"
#include "../../Window/Window.h"
#include "../Camera.h"

class RippleEffect final : public App
{
private:
    Window *m_Window;
    Shader *m_Shader;
    Camera *m_Camera;

    uint32 m_VAO;
    uint32 m_VBO;
    uint32 m_EBO;

    std::vector<float32> vertices;
    std::vector<uint32> indices;

public:
    RippleEffect(Window *window);
    ~RippleEffect();

    virtual void Update(float32 deltaTime) override;

private:
    void CreateMesh();
};
