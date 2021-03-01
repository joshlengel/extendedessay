#include"window/Window.h"
#include"camera/Camera.h"
#include"camera/CameraController.h"
#include"body/SolarSystem.h"
#include"stars/StarMap.h"

#include<iostream>
#include<chrono>

#include<SpiceUsr.h>

/*
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

    std::cout << "Starting dv: " << launch_data.dv_1 << ", starting phi: " << launch_data.phi_2 << std::endl;

    double T = 1e11;
    constexpr const uint32_t num_samples = 1;
    double stepT = T / static_cast<double>(num_samples);

    double dv_bias = -6580.0;
    double dv_variance = 0.0;
    double phi_bias = -0.7;
    double phi_variance = 0.0;

    SampleResult best = { INFINITY, launch_data.dv_1 + dv_bias, launch_data.phi_2 + phi_bias };
    SampleResult approx = best;

    std::random_device rd;
    std::uniform_real_distribution<double> r_dist(0.0, 1.0);

    std::vector<double> ex, ey, ez, ebx, eby, ebz;
    std::vector<double> mx, my, mz, mbx, mby, mbz;
    std::vector<double> rx, ry, rz, rbx, rby, rbz;

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
        Vec rocket_pos = solar_system.earth.GetPosition(simulation_inject_time) + Vec::Rotate(launch_data.inject_normal, em_plane_normal, -(current_phi_2 + M_PI_2)) * launch_data.Rleo;

        Entity rocket(rocket_pos, solar_system.earth.GetVelocity(simulation_inject_time) + vel_normal * (current_dv_1 + sqrt(Constants::G * solar_system.earth.mass / launch_data.Rleo)), launch_data.rocket_weight);
        simulation.entities.push_back(&rocket);
        std::cout << "Rocket velocity: " << rocket.velocity << std::endl;

        simulation.launch_phases[simulation_exit_time - simulation_inject_time] =
        {
            callback: [](Simulation &sim, void *data)
            {
                Entity *rocket = reinterpret_cast<Entity*>(data);
                double vel = (solar_system.mars.GetVelocity(simulation_exit_time) - rocket->velocity).Length();
                double req = sqrt(Constants::G * solar_system.mars.mass / (rocket->position - solar_system.mars.GetPosition(simulation_exit_time)).Length());
                double dv_2 = req + vel;

                rocket->momentum = rocket->momentum + rocket->momentum.Normalized() * dv_2 * rocket->mass;
                std::cout << "Vel: " << vel << " Delta V2: " << dv_2 << std::endl;

            },
            data: &rocket
        };
        
        // Do simulation

        Time current = simulation_inject_time;
        Time graph_current = Time::FromDays(0);
        Time stop = simulation_exit_time + Time::FromDays(400);

        Time dt = Time::FromSeconds(60);
        Time graph_dt = Time::FromDays(1);

        ex.clear(); ey.clear(); ez.clear();
        mx.clear(); my.clear(); mz.clear();
        rx.clear(); ry.clear(); rz.clear();
        
        do
        {
            simulation.Step(dt);
            current += dt;
            graph_current += dt;

            if (graph_current > graph_dt)
            {
                graph_current -= graph_dt;
                Vec ep = solar_system.earth.GetPosition(current);
                ex.push_back(ep.x);
                ey.push_back(ep.y);
                ez.push_back(ep.z);

                Vec mp = solar_system.mars.GetPosition(current);
                mx.push_back(mp.x);
                my.push_back(mp.y);
                mz.push_back(mp.z);

                rx.push_back(rocket.position.x);
                ry.push_back(rocket.position.y);
                rz.push_back(rocket.position.z);
            }
        }
        while (current < stop);

        double dist = (rocket.position - solar_system.mars.GetPosition(current)).Length();

        if (dist < approx.dist || exp((approx.dist - dist) / T) > r_dist(rd))
        {
            approx.dist = dist;
            approx.dv_1 = current_dv_1;
            approx.phi_2 = current_phi_2;
        }

        if (approx.dist < best.dist)
        {
            best = approx;
            ebx = ex;
            eby = ey;
            ebz = ez;
            mbx = mx;
            mby = my;
            mbz = mz;
            rbx = rx;
            rby = ry;
            rbz = rz;
        }

        T -= stepT;
    }

    std::cout << "Finished sampling, results:" << std::endl;

    std::cout << "Best distance: " << best.dist << "m" << std::endl;
    std::cout << "Dv 1: " << best.dv_1 << "m/s" << std::endl;
    std::cout << "Phi 2: " << best.phi_2 << "rad" << std::endl;

    WritePositions("python/graph-earth.json", ebx, eby, ebz);
    WritePositions("python/graph-mars.json", mbx, mby, mbz);
    WritePositions("python/graph-rocket.json", rbx, rby, rbz);
}*/

constexpr const static uint32_t FPS_CAP = 60;
constexpr const static double FRAME_LENGTH = 1.0 / static_cast<double>(FPS_CAP);

int main(int argc, char **argv)
{
    /*
    // -----------------------------------------

    // GetLaunchWindowAndPorkchop();

    // -----------------------------------------

    AdjustLaunchValues();*/
    Window window("Extended Essay");
    window.Show();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Camera camera;
    camera.fov = M_PI_2;

    Player player;
    player.pitch = 0.0f;
    player.yaw = 0.0f;

    player.run_speed = 2e2f;
    player.sprint_speed = 5e2f;
    player.elevate_speed = 3e2f;

    player.sensitivity = 0.001f;

    camera.TakeControl(&player);

    furnsh_c("assets/naif0012.tls"); // Leapseconds kernel for Time::FromStr use
    SolarSystem solar_system(Time::FromStr("2020 Jan 1 00:00:00.00"), camera);

    player.position = solar_system.GetMars().GetPosition();

    // Star map
    StarMap star_map(1000, 1e7, 2e4, 1e-8f);

    ShaderProgram star_shader;
    star_shader.AttachShader(Shader::Load(ShaderType::VERTEX, "assets/shaders/star.vert"));
    star_shader.AttachShader(Shader::Load(ShaderType::FRAGMENT, "assets/shaders/star.frag"));
    star_shader.Make();

    star_shader.DeclareUniform("view");
    star_shader.DeclareUniform("projection");
    star_shader.DeclareUniform("color");

    // Start loop
    window.GetCursor()->Disable();
    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();

    while (!window.ShouldClose())
    {
        std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
        double dt_d = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1).count();
        float dt_f = static_cast<float>(dt_d);

        if (dt_d > FRAME_LENGTH)
        {
            t1 = t2;

            window.Update();

            if (window.GetKeyboard()->KeyPressed(GLFW_KEY_ESCAPE)) break;

            // Update
            camera.Update(window, dt_f);
            solar_system.Update(dt_d);

            // Render
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glDisable(GL_DEPTH_TEST);
            star_shader.Bind();
            star_shader.SetUniform("view", camera.GetViewMatrix());
            star_shader.SetUniform("projection", camera.GetProjectionMatrix());
            star_shader.SetUniform("color", glm::vec3(1.0f, 1.0f, 1.0f));
            star_map.Render();

            glEnable(GL_DEPTH_TEST);
            solar_system.Render();
        }
    }

    unload_c("assets/naif0012.tls"); 
}