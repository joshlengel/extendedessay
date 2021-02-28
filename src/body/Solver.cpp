#include"body/Solver.h"

// Euler integration
void EulerSolver::Step(double dt, PhysicalState &state)
{
    glm::dvec3 delta_p = state.force * dt;
    glm::dvec3 delta_s = state.momentum * dt / state.mass;

    state.momentum += delta_p;
    state.position += delta_s;
}