#pragma once

#include <glm/vec4.hpp>

#include <RenderingStudies/Types.h>

class Shader;
class TextRenderer;

class Renderer final
{
private:
    Shader* m_TextShader;
    TextRenderer* m_TextRenderer;

    mutable float64 m_FpsLastTime = 0.0;
    mutable int32 m_FpsNbFrames = 0;

    uint32 m_WindowInitialWidth;
    uint32 m_WindowInitialHeight;

public:
     Renderer(const uint32 windowInitialWidth, const uint32 windowInitialHeight);
     ~Renderer();

     void Clear(const glm::vec4& color) const;
     void RenderFPS(const float64 currentTime, const bool display) const;
     void SetPolygonMode(const bool enabled) const;
};
