#pragma once

#include "Demo.h"

#include <string>
#include <unordered_map>
#include <vector>

class Window;
class Camera;

class DemoRegistry
{
public:
    typedef Demo* (*Creator)(Window&, Camera&);

    static DemoRegistry& Instance()
    {
        static DemoRegistry registry;
        return registry;
    }

    void Register(const std::string& name, Creator creator, bool webCompatible = true)
    {
#ifdef __EMSCRIPTEN__
        if (!webCompatible) return;
#else
        (void)webCompatible;
#endif
        m_Registry[name] = creator;
    }

    Demo* Create(const std::string& name, Window& window, Camera& camera) const
    {
        auto it = m_Registry.find(name);
        if (it == m_Registry.end())
        {
            return nullptr;
        }
        return (it->second)(window, camera);
    }

    std::vector<std::string> GetAllDemoNames() const
    {
        std::vector<std::string> demoNames;

        for (auto it = m_Registry.begin(); it != m_Registry.end(); ++it)
        {
            demoNames.push_back(it->first);
        }
        return demoNames;
    }

private:
    std::unordered_map<std::string, Creator> m_Registry;
};
