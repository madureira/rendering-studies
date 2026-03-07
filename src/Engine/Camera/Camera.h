#pragma once

#include <RenderingStudies/Types.h>
#include <glm/glm.hpp>

enum class CameraMove
{
    BACKWARD,
    FORWARD,
    LEFT,
    RIGHT
};

class Camera final
{
private:
    const float32 SPEED = 2.5f;
    const float32 SENSITIVITY = 0.05f;
    const float32 ZOOM = 45.0f;
    const float32 NEAR_CLIP = 0.1f;
    const float32 FAR_CLIP = 10000.0f;

    glm::dvec3 m_Position;
    glm::vec3 m_Up;
    glm::vec3 m_Front;
    glm::vec3 m_Right;
    glm::vec3 m_WorldUp;

    float32 m_Yaw;
    float32 m_Pitch;
    float32 m_MovementSpeed;
    float32 m_MouseSensitivity;
    float32 m_Zoom;

    mutable glm::mat4 m_ProjectionCache{ 1.0f };
    mutable uint32 m_LastWindowWidth = 0;
    mutable uint32 m_LastWindowHeight = 0;
    mutable float32 m_LastZoom = 0.0f;

    void UpdateCameraVectors();

public:
    Camera(glm::vec3 position, glm::vec3 up, float32 yaw, float32 pitch);

    glm::vec3 GetPosition() const;
    glm::dvec3 GetPositionHP() const;
    glm::vec3 GetFront() const;
    glm::vec3 GetUp() const;
    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetViewMatrixRelative() const;
    glm::mat4 GetProjectionMatrix(uint32 windowWidth, uint32 windowHeight) const;
    float32 GetZoom() const;

    void Move(CameraMove direction, float32 deltaTime, float32 speed = 1.0f);
    void Look(float32 dx, float32 dy, bool constrainPitch = true);
    void Zoom(float32 yoffset);
};
