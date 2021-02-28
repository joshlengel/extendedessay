#include"body/Body.h"
#include"render/Shader.h"

#include<gtc/matrix_transform.hpp>

BodyModel::BodyModel(Renderable &renderable, ShaderProgram &program):
    m_renderable(renderable),
    m_program(program),
    m_position(0.0f, 0.0f, 0.0f),
    m_axis(1.0f, 0.0f, 0.0f),
    m_angle(0.0f),
    m_scale(1.0f, 1.0f, 1.0f),
    m_model_matrix(1.0f)
{}

glm::mat4 BodyModel::GetModelMatrix() const { return m_model_matrix; }

void BodyModel::SetPosition(glm::vec3 position) { m_position = position; }
void BodyModel::SetRotation(glm::vec3 axis, float angle) { m_axis = axis; m_angle = angle; }
void BodyModel::SetScale(glm::vec3 scale) { m_scale = scale; }

void BodyModel::Update()
{
    m_model_matrix = glm::translate(glm::mat4(1.0f), m_position);
    m_model_matrix = glm::rotate(m_model_matrix, m_angle, m_axis);
    m_model_matrix = glm::scale(m_model_matrix, m_scale);
}

EulerSolver Body::DEFAULT_SOLVER;

Body::Body(glm::dvec3 position, glm::dvec3 velocity, double mass, Renderable &renderable, ShaderProgram &program):
    m_state({ position: position, momentum: velocity * mass, force: glm::dvec3(), mass: mass }),
    m_cached_velocity(velocity),
    m_model(renderable, program)
{}

BodyModel &Body::GetModel() { return m_model; }
    
glm::dvec3 Body::GetPosition() const { return m_state.position; }
glm::dvec3 Body::GetVelocity() const { return m_cached_velocity; }
double Body::GetMass() const { return m_state.mass; }

void Body::SetPosition(glm::dvec3 position) { m_state.position = position; }
void Body::SetVelocity(glm::dvec3 velocity) { m_state.momentum = velocity * m_state.mass; }
void Body::SetMass(double mass) { m_state.mass = mass; }

void Body::Update(double dt, Solver &solver)
{
    solver.Step(dt, m_state);
    m_state.force = glm::dvec3();

    m_model.SetPosition(glm::vec3(m_state.position));
    m_model.Update();
}

void Body::ApplyForce(glm::dvec3 force)
{
    m_state.force += force;
}

void Body::Render() const
{
    m_model.m_program.SetUniform("model", m_model.GetModelMatrix());
    m_model.m_renderable.Render();
}