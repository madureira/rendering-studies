#include "FileManager.h"

#include <RenderingStudies/Log.h>
#include <fstream>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

std::string FileManager::ReadText(const std::string& filename)
{
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    std::stringstream fileStream;

    try
    {
        file.open(filename.c_str());
        fileStream << file.rdbuf();
        file.close();
    }
    catch (std::ifstream::failure e)
    {
        LOG_ERROR("FileManager: error reading file: {}", filename);
    }

    return fileStream.str();
}

uchar* FileManager::LoadTexture(const std::string& filename, int32& width, int32& height, int32& channels, bool flip)
{
    stbi_set_flip_vertically_on_load(flip);
    return stbi_load(filename.c_str(), &width, &height, &channels, 0);
}

void FileManager::FreeTexture(uchar* data)
{
    stbi_image_free(data);
}
