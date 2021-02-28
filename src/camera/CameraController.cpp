#include"camera/CameraController.h"

#include"window/Window.h"
#include"camera/Camera.h"

void Player::Update(Camera &camera, Window &window, float dt)
{
    camera.position = position;
    camera.pitch = pitch;
    camera.yaw = yaw;

    float run_step = run_speed * dt;
    float sprint_step = sprint_speed * dt;
    float elevate_step = elevate_speed * dt;

    float plane_step = window.GetKeyboard()->KeyDown(GLFW_KEY_LEFT_SHIFT) || window.GetKeyboard()->KeyDown(GLFW_KEY_RIGHT_SHIFT)? sprint_step : run_step;
    
    if (window.GetKeyboard()->KeyDown(GLFW_KEY_UP))
    {
        camera.MoveForward(plane_step);
    }

    if (window.GetKeyboard()->KeyDown(GLFW_KEY_DOWN))
    {
        camera.MoveBackward(plane_step);
    }

    if (window.GetKeyboard()->KeyDown(GLFW_KEY_LEFT))
    {
        camera.MoveLeft(plane_step);
    }

    if (window.GetKeyboard()->KeyDown(GLFW_KEY_RIGHT))
    {
        camera.MoveRight(plane_step);
    }

    if (window.GetKeyboard()->KeyDown(GLFW_KEY_SPACE))
    {
        camera.MoveUp(elevate_step);
    }

    if (window.GetKeyboard()->KeyDown(GLFW_KEY_C))
    {
        camera.MoveDown(elevate_step);
    }

    camera.RotateUp(window.GetCursor()->GetCursorDY() * sensitivity);
    camera.RotateRight(window.GetCursor()->GetCursorDX() * sensitivity);

    position = camera.position;
    pitch = camera.pitch;
    yaw = camera.yaw;
}