#include"Sim.h"
#include"Launch.h"
#include"Constants.h"
#include"Json.h"

#include<vector>
#include<fstream>
#include<iostream>
#include<iomanip>

#include<random>
#include<chrono>

SolarSystem solar_system("/home/joshlengel/Dev/C++/ExtendedEssay/assets/solar_system.tm");
std::string launch_window_start = "2020 Jan 1 00:00:00.00";

Time simulation_inject_time = Time::FromStr("2022 Sep 28 05:35:00.00");
Time simulation_exit_time = Time::FromStr("2023 Jun 1 03:48:00.00");

LaunchData launch_data =
{
    payload: 100000.0, // kg
    rocket_weight: 85000.0, // kg
    rocket_thrust: 6000000.0, // N
    fuel_flow_rate: 931.2, // kg / s

    Rleo: 1000000.0 + 6371000, // m
    Rlmo: 1000000.0 + 3389500 // m
};

void GetLaunchWindowAndPorkchop()
{
    solar_system.Init(Time::FromStr(launch_window_start));

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

void WritePositions(const std::string &file_name, const std::vector<double> &x, const std::vector<double> &y, const std::vector<double> &z)
{
    std::ofstream file(file_name);
    JsonWriter writer(file);
    writer.Begin();
    writer.Write("data");
    JsonObject positions(writer);
    positions.Begin();
    positions.Write("x");
    JsonArray arr_x(positions);
    arr_x.Begin();
    arr_x.WritePrimitiveArray(x.begin(), x.end());
    arr_x.End();
    positions.Write("y");
    JsonArray arr_y(positions);
    arr_y.Begin();
    arr_y.WritePrimitiveArray(y.begin(), y.end());
    arr_y.End();
    positions.Write("z");
    JsonArray arr_z(positions);
    arr_z.Begin();
    arr_z.WritePrimitiveArray(z.begin(), z.end());
    arr_z.End();
    positions.End();
    writer.End();
}

void AdjustLaunchValues()
{
    solar_system.Init(simulation_inject_time);

    launch_data.time_inject = simulation_inject_time;
    launch_data.time_exit = simulation_exit_time;

    launch_data.Compute(&solar_system);

    Vec em_plane_1 = launch_data.inject_normal;
    Vec em_plane_2 = solar_system.mars.GetPosition(simulation_exit_time) - solar_system.earth.GetPosition(simulation_inject_time);
    Vec em_plane_normal = Vec::Cross(em_plane_1, em_plane_2).Normalized();

    // Sampling
    struct SampleResult
    {
        double dist;
        double dv_1;
        double phi_2;
    };

    double T = 100.0;
    constexpr const uint32_t num_samples = 100;
    double stepT = T / static_cast<double>(num_samples);

    double dv_bias = 500.0;
    double dv_variance = 200.0;
    double phi_bias = M_PI / 8.0;
    double phi_variance = M_PI / 4.0;

    SampleResult best = { INFINITY, launch_data.dv_1 + dv_bias, launch_data.phi_2 + phi_bias };
    SampleResult approx = best;

    std::random_device rd;
    std::uniform_real_distribution<double> r_dist(0.0, 1.0);

    for (uint32_t i = 0; i < num_samples; ++i)
    {
        SolarSystem system(solar_system);

        Simulation simulation;
        simulation.entities.push_back(&system.sun    );
        simulation.entities.push_back(&system.mercury);
        simulation.entities.push_back(&system.venus  );
        simulation.entities.push_back(&system.earth  );
        simulation.entities.push_back(&system.moon   );
        simulation.entities.push_back(&system.mars   );
        simulation.entities.push_back(&system.jupyter);
        simulation.entities.push_back(&system.saturn );
        simulation.entities.push_back(&system.uranus );
        simulation.entities.push_back(&system.neptune);

        // Find neighbor
        double current_dv_1 = approx.dv_1 + (r_dist(rd) * 2.0f - 1.0f) * dv_variance;
        double current_phi_2 = approx.phi_2 + (r_dist(rd) * 2.0f - 1.0f) * phi_variance;

        // Reset simulation
        Vec vel_normal = Vec::Rotate(launch_data.inject_normal, em_plane_normal, -current_phi_2);
        Vec rocket_pos = solar_system.earth.GetPosition(simulation_inject_time) + Vec::Rotate(launch_data.inject_normal, em_plane_normal, -current_phi_2 - M_PI_2) * launch_data.Rleo;

        Entity rocket(rocket_pos, solar_system.earth.GetVelocity(simulation_inject_time) + vel_normal * (current_dv_1 + sqrt(Constants::G * solar_system.earth.mass / launch_data.Rleo)), launch_data.rocket_weight);
        simulation.entities.push_back(&rocket);
        
        // Do simulation

        Time current = simulation_inject_time;
        Time stop = simulation_exit_time + Time::FromDays(1);

        Time dt = Time::FromSeconds(60);
        
        do
        {
            simulation.Step(dt);
            current += dt;
        }
        while (current < stop);

        double dist = (rocket.position - solar_system.mars.GetPosition(simulation_exit_time)).Length();

        if (dist < approx.dist || exp((approx.dist - dist) / T) > r_dist(rd))
        {
            approx.dist = dist;
            approx.dv_1 = current_dv_1;
            approx.phi_2 = current_phi_2;
        }

        if (approx.dist < best.dist)
        {
            best = approx;
        }

        T -= stepT;
    }

    std::cout << "Finished sampling, results:" << std::endl;

    std::cout << "Best distance: " << best.dist << "m" << std::endl;
    std::cout << "Dv 1: " << best.dv_1 << "m/s" << std::endl;
    std::cout << "Phi 2: " << best.phi_2 << "rad" << std::endl;
}

int main(int argc, char **argv)
{
    // -----------------------------------------

    // GetLaunchWindowAndPorkchop();

    // -----------------------------------------

    AdjustLaunchValues();
}