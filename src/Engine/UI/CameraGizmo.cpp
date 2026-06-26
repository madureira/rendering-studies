#include <algorithm>
#include <array>
#include <cmath>

#include <imgui.h>

#define IMVIEWGUIZMO_IMPLEMENTATION
#include <ImViewGuizmo/ImViewGuizmo.h>

#include "CameraGizmo.h"

namespace CameraGizmo
{

void Render(const Camera& cam, uint32 winWidth)
{
    // gizmoDiameter must match ImViewGuizmo's internal value (256 * style.scale, scale=1 by default)
    static constexpr float gizmoDiameter = 256.0f;
    static constexpr float margin = 1.0f;

    // position passed to ImViewGuizmo::Rotate is the CENTER of the gizmo
    const ImVec2 gizmoCenter = {
        static_cast<float>(winWidth) - gizmoDiameter * 0.5f - margin,
        gizmoDiameter * 0.5f + margin
    };

    // Build quaternion from Camera front/up vectors
    glm::vec3 cameraPos = cam.GetPosition();
    // glm::quatLookAt(dir, up): creates orientation where q*(0,0,-1) = dir (RH -Z forward)
    glm::quat cameraRot = glm::quatLookAt(cam.GetFront(), cam.GetUp());

    const glm::vec3 pivot(0.0f, 0.0f, 0.0f);

    ImViewGuizmo::BeginFrame();

    // ImViewGuizmo::Rotate requires an active ImGui window for GetWindowDrawList()
    ImGui::SetNextWindowPos(ImVec2(gizmoCenter.x - gizmoDiameter * 0.5f, gizmoCenter.y - gizmoDiameter * 0.5f));
    ImGui::SetNextWindowSize(ImVec2(gizmoDiameter, gizmoDiameter));
    constexpr ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNav;
    ImGui::Begin("##camera_gizmo", nullptr, flags);

    const bool changed = ImViewGuizmo::Rotate(cameraPos, cameraRot, pivot, gizmoCenter);

    ImGui::End();

    if (changed)
    {
        // q*(0,0,-1) = front, q*(0,1,0) = up  (GLM quatLookAt RH convention)
        const glm::vec3 newFront = glm::normalize(cameraRot * glm::vec3(0.0f, 0.0f, -1.0f));
        const glm::vec3 newUp = glm::normalize(cameraRot * glm::vec3(0.0f, 1.0f, 0.0f));
        const float pitch = glm::degrees(std::asin(glm::clamp(newFront.y, -1.0f, 1.0f)));
        const float yaw = glm::degrees(std::atan2(newFront.z, newFront.x));

        // Pass newUp as worldUp so Camera::UpdateCameraVectors doesn't degenerate
        // when front is parallel to (0,1,0) (top/bottom views, pitch = +-90 degrees).
        cam.OverrideInitialPosition(cameraPos, newUp, yaw, pitch);
    }
}

} // namespace CameraGizmo
