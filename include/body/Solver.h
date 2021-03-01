#pragma once

#include<vec3.hpp>

struct PhysicalState
{
    glm::dvec3 position;
    glm::dvec3 momentum;
    glm::dvec3 force;

    double mass;
};

class Solver
{
public:
    virtual ~Solver() {}

    virtual void Step(double dt, PhysicalState &state) = 0;
};

class EulerSolver : public virtual Solver
{
public:
    virtual void Step(double dt, PhysicalState &state) override;
};

class NoSolver : public virtual Solver
{
public:
    virtual void Step(double dt, PhysicalState &state) override;
};