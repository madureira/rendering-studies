#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 position, glm::vec3 up, float32 yaw, float32 pitch)
    : m_Position(position)
    , m_Front(glm::vec3(0.0f, 0.0f, -1.0f))
    , m_MovementSpeed(SPEED)
    , m_MouseSensitivity(SENSITIVITY)
    , m_Zoom(ZOOM)
{
    m_WorldUp = up;
    m_Yaw = yaw;
    m_Pitch = pitch;

    UpdateCameraVectors();
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
    glm::vec3 pos = glm::vec3(m_Position);
    return glm::lookAt(pos, pos + m_Front, m_Up);
}

glm::mat4 Camera::GetProjectionMatrix(uint32 windowWidth, uint32 windowHeight)
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

void Camera::ProcessKeyboard(CameraMove direction, float32 deltaTime, float32 speed)
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
}

void Camera::ProcessMouseDelta(float32 dx, float32 dy, bool constrainPitch)
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
            m_Pitch = 89.0f;
        if (m_Pitch < -89.0f)
            m_Pitch = -89.0f;
    }

    UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(float32 yoffset)
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

void Camera::UpdateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    front.y = sin(glm::radians(m_Pitch));
    front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    m_Front = glm::normalize(front);
    m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
    m_Up = glm::normalize(glm::cross(m_Right, m_Front));
}
