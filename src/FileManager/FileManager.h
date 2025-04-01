#pragma once

#include <string>

#include <RenderingStudies/Types.h>

class FileManager final
{
public:
    FileManager();

    static std::string ReadText(const std::string& filename);
    static uchar* LoadTexture(const std::string& filename, int32& width, int32& height, int32& channels, bool flip = true);
    static void FreeTexture(uchar* data);
};
