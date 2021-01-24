#include"Sim.h"
#include"Launch.h"

#include<vector>
#include<fstream>
#include<iostream>
#include<iomanip>

int main(int argc, char **argv)
{
    SolarSystem solar_system("assets/solar_system.tm");
    solar_system.Init(Time::FromStr("2020 Jan 1 00:00:00.00"));

    Simulation simulation;
    simulation.entities.push_back(&solar_system.sun    );
    simulation.entities.push_back(&solar_system.mercury);
    simulation.entities.push_back(&solar_system.venus  );
    simulation.entities.push_back(&solar_system.earth  );
    simulation.entities.push_back(&solar_system.moon   );
    simulation.entities.push_back(&solar_system.mars   );
    simulation.entities.push_back(&solar_system.jupyter);
    simulation.entities.push_back(&solar_system.saturn );
    simulation.entities.push_back(&solar_system.uranus );
    simulation.entities.push_back(&solar_system.neptune);

    // --------------------------------------------------------------------------

    LaunchData launch_data;
    launch_data.payload = 100000.0; // kg
    launch_data.rocket_thrust = 6000000.0; // N
    launch_data.rocket_weight = 85000.0; // kg
    launch_data.fuel_flow_rate = 931.2; // kg / s
    launch_data.Rleo = 1000000.0; // m
    launch_data.Rlmo = 1000000.0; // m

    Time inject_time_center = Time::FromStr("2020 Jul 30 11:50:00.00");
    Time inject_time_span = Time::FromDays(100);
    Time inject_time_start = inject_time_center - inject_time_span;
    Time inject_time_end = inject_time_center + inject_time_span;

    Time exit_time_center = Time::FromStr("2021 Feb 18 00:00:00.00");
    Time exit_time_span = Time::FromDays(150);
    Time exit_time_start = exit_time_center - exit_time_span;
    Time exit_time_end = exit_time_center + exit_time_span;

    uint32_t precision = 1000;

    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> z;

    Time x_step = Time::FromDays(inject_time_span.Days() * 2 / precision);
    Time y_step = Time::FromDays(exit_time_span.Days() * 2 / precision);

    Time tx = inject_time_start;

    for (uint32_t i = 0; i < precision; ++i)
    {
        Time ty = exit_time_start;

        for (uint32_t j = 0; j < precision; ++j)
        {
            launch_data.time_inject = tx; 
            launch_data.time_exit = ty;

            launch_data.Compute(&solar_system);

            x.push_back(tx.Days());
            y.push_back(ty.Days());
            z.push_back(launch_data.dv_1);

            ty += y_step;
        }

        tx += x_step;
    }

    std::ofstream out("python/graph-1.json");
    out << std::setprecision(13);
    out << "{\n"
        << "\t\"x\":[";

    for (uint32_t i = 0; i < x.size() - 1; ++i)
    {
        out << x[i] << ",";
    }

    out << x.back() << "],\n"
        << "\t\"y\":[";
    
    for (uint32_t i = 0; i < y.size() - 1; ++i)
    {
        out << y[i] << ",";
    }

    out << y.back() << "],\n"
        << "\t\"z\":[";
    
    for (uint32_t i = 0; i < z.size() - 1; ++i)
    {
        out << z[i] << ",";
    }

    out << z.back() << "]\n"
        << "}";

    return 0;
}