#pragma once

#include <map>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
    std::map<char, Character> m_Characters;
    uint32 m_VAO;
    uint32 m_VBO;

public:
    TextRenderer(std::string fontPath);

    void Render(Shader &shader, std::string text, float32 x, float32 y, float32 scale, glm::vec3 color);
};
