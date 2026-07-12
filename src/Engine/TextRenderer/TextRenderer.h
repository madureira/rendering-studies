#pragma once

#include <array>
#include <string>

#include <glm/glm.hpp>

class Shader;

class TextRenderer final
{
private:
    // Batch state — fixed-size CPU buffer, no heap allocation per frame
    static constexpr int MAX_BATCH_CHARS = 128;

    struct Character
    {
        glm::ivec2 Size;
        glm::ivec2 Bearing;
        uint32 Advance;
        glm::vec2 TexOffset;
        glm::vec2 TexSize;
    };
    std::array<Character, 256> m_Characters{};

    mutable const Shader* m_BoundShader = nullptr;
    uint32 m_VAO;
    uint32 m_VBO;
    uint32 m_AtlasTexture = 0;
    mutable int m_BatchGlyphCount = 0;
    mutable glm::vec3 m_LastBatchColor{ -1.0f };
    mutable float32 m_BatchBuffer[MAX_BATCH_CHARS * 6 * 4]{};

public:
    TextRenderer(const std::string& fontPath);
    ~TextRenderer();

    void BeginBatch(const Shader& shader) const;
    void SubmitText(const std::string& text, const glm::vec3& color, float32 x, float32 y, float32 scale) const;
    void EndBatch() const;
    void Render(const Shader& shader, const std::string& text, const glm::vec3& color, float32 x, float32 y, float32 scale) const;
    float32 MeasureText(const std::string& text, float32 scale) const;

private:
    void FlushBuffer() const;
};
