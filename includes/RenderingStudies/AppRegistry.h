#pragma once

#include "App.h"

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

class Window;

class AppRegistry
{
public:
    using Creator = std::function<std::unique_ptr<App>(Window*)>;

    static AppRegistry& Instance()
    {
        static AppRegistry registry;
        return registry;
    }

    void Register(const std::string& name, Creator creator)
    {
        m_Registry[name] = std::move(creator);
    }

    std::unique_ptr<App> Create(const std::string& name, Window* window) const
    {
        auto it = m_Registry.find(name);
        if (it == m_Registry.end())
        {
            throw std::runtime_error("Unknown app: " + name);
        }
        return (it->second)(window);
    }

private:
    std::unordered_map<std::string, Creator> m_Registry;
};
