#pragma once

#include<vec3.hpp>

class Camera;
class Window;

class CameraController
{
public:
    virtual ~CameraController() {}

    virtual void OnTakeControl(Camera &camera) {}
    virtual void OnDropControl(Camera &camera) {}
    virtual void Update(Camera &camera, Window &window, float dt) = 0;
};

class Player : public virtual CameraController
{
public:
    glm::vec3 position;
    float pitch, yaw;

    float run_speed;
    float sprint_speed;
    float elevate_speed;

    float sensitivity;

    virtual void Update(Camera &camera, Window &window, float dt) override;    
};