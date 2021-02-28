#pragma once

#include"render/Model.h"
#include"body/Solver.h"

#include<vec3.hpp>
#include<mat4x4.hpp>

class ShaderProgram;

class BodyModel
{
    friend class Body;
public:
    BodyModel(Renderable &renderable, ShaderProgram &program);

    void SetRotation(glm::vec3 axis, float angle);
    void SetScale(glm::vec3 scale);

private:
    glm::mat4 GetModelMatrix() const;

    void SetPosition(glm::vec3 position);
    void Update();

    Renderable &m_renderable;
    ShaderProgram &m_program;

    glm::vec3 m_position;
    glm::vec3 m_axis;
    float m_angle;
    glm::vec3 m_scale;
    glm::mat4 m_model_matrix;
};

class Body : public virtual Renderable
{
public:
    Body(glm::dvec3 position, glm::dvec3 velocity, double mass, Renderable &renderable, ShaderProgram &program);
    
    BodyModel &GetModel();

    glm::dvec3 GetPosition() const;
    glm::dvec3 GetVelocity() const;
    double GetMass() const;

    void SetPosition(glm::dvec3 position);
    void SetVelocity(glm::dvec3 velocity);
    void SetMass(double mass);

    void Update(double dt, Solver &solver = DEFAULT_SOLVER);
    void ApplyForce(glm::dvec3 force);

    virtual void Render() const override;
private:
    PhysicalState m_state;
    glm::dvec3 m_cached_velocity;

    BodyModel m_model;

    static EulerSolver DEFAULT_SOLVER;
};

class Planet : public virtual Body
{
public:
    Planet(glm::dvec3 position, glm::dvec3 velocity, double mass, double radius);
};
