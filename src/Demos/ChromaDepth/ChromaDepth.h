#pragma once

#include <RenderingStudies/Demo.h>
#include <RenderingStudies/Types.h>

class Camera;
class Model;
class Shader;
class Window;

class ChromaDepth final : public Demo
{
private:
    const Window& m_Window;
    const Camera& m_Camera;
    Shader* m_Shader;
    Model* m_Model;

public:
    ChromaDepth(const Window& window, const Camera& camera);
    ~ChromaDepth();

    virtual void Update(float32 deltaTime) override;
    virtual void Render() override;
};
