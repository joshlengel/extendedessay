#include"camera/Camera.h"
#include"camera/CameraController.h"
#include"window/Window.h"

#include<gtc/matrix_transform.hpp>

void Camera::MoveForward(float step)
{
    position.x += std::sin(yaw) * step;
    position.z -= std::cos(yaw) * step;
}

void Camera::MoveBackward(float step)
{
    position.x -= std::sin(yaw) * step;
    position.z += std::cos(yaw) * step;
}

void Camera::MoveLeft(float step)
{
    position.x -= std::cos(yaw) * step;
    position.z -= std::sin(yaw) * step;
}

void Camera::MoveRight(float step)
{
    position.x += std::cos(yaw) * step;
    position.z += std::sin(yaw) * step;
}

void Camera::MoveUp(float step)
{
    position.y += step;
}

void Camera::MoveDown(float step)
{
    position.y -= step;
}

void Camera::RotateUp(float angle)
{
    pitch += angle;
}

void Camera::RotateRight(float angle)
{
    yaw += angle;
}

void Camera::Update(Window &window, float dt)
{
    if (m_current_controller)
        m_current_controller->Update(*this, window, dt);

    m_view_matrix = glm::rotate(glm::mat4(1.0f), pitch, { -1.0f, 0.0f, 0.0f });
    m_view_matrix = glm::rotate(m_view_matrix, yaw, { 0.0f, 1.0f, 0.0f });
    m_view_matrix = glm::translate(m_view_matrix, -position);

    m_projection_matrix = glm::perspective(fov, window.GetAspectRatio(), 0.01f, 1000.0f);
}

glm::mat4 Camera::GetProjectionMatrix() { return m_projection_matrix; }
glm::mat4 Camera::GetViewMatrix() { return m_view_matrix; }

void Camera::TakeControl(CameraController *controller)
{
    if (m_current_controller == controller) return;

    if (m_current_controller)
        m_current_controller->OnDropControl(*this);
    
    m_current_controller = controller;

    if (m_current_controller)
        m_current_controller->OnTakeControl(*this);
}