#pragma once

#include <RenderingStudies/Demo.h>
#include <RenderingStudies/Types.h>

class Camera;
class Skybox;
class Window;

class SkyboxExample final : public Demo
{
private:
    const Window& m_Window;
    const Camera& m_Camera;
    Skybox* m_Skybox = nullptr;
    int32 m_CurrentSkybox = 0;

    static const char* const s_SkyboxOptions[2];

public:
    SkyboxExample(const Window& window, const Camera& camera);
    ~SkyboxExample();

    virtual void Update(float32 deltaTime) override;
    virtual void Render() override;

private:
    void CreateSkybox();
};
