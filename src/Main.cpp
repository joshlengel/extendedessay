#include"Sim.h"
#include"Launch.h"

#include<vector>
#include<fstream>
#include<iostream>
#include<iomanip>

#include<random>
#include<chrono>

SolarSystem solar_system("/home/joshlengel/Dev/C++/ExtendedEssay/assets/solar_system.tm");
std::string start_date = "2020 Jan 1 00:00:00.00";

LaunchData launch_data;

void GetLaunchWindowAndPorkchop()
{
    solar_system.Init(Time::FromStr(start_date));

    // Use simulated annealing to find optimal launch date
    int32_t T_iterations = 10000;
    double T_max = 10000.0;

    double T_step = T_max / T_iterations;

    std::default_random_engine time_engine;
    std::default_random_engine prob_engine;

    std::seed_seq seed_seq({ static_cast<uint_fast32_t>(std::chrono::system_clock::now().time_since_epoch().count()) });
    time_engine.seed(seed_seq);
    prob_engine.seed(seed_seq);

    std::uniform_real_distribution time_dist(-Time::FromDays(60).Seconds(), Time::FromDays(60).Seconds());
    std::uniform_real_distribution prob_dist(0.0, 1.0);

    Time min_start = Time::FromStr("2021 Jan 1 00:00");
    Time min_end = Time::FromStr("2021 Mar 1 00:00");

    launch_data.time_inject = min_start;
    launch_data.time_exit = min_end;
    launch_data.Compute(&solar_system);

    LaunchData test_data = launch_data;

    // Use linear temperature decrease
    for (double T = T_max; T > 0; T -= T_step)
    {
        Time ns = test_data.time_inject + Time::FromSeconds(time_dist(time_engine));
        Time ne = test_data.time_exit + Time::FromSeconds(time_dist(time_engine));

        // Don't allow launches before 2021
        if (ns < min_start || ne < min_end) continue;

        LaunchData nd = test_data;
        nd.time_inject = ns;
        nd.time_exit = ne;
        nd.Compute(&solar_system);

        if (nd.dv_1 < test_data.dv_1)
        {
            // If new solution is more efficient, replace directly
            test_data.time_inject = ns;
            test_data.time_exit = ne;
            test_data.dv_1 = nd.dv_1;
        }
        else
        {
            // Use worse solution to enable possibility of exiting local minimum
            double alpha = exp(-(nd.dv_1 - test_data.dv_1) / T);

            if (prob_dist(prob_engine) < alpha)
            {
                test_data.time_inject = ns;
                test_data.time_exit = ne;
                test_data.dv_1 = nd.dv_1;
            }
        }
        
        // Update absolute minimum
        if (test_data.dv_1 < launch_data.dv_1)
        {
            launch_data = test_data;
        }
    }

    std::cout << "Launch date: " << launch_data.time_inject.Str() << ", Arrival date: " << launch_data.time_exit.Str() << ", Delta v: " << launch_data.dv_1 << std::endl;

    GeneratePorkChop("python/graph-1.json", &solar_system, launch_data);
}

void SimulateLaunch()
{
    solar_system.Init(Time::FromStr(start_date));

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

    launch_data.time_inject = Time::FromStr("2022 Sep 27 00:00");
    launch_data.time_exit = Time::FromStr("2023 Jun 01 00:00");

    launch_data.Compute(&solar_system);

    std::cout << "C3: " << launch_data.c3 << ", v_inf: " << launch_data.v_inf << ", Dv 1: " << launch_data.dv_1 << ", Dv 2: " << launch_data.dv_2 << ", Dv: " << (launch_data.dv_1 + launch_data.dv_2) << std::endl;
}

int main(int argc, char **argv)
{
    launch_data.payload = 100000.0; // kg
    launch_data.rocket_thrust = 6000000.0; // N
    launch_data.rocket_weight = 85000.0; // kg
    launch_data.fuel_flow_rate = 931.2; // kg / s

    double Re = 6371000; // m
    double Rm = 3389500; // m
    launch_data.Rleo = 1000000.0 + Re; // m
    launch_data.Rlmo = 1000000.0 + Rm; // m

    // -----------------------------------------

    // GetLaunchWindowAndPorkchop();

    // -----------------------------------------

    SimulateLaunch();
}