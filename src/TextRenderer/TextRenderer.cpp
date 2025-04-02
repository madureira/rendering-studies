
#include "TextRenderer.h"

#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <RenderingStudies/GL.h>

TextRenderer::TextRenderer(std::string fontPath)
{
    FT_Library ft;

    if (FT_Init_FreeType(&ft))
    {
        LOG_ERROR("FREETYPE: {0}", "Could not init FreeType Library");
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
    {
        LOG_ERROR("FREETYPE: {0}", "Failed to load font");
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);

    GL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

    for (GLubyte c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            LOG_ERROR("FREETYPE: {0}", "Failed to load Glyph");
            continue;
        }

        GLuint texture;
        GL(glGenTextures(1, &texture));
        GL(glBindTexture(GL_TEXTURE_2D, texture));
        GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer));

        GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

        GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

        Character character = { texture, glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows), glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top), (GLuint)face->glyph->advance.x };
        m_Characters.insert(std::pair<GLchar, Character>(c, character));
    }

    GL(glBindTexture(GL_TEXTURE_2D, 0));

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    GL(glGenVertexArrays(1, &m_VAO));
    GL(glGenBuffers(1, &m_VBO));
    GL(glBindVertexArray(m_VAO));
    GL(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
    GL(glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW));
    GL(glEnableVertexAttribArray(0));
    GL(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0));
    GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GL(glBindVertexArray(0));
}

void TextRenderer::Render(Shader& shader, std::string text, float32 x, float32 y, float32 scale, glm::vec3 color)
{
    shader.Bind();

    shader.SetVec3("uTextColor", color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(m_VAO);

    std::string::const_iterator c;

    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = m_Characters[*c];

        float32 xpos = x + ch.Bearing.x * scale;
        float32 ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float32 w = ch.Size.x * scale;
        float32 h = ch.Size.y * scale;

        float32 vertices[6][4] = {
            { xpos, ypos + h, 0.0, 0.0 },
            { xpos, ypos, 0.0, 1.0 },
            { xpos + w, ypos, 1.0, 1.0 },
            { xpos, ypos + h, 0.0, 0.0 },
            { xpos + w, ypos, 1.0, 1.0 },
            { xpos + w, ypos + h, 1.0, 0.0 }
        };

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);

        GLint width, height;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

        if (width > 0 && height > 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        x += (ch.Advance >> 6) * scale;
    }

    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
    shader.Unbind();
}
