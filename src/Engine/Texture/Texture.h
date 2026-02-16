#pragma once

#include <string>
#include <RenderingStudies/Types.h>

struct TextureParams
{
    bool flipY = true;
    bool generateMipmaps = true;
    bool srgb = false;
    int32 wrapS = 0; // 0 = REPEAT, 1 = CLAMP_TO_EDGE
    int32 wrapT = 0; // 0 = REPEAT, 1 = CLAMP_TO_EDGE
};

class Texture final
{
private:
    uint32 m_ID = 0;
    int32 m_Width = 0;
    int32 m_Height = 0;
    int32 m_Channels = 0;

public:
    explicit Texture(const std::string& path, const TextureParams& params = TextureParams{});
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    void Bind(uint32 slot = 0) const;
    void Unbind();

    uint32 GetID() const
    {
        return m_ID;
    }
    int32 GetWidth() const
    {
        return m_Width;
    }
    int32 GetHeight() const
    {
        return m_Height;
    }
    int32 GetChannels() const
    {
        return m_Channels;
    }
    bool IsValid() const
    {
        return m_ID != 0;
    }
};
