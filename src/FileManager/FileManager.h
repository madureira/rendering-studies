#pragma once

#include <RenderingStudies/Types.h>
#include <string>

class FileManager
{
public:
    FileManager();

    static std::string ReadText(const std::string &filename);
    static uchar *LoadTexture(const std::string &filename, int32 &width, int32 &height, int32 &channels, bool flip = true);
    static void FreeTexture(uchar *data);
};
