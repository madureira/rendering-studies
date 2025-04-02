#pragma once

#include <glm/glm.hpp>
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

    float32 m_LastX;
    float32 m_LastY;
    bool m_FirstMouse = true;

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

        float32 aspectRatio = (float32)windowWidth / (float32)windowHeight;

        return glm::perspective(glm::radians(m_Zoom), // Field of view (45 degrees in radians)
            aspectRatio,                              // Aspect ratio (width / height)
            NEAR_CLIP,                                // Near clipping plane
            FAR_CLIP                                  // Far clipping plane
        );
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

    void ProcessMouseMovement(float32 mouseX, float32 mouseY, bool constrainPitch = true)
    {
        if (m_FirstMouse)
        {
            m_LastX = mouseX;
            m_LastY = mouseY;
            m_FirstMouse = false;
        }

        float32 xoffset = mouseX - m_LastX;
        float32 yoffset = m_LastY - mouseY; // Reversed since Y-coordinates go from bottom to top
        m_LastX = mouseX;
        m_LastY = mouseY;

        float32 dampingFactor = 0.5f; // Reduce sudden changes
        xoffset *= m_MouseSensitivity * dampingFactor;
        yoffset *= m_MouseSensitivity * dampingFactor;

        m_Yaw += xoffset;
        m_Pitch += yoffset;

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
