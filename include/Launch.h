#pragma once

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
    Vec inject_normal;

    double c3;
    double v_inf;

    double dv_1; // m / s
    double dv_2; // m / s

    double phi_2; // rad

    void Compute(SolarSystem *solar_system);
};

void GeneratePorkChop(const std::string &out, SolarSystem *solar_system, const LaunchData &launch_data);