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
    bool m_ControlsEnabled = true;
    float32 m_CameraSpeed = 5.0f;
    float32 m_CameraAcceleratedSpeed = 15.0f;
    float32 m_PanelBottom = 10.0f;

public:
    inline void RenderSettings()
    {
        m_AppNames = AppRegistry::Instance().GetAllAppNames();

#ifdef __EMSCRIPTEN__
        ImGui::SetNextWindowPos(ImVec2(10.0f, 80.0f), ImGuiCond_Appearing);
#else
        ImGui::SetNextWindowPos(ImVec2(10.0f, 50.0f), ImGuiCond_Appearing);
#endif
        ImGui::Begin("Settings");

        ImGui::TextUnformatted("App");
        ImGui::SameLine();
        ImGui::Combo("##App", &m_SelectedAppIndex, &AppSelector::ComboGetter, &m_AppNames, static_cast<int>(m_AppNames.size()));

        ImGui::NewLine();

        ImGui::Checkbox("FPS", &m_FpsEnabled);
        ImGui::Checkbox("Grid", &m_GridEnabled);
        ImGui::Checkbox("Controls", &m_ControlsEnabled);
#ifndef __EMSCRIPTEN__
        ImGui::Checkbox("Wireframe", &m_PolygonModeEnabled);
#endif
        ImGui::Checkbox("Z-buffering", &m_ZBufferEnabled);
        ImGui::Checkbox("Face culling", &m_CullFaceEnabled);

        float32 itemWidth = ImGui::CalcItemWidth() / 3.0f;

        ImGui::AlignTextToFramePadding();
        ImGui::SetNextItemWidth(itemWidth - 10.0f);
        ImGui::SliderFloat("Camera speed", &m_CameraSpeed, 1.0f, 50.0f);
        ImGui::SetNextItemWidth(itemWidth - 10.0f);
        ImGui::SliderFloat("Camera speed (holding Shift)", &m_CameraAcceleratedSpeed, 1.0f, 100.0f);

        m_PanelBottom = ImGui::GetWindowPos().y + ImGui::GetWindowSize().y;
        ImGui::End();
    }

    inline void RenderControls(uint32 winWidth)
    {
        if (!m_ControlsEnabled)
        {
            return;
        }

        const float32 panelWidth = 250.0f;
        ImGui::SetNextWindowPos(ImVec2(winWidth - panelWidth - 160.0f, 50.0f), ImGuiCond_Appearing);
        ImGui::SetNextWindowSize(ImVec2(panelWidth, 0.0f), ImGuiCond_Appearing);
        ImGui::Begin("Controls", &m_ControlsEnabled, ImGuiWindowFlags_NoSavedSettings);

        auto row = [](const char* key, const char* action, const char* hint) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextDisabled("%s", key);
            ImGui::TableSetColumnIndex(1);
            ImGui::TextUnformatted(action);
            if (hint && hint[0] != '\0')
            {
                ImGui::SameLine();
                ImGui::TextDisabled("(?)");
                if (ImGui::IsItemHovered())
                {
                    ImGui::BeginTooltip();
                    ImGui::TextUnformatted(hint);
                    ImGui::EndTooltip();
                }
            }
        };

        constexpr ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV;

        ImGui::TextDisabled("Camera");
        ImGui::Separator();
        if (ImGui::BeginTable("##cam", 2, tableFlags))
        {
            ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthFixed, 110.0f);
            ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthStretch);
            row("W / Up", "Move forward", "");
            row("S / Down", "Move backward", "");
            row("A / Left", "Move left", "");
            row("D / Right", "Move right", "");
            row("Left click+drag", "Rotate", "Hold the left mouse button and drag");
            row("Scroll", "Zoom", "Scroll wheel moves the camera forward/backward");
            row("Shift", "Fast move", "Hold Shift to move at accelerated speed");
            ImGui::EndTable();
        }

#ifndef __EMSCRIPTEN__
        ImGui::NewLine();
        ImGui::TextDisabled("Window");
        ImGui::Separator();
        if (ImGui::BeginTable("##win", 2, tableFlags))
        {
            ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthFixed, 110.0f);
            ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthStretch);
            row("Enter", "Toggle fullscreen", "Switch between windowed and fullscreen mode");
            row("Esc", "Quit", "Close the application");
            ImGui::EndTable();
        }
#endif

        ImGui::End();
    }

    inline float32 GetPanelBottom() const
    {
        return m_PanelBottom;
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

    inline bool IsControlsEnabled() const
    {
        return m_ControlsEnabled;
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
    static const char* ComboGetter(void* user_data, int32 idx)
    {
        return (*static_cast<std::vector<std::string>*>(user_data))[static_cast<size_t>(idx)].c_str();
    }
};
