#include "TextRenderer.h"

#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <RenderingStudies/GL.h>

#include "../Shader/Shader.h"

TextRenderer::TextRenderer(const std::string& fontPath)
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

    // Compute atlas dimensions
    uint32 atlasWidth = 0;
    uint32 atlasHeight = 0;
    for (uint32_t c = 0; c < 256; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            continue;
        }
        atlasWidth += face->glyph->bitmap.width + 1; // +1px padding to avoid linear filter bleed
        if (face->glyph->bitmap.rows > atlasHeight)
        {
            atlasHeight = face->glyph->bitmap.rows;
        }
    }

    if (atlasWidth == 0 || atlasHeight == 0)
    {
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
        return;
    }

    // Allocate single atlas texture
    GL(glGenTextures(1, &m_AtlasTexture));
    GL(glBindTexture(GL_TEXTURE_2D, m_AtlasTexture));
    {
        std::vector<uint8_t> zeros(atlasWidth * atlasHeight, 0);
#ifdef __EMSCRIPTEN__
        GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, static_cast<GLsizei>(atlasWidth), static_cast<GLsizei>(atlasHeight), 0, GL_RED, GL_UNSIGNED_BYTE, zeros.data()));
#else
        GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, static_cast<GLsizei>(atlasWidth), static_cast<GLsizei>(atlasHeight), 0, GL_RED, GL_UNSIGNED_BYTE, zeros.data()));
#endif
    }
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    // Upload each glyph bitmap into the atlas and record UV coordinates
    uint32 xOffset = 0;
    for (uint32_t c = 0; c < 256; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            continue;
        }
        const auto& bm = face->glyph->bitmap;
        if (bm.width > 0 && bm.rows > 0 && bm.buffer)
        {
            GL(glTexSubImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(xOffset), 0, static_cast<GLsizei>(bm.width), static_cast<GLsizei>(bm.rows), GL_RED, GL_UNSIGNED_BYTE, bm.buffer));
        }

        m_Characters[c] = {
            glm::ivec2(bm.width, bm.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<uint32>(face->glyph->advance.x),
            glm::vec2(static_cast<float32>(xOffset) / static_cast<float32>(atlasWidth), 0.0f),
            glm::vec2(static_cast<float32>(bm.width) / static_cast<float32>(atlasWidth),
                static_cast<float32>(bm.rows) / static_cast<float32>(atlasHeight))
        };
        xOffset += bm.width + 1;
    }

    GL(glBindTexture(GL_TEXTURE_2D, 0));
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    GL(glGenVertexArrays(1, &m_VAO));
    GL(glBindVertexArray(m_VAO));
    GL(glGenBuffers(1, &m_VBO));
    GL(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
    GL(glBufferData(GL_ARRAY_BUFFER, sizeof(float32) * 6 * 4 * MAX_BATCH_CHARS, NULL, GL_STREAM_DRAW));
    GL(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0));
    GL(glEnableVertexAttribArray(0));
    GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GL(glBindVertexArray(0));
}

TextRenderer::~TextRenderer()
{
    if (m_AtlasTexture != 0)
    {
        GL(glDeleteTextures(1, &m_AtlasTexture));
    }
    GL(glDeleteBuffers(1, &m_VBO));
    GL(glDeleteVertexArrays(1, &m_VAO));
}

void TextRenderer::FlushBuffer() const
{
    if (m_BatchGlyphCount == 0)
    {
        return;
    }
    const GLsizeiptr needed = static_cast<GLsizeiptr>(m_BatchGlyphCount) * 6 * 4 * sizeof(float32);
    GL(glBufferSubData(GL_ARRAY_BUFFER, 0, needed, m_BatchBuffer));
    GL(glDrawArrays(GL_TRIANGLES, 0, m_BatchGlyphCount * 6));
    m_BatchGlyphCount = 0;
}

void TextRenderer::BeginBatch(const Shader& shader) const
{
    m_BoundShader = &shader;
    GL(glActiveTexture(GL_TEXTURE0));
    GL(glBindTexture(GL_TEXTURE_2D, m_AtlasTexture));
    GL(glBindVertexArray(m_VAO));
#ifndef __EMSCRIPTEN__
    GL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
#endif
    GL(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
    m_BatchGlyphCount = 0;
    m_LastBatchColor = glm::vec3(-1.0f);
}

void TextRenderer::SubmitText(const std::string& text, const glm::vec3& color, float32 x, float32 y, float32 scale) const
{
    if (color != m_LastBatchColor)
    {
        FlushBuffer();
        m_BoundShader->SetVec3("u_TextColor", color.x, color.y, color.z);
        m_LastBatchColor = color;
    }

    for (const char c : text)
    {
        if (m_BatchGlyphCount >= MAX_BATCH_CHARS)
        {
            break;
        }

        const Character& ch = m_Characters[static_cast<unsigned char>(c)];
        if (ch.Size.x > 0 && ch.Size.y > 0)
        {
            const float32 xpos = x + ch.Bearing.x * scale;
            const float32 ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
            const float32 w = ch.Size.x * scale;
            const float32 h = ch.Size.y * scale;
            const float32 u0 = ch.TexOffset.x;
            const float32 v0 = ch.TexOffset.y;
            const float32 u1 = u0 + ch.TexSize.x;
            const float32 v1 = v0 + ch.TexSize.y;

            float32* dst = m_BatchBuffer + m_BatchGlyphCount * 6 * 4;
            // clang-format off
            dst[ 0] = xpos;     dst[ 1] = ypos + h; dst[ 2] = u0; dst[ 3] = v0;
            dst[ 4] = xpos;     dst[ 5] = ypos;     dst[ 6] = u0; dst[ 7] = v1;
            dst[ 8] = xpos + w; dst[ 9] = ypos;     dst[10] = u1; dst[11] = v1;
            dst[12] = xpos;     dst[13] = ypos + h; dst[14] = u0; dst[15] = v0;
            dst[16] = xpos + w; dst[17] = ypos;     dst[18] = u1; dst[19] = v1;
            dst[20] = xpos + w; dst[21] = ypos + h; dst[22] = u1; dst[23] = v0;
            // clang-format on

            m_BatchGlyphCount++;
        }
        x += (ch.Advance >> 6) * scale;
    }
}

void TextRenderer::EndBatch() const
{
    FlushBuffer();
    GL(glBindTexture(GL_TEXTURE_2D, 0));
    GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GL(glBindVertexArray(0));
    m_BoundShader = nullptr;
}

void TextRenderer::Render(const Shader& shader, const std::string& text, const glm::vec3& color, float32 x, float32 y, float32 scale) const
{
    if (text.empty())
    {
        return;
    }
    shader.Bind();
    BeginBatch(shader);
    SubmitText(text, color, x, y, scale);
    EndBatch();
    shader.Unbind();
}

float32 TextRenderer::MeasureText(const std::string& text, float32 scale) const
{
    float32 width = 0.0f;
    for (const char& c : text)
    {
        width += (m_Characters[static_cast<unsigned char>(c)].Advance >> 6) * scale;
    }
    return width;
}
