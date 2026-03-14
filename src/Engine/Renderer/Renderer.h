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

     void Clear(const float32 r, float32 g, float32 b) const;
     void RenderFPS(const float64 currentTime, const bool display = false) const;
     void SetPolygonMode(const bool enabled = false);
     void SetZBuffer(const bool enabled = true);
     void SetCullFace(const bool enabled = true);
     void RenderGrid(const uint32 winWidth, const uint32 winHeight, const bool enabled = true) const;
     void ResetCameraPosition() const;
     Camera* GetCamera() const;
};
