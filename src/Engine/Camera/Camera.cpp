#include "Camera.h"

#include <cmath>

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 position, glm::vec3 up, float32 yaw, float32 pitch)
    : m_Position(position)
    , m_Front(glm::vec3(0.0f, 0.0f, -1.0f))
    , m_Zoom(ZOOM)
    , m_MovementSpeed(SPEED)
    , m_MouseSensitivity(SENSITIVITY)
{
    m_WorldUp = up;
    m_Yaw = yaw;
    m_Pitch = pitch;

    UpdateCameraVectors();
}

void Camera::OverrideInitialPosition(glm::vec3 position, float32 yaw, float32 pitch, float32 roll) const
{
    m_Position = position;
    m_Yaw = yaw;
    m_Pitch = pitch;
    m_Roll = roll;

    UpdateCameraVectors();
    m_ViewDirty = true;
}

glm::vec3 Camera::GetPosition() const
{
    return glm::vec3(m_Position);
}

glm::dvec3 Camera::GetPositionHP() const
{
    return m_Position;
}

glm::vec3 Camera::GetFront() const
{
    return m_Front;
}

glm::vec3 Camera::GetUp() const
{
    return m_Up;
}

glm::mat4 Camera::GetViewMatrix() const
{
    if (m_ViewDirty)
    {
        glm::vec3 pos = glm::vec3(m_Position);
        m_ViewCache = glm::lookAt(pos, pos + m_Front, m_Up);

        glm::dvec3 origin = m_Position;
        origin.y = 0.0;
        glm::vec3 posRel = glm::vec3(m_Position - origin);
        m_ViewRelCache = glm::lookAt(posRel, posRel + m_Front, m_Up);

        m_ViewDirty = false;
    }
    return m_ViewCache;
}

glm::mat4 Camera::GetViewMatrixRelative() const
{
    if (m_ViewDirty)
    {
        GetViewMatrix();
    }
    return m_ViewRelCache;
}

glm::mat4 Camera::GetProjectionMatrix(uint32 windowWidth, uint32 windowHeight) const
{
    if (windowHeight == 0)
    {
        windowHeight = 1;
    }

    if (windowWidth != m_LastWindowWidth || windowHeight != m_LastWindowHeight || m_Zoom != m_LastZoom)
    {
        float32 aspectRatio = (float32)windowWidth / (float32)windowHeight;
        m_ProjectionCache = glm::perspective(glm::radians(m_Zoom), aspectRatio, NEAR_CLIP, FAR_CLIP);
        m_LastWindowWidth = windowWidth;
        m_LastWindowHeight = windowHeight;
        m_LastZoom = m_Zoom;
    }
    return m_ProjectionCache;
}

float32 Camera::GetZoom() const
{
    return m_Zoom;
}

float32 Camera::GetYaw() const
{
    float32 yaw = std::fmod(m_Yaw, 360.0f);
    if (yaw > 180.0f)
    {
        yaw -= 360.0f;
    }
    if (yaw < -180.0f)
    {
        yaw += 360.0f;
    }
    return yaw;
}

float32 Camera::GetPitch() const
{
    return m_Pitch;
}

float32 Camera::GetRoll() const
{
    return m_Roll;
}

void Camera::Move(CameraMove direction, float32 deltaTime, float32 speed) const
{
    float64 velocity = static_cast<float64>(m_MovementSpeed * deltaTime * speed);
    glm::dvec3 front = glm::dvec3(m_Front);
    glm::dvec3 right = glm::dvec3(m_Right);

    if (direction == CameraMove::FORWARD)
    {
        m_Position += front * velocity;
    }

    if (direction == CameraMove::BACKWARD)
    {
        m_Position -= front * velocity;
    }

    if (direction == CameraMove::LEFT)
    {
        m_Position -= right * velocity;
    }

    if (direction == CameraMove::RIGHT)
    {
        m_Position += right * velocity;
    }

    m_ViewDirty = true;
}

void Camera::Look(float32 dx, float32 dy, bool constrainPitch) const
{
    float32 dampingFactor = 0.5f;
    dx *= m_MouseSensitivity * dampingFactor;
    dy *= m_MouseSensitivity * dampingFactor;

    dx = glm::clamp(dx, -50.0f, 50.0f);
    dy = glm::clamp(dy, -50.0f, 50.0f);

    m_Yaw += dx;
    m_Pitch -= dy;

    if (constrainPitch)
    {
        if (m_Pitch > 89.0f)
        {
            m_Pitch = 89.0f;
        }

        if (m_Pitch < -89.0f)
        {
            m_Pitch = -89.0f;
        }
    }

    UpdateCameraVectors();
    m_ViewDirty = true;
}

void Camera::Zoom(float32 yoffset) const
{
    m_Zoom -= yoffset;

    if (m_Zoom < 1.0f)
    {
        m_Zoom = 1.0f;
    }

    if (m_Zoom > 45.0f)
    {
        m_Zoom = 45.0f;
    }
}

void Camera::UpdateCameraVectors() const
{
    glm::vec3 front;
    front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    front.y = sin(glm::radians(m_Pitch));
    front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    m_Front = glm::normalize(front);

    // Use a fallback reference when front is nearly parallel to world up (pitch ~ +-90deg)
    glm::vec3 ref = (std::abs(m_Front.y) < 0.999f) ? m_WorldUp : glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 right0 = glm::normalize(glm::cross(m_Front, ref));
    glm::vec3 up0 = glm::normalize(glm::cross(right0, m_Front));

    // Apply roll (rotation around the front axis)
    float32 cosR = std::cos(glm::radians(m_Roll));
    float32 sinR = std::sin(glm::radians(m_Roll));
    m_Right = glm::normalize(right0 * cosR - up0 * sinR);
    m_Up = glm::normalize(right0 * sinR + up0 * cosR);
}
