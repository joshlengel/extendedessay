#include"Sim.h"
#include"Launch.h"

#include<iostream>

int main(int argc, char **argv)
{
    StaticEntity sun    ("../assets/body-data-10.txt",   "../assets/body-10.csv");
    StaticEntity mercury("../assets/body-data-199.txt",   "../assets/body-1.csv");
    StaticEntity venus  ("../assets/body-data-299.txt",   "../assets/body-2.csv");
    StaticEntity earth  ("../assets/body-data-399.txt", "../assets/body-399.csv");
    StaticEntity moon   ("../assets/body-data-301.txt", "../assets/body-301.csv");
    StaticEntity mars   ("../assets/body-data-499.txt",   "../assets/body-4.csv");
    StaticEntity jupyter("../assets/body-data-599.txt",   "../assets/body-5.csv");
    StaticEntity saturn ("../assets/body-data-699.txt",   "../assets/body-6.csv");
    StaticEntity uranus ("../assets/body-data-799.txt",   "../assets/body-7.csv");
    StaticEntity neptune("../assets/body-data-899.txt",   "../assets/body-8.csv");

    SolarSystem solar_system =
    {
        &sun, &mercury, &venus, &earth, &moon, &mars, &jupyter, &saturn, &uranus, &neptune
    };

    Simulation simulation;
    simulation.entities.push_back(solar_system.sun    );
    simulation.entities.push_back(solar_system.mercury);
    simulation.entities.push_back(solar_system.venus  );
    simulation.entities.push_back(solar_system.earth  );
    simulation.entities.push_back(solar_system.moon   );
    simulation.entities.push_back(solar_system.mars   );
    simulation.entities.push_back(solar_system.jupyter);
    simulation.entities.push_back(solar_system.saturn );
    simulation.entities.push_back(solar_system.uranus );
    simulation.entities.push_back(solar_system.neptune);

    // --------------------------------------------------------------------------

    LaunchData launch_data;
    launch_data.payload = 100000.0; // kg
    launch_data.rocket_thrust = 6000000.0; // N
    launch_data.rocket_weight = 85000.0; // kg
    launch_data.fuel_flow_rate = 931.2; // kg / s
    launch_data.Rleo = 1000000.0; // m
    launch_data.Rlmo = 1000000.0; // m

    Time inject_time_center = Time::FromDate(2020, 7, 30, 11, 50);
    Time inject_time_span = Time::FromMinutes(70);
    Time inject_time_start = inject_time_center - inject_time_span;
    Time inject_time_end = inject_time_center + inject_time_span;

    Time exit_time_center = Time::FromDate(2021, 2, 18, 0, 0);
    Time exit_time_span = Time::FromDays(20);
    Time exit_time_start = exit_time_center - exit_time_span;
    Time exit_time_end = exit_time_center + exit_time_span;

    std::cout
        << "Earth position: " << reinterpret_cast<StaticEntity*>(solar_system.earth)->GetPosition(launch_data.time_inject) << '\n'
        << "Earth Velocity: " << reinterpret_cast<StaticEntity*>(solar_system.earth)->GetVelocity(launch_data.time_inject) << '\n'
        << "Mars position: " << reinterpret_cast<StaticEntity*>(solar_system.mars)->GetPosition(launch_data.time_inject) << '\n'
        << "Mars velocity: " << reinterpret_cast<StaticEntity*>(solar_system.mars)->GetVelocity(launch_data.time_inject) << std::endl;

    launch_data.Compute(&solar_system);

    return 0;
}