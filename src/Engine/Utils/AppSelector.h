#pragma once

#include <RenderingStudies/AppRegistry.h>
#include <RenderingStudies/Types.h>
#include <imgui.h>
#include <string>
#include <vector>

class Window;
class Camera;

class AppSelector final
{
private:
    std::vector<std::string> m_AppNames;
    int32 m_SelectedAppIndex = 0;
    bool m_FpsEnabled = true;
    bool m_PolygonModeEnabled = false;
    bool m_ZBufferEnabled = true;
    bool m_CullFaceEnabled = true;
    bool m_GridEnabled = true;
    float32 m_CameraSpeed = 5.0f;
    float32 m_CameraAcceleratedSpeed = 15.0f;

public:
    inline void Render()
    {
        m_AppNames = AppRegistry::Instance().GetAllAppNames();

        ImGui::Begin("Settings");

        ImGui::TextUnformatted("App");
        ImGui::SameLine();
        ImGui::Combo("##App", &m_SelectedAppIndex, &AppSelector::ComboGetter, &m_AppNames, static_cast<int>(m_AppNames.size()));

        ImGui::NewLine();

        ImGui::Checkbox("FPS", &m_FpsEnabled);
        ImGui::Checkbox("Grid", &m_GridEnabled);
        ImGui::Checkbox("Wireframe", &m_PolygonModeEnabled);
        ImGui::Checkbox("Z-buffering", &m_ZBufferEnabled);
        ImGui::Checkbox("Face culling", &m_CullFaceEnabled);

        float32 itemWidth = ImGui::CalcItemWidth() / 3.0f;

        ImGui::AlignTextToFramePadding();
        ImGui::SetNextItemWidth(itemWidth - 10.0f);
        ImGui::SliderFloat("Camera speed", &m_CameraSpeed, 1.0f, 50.0f);
        ImGui::SetNextItemWidth(itemWidth - 10.0f);
        ImGui::SliderFloat("Camera speed (holding Shift)", &m_CameraAcceleratedSpeed, 1.0f, 100.0f);

        ImGui::End();
    }

    inline int32 GetSelectedIndex() const
    {
        return m_SelectedAppIndex;
    }

    inline App* GetSelectedApp(Window& window, Camera& camera) const
    {
        return AppRegistry::Instance().Create(m_AppNames[m_SelectedAppIndex], window, camera);
    }

    inline bool IsPolygonModeEnabled() const
    {
        return m_PolygonModeEnabled;
    }

    inline bool IsFpsEnabled() const
    {
        return m_FpsEnabled;
    }

    inline bool IsZBufferEnabled() const
    {
        return m_ZBufferEnabled;
    }

    inline bool IsCullFaceEnabled() const
    {
        return m_CullFaceEnabled;
    }

    inline bool IsGridEnabled() const
    {
        return m_GridEnabled;
    }

    inline float32 GetCameraSpeed() const
    {
        return m_CameraSpeed;
    }

    inline float32 GetCameraAcceleratedSpeed() const
    {
        return m_CameraAcceleratedSpeed;
    }

private:
    static const char* ComboGetter(void* user_data, int idx)
    {
        return (*static_cast<std::vector<std::string>*>(user_data))[static_cast<size_t>(idx)].c_str();
    }
};
