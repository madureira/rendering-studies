#pragma once

#include <glm/gtc/matrix_transform.hpp>

#include <RenderingStudies/Types.h>

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
    const float32 NEAR_CLIP = 0.01f;
    const float32 FAR_CLIP = 10000.0f;

    glm::vec3 m_Position;
    glm::vec3 m_Up;
    glm::vec3 m_Front;
    glm::vec3 m_Right;
    glm::vec3 m_WorldUp;

    float32 m_Yaw;
    float32 m_Pitch;
    float32 m_MovementSpeed;
    float32 m_MouseSensitivity;
    float32 m_Zoom;

    mutable glm::mat4 m_ProjectionCache{1.0f};
    mutable uint32 m_LastWindowWidth = 0;
    mutable uint32 m_LastWindowHeight = 0;
    mutable float32 m_LastZoom = 0.0f;

public:
    Camera(glm::vec3 position, glm::vec3 up, float32 yaw, float32 pitch)
        : m_Front(glm::vec3(0.0f, 0.0f, -1.0f))
        , m_MovementSpeed(SPEED)
        , m_MouseSensitivity(SENSITIVITY)
        , m_Zoom(ZOOM)
    {
        m_Position = position;
        m_WorldUp = up;
        m_Yaw = yaw;
        m_Pitch = pitch;

        UpdateCameraVectors();
    }

    glm::vec3 GetPosition() const
    {
        return m_Position;
    }

    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
    }

    glm::mat4 GetProjectionMatrix(uint32 windowWidth, uint32 windowHeight)
    {
        if (windowHeight == 0)
        {
            windowHeight = 1; // Prevent division by zero
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

    float32 GetZoom()
    {
        return m_Zoom;
    }

    void ProcessKeyboard(CameraMove direction, float32 deltaTime, float32 speed = 1.0f)
    {
        float32 velocity = m_MovementSpeed * deltaTime * speed;

        if (direction == CameraMove::FORWARD)
        {
            m_Position += m_Front * velocity;
        }

        if (direction == CameraMove::BACKWARD)
        {
            m_Position -= m_Front * velocity;
        }

        if (direction == CameraMove::LEFT)
        {
            m_Position -= m_Right * velocity;
        }

        if (direction == CameraMove::RIGHT)
        {
            m_Position += m_Right * velocity;
        }
    }

    void ProcessMouseDelta(float32 dx, float32 dy, bool constrainPitch = true)
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
            if (m_Pitch > 89.0f)  m_Pitch = 89.0f;
            if (m_Pitch < -89.0f) m_Pitch = -89.0f;
        }

        UpdateCameraVectors();
    }

    void ProcessMouseScroll(float32 yoffset)
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

private:
    void UpdateCameraVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        front.y = sin(glm::radians(m_Pitch));
        front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        m_Front = glm::normalize(front);
        m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
        m_Up = glm::normalize(glm::cross(m_Right, m_Front));
    }
};
