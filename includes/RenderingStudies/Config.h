#pragma once

#include "Log.h"
#include "Types.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <string>

struct Config
{
    std::string window_title;
    int16 window_width;
    int16 window_height;
    bool vsync_on;
    int32 monitor_index;
};

namespace
{
inline bool parseBool(const std::string& raw)
{
    std::string v = raw;
    v.erase(v.begin(), std::find_if(v.begin(), v.end(), [](unsigned char c) { return !std::isspace(c); }));
    v.erase(std::find_if(v.rbegin(), v.rend(), [](unsigned char c) { return !std::isspace(c); }).base(),
        v.end());

    std::transform(v.begin(), v.end(), v.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    if (v == "true" || v == "1")
    {
        return true;
    }
    if (v == "false" || v == "0")
    {
        return false;
    }

    throw std::runtime_error("Invalid boolean value in config: '" + raw + "'");
}
} // namespace

inline Config loadConfig(const std::string& path)
{
    std::ifstream file(path);
    if (!file)
    {
        throw std::runtime_error("Failed to open config file: " + path);
    }

    std::string line;

    std::string window_title;
    int16 window_width = 0;
    int16 window_height = 0;
    bool vsync_on = false;
    int8 monitor_index = 0;

    bool has_width = false;
    bool has_height = false;

    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
        {
            continue;
        }

        const std::string::size_type pos = line.find('=');
        if (pos == std::string::npos)
        {
            continue;
        }

        const std::string key = line.substr(0, pos);
        const std::string value = line.substr(pos + 1);

        if (key == "window_title")
        {
            window_title = value;
        }
        else if (key == "window_width")
        {
            int v = std::stoi(value);
            if (v < std::numeric_limits<int16>::min() || v > std::numeric_limits<int16>::max())
            {
                throw std::runtime_error("window_width out of int16 range");
            }
            window_width = static_cast<int16>(v);
            has_width = true;
        }
        else if (key == "window_height")
        {
            int v = std::stoi(value);
            if (v < std::numeric_limits<int16>::min() || v > std::numeric_limits<int16>::max())
            {
                throw std::runtime_error("window_height out of int16 range");
            }
            window_height = static_cast<int16>(v);
            has_height = true;
        }
        else if (key == "vsync_on")
        {
            vsync_on = parseBool(value);
        }
        else if (key == "monitor_index")
        {
            int v = std::stoi(value);
            if (v < std::numeric_limits<int32>::min() || v > std::numeric_limits<int32>::max())
            {
                throw std::runtime_error("monitor_index out of int8 range");
            }
            monitor_index = static_cast<int32>(v);
        }
    }

    if (window_title.empty())
    {
        throw std::runtime_error("No 'window_title' key found in config file");
    }

    if (!has_width || !has_height)
    {
        throw std::runtime_error("Missing 'window_width' or 'window_height' in config file");
    }


    return Config {
        window_title,
        window_width,
        window_height,
        vsync_on,
        monitor_index
    };
}
