#pragma once

#include "Types.h"
#include <fstream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>

struct Config
{
    std::string window_title;
    int16 window_width;
    int16 window_height;
    bool window_fullscreen;
    bool show_fps;
    bool vsync_on;
    std::string app_name;
};

inline bool parseBool(const std::string& raw)
{
    std::string v = raw;
    // optional: trim spaces
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
    bool window_fullscreen = false;
    bool show_fps = false;
    bool vsync_on = false;
    std::string app_name;

    bool has_width = false;
    bool has_height = false;

    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
        {
            continue;
        }

        const auto pos = line.find('=');
        if (pos == std::string::npos)
        {
            continue;
        }

        const std::string key = line.substr(0, pos);
        const std::string value = line.substr(pos + 1); // can contain spaces

        if (key == "app")
        {
            app_name = value;
        }
        else if (key == "window_title")
        {
            // value can have spaces, we just keep the whole substring
            window_title = value;
        }
        else if (key == "window_width")
        {
            int v = std::stoi(value); // throws if invalid
            if (v < std::numeric_limits<int16>::min() ||
                v > std::numeric_limits<int16>::max())
            {
                throw std::runtime_error("window_width out of int16 range");
            }
            window_width = static_cast<int16>(v);
            has_width = true;
        }
        else if (key == "window_height")
        {
            int v = std::stoi(value);
            if (v < std::numeric_limits<int16>::min() ||
                v > std::numeric_limits<int16>::max())
            {
                throw std::runtime_error("window_height out of int16 range");
            }
            window_height = static_cast<int16>(v);
            has_height = true;
        }
        else if (key == "window_fullscreen")
        {
            window_fullscreen = parseBool(value);
        }
        else if (key == "show_fps")
        {
            show_fps = parseBool(value);
        }
        else if (key == "vsync_on")
        {
            vsync_on = parseBool(value);
        }
    }

    if (app_name.empty())
    {
        throw std::runtime_error("No 'app' key found in config file");
    }

    if (window_title.empty())
    {
        throw std::runtime_error("No 'window_title' key found in config file");
    }

    if (!has_width || !has_height)
    {
        throw std::runtime_error("Missing 'window_width' or 'window_height' in config file");
    }

    LOG_INFO("Rendering app: {}", app_name);

    // Order matches the struct: width, height, title, app_name
    return Config{ window_title, window_width, window_height, window_fullscreen, show_fps, vsync_on, app_name };
}
