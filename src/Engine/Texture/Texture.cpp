#include "Texture.h"

#include "../FileManager/FileManager.h"
#include <RenderingStudies/GL.h>

static GLenum WrapFromParam(int32 v)
{
    // keep it simple for now
    // v == 0 => REPEAT, else CLAMP_TO_EDGE
    return (v == 0) ? GL_REPEAT : GL_CLAMP_TO_EDGE;
}

Texture::Texture(const std::string& path, const TextureParams& params)
{
    uchar* data = FileManager::LoadTexture(path, m_Width, m_Height, m_Channels, params.flipY);

    if (!data)
    {
        LOG_ERROR("Texture: failed to load '{}'", path);
        return;
    }

    GLenum dataFormat = 0;
    GLenum internalFormat = 0;

    if (m_Channels == 1)
    {
        dataFormat = GL_RED;
        internalFormat = GL_R8;
    }
    else if (m_Channels == 3)
    {
        dataFormat = GL_RGB;
#ifdef __EMSCRIPTEN__
        internalFormat = GL_RGB8;
#else
        internalFormat = params.srgb ? GL_SRGB8 : GL_RGB8;
#endif
    }
    else if (m_Channels == 4)
    {
        dataFormat = GL_RGBA;
#ifdef __EMSCRIPTEN__
        internalFormat = GL_RGBA8;
#else
        internalFormat = params.srgb ? GL_SRGB8_ALPHA8 : GL_RGBA8;
#endif
    }
    else
    {
        LOG_ERROR("Texture: unsupported channel count {} for '{}'", m_Channels, path);
        FileManager::FreeTexture(data);
        return;
    }

    GL(glGenTextures(1, &m_ID));
    GL(glBindTexture(GL_TEXTURE_2D, m_ID));

    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WrapFromParam(params.wrapS)));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WrapFromParam(params.wrapT)));

    if (params.generateMipmaps)
    {
        GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
        GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    }
    else
    {
        GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    }

    GL(glTexImage2D(
        GL_TEXTURE_2D,
        0,
        internalFormat,
        m_Width,
        m_Height,
        0,
        dataFormat,
        GL_UNSIGNED_BYTE,
        data));

    if (params.generateMipmaps)
    {
        GL(glGenerateMipmap(GL_TEXTURE_2D));
    }

    FileManager::FreeTexture(data);
    GL(glBindTexture(GL_TEXTURE_2D, 0));
}

Texture::~Texture()
{
    if (m_ID != 0)
    {
        GL(glDeleteTextures(1, &m_ID));
        m_ID = 0;
    }
}

Texture::Texture(Texture&& other) noexcept
{
    *this = std::move(other);
}

Texture& Texture::operator=(Texture&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    if (m_ID != 0)
    {
        GL(glDeleteTextures(1, &m_ID));
    }

    m_ID = other.m_ID;
    m_Width = other.m_Width;
    m_Height = other.m_Height;
    m_Channels = other.m_Channels;

    other.m_ID = 0;
    other.m_Width = 0;
    other.m_Height = 0;
    other.m_Channels = 0;

    return *this;
}

void Texture::Bind(uint32 slot) const
{
    GL(glActiveTexture(GL_TEXTURE0 + slot));
    GL(glBindTexture(GL_TEXTURE_2D, m_ID));
}

void Texture::Unbind()
{
    GL(glBindTexture(GL_TEXTURE_2D, 0));
}
