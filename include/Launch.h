#pragma once

#include"Sim.h"

struct LaunchData
{
    // Independent variables
    double payload; // kg
    double rocket_weight; // kg

    double rocket_thrust; // N (kg * m / s^2)
    double fuel_flow_rate; // kg / s

    double Rleo; // m
    double Rlmo; // m

    Time time_inject;
    Time time_exit;

    // Dependent variables
    double dv_1; // m / s
    double dv_2; // m / s

    double phi_2; // rad

    void Compute(SolarSystem *solar_system);
};