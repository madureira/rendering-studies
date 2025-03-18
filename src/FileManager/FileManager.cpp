#include "FileManager.h"

#include <RenderingStudies/Types.h>

#include <fstream>
#include <sstream>

FileManager::FileManager()
{
}

FileManager::~FileManager()
{
}

std::string FileManager::Read(const std::string &filename)
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
