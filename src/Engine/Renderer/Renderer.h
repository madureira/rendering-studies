#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <RenderingStudies/Types.h>

class Shader;
class TextRenderer;
class Grid;
class Camera;

class Renderer final
{
private:
    Shader* m_TextShader;
    TextRenderer* m_TextRenderer;
    Camera* m_Camera;
    Grid* m_Grid;

    mutable float64 m_FpsLastTime = 0.0;
    mutable int32 m_FpsNbFrames = 0;

    uint32 m_WindowInitialWidth;
    uint32 m_WindowInitialHeight;

public:
     Renderer(const uint32 windowInitialWidth, const uint32 windowInitialHeight);
     ~Renderer();

     void Clear(float32 r, float32 g, float32 b) const;
     void RenderFPS(float64 currentTime, bool display = false) const;
     void SetPolygonMode(bool enabled = false);
     void SetZBuffer(bool enabled = true);
     void SetCullFace(bool enabled = true);
     void RenderGrid(uint32 winWidth, uint32 winHeight, bool enabled = true) const;
     void ResetCameraPosition() const;
     Camera* GetCamera() const;
};
