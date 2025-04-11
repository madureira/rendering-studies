#include "Texture.h"

#include <RenderingStudies/GL.h>

#include "../FileManager/FileManager.h"

Texture::Texture(const std::string& path)
{
    uchar* data = FileManager::LoadTexture(path, m_Width, m_Height, m_Channels, true);

    if (data)
    {
        GL(glGenTextures(1, &m_ID));
        GL(glBindTexture(GL_TEXTURE_2D, m_ID));

        GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
        GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
        GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

        if (m_Channels == 3)
        {
            GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
        }
        else if (m_Channels == 4)
        {
            GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
        }
        else
        {
            LOG_ERROR("Texture format not supported");
        }

        GL(glGenerateMipmap(GL_TEXTURE_2D));
    }
    else
    {
        LOG_ERROR("Failed to load texture");
    }

    FileManager::FreeTexture(data);

    Texture::Unbind();
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_ID);
}

void Texture::Bind(uint32 slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_ID);
}

void Texture::Unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}
