#pragma once

#include <RenderingStudies/App.h>
#include <RenderingStudies/Types.h>

class Camera;
class Grid;
class Skybox;
class Window;

class SkyboxExample final : public App
{
private:
    Window* m_Window;
    Camera* m_Camera;
    Grid* m_Grid;
    Skybox* m_Skybox;
    int32 m_CurrentSkybox;

    static const char* const s_SkyboxOptions[2];

public:
    SkyboxExample(Window* window);
    ~SkyboxExample();

    virtual void Update(float32 deltaTime) override;
    virtual void Render() override;

private:
    void CreateSkybox();
};
