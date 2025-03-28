#pragma once

#include <string>

#include <RenderingStudies/Types.h>

class Texture
{
private:
    uint32 m_ID;
    int32 m_Width;
    int32 m_Height;
    int32 m_Channels;

public:
    Texture(const std::string &path);
    ~Texture();

    void Bind(uint32 slot = 0) const;
    void Unbind() const;

    inline int32 GetWidth() const
    {
        return m_Width;
    }

    inline int32 GetHeight() const
    {
        return m_Height;
    }
};
