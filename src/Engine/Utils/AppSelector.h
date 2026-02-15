#pragma once

#include "../Window/Window.h"
#include <RenderingStudies/App.h>
#include <RenderingStudies/AppRegistry.h>
#include <RenderingStudies/Types.h>
#include <imgui.h>
#include <string>
#include <vector>

class AppSelector final
{
private:
    std::vector<std::string> m_AppNames;
    int32 m_SelectedAppIndex = 0;

public:
    void Render()
    {
        m_AppNames = AppRegistry::Instance().GetAllAppNames();

        ImGui::Begin("Select an app");
        ImGui::Combo("##App", &m_SelectedAppIndex, &AppSelector::ComboGetter, &m_AppNames, static_cast<int>(m_AppNames.size()));
        ImGui::End();
    }

    int32 GetSelectedIndex()
    {
        return m_SelectedAppIndex;
    }

    App* GetSelectedApp(Window* window)
    {
        return AppRegistry::Instance().Create(m_AppNames[m_SelectedAppIndex], window);
    }

private:
    static const char* ComboGetter(void* user_data, int idx)
    {
        return (*static_cast<std::vector<std::string>*>(user_data))[static_cast<size_t>(idx)].c_str();
    }
};
