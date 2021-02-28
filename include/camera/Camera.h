#pragma once

#include<vec3.hpp>
#include<mat4x4.hpp>

class Window;
class CameraController;

class Camera
{
public:
    glm::vec3 position;
    float pitch, yaw;

    float fov;

    Camera();

    void MoveForward(float step);
    void MoveBackward(float step);
    void MoveLeft(float step);
    void MoveRight(float step);
    void MoveUp(float step);
    void MoveDown(float step);

    void RotateUp(float angle);
    void RotateRight(float angle);

    void Update(Window &window, float dt);

    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix();

    void TakeControl(CameraController *controller);

private:
    glm::mat4 m_view_matrix;
    glm::mat4 m_projection_matrix;

    CameraController *m_current_controller;
};