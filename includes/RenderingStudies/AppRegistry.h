#pragma once

#include "App.h"

#include <string>
#include <unordered_map>
#include <vector>

class Window;
class Camera;

class AppRegistry
{
public:
    typedef App* (*Creator)(Window&, Camera&);

    static AppRegistry& Instance()
    {
        static AppRegistry registry;
        return registry;
    }

    void Register(const std::string& name, Creator creator)
    {
        m_Registry[name] = creator;
    }

    App* Create(const std::string& name, Window& window, Camera& camera) const
    {
        auto it = m_Registry.find(name);
        if (it == m_Registry.end())
        {
            return nullptr;
        }
        return (it->second)(window, camera);
    }

    std::vector<std::string> GetAllAppNames() const
    {
        std::vector<std::string> appNames;

        for (auto it = m_Registry.begin(); it != m_Registry.end(); ++it)
        {
            appNames.push_back(it->first);
        }
        return appNames;
    }

private:
    std::unordered_map<std::string, Creator> m_Registry;
};
