#pragma once

#include <array>
#include <string>

#include <glm/glm.hpp>

#include <RenderingStudies/Types.h>

class Shader;

class TextRenderer final
{
private:
    struct Character
    {
        uint32 TextureID;
        glm::ivec2 Size;
        glm::ivec2 Bearing;
        uint32 Advance;
    };
    std::array<Character, 128> m_Characters{};
    uint32 m_VAO;
    uint32 m_VBO;

public:
    TextRenderer(std::string fontPath);
    ~TextRenderer();

    void Render(Shader& shader, const std::string& text, float32 x, float32 y, float32 scale, glm::vec3 color);
};
