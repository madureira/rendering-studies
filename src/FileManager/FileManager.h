#pragma once

#include <string>

class FileManager
{
public:
    FileManager();
    ~FileManager();

    static std::string Read(const std::string &filename);
};
