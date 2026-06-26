#pragma once

#include <RenderingStudies/Demo.h>
#include <RenderingStudies/Types.h>

class Camera;
class ModelSelector;
class Shader;
class Window;

class DisplayNormals final : public Demo
{
private:
    const Window& m_Window;
    const Camera& m_Camera;
    Shader* m_Shader = nullptr;
    Shader* m_NormalShader = nullptr;
    ModelSelector* m_ModelSelector = nullptr;

    bool m_ShowNormals = true;
    bool m_IsFaceNormal = false;

public:
    DisplayNormals(const Window& window, const Camera& camera);
    ~DisplayNormals();

    virtual void Update(float32 deltaTime) override;
    virtual void Render() override;
};
