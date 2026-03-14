#pragma once

#include <RenderingStudies/App.h>
#include <RenderingStudies/Types.h>

class Camera;
class Model;
class Shader;
class Window;

class Dragon final : public App
{
private:
    const Window& m_Window;
    const Camera& m_Camera;
    Shader* m_Shader;
    Model* m_Model;

public:
    Dragon(const Window& window, const Camera& camera);
    ~Dragon();

    virtual void Update(float32 deltaTime) override;
    virtual void Render() override;
};
