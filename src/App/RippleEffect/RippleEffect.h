#pragma once

#include <vector>

#include <RenderingStudies/App.h>
#include <RenderingStudies/Types.h>

class Camera;
class Shader;
class Window;

class RippleEffect final : public App
{
private:
    Window* m_Window;
    Shader* m_Shader;
    Camera* m_Camera;

    uint32 m_VAO;
    uint32 m_VBO;
    uint32 m_EBO;
    uint32 m_IndexCount;
    std::vector<float32> m_Vertices;
    std::vector<uint32> m_Indices;

    int32 m_TessLevel;
    float32 m_Amplitude;
    float32 m_Frequency;

public:
    RippleEffect(Window* window);
    ~RippleEffect();

    virtual void Update(float32 deltaTime) override;
    virtual void Render() override;

private:
    void CreateMesh();
};
