#pragma once

#include <imgui.h>

namespace UIComponent
{
inline void Vector3Sliders(const char* label, float32& x, float32& y, float32& z, const float32 min = -100.0f, const float32 max = 100.f)
{
    const float32 itemWidth = ImGui::CalcItemWidth() / 3.0f;

    ImGui::PushID(label);
    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted(label);
    ImGui::SameLine();

    ImGui::TextColored(ImVec4(1, 0, 0, 1), "X");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(itemWidth - 20.0f);
    ImGui::DragFloat("##x", &x, 0.1f, min, max);
    ImGui::SameLine();

    ImGui::TextColored(ImVec4(0, 1, 0, 1), "Y");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(itemWidth - 20.0f);
    ImGui::DragFloat("##y", &y, 0.1f, min, max);
    ImGui::SameLine();

    ImGui::TextColored(ImVec4(0, 0.5f, 1, 1), "Z");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(itemWidth - 20.0f);
    ImGui::DragFloat("##z", &z, 0.1f, min, max);
    ImGui::PopID();
}
} // namespace UIComponent
