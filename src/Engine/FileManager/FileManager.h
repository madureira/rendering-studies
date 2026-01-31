#pragma once

#include <string>

#include <RenderingStudies/Types.h>

class FileManager final
{
private:
    FileManager() = default; // Prevent instantiation
    ~FileManager() = default;

    FileManager(const FileManager&) = delete;            // Prevent copy construction
    FileManager& operator=(const FileManager&) = delete; // Prevent copy assignment

    FileManager(FileManager&&) = delete;            // Prevent move construction
    FileManager& operator=(FileManager&&) = delete; // Prevent move assignment

public:
    static std::string ReadText(const std::string& filename);
    static uchar* LoadTexture(const std::string& filename, int32& width, int32& height, int32& channels, bool flip = true);
    static void FreeTexture(uchar* data);
};
